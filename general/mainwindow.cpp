#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QSqlError>
#include <QModelIndex>
#include <QMessageBox>
#include <QPair>
#include <QTime>
#include "environmentdialog.h"
#include <math.h>
#include <QUuid>
#include <QDateTime>
#include "mustache.h"
#include "reportdialog.h"
#include "atagcompleter.h"
#include "../ReportCreator/reportcreatorinterface.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //---------------------------------------
    //          Настраиваем класс логирования
    //      понадобится сразу для вывода сообщений об ошибках
    logger = new ALoggerModel(this);
    ui->listView->setModel(logger);

    //инициализируем хранилище настроек
    settings = new QSettings("settings.ini" , QSettings::IniFormat);
    //Загружаем сохранненные настройки
    currentCPADriver = settings->value("kalibri2/currentCPADriver", "iksu2000.dll").toString();
    //***************************************************
    //      Инициализация задатчика
    //***************************************************
    //Устанавливаем каталог драйверов задачиков
    this->CPADriverDir = new QDir(qApp->applicationDirPath());
    CPADriverDir->cd("CPADrivers");
    //Инициализируем плагин загрузчика драйверов задатчиков
    CPAPluginLoader = new QPluginLoader();
    //Создаем фильтр для поиска задатчиков
    QStringList filterList;
    filterList<<"*.dll";
    //Инициализируем список драйверов задатчиков
    CPADrivers = new QMap<QString, QString>;
    //Создаем поток для выполнения задатчика
    //CPADriverThread = new QThread(this);
    //Заполняем список драйверов задатчиков
    foreach (QString fileName, CPADriverDir->entryList(filterList, QDir::Files))
    {
        //Загружаем библиотеку в загрузчик плагинов
        CPAPluginLoader->setFileName(CPADriverDir->absoluteFilePath(fileName));
        QObject * testObject = CPAPluginLoader->instance();
        //Проверяем является ли библиотека плагином
        if (testObject) {
            //Проверяем является ли плагин драйвером задатчика
            CPAInterface * testCPADriver = qobject_cast<CPAInterface * >(testObject);
            if (testCPADriver) {
                qDebug()<<testCPADriver;
                //добавляем драйвер в список
                CPADrivers->insert(fileName, testCPADriver->getName());
                qDebug()<<testCPADriver->getName();
            }
            delete testObject;
        }
        //выгружаем плагин
        CPAPluginLoader->unload();
    }
    //Создаем и заполняем группу пунктов меню для выбора задатчика
    CPAActionGroup = new QActionGroup(this->ui->menu_3);
        //Перебираем все задатчики
    foreach (QString fileName, CPADrivers->keys()) {
        //Создаем для каждого пункт меню
        QAction * newAction = new QAction(CPADrivers->value(fileName), CPAActionGroup);
        //Делаем его отмечаемым
        newAction->setCheckable(true);
        //actionGroup->addAction(newAction);
        newAction->setProperty("fileName", fileName);
        //Соединяем сигнал со слотом
        connect(newAction, SIGNAL(toggled(bool)), this, SLOT(on_changeCPA(bool)));
        if (fileName==currentCPADriver) newAction->setChecked(true);
    }
    //Делаем пункты меню эксклюзивными
    CPAActionGroup->setExclusive(true);
    //Добавляем группу в меню
    this->ui->menu_3->addActions(CPAActionGroup->actions());

    //Настраиваем таблицы
    ui->tableView->verticalHeader()->setDefaultSectionSize(ui->tableView->verticalHeader()->minimumSectionSize());
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    setStart(false);
    //connect(this, SIGNAL(startWork(bool)),this , SLOT(setStart(bool)));
    emit this->startWork(start);
    //Соединение пунктов меню с функциями
    connect(ui->action_5,SIGNAL(triggered(bool)), this, SLOT(settingDialog()));
    connect(ui->create_db, SIGNAL(triggered(bool)), this, SLOT(createDB()));
    connect(ui->open_db, SIGNAL(triggered(bool)),this, SLOT(openDB()));
    automaticSet = false;


    //Создаем диалоги
    //Диалог настройки программы
    setting = new SettingsDialog(this->settings);
    setting->setModal(true);
    //Диалог создания базы данных
    createDlg = new createDialog();
    createDlg->setModal(true);



    dom = new ADomCalibration();
    XMLmodel = new AXMLCalibrationModel(dom);
    ui->tableView->setModel(XMLmodel);
    XMLResultsModel = new AXMLCalibrationResultModel(dom);
    ui->calibrateView->setModel(XMLResultsModel);
    XMLCalibrationModel = new AXMLCalibrationListModel(dom);
    ui->calibrationBox->setModel(XMLCalibrationModel);
    //Соединяем сигнал изменения текущего канала
    connect(ui->tableView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), XMLResultsModel, SLOT(change_current_channel(QModelIndex,QModelIndex)));

//    connect(ui->tableView, SIGNAL(clicked(QModelIndex)), XMLResultsModel, SLOT(change_current_channel(QModelIndex)));
//    connect(ui->tableView, SIGNAL(activated(QModelIndex)), XMLResultsModel, SLOT(change_current_channel(QModelIndex)));
   //Подключаем логирование сообщений от базы данных
    connect(dom, SIGNAL(log(QString,Qt::GlobalColor)), logger, SLOT(on_log(QString,Qt::GlobalColor)));
    //ui->tableView->setModel(XMLmodel);
    //----------------------------------


