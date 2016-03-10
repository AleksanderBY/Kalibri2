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
    //Инициализируем список каналов выбранных для калибровки
    pollList = new QList<PollClass*>;
    currentPollList = new QList<PollClass*>;


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

    //Создание подключения к базе данных сигналов
    //this->sgDb = QSqlDatabase::addDatabase("QSQLITE");

    //Создаем диалоги
    //Диалог настройки программы
    setting = new SettingsDialog();
    setting->setModal(true);
    //Диалог создания базы данных
    createDlg = new createDialog();
    createDlg->setModal(true);



    dom = new ADomCalibration();
    XMLmodel = new AXMLCalibrationModel(dom);
    ui->tableView->setModel(XMLmodel);
    //connect(ui->tableView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
    XMLResultsModel = new AXMLCalibrationResultModel(dom);
    ui->calibrateView->setModel(XMLResultsModel);
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
    connect(timer, SIGNAL(timeout()), this, SLOT(poll()));

    sgList = new QMap<QString, float>;

    //Создаем список драйверов доступа к данным в формате -файл драйвера, имя драйвера-
    connectDrivers = new QMap<QString, QString>;
    //Каталог драйверов доступа к базе данных
    this->connectDriverDir = new QDir(qApp->applicationDirPath());
    connectDriverDir->cd("ConnectDrivers");
    //ищем драйвера доступа к данным
    findConnectPlugin();

    connectPluginLoader = new QPluginLoader();

//    //Создаем поток для калибротора
//    calibratorThread = new QThread();

//    myCalibrator = new Calibrator();
//    myCalibrator->moveToThread(calibratorThread);
//    calibratorThread->start();

    //Связываем сигналы калибратора с главным процессом
    connect(this, SIGNAL(end_init()),SLOT(sl_set_next_point()));
    connect(this->timer, SIGNAL(timeout()), this, SLOT(timer_overflow()));
    connect(this, SIGNAL(end_calibration_next_point()), SLOT(sl_set_next_point()));
    connect(this, SIGNAL(error_calibrations(QString)), SLOT(sl_error_calibration(QString)));
    //connect(myCalibrator, SIGNAL(start_calibration()), this, SLOT(sl_start_calibration()));
    //connect(myCalibrator, SIGNAL(set_next_point(double)), this, SLOT(sl_set_next_point(double)));
    //connect(this, SIGNAL(set_next_point_complete(bool)), myCalibrator, SLOT(on_set_next_point_complete(bool)));
    //Создаем диалог условий калибровки
    this->ed = new EnvironmentDialog(this->settings);
    ed->setModal(true);

//    qDebug()<<"Создание основного окна";
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::settingDialog()
{
    //setting->exec();

//    QDir appDir(qApp->applicationDirPath());
//    appDir.cdUp(); appDir.cdUp();
//    appDir.cd("trei5b/debug");
//    qDebug()<< appDir.absolutePath();

//    QString FileName = "trei5b.dll";
//    QPluginLoader oprosDriverLoader(appDir.absoluteFilePath(FileName));
//    QObject *oprosDriver = oprosDriverLoader.instance();

//    DataInterface *data = qobject_cast<DataInterface *>(oprosDriver);

//    data->initialization(sgModel);
//    int sock = data->createDevice("k2_tzdu_1");

//    qDebug()<<data->getLastError();

//    qDebug()<<sock;



//    qDebug()<<oprosDriver;

//    sgList->insert("1B80",0);
//    sgList->insert("1B82",0);
//    data->getValues(sock, sgList);
//    foreach (QString key, sgList->keys()) {
//         qDebug()<<sgList->value(key);
//    }


    //foreach (QString fileName, appDir.entryList(QDir::Files)) {
        //QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        //QObject *plugin = pluginLoader.instance();
        //qDebug()<<fileName;
        //if (plugin) {
        //    echoInterface = qobject_cast<EchoInterface *>(plugin);
        //    if (echoInterface)
        //        return true;
        //}
    //}

//    qDebug()<<setting->getData();
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
            dom->addSetting("field"+QString::number(i), fieldsDB->at(i-1));
        }
        dom->save("");
    }

//        else {
//            //создаем базу данных
//            DBFile.open(QIODevice::WriteOnly);
//            DBFile.close();
//            qDebug()<<"Файл базы данных создан";
            //Открываем базу
            //this->sgDbName = createDlg->getNameDB()+".db3";
            //this->sgDb.setDatabaseName(this->sgDbName);
            //if (this->sgDb.open()){
                //qDebug()<<"База данных открыта";
                //qDebug()<<"Создаем структуру данных";


