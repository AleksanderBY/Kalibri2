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
    CPADriverThread = new QThread(this);
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

    //Создаем поток для калибротора
    calibratorThread = new QThread();

    myCalibrator = new Calibrator();
    myCalibrator->moveToThread(calibratorThread);
    calibratorThread->start();

    //Связываем сигналы калибратора с главным процессом
    connect(myCalibrator, SIGNAL(start_calibration()), this, SLOT(sl_start_calibration()));
    connect(myCalibrator, SIGNAL(set_next_point(double)), this, SLOT(sl_set_next_point(double)));
    connect(this, SIGNAL(set_next_point_complete(bool)), myCalibrator, SLOT(on_set_next_point_complete(bool)));



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
        //dom->clear();
//        currentConnectDriver = this->connectDrivers->keys().at(createDlg->getIndexDriver());
//        loadConnectPlugin(currentConnectDriver);
        //dom->addSetting("driver", currentConnectDriver);
        QVector<QString> *fieldsDB = new QVector<QString>;
        connectDriver->getFieldsDB(fieldsDB);
        //dom->addSetting("fieldscount", QString::number(fieldsDB.count()));
//        int i=1;
//        foreach (QString field, fieldsDB) {
//            dom->addSetting("field"+QString::number(i), field);
//            i++;
//        }
//        dom->save("");
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
        qDebug()<<CPADriver->thread();
        CPADriver->moveToThread(CPADriverThread);
        settings->setValue("kalibri2/currentCPADriver", fileName);
        CPADriverThread->start();
        return true;
    }
    return false;
}

void MainWindow::poll()
{
    step++;
}

void MainWindow::sl_start_calibration()
{
     logger->log("Старт калибровки");
}

void MainWindow::sl_set_next_point(double point)
{
    logger->log("Установка значения на эталоне: " + QString::number(point));
    QMessageBox * mBox = new QMessageBox("Задание новой точки", "Задайте на входе канала значение "+QString::number(point),QMessageBox::Information,
                                         QMessageBox::Yes,
                                         QMessageBox::No,
                                         QMessageBox::Cancel | QMessageBox::Escape);
    if (mBox->exec() == QMessageBox::Yes) emit set_next_point_complete(true); else emit set_next_point_complete(false);
}

void MainWindow::on_pushButton_2_clicked()
{
    //Определяем список индексов каналов калибровки
    QModelIndexList calibrateIDList = ui->tableView->selectionModel()->selectedRows();
    //
    QList<PollClass*> * pollList = new QList<PollClass*>;
    foreach (QModelIndex index, calibrateIDList) {
        PollClass * poll = new PollClass();
        QHash<QString, QString> attr = dom->getChannel(index.row())->getChannelData();
        poll->attr = attr;
        pollList->push_back(poll);
     }
    QList<double> pointList = this->connectDriver->getPoints(pollList);

    for (int i=0; i<pointList.count(); i++)
    {
        qDebug()<<pointList.at(i);
    }

    //this->connectDriver->getValues(pollList);
    //for (int i=0; i<pollList->count();i++) {
//        qDebug()<<QString::number(pollList->at(i)->value, 'g' ,12);
//    }

//    AChannelCalibration * channel;
//    QList<QHash<QString, QString> > * list = new  QList<QHash<QString, QString> >;
//    foreach (QModelIndex index, calibrateIDList) {
//        channel = dom->getChannel(index.row());
//        list->push_back(channel->getChannelData());
//    }
//    qDebug()<<list->count();
//    //выходим если нет выбранных каналов
//    if (calibrateIDList.count()<0) return;
//    //Очищаем и заполняем поля калибратора новыми каналами
//    myCalibrator->clearCalibrationRecordList();
//    for (int i=0; i<calibrateIDList.count(); i++)
//    {
//        myCalibrator->addCalibrationRecord(sgModel->record(calibrateIDList.at(i).row()));
//    }
//    //Стартуем калибратор
//    myCalibrator->start(connectDriver);

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

void MainWindow::timer_overflow()
{

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
    qDebug()<<CPADriver->thread();
    CPADriver->setValue(ui->doubleSpinBox->value(), m);


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
    EnvironmentDialog ed(this->settings);
    ed.setModal(true);
    ed.exec();
}

void MainWindow::on_saveAction_triggered()
{
    this->dom->save("sdf");
}