//    //Модели подключения к базе данных
//    sgModel = new QSqlTableModel();
//    proxySgModel = new QSortFilterProxyModel();
//    proxySgModel->setSourceModel(sgModel);

//    resModel = new QSqlTableModel();
//    proxyResModel = new QSortFilterProxyModel();
//    proxyResModel->setSourceModel(resModel);
//    //sgModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
//    resModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

    timer = new QTimer(this);
    timer->setInterval(1000);
    delayTimer = new QTimer(this);
    connect(delayTimer, SIGNAL(timeout()), this, SLOT(delayTimer_overflow()));

    sgList = new QMap<QString, float>;

    //Создаем список драйверов доступа к данным в формате -файл драйвера, имя драйвера-
    connectDrivers = new QMap<QString, QString>;
    //Каталог драйверов доступа к базе данных
    this->connectDriverDir = new QDir(qApp->applicationDirPath());
    connectDriverDir->cd("ConnectDrivers");
    //ищем драйвера доступа к данным
    findConnectPlugin();

    connectPluginLoader = new QPluginLoader();

    //Связываем сигналы калибратора с главным процессом
    connect(this, SIGNAL(end_init()),SLOT(sl_set_next_point()));
    connect(this->timer, SIGNAL(timeout()), this, SLOT(timer_overflow()));
    connect(this, SIGNAL(end_calibration_next_point()), SLOT(sl_set_next_point()));
    connect(this, SIGNAL(error_calibrations(QString)), SLOT(sl_error_calibration(QString)));
    connect(CPADriver, SIGNAL(set_value_ok()), this, SLOT(sl_end_set_value()));
    //connect(myCalibrator, SIGNAL(start_calibration()), this, SLOT(sl_start_calibration()));
    //connect(myCalibrator, SIGNAL(set_next_point(double)), this, SLOT(sl_set_next_point(double)));
    //connect(this, SIGNAL(set_next_point_complete(bool)), myCalibrator, SLOT(on_set_next_point_complete(bool)));
    //Создаем диалог условий калибровки
    this->ed = new EnvironmentDialog(this->settings);
    ed->setModal(true);
    ui->progressBar->setValue(0);

    this->calibrationDom = new QVector<TChannelCalibration*>;
    this->currentCalibrationDom = new QVector<TChannelCalibration*>;

    connect(this, SIGNAL(get_next_values()),this->XMLResultsModel, SLOT(model_reset()));


//    qDebug()<<"Создание основного окна";
}

MainWindow::~MainWindow()
{
    qDebug()<<"finish";
    connectPluginLoader->unload();
    delete ui;
}

void MainWindow::settingDialog()
{
    setting->exec();
}

//Создаем новую базу данных
void MainWindow::createDB()
{
    qDebug()<<"Создание базы данных";
    //Список драйверов опроса
    QStringList drvList;
    //заполняем список именами найденых драйверов
    foreach (QString key, this->connectDrivers->keys()) {
        drvList.append(connectDrivers->value(key));
    }
    //заполняем поля запроса диалога
    createDlg->setDefault(drvList);
    //вызываем диалог создания базы данных
    if (createDlg->exec()==QDialog::Accepted) {
        qDebug()<<"Нажата кнопка ОК. Создаем базу данных";
        //Проверяем наличие базы данных в каталоге баз
        QFile DBFile;
        DBFile.setFileName(createDlg->getNameDB()+".xml");
        if (DBFile.exists()) {
            qDebug()<<"База данных с таким именем уже существует";
            return;
        }
        dom->clear();
        currentConnectDriver = this->connectDrivers->keys().at(createDlg->getIndexDriver());
        loadConnectPlugin(currentConnectDriver);
        dom->addSetting("driver", currentConnectDriver);
        QVector<QString> *fieldsDB = new QVector<QString>;
        connectDriver->getFieldsDB(fieldsDB);
        dom->addSetting("fieldscount", QString::number(fieldsDB->count()));

        for (int i=1; i<=fieldsDB->count(); i++) {
            dom->addSetting("field"+QString::number(i-1), fieldsDB->at(i-1));
        }
        dom->addSetting("version","3");
        dom->save(createDlg->getNameDB()+".xml");
    }
}

void MainWindow::openDB()
{
    logger->log("Загрузка нового файла");
    //Вызываем диалог открытия файла базы xml
    QString str = QFileDialog::getOpenFileName(0, "Open Dialog", "ConnectDrivers", "*.xml");
    if (str=="") {
        logger->log("Загрузка файла отменена");
        return;
    }
    dom->clear();
    if (!dom->open(str)) {
        logger->log("Файл не был открыт", Qt::red);
        return;
    }
    //Переопределяем ширину колонок по содержимому
    ui->tableView->resizeColumnsToContents();
    ui->calibrateView->resizeRowsToContents();
    this->fileName=str;

    //Получаем имя драйвера доступа к данным
    currentConnectDriver = dom->getDriverFile();
    //Загружаем драйвер доступа к данным
    loadConnectPlugin(currentConnectDriver);

    setStart(true);
}