//                //Загружаем выбранный драйвер в переменную --connectDriver--
//                loadConnectPlugin(this->connectDrivers->keys().at(createDlg->getIndexDriver()));

//                //Создаем в базе таблицу настроек
//                //QSqlQuery setQuery("CREATE TABLE settings (id STRING(10) PRIMARY KEY, value STRING(20));", this->sgDb);

//                QSqlError err;
//                err = setQuery.lastError();
//                setQuery.prepare("INSERT INTO settings VALUES (?, ?);");
//                setQuery.addBindValue("driver");
//                setQuery.addBindValue(this->connectDrivers->keys().at(createDlg->getIndexDriver()));
//                setQuery.exec();

//                QMap<QString, QString> *fieldList = new QMap<QString, QString>;
//                this->connectDriver->getFieldsDB(fieldList);


//                //Формируем строку запроса на создание базы данных
//                QString queryText = "CREATE TABLE sg (id INTEGER PRIMARY KEY AUTOINCREMENT";
//                QMap<QString, QString>::const_iterator i;
//                for (i = fieldList->constBegin(); i!=fieldList->constEnd(); ++i)
//                {
//                    QString key = i.key();
//                    key.remove(0,3);
//                   queryText+=", "+key+" "+i.value();
//                }
//                queryText+=")";
//                //qDebug()<<queryText;

//                //QSqlQuery sgQuery(sgDb);
//                //sgQuery.prepare(queryText);
//                //sgQuery.exec();
//                //Создаем таблицу хранения данных калибровки
//                queryText = "CREATE TABLE calibrate (id INTEGER PRIMARY KEY AUTOINCREMENT,"
//                            "sg_id INTEGER,"
//                            "point DOUBLE,"
//                            "c1 DOUBLE,"
//                            "c2 DOUBLE,"
//                            "c3 DOUBLE,"
//                            "c4 DOUBLE,"
//                            "c5 DOUBLE,"
//                            "c6 DOUBLE,"
//                            "c7 DOUBLE,"
//                            "c8 DOUBLE,"
//                            "c9 DOUBLE,"
//                            "c10 DOUBLE,"
//                            "date DATE,"
//                            "time TIME,"
//                            "pressure STRING,"
//                            "humidity STRING,"
//                            "temperature STRING,"
//                            "tension STRING,"
//                            "etalon  STRING)";
//                sgQuery.prepare(queryText);
//                sgQuery.exec();

//                sgModel->setTable("sg");
//                sgModel->select();
//                ui->tableView->setModel(proxySgModel);
//                ui->tableView->resizeColumnsToContents();
//                setStart(true);


//            }

//        }

//    }
//    else{
//        qDebug()<<"Нажата кнопка CANCEL";
//    }



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

    //Получаем имя драйвера доступа к данным
    currentConnectDriver = dom->getDriverFile();
    //Загружаем драйвер доступа к данным
    loadConnectPlugin(currentConnectDriver);

    setStart(true);
//    //qDebug()<<"Открытие базы данных";
//    QString str = QFileDialog::getOpenFileName(0, "Open Dialog", "ConnectDrivers", "*.db3");
//    qDebug()<<str;
//    if (str=="") return;
//    sgDb.setDatabaseName(str);
//    if (sgDb.open()) {
//        QSqlQuery setQuery("SELECT value FROM settings WHERE id='driver'");
//        setQuery.exec();
//        setQuery.first();
//        QString fileName = setQuery.value(0).toString();
//        loadConnectPlugin(fileName);
//        //qDebug()<<this->connectDriver->getName();
//        QStringList * supportTypesList = connectDriver->supportTypes();
//        qDebug()<<supportTypesList->at(0);
//        sgModel->setTable("sg");
//        sgModel->select();
//        ui->tableView->setModel(XMLmodel);

//        resModel->setTable("calibrate");
//        resModel->select();
//        ui->calibrateView->setModel(resModel);

//        sgModel->setHeaderData(1, Qt::Horizontal, "Контроллер");
//        setStart(true);

//        ui->tableView->resizeColumnsToContents();
//        //connectDriver->editDialog(sgModel);
//    }
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
            CPAInterface * del = CPADriver;
            CPADriver = 0;
            //delete del;
            CPAPluginLoader->unload();
        }
        CPAPluginLoader->setFileName(CPADriverDir->absoluteFilePath(fileName));
        qDebug()<<"Загрузка нового драйвера";
        CPADriver = qobject_cast<CPAInterface * >(CPAPluginLoader->instance());
        connect(CPADriver, SIGNAL(log(QString,Qt::GlobalColor)), logger, SLOT(on_log(QString,Qt::GlobalColor)));
        CPADriver->initialization(settings);
        settings->setValue("kalibri2/currentCPADriver", fileName);
        return true;
    }
    return false;
}