void MainWindow::setStart(bool start)
{
        ui->addButton->setEnabled(start);
        ui->deleteButton->setEnabled(start);
        ui->cloneButton->setEnabled(start);
        ui->pushButton_2->setEnabled(start);
        ui->tableView->setEnabled(start);
        ui->calibrateView->setEnabled(start);
}

//Считывание данных с контроллера
void MainWindow::sl_read_values()
{

}

//Обработка ошибок калибровки
void MainWindow::sl_error_calibration(QString error)
{

}

//Поиск драйверов доступа к данным
void MainWindow::findConnectPlugin()
{
    //qDebug()<<connectDriverDir.absolutePath();
    //Задаем фильтр по расширению
    QStringList filterList;
    filterList<<"*.dll";
    //Проверяем все подхдящие файлы
    foreach (QString fileName, connectDriverDir->entryList(filterList, QDir::Files))
    {
        //Загрузчик плагина
        QPluginLoader connectDriverLoader(connectDriverDir->absoluteFilePath(fileName));
        //Получаем корневой объект плагина
        QObject *connectDriverF = connectDriverLoader.instance();
        if (connectDriverF){
            //Если корневой объект получен - преобразуем его в интерфейс доступа к данным
            DataInterface *connectDriver = qobject_cast<DataInterface *>(connectDriverF);
            if (connectDriver)
            {
                //Получаем имя интерфейса и добавляем в список
                QString nameDriver = connectDriver->getName();
                this->connectDrivers->insert(fileName, nameDriver);
                //qDebug()<<nameDriver;
            }
        }
        //выгружаем плагин
        connectDriverLoader.unload();
    }
}

//Загрузка драйвер
bool MainWindow::loadConnectPlugin(QString fileName)
{
    //Проверяем дествительна ли ссылка
    QPluginLoader * di = qobject_cast<QPluginLoader *>(this->connectPluginLoader);
    if (di){
        //Если драйвер уже загружен - выходим
        if (this->connectPluginLoader->fileName()==fileName) return true;
        //выгружаем старый плагин
        this->connectPluginLoader->unload();
    }
    connectPluginLoader->setFileName(connectDriverDir->absoluteFilePath(fileName));

    QObject *connectDriverF = connectPluginLoader->instance();
    if (connectDriverF){
        //Если корневой объект получен - преобразуем его в интерфейс доступа к данным
        DataInterface *connectDriver = qobject_cast<DataInterface *>(connectDriverF);
        if (connectDriver)
        {
            this->connectDriver = connectDriver;
            this->connectDriver->initialization(XMLmodel);
            return true;
        }
    }
    return false;
}

//смена драйвера задатчика
bool MainWindow::changeCPADriver(QString fileName)
{
    //Останавливаем поток если он запущен
    //if (CPADriverThread->isRunning()) CPADriverThread->quit();
    //если имя драйвера присутствует в списке
    if (CPADrivers->contains(fileName)) {
        //qDebug()<<CPADriver;
        if (CPADriver) {
            //qDebug()<<"выгрузка плагина";
            //Уничтожение старого плагина
            //disconnect(CPADriver, SIGNAL(set_value_ok()),this ,SLOT(sl_end_set_value()));
            CPAInterface * del = CPADriver;
            CPADriver = 0;
            //delete del;
            CPAPluginLoader->unload();
        }
        CPAPluginLoader->setFileName(CPADriverDir->absoluteFilePath(fileName));
        qDebug()<<"Загрузка нового драйвера";
        CPADriver = qobject_cast<CPAInterface * >(CPAPluginLoader->instance());
        connect(CPADriver, SIGNAL(log(QString,Qt::GlobalColor)), logger, SLOT(on_log(QString,Qt::GlobalColor)));
        //connect(CPADriver, SIGNAL(set_value_ok()),this ,SLOT(sl_end_set_value()));
        CPADriver->initialization(settings);
        settings->setValue("kalibri2/currentCPADriver", fileName);
        return true;
    }
    return false;
}