void MainWindow::poll()
{
    step++;
}



//-------------------------------------------------------------------------------
//------------------------Старт калибровки---------------------------------------
//-------------------------------------------------------------------------------
void MainWindow::on_pushButton_2_clicked()
{
    //Определяем список индексов каналов калибровки
    QModelIndexList calibrateIDList = ui->tableView->selectionModel()->selectedRows();
    //Очищаем список каналов от данных преведущей калибровки
    // qDeleteAll(pollList);
    pollList->clear();
    foreach (QModelIndex index, calibrateIDList) {
        PollClass * poll = new PollClass();
        QHash<QString, QString> attr = dom->getChannel(index.row())->getChannelData();
        attr.insert("num", QString::number(index.row()));
        poll->attr = attr;
        pollList->push_back(poll);
     }
    points = this->connectDriver->getPoints(pollList);
    if (points.count()<0) {
        this->logger->log("Нет заданных точек для калибровки", Qt::yellow);
        return;
    }
    currentPoint=0;

    emit this->end_init();
}

//Задание следующей точки калибровки
void MainWindow::sl_set_next_point()
{
    if (currentPoint>=points.count()) {
        logger->log("Калибровка успешно завершена", Qt::green);
        return;
    }
    //Формируем список подписчиков на точку
    currentPollList->clear();
    for (int i=0;i<pollList->count(); i++) {
        foreach (QString key, pollList->at(i)->points.keys()) {
            if (points.at(currentPoint)==pollList->at(i)->points.value(key)) {
                currentPollList->push_back(pollList->at(i));
            }

        }
    }
    qDebug()<<"Подписано: "+QString::number(currentPollList->count())+" каналов";

    //Создаем поля для результатов калибровки в доме
    calibrationChannel.clear();
    resultCalibrationList.clear();
    for (int i=0; i<currentPollList->count();i++){
        int tempChannel = currentPollList->at(i)->attr.value("num").toInt();

        calibrationChannel.insert(tempChannel, dom->getChannel(tempChannel));

        AResultCalibration *result = new AResultCalibration();

        //result->addOther("point", QString::number(currentPollList->at(i)->points.at(i)));
        foreach (QString key, currentPollList->at(i)->points.keys()) {
            if (points.at(currentPoint)==currentPollList->at(i)->points.value(key)) {
                result->addOther("point", key);
                break;
            }
        }

        QDateTime dt = QDateTime::currentDateTime();

        result->addOther("date", dt.date().toString("dd.MM.yyyy"));
        result->addOther("time", dt.time().toString());

        dom->getChannel(tempChannel)->addResultCalibration(result);

        resultCalibrationList.insert(tempChannel, result);
        XMLResultsModel->insertRow(dom->getChannel(dom->getCurrentChannel())->getResultsCount());
    }

    this->logger->log("Устанавливаем на задатчике точку: "+QString::number(points.at(currentPoint)));
    //Задаем точку
    if (CPADriver->setValue(points.at(currentPoint), mA)) {
        this->logger->log("Точка установлена");
        this->measurement1 = 1;
        this->timer->setInterval(1000);
        this->timer->start();
    }
}