//-------------------------------------------------------------------------------
//------------------------Старт калибровки---------------------------------------
//-------------------------------------------------------------------------------
void MainWindow::on_pushButton_2_clicked()
{
    //Проверяем актуальность условий калибровки
    QDateTime lastEdit = ed->getLastEdit();
    int deltaTime = lastEdit.secsTo(QDateTime::currentDateTime());
    if (deltaTime>14400) {
        this->logger->log("Данные об условиях проведения калибровки устарели", Qt::red);
        return;
    }
    //Проверяем условия калибровки
    conditions.clear();
    conditions.insert("temperature", ed->getTemperature());
    conditions.insert("humidity", ed->getHumidity());
    conditions.insert("pressure", ed->getPressure());
    conditions.insert("voltage", ed->getVoltage());
    if (!CPADriver->checkConditions(conditions)) {
        this->logger->log("Не пройден контроль условий эксплуатации эталона "+CPADriver->getName(), Qt::red);
        return;
    }
    //Очищаем список калибровок
    calibrationDom->clear();
    //Определяем список индексов каналов калибровки
    QModelIndexList calibrateIDList = ui->tableView->selectionModel()->selectedRows();
    //Формируем Структуру для запроса калибровки
    foreach (QModelIndex index, calibrateIDList) {
        //Формируем экземпляр дома для калибровки
        calibrationDom->append(&dom->tChannelList[index.row()]);
        TCalibration newCalibration;
        newCalibration.calibrationInfo.insert("position", setting->getPosition());
        newCalibration.calibrationInfo.insert("FIO", setting->getFIO());
        newCalibration.calibrationInfo.insert("metod", "БГРЭС.АСУТП.МК 002-2016");
        QDateTime dt = QDateTime::currentDateTime();
        newCalibration.calibrationInfo.insert("date", dt.date().toString("dd.MM.yyyy"));
        newCalibration.calibrationInfo.insert("time", dt.time().toString());
        newCalibration.calibrationInfo.insert("uuid", QUuid::createUuid().toString());
        newCalibration.conditions = conditions;
        calibrationDom->last()->CalibtationList.push_back(newCalibration);
     }
    qDebug()<< calibrationDom->at(0)->CalibtationList.count();
    //Проверяем возможность совместной калибровки каналов
    if (!this->connectDriver->validationPollList(calibrationDom)) {
        this->logger->log("Несовместимые типы измерительных каналов", Qt::red);
        return;
    }
    //Получаем список поддерживаемы задатчиков
    QList<measurement> measurementTypeList = this->connectDriver->getMeasurementTypes(calibrationDom);
    if (measurementTypeList.count()<=0) {
        this->logger->log("Ошибка: список задатчиков пуст", Qt::red);
        return;
    }
    //Проверяем, поддерживаем ли задатчик необходимый тип
    this->measurementType = this->CPADriver->getMeasurenentType(measurementTypeList);
    if (this->measurementType == notSupport) {
        this->logger->log("Задатчик не поддерживает требуемый тип" , Qt::red);
        return;
    }

    //Получаем список уникальных точек калибровки для всех каналов
    points.clear();

    points = this->connectDriver->getPoints(calibrationDom, this->measurementType);

    if (points.count()<=0) {
        this->logger->log("Нет заданных точек для калибровки", Qt::yellow);
        return;
    }


    //Получаем значения задержек из первого канала
    startDelay = calibrationDom->at(0)->startDelay;
    delay = calibrationDom->at(0)->delay;
    qDebug()<<"Задержка"+QString::number(delay);
    qDebug()<<"количество точек калибровки " + QString::number(points.count());
    ui->progressBar->setMaximum(points.count()*10);
    ui->progressBar->setValue(0);
    currentPoint=0;
    qDebug()<<"инициализация успешно завершена";
    emit this->end_init();
}

//Задание следующей точки калибровки
void MainWindow::sl_set_next_point()
{
    if (currentPoint>=points.count()) {
        QVector<TDevice> deviceList;
        TDevice device;
        device.deviceInfo.insert("Type", CPADriver->getType());
        device.deviceInfo.insert("SN", CPADriver->getSN());
        device.deviceInfo.insert("SKN", CPADriver->getSKN());
        device.deviceInfo.insert("SROK", CPADriver->getSROK());
        device.deviceInfo.insert("role", "general");
        deviceList.push_back(device);
        if (ed->getInReportTemperature()) {
            device.deviceInfo.insert("Type", ed->getTemperatureDeviceType());
            device.deviceInfo.insert("SN", ed->getTemperatureDeviceZN());
            device.deviceInfo.insert("SKN", ed->getTemperatureDeviceSKN());
            device.deviceInfo.insert("SROK", ed->getTemperatureDeviceSROK().toString("dd.MM.yyyy"));
            device.deviceInfo.insert("role", "secondary");
            deviceList.push_back(device);
        }
        if (ed->getInReportHumidity()) {
            device.deviceInfo.insert("Type", ed->getHumidityDeviceType());
            device.deviceInfo.insert("SN", ed->getHumidityDeviceZN());
            device.deviceInfo.insert("SKN", ed->getHumidityDeviceSKN());
            device.deviceInfo.insert("SROK", ed->getHumidityDeviceSROK().toString("dd.MM.yyyy"));
            device.deviceInfo.insert("role", "secondary");
            deviceList.push_back(device);
        }
        if (ed->getInReportPressure()) {
            device.deviceInfo.insert("Type", ed->getPressureDeviceType());
            device.deviceInfo.insert("SN", ed->getPressureDeviceZN());
            device.deviceInfo.insert("SKN", ed->getPressureDeviceSKN());
            device.deviceInfo.insert("SROK", ed->getPressureDeviceSROK().toString("dd.MM.yyyy"));
            device.deviceInfo.insert("role", "secondary");
            deviceList.push_back(device);
        }
        if (ed->getInReportVoltage()) {
            device.deviceInfo.insert("Type", ed->getVoltageDeviceType());
            device.deviceInfo.insert("SN", ed->getVoltageDeviceZN());
            device.deviceInfo.insert("SKN", ed->getVoltageDeviceSKN());
            device.deviceInfo.insert("SROK", ed->getVoltageDeviceSROK().toString("dd.MM.yyyy"));
            device.deviceInfo.insert("role", "secondary");
            deviceList.push_back(device);
        }
        for (int i=0; i<calibrationDom->count();i++) {
            calibrationDom->at(i)->CalibtationList.last().deviceList = deviceList;
        }
        QMediaPlayer * mp3 = new QMediaPlayer();
        mp3->setMedia(QUrl::fromLocalFile("end.mp3"));
        mp3->setVolume(100);
        mp3->play();
        logger->log("Калибровка успешно завершена", Qt::green);
        return;
    }

    //Формируем список подписчиков на точку
    currentCalibrationDom->clear();
    TPoint newPoint;
    for (int i=0; i<calibrationDom->count(); i++) {
        foreach (QString key, calibrationDom->at(i)->tempPoint.keys()) {
            if (points.at(currentPoint)==calibrationDom->at(i)->tempPoint.value(key)) {
                newPoint.pointInfo.insert("value", key.replace(',','.'));
                newPoint.pointInfo.insert("uuid", QUuid::createUuid().toString());
                calibrationDom->at(i)->CalibtationList.last().pointList.append(newPoint);
                currentCalibrationDom->push_back(calibrationDom->at(i));
            }
        }
    }
    qDebug()<<"Подписано: "+QString::number(currentCalibrationDom->count())+" каналов";

    //НОВОЕ создаем группу в устройстве опроса
    connectDriver->createGroup(currentCalibrationDom);

    this->logger->log("Устанавливаем на задатчике точку: "+QString::number(points.at(currentPoint)));
    qDebug()<<startDelay;
    //Задаем точку
    if (CPADriver->setValue(points.at(currentPoint), this->measurementType)) {
        this->logger->log("Точка установлена");
    }
}

//Окончание установки значения
void MainWindow::sl_end_set_value()
{  
    this->measurement1 = 1;
    ui->progressBar->setValue(currentPoint*10+measurement1);
    this->timer->setInterval(startDelay);
    this->timer->start();
}

void MainWindow::timer_overflow()
{
    ui->progressBar->setValue(currentPoint*10+measurement1);
    if (measurement1<=10) {
        logger->log("Получаем значения №"+QString::number(measurement1));
        if (connectDriver->getValues(currentCalibrationDom)) {

        }
        measurement1++;
        this->timer->setInterval(delay);
    }
    else {
        logger->log("Калибровка точки завершена");
        this->timer->stop();

        //НОВОЕ удаляем группу в устройстве опросса
        connectDriver->removeGroup();

        for (int i=0; i<currentCalibrationDom->count(); i++) {
            //**********************************************//
            //          Добавляем результаты                //
            //**********************************************//
            double summa = 0;
            QList<double> p;
            foreach (QString key, currentCalibrationDom->at(i)->CalibtationList.last().pointList.last().results.keys()) {
                p.push_back(currentCalibrationDom->at(i)->CalibtationList.last().pointList.last().results.value(key).toDouble());
                summa += p.last();
            }
            int n=p.count();
            double av = summa/n;
            currentCalibrationDom->at(i)->CalibtationList.last().pointList.last().calculations.insert("av", QString::number(av,'g',12));
            double delta = currentCalibrationDom->at(i)->CalibtationList.last().pointList.last().pointInfo.value("value").toDouble()-av;
            currentCalibrationDom->at(i)->CalibtationList.last().pointList.last().calculations.insert("delta", QString::number(delta,'g',12));
            //Вычисляем неопределенность измерения
            double summa2=0;
            foreach (double p_i, p) {
                summa2 += pow(p_i-av, 2);
            }
            double koef = 1.0/(n*(n-1));
            double neoprIzm = sqrt(koef*summa2);
            currentCalibrationDom->at(i)->CalibtationList.last().pointList.last().calculations.insert("neoprIzm", QString::number(neoprIzm,'g',12));
            //Основная неопределенность прибора
            currentCalibrationDom->at(i)->CalibtationList.last().pointList.last().calculations.insert("neoprEtOsn", QString::number(CPADriver->getIndeterminacyGeneral(points.at(currentPoint),measurementType)));
            //Дополнительная неопределенность прибора
            currentCalibrationDom->at(i)->CalibtationList.last().pointList.last().calculations.insert("neoprEtDop", QString::number(CPADriver->getIndeterminacySecondary(points.at(currentPoint),measurementType, this->conditions)));
        }
        currentPoint++;
        emit this->end_calibration_next_point();
    }
    XMLResultsModel->model_reset();
}

void MainWindow::delayTimer_overflow()
{
    delayTimer->stop();
    this->on_pushButton_2_clicked();
}


void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{
    connectDriver->editDialog(index.row());
}

void MainWindow::on_addButton_clicked()
{
    XMLmodel->insertRow(XMLmodel->rowCount());
    if (!connectDriver->editDialog(XMLmodel->rowCount()-1)) XMLmodel->removeRow(XMLmodel->rowCount()-1);
}

void MainWindow::on_deleteButton_clicked()
{
    QModelIndexList index = ui->tableView->selectionModel()->selectedRows();
    if (!index.count()) return;  

    for (int i=index.count()-1;i>=0; i--)
    {
        XMLmodel->removeRow(index.at(i).row());
    }
}

void MainWindow::on_cloneButton_clicked()
{
    dom->save("11.xml");
}

void MainWindow::on_exitAction_triggered()
{
    close();
}