void MainWindow::timer_overflow()
{
    if (measurement1<=10) {
        logger->log("Получаем значения №"+QString::number(measurement1));
        if (connectDriver->getValues(currentPollList)) {
            for (int i=0; i<currentPollList->count(); i++) {
                int tempChannel = currentPollList->at(i)->attr.value("num").toInt();
                resultCalibrationList.value(tempChannel)->addResult("V"+QString::number(measurement1), QString::number(currentPollList->at(i)->value));
                qDebug()<<currentPollList->at(i)->value;
            }

        }
        measurement1++;
    }
    else {
        logger->log("Калибровка точки завершена");
        this->timer->stop();
        //Оформляем расчетные данные и др. параметры
        for (int i=0; i<resultCalibrationList.count();i++) {
            int tempChannel = currentPollList->at(i)->attr.value("num").toInt();
            AResultCalibration *result = resultCalibrationList.value(tempChannel);
            //Расчитываем среднее значение
            double summa = 0;
            for (int j=1; j<=10; j++) {
                summa = summa+result->getRes("V"+QString::number(j)).toDouble();
            }
            double av = summa/10;
            result->addCalulation("av", QString::number(av, 'g', 12));
            //Расчитываем отклонение среднего значения
            double point = result->getOther("point").toDouble();
            double delta = point - av;
            //qDebug()<<delta;
            result->addCalulation("delta", QString::number(delta, 'g', 12));
            //Расчитываем стандартную неопределенность измерения по типу А
            summa = 0;
            for (int i=1; i<=10; i++) {
                summa = summa + pow(av-result->getRes("V"+QString::number(i)).toDouble(), 2);
            }
            summa = summa/90;
            summa = sqrt(summa);
            result->addCalulation("neoprIzm", QString::number(summa, 'g', 12));
            //Добавляем условия калибровки
            result->addCondition("temperature", this->ed->getTemperature());
            result->addCondition("pressure", this->ed->getPressure());
            result->addCondition("humidity", this->ed->getHumidity());
            result->addCondition("voltage", this->ed->getVoltage());
            //Добавляем средства измерения
            AMeasuringDevice device;
            device.addParam("Type", CPADriver->getType());
            device.addParam("SN", CPADriver->getSN());
            device.addParam("SKN", CPADriver->getSKN());
            device.addParam("SROK", CPADriver->getSROK());
            device.addParam("role", "general");
            result->addDevice(device);

            AMeasuringDevice device1;
            device1.addParam("Type", ed->getTemperatureDeviceType());
            device1.addParam("SN", ed->getTemperatureDeviceZN());
            device1.addParam("SKN", ed->getTemperatureDeviceSKN());
            device1.addParam("SROK", ed->getTemperatureDeviceSROK().toString("dd.MM.yyyy"));
            device1.addParam("role", "secondary");
            result->addDevice(device1);
            AMeasuringDevice device2;
            device2.addParam("Type", ed->getHumidityDeviceType());
            device2.addParam("SN", ed->getHumidityDeviceZN());
            device2.addParam("SKN", ed->getHumidityDeviceSKN());
            device2.addParam("SROK", ed->getHumidityDeviceSROK().toString("dd.MM.yyyy"));
            device2.addParam("role", "secondary");
            result->addDevice(device2);
            AMeasuringDevice device3;
            device3.addParam("Type", ed->getPressureDeviceType());
            device3.addParam("SN", ed->getPressureDeviceZN());
            device3.addParam("SKN", ed->getPressureDeviceSKN());
            device3.addParam("SROK", ed->getPressureDeviceSROK().toString("dd.MM.yyyy"));
            device3.addParam("role", "secondary");
            result->addDevice(device3);
            AMeasuringDevice device4;
            device4.addParam("Type", ed->getPressureDeviceType());
            device4.addParam("SN", ed->getPressureDeviceZN());
            device4.addParam("SKN", ed->getPressureDeviceSKN());
            device4.addParam("SROK", ed->getPressureDeviceSROK().toString("dd.MM.yyyy"));
            device4.addParam("role", "secondary");
            result->addDevice(device4);
            AMeasuringDevice device5;
            device5.addParam("Type", ed->getVoltageDeviceType());
            device5.addParam("SN", ed->getVoltageDeviceZN());
            device5.addParam("SKN", ed->getVoltageDeviceSKN());
            device5.addParam("SROK", ed->getVoltageDeviceSROK().toString("dd.MM.yyyy"));
            device5.addParam("role", "secondary");
            result->addDevice(device5);
        }




        currentPoint++;
        emit this->end_calibration_next_point();
    }
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
//    measurement m;
//    switch (ui->spinBox->value()) {
//    case 0: m=mA;
//        break;
//    case 1: m=mV;
//        break;
//    case 2: m=V;
//        break;
//    case 3: m=Om;
//        break;
//    case 4: m=C100M1_426;
//        break;
//    case 5: m=C100M1_428;
//        break;
//    case 6: m=C50M1_426;
//        break;
//    case 7: m=C50M1_428;
//        break;
//    case 8: m=C50P;
//        break;
//    case 9: m=C100P;
//        break;
//    case 10: m=CPt100IEC385;
//        break;
//    case 11: m=CTypeJ;
//        break;
//    case 12: m=CTypeK;
//        break;
//    case 13: m=CTypeB;
//        break;
//    case 14: m=CTypeA1;
//        break;
//    case 15: m=CTypeS;
//        break;
//    case 16: m=CTypeXK;
//        break;
//    default:
//        m=mA;
//        break;
//    }
//    //CPADriver->setup();
//    qDebug()<<CPADriver->thread();
//    CPADriver->setValue(ui->doubleSpinBox->value(), m);

    CPADriver->test();

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
    QString fileName = QFileDialog::getSaveFileName();
    this->dom->save(fileName);
}