void MainWindow::on_pushButton_clicked()
{
    measurement m;
    switch (ui->spinBox->value()) {
    case 0: m=mA;
        break;
    case 1: m=mV;
        break;
    case 2: m=V;
        break;
    case 3: m=Om;
        break;
    case 4: m=C100M1_426;
        break;
    case 5: m=C100M1_428;
        break;
    case 6: m=C50M1_426;
        break;
    case 7: m=C50M1_428;
        break;
    case 8: m=C50P;
        break;
    case 9: m=C100P;
        break;
    case 10: m=CPt100IEC385;
        break;
    case 11: m=CTypeJ;
        break;
    case 12: m=CTypeK;
        break;
    case 13: m=CTypeB;
        break;
    case 14: m=CTypeA1;
        break;
    case 15: m=CTypeS;
        break;
    case 16: m=CTypeXK;
        break;
    default:
        m=mA;
        break;
    }
    //CPADriver->setup();
    CPADriver->setValue(ui->doubleSpinBox->value(), m);

 //   CPADriver->test();

}

//Изменение пункта меню
void MainWindow::on_changeCPA(bool checked)
{
    if (!checked) return;
    QAction * getAction = qobject_cast<QAction *>(QObject::sender());
    qDebug()<<getAction->property("fileName").toString();
    currentCPADriver=getAction->property("fileName").toString();
    changeCPADriver(currentCPADriver);
}

void MainWindow::on_action_triggered()
{
   //qDebug()<<connectDriver->thread();
    qDebug()<<CPADriver->thread();
    CPADriver->setup();
}

void MainWindow::on_environmentAction_triggered()
{   
    ed->exec();
}

void MainWindow::on_saveAction_triggered()
{
    //QString fileName = QFileDialog::getSaveFileName();
    this->dom->save(fileName);
}

//кнопка для тестов
void MainWindow::on_pushButton_3_clicked()
{
    //карта данных для шаблона
    QVariantHash map;

    int index_ = ui->tableView->selectionModel()->currentIndex().row();
    map["channalInfo"] = dom->tChannelList.at(index_).channelInfo;
    map["conditions"] = dom->tChannelList.at(index_).CalibtationList.last().conditions;

    QVariantList deviceList;
    QVariantHash deviceMeasurment;
    for (int i=0;i<dom->tChannelList.at(index_).CalibtationList.last().deviceList.count();i++) {
        foreach (QString key, dom->tChannelList.at(index_).CalibtationList.last().deviceList.at(i).deviceInfo.keys()) {
            deviceMeasurment[key] = dom->tChannelList.at(index_).CalibtationList.last().deviceList.at(i).deviceInfo.value(key);
        }
        deviceList<<deviceMeasurment;
    }
    map["devices"] = deviceList;

    QVariantHash calibrationInfo;
    foreach (QString key, dom->tChannelList.at(index_).CalibtationList.last().calibrationInfo.keys()) {
        calibrationInfo[key] = dom->tChannelList.at(index_).CalibtationList.last().calibrationInfo.value(key);
    }
    map["calibrationInfo"] = calibrationInfo;


    QVariantList pointList;
    QVariantHash pointInfo;
    QVariantHash results;
    for (int i=0;i<dom->tChannelList.at(index_).CalibtationList.last().pointList.count();i++) {
        foreach (QString key, dom->tChannelList.at(index_).CalibtationList.last().pointList.at(i).pointInfo.keys()) {
            pointInfo[key] = dom->tChannelList.at(index_).CalibtationList.last().pointList.at(i).pointInfo.value(key);
        }
        pointList<<deviceMeasurment;
    }
    map["devices"] = deviceList;



//    QVariantList calibration;
//    calibration["info"] = dom->tChannelList.at(index_).CalibtationList.last().calibrationInfo;
//    calibration["conditions"] = dom->tChannelList.at(index_).CalibtationList.last().conditions;
//    for (int i=0; i<dom->tChannelList.at(index_).CalibtationList.count();i++)
//    {
//        QVariantList pointList;
//        QVariantHash pointInfo = dom->tChannelList.at(index_).CalibtationList.at(i).pointList.at(j).pointInfo;
//    }

    QTextDocument doc;
    QFile f1("temp.html");
    f1.open(QIODevice::ReadOnly);

    QTextStream stream(&f1);

    QString renderTemplate = stream.readAll();

    Mustache::Renderer renderer;

    Mustache::QtVariantContext context(map);

    QString str = renderer.render(renderTemplate, &context);

    qDebug()<<str;
    doc.setHtml(str);

    QPrinter printer;
    printer.setOutputFileName("temp.pdf");
    printer.setOutputFormat(QPrinter::PdfFormat);
    doc.print(&printer);
    printer.newPage();
}

void MainWindow::on_saveAsAction_triggered()
{
    QString fileName = QFileDialog::getSaveFileName();
    this->dom->save(fileName);
    this->fileName = fileName;
}

void MainWindow::on_deleteAll_triggered()
{
    if (dom->tChannelList.count()<=0) return;
    for (int i=0;i<dom->tChannelList.count();i++) {
        dom->tChannelList[i].CalibtationList.clear();
    }
}

void MainWindow::on_pushButton_4_clicked()
{
    QTextDocument doc;
//    QFile tempFile;
//    QString str;
//    QRegExp regExp("([{]{2,2})(.*)([}]{2,2})");
//    regExp.setMinimal(true);
//    tempFile.setFileName("trei5b-02_1.txt");
//    if (tempFile.open(QFile::ReadOnly)) {
//        QTextStream textStream(&tempFile);
//        qDebug()<<"файл открыт";

//        str =textStream.readAll();
//    }
//    int pos=0;

//    tempFile.close();
//    QStringList list;
//    while ((pos=regExp.indexIn(str, pos)) !=-1) {
//        list<<regExp.cap(0);
//        pos+=regExp.matchedLength();
//        qDebug()<<list.last();
//    }
//    qDebug()<<list.count();
    int index_ = ui->tableView->selectionModel()->currentIndex().row();
    QString str;
    str.append("<h1 align=\"center\">Протокол калибровки</h1>");
    //str.append(QChar(581));
    qDebug()<<index_;
    str.append("<h2>№1 от 05.04.2016</h2>");
    str.append("<h2 align=\"center\">Измерительного канала:</h2>");
    str.append("<h2>"); str.append(dom->tChannelList.at(index_).channelInfo.value("cipher").toString()+"   "+dom->tChannelList.at(index_).channelInfo.value("name").toString());str.append("</h2>");
    str.append("<h2>Контроллер "); str.append(dom->tChannelList.at(index_).channelInfo.value("controller").toString()+" Модуль "+dom->tChannelList.at(index_).channelInfo.value("module").toString()+ " Канал "+ dom->tChannelList.at(index_).channelInfo.value("pipe").toString());str.append("</h2>");
    str.append("<h2>Методика калибровки: "); str.append(dom->tChannelList.at(index_).CalibtationList.last().calibrationInfo.value("metod").toString());str.append("</h2>");
    str.append("<h2>Условия проведения калибровки: </h2>");
    //Формируем таблицу условий проведения калибровки
    str.append("<table border=\"1\" style = \"border-style:solid; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;\" cellspacing=\"0\" cellpadding=\"0\" width =\"100%\"><tr style = \"margin-top:2px; margin-bottom:2px; margin-left:2px; margin-right:2px;\">");
    str.append("<td align=\"center\">Температура, "); str.append(QChar(8451)); str.append("</td>");
    str.append("<td align=\"center\">Влажность, %</td>");
    str.append("<td align=\"center\">Давление, кПа</td>");
    str.append("<td align=\"center\">Напряжение питания, В</td>");
    str.append("</tr>");
    str.append("<tr>");
    str.append("<td align=\"center\">"+dom->tChannelList.at(index_).CalibtationList.last().conditions.value("temperature").toString()+"</td>");
    str.append("<td align=\"center\">"+dom->tChannelList.at(index_).CalibtationList.last().conditions.value("humidity").toString()+"</td>");
    str.append("<td align=\"center\">"+dom->tChannelList.at(index_).CalibtationList.last().conditions.value("pressure").toString()+"</td>");
    str.append("<td align=\"center\">"+dom->tChannelList.at(index_).CalibtationList.last().conditions.value("voltage").toString()+"</td>");
    str.append("</tr></table>");

    str.append("<h2>Образцовые и вспомогательные СИ, применяемые при проведении калибровки: </h2>");
    str.append("<table border=\"1\" style = \"border-style:solid; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;\" cellspacing=\"0\" cellpadding=\"0\" width =\"100%\">");
    str.append("<tr>");
    str.append("<td align=\"center\">Средство измерения</td>");
    str.append("<td align=\"center\">Заводской №</td>");
    str.append("<td align=\"center\">Свидетельство №</td>");
    str.append("<td align=\"center\">Действительно до</td>");
    str.append("</tr>");
    for (int i=0;i<dom->tChannelList.at(index_).CalibtationList.last().deviceList.count(); i++)
    {
        str.append("<tr>");
        str.append("<td>"+dom->tChannelList.at(index_).CalibtationList.last().deviceList.at(i).deviceInfo.value("Type").toString()+"</td>");
        str.append("<td>"+dom->tChannelList.at(index_).CalibtationList.last().deviceList.at(i).deviceInfo.value("SN").toString()+"</td>");
        str.append("<td>"+dom->tChannelList.at(index_).CalibtationList.last().deviceList.at(i).deviceInfo.value("SKN").toString()+"</td>");
        str.append("<td>"+dom->tChannelList.at(index_).CalibtationList.last().deviceList.at(i).deviceInfo.value("SROK").toString()+"</td>");
        str.append("</tr>");
    }
    str.append("</table>");

    str.append("<h2>Результаты измерений:</h2>");
    str.append("<table border=\"1\" style = \"border-style:solid; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;\" cellspacing=\"0\" cellpadding=\"0\" width =\"100%\">");
    str.append("<tr>");
    str.append("<td>Внешний осмотр</td>");
    str.append("<td>Годен</td>");
    str.append("</tr>");
    str.append("<tr>");
    str.append("<td>Проверка функционирования</td>");
    str.append("<td>Годен</td>");
    str.append("</tr>");
    str.append("<tr>");
    str.append("<td>Определение метрологических характеристик</td>");
    str.append("<td>см. последующие страницы протокола</td>");
    str.append("</tr>");
    str.append("<tr>");
    str.append("<td>Результаты измерений</td>");
    str.append("<td>см. последующие страницы протокола</td>");
    str.append("</tr>");
    str.append("</table>");

    str.append("<h2/>");

    str.append("<table border=\"1\" style = \"border-style:solid; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;\" cellspacing=\"0\" cellpadding=\"0\" width =\"100%\">");
    str.append("<tr>");
    str.append("<td>Калибровку выполнил</td>");
    str.append("<td>"+dom->tChannelList.at(index_).CalibtationList.last().calibrationInfo.value("position").toString()+ " "+dom->tChannelList.at(index_).CalibtationList.last().calibrationInfo.value("FIO").toString()+"</td>");
    str.append("</tr>");
    str.append("<tr>");
    str.append("<td>Подпись</td>");
    str.append("<td></td>");
    str.append("</tr>");
    str.append("<tr>");
    str.append("<td>Дата</td>");
    str.append("<td>"+QDateTime::currentDateTime().toString() +"</td>");
    str.append("</tr>");
    str.append("</table>");

    str.append("<h2>Результаты измерений:</h2>");
    str.append("<table border=\"1\" style = \"border-style:solid; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;\" cellspacing=\"0\" cellpadding=\"0\" width =\"100%\">");

    int pointCount = dom->tChannelList.at(index_).CalibtationList.last().pointList.count();
    QString rowCount = QString::number(pointCount+1);

    str.append("<tr>");
    str.append("<td align=\"center\">Исследуемая точка, ");str.append(QChar(8451));str.append("</td>");
    for (int i=0;i<pointCount;i++)
    {
        str.append("<td align=\"center\">"+dom->tChannelList.at(index_).CalibtationList.last().pointList.at(i).pointInfo.value("value").toString()+"</td>");
    }
    str.append("</tr>");
    str.append("<tr>");
    str.append("<td  align=\"center\" colspan=\""+rowCount+"\">Значения результатов калибровки, "+dom->tChannelList.at(index_).channelInfo.value("unit").toString()+"</tr>");

    for (int i=1;i<=10;i++)
    {
        str.append("<tr>");str.append("<td>Значение "+QString::number(i));
        for (int j=0;j<pointCount;j++)
        {
            str.append("<td align=\"center\">"+dom->tChannelList.at(index_).CalibtationList.last().pointList.at(j).results.value("V"+QString::number(i)).toString()+"</td>");
        }
        str.append("</tr>");
    }

    str.append("<tr>");
    str.append("<td  align=\"center\">Среднее значение, "+dom->tChannelList.at(index_).channelInfo.value("unit").toString()+"</td>");
    for (int i=0;i<pointCount;i++)
    {
        str.append("<td align=\"center\">"+dom->tChannelList.at(index_).CalibtationList.last().pointList.at(i).calculations.value("av").toString()+"</td>");
    }
    str.append("</tr>");

    str.append("<tr>");
    str.append("<td  align=\"center\">Стандартная неопределенность, "+dom->tChannelList.at(index_).channelInfo.value("unit").toString()+"</td>");
    for (int i=0;i<pointCount;i++)
    {
        str.append("<td align=\"center\">"+dom->tChannelList.at(index_).CalibtationList.last().pointList.at(i).calculations.value("neoprIzm").toString()+"</td>");
    }
    str.append("</tr>");

    str.append("</table>");

    str.append("<h1/>");

    str.append("<table border=\"1\" style = \"border-style:solid; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;\" cellspacing=\"0\" cellpadding=\"0\" width =\"100%\">");
    str.append("<tr>");
    str.append("<td colspan=\"3\" align=\"center\">Входные величины</td>");
    str.append("<td align=\"center\">Тип неопределенности</td>");
    str.append("<td align=\"center\">Вид распределения</td>");
    str.append("<td colspan=\""+QString::number(pointCount)+"\" align=\"center\">Оцениваемое значение</td>");
    str.append("</tr>");
    str.append("<tr>");
    str.append("<td align=\"center\">I<sub>s</sub></td>");
    str.append("<td align=\"center\">мА</td>");
    str.append("<td align=\"center\">Показания эталона</td>");
    str.append("<td></td>");
    str.append("<td></td>");
    str.append("<td></td>");
    str.append("<td></td>");
    str.append("</tr>");
    str.append("<tr>");
    str.append("<td align=\"center\">P<sub>ind</sub></td>");
    str.append("<td align=\"center\">МПа</td>");
    str.append("<td align=\"center\">Показания ИК</td>");
    str.append("<td>A</td>");
    str.append("<td>нормальное</td>");
    str.append("<td></td>");
    str.append("<td></td>");
    str.append("</tr>");

    str.append("</table>");
    doc.setHtml(str);
    QPrinter printer;
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName("test1.pdf");
    doc.print(&printer);
    //printer.newPage();
}

void MainWindow::on_pushButton_5_clicked()
{
    delayTimer->setInterval(40000);
    delayTimer->start();
}

void MainWindow::on_pushButton_7_clicked()
{
    //Удаление текущей калибровки
    int calibrationCount = dom->tChannelList[dom->getCurrentChannel()].CalibtationList.count();
    if (calibrationCount<=0) return;
    dom->tChannelList[dom->getCurrentChannel()].CalibtationList.remove(calibrationCount-1);
    XMLResultsModel->model_reset();
}

//вызов генератора отчетов
void MainWindow::on_reportDialog_triggered()
{
    QPluginLoader * pl = new QPluginLoader(this);
    pl->setFileName("ReportCreator.dll");
    QObject * tempObject = pl->instance();
    if (tempObject) {
        ReportCreatorInterface * rc = qobject_cast<ReportCreatorInterface*>(tempObject);
        if (rc) {
            rc->initialization();
            rc->getReportEditor();
        }
    //pl->unload();

    }
//    reportdialog * rd = new reportdialog(this);
//    rd->exec();
//    rd->deleteLater();
}
