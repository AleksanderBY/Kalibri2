#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "settingsdialog.h"
#include "createdialog.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlTableModel>
#include <QSortFilterProxyModel>
#include <QDir>
#include <QPluginLoader>
#include <QMap>
#include <QFileInfo>
#include <QTimer>
#include <QThread>
#include <QAction>
#include <QActionGroup>
#include "calibrator.h"
#include "axmlcalibrationmodel.h"
#include "aloggermodel.h"
#include "cpainterface.h"

#include "../trei5b/dataineterface.h"

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


public slots:
    void settingDialog();
    void createDB();
    void openDB();
    void setStart(bool start);
    void sl_start_calibration();
    void sl_set_next_point(double point);

private slots:
    void on_pushButton_2_clicked();
    void on_tableView_doubleClicked(const QModelIndex &index);
    void on_addButton_clicked();
    void on_deleteButton_clicked();
    void on_cloneButton_clicked();
    void on_exitAction_triggered();
    void timer_overflow();
    void poll();
    void on_pushButton_clicked();
    void on_changeCPA(bool checked);
    void on_action_triggered();

    void on_environmentAction_triggered();

    void on_saveAction_triggered();

signals:

    startWork(bool start);
    set_next_point_complete(bool complete);


private:
    Ui::MainWindow *ui;
    bool start;                         //Признак разрешения на калибровку
    //QSqlDatabase sgDb;                //База данных сигналов
    //QSqlQuery sgQuery;                //Запрос к базе данных
    //QString sgDbName;                   //Название базы данных сигналов
    //QSqlTableModel *sgModel;            //Модель представления данных сигналов
    //QSqlTableModel *resModel;           //Модель представления результатов калибровки
    QSortFilterProxyModel *proxySgModel; //прокси модель представления данных
    QSortFilterProxyModel * proxyResModel;  //прокси модель результатов калибровки
    //------------------Интерфейс доступа к данным---------------------------
    DataInterface *connectDriver;       //драйвер соединения с контроллером
                                         //плагин драйвера должен находиться в папке ConnectDrivers
    QDir *connectDriverDir;             //Каталог драйверов доступа к данным
    QPluginLoader *connectPluginLoader; //Модуль загрузки плагина доступа к данным
    QMap<QString, QString> * connectDrivers;    //Список имеющихся в папке ConnectDrivers драйверов доступа к данным
    QMap<QString, float> * sgList;      //Буфер запроса
    QString currentConnectDriver;       //Текущий драйвер опроса
    void findConnectPlugin();           //Поиск драйверов доступа к данным
    bool loadConnectPlugin(QString fileName); //Загрузка драйвера доступа к данным

    SettingsDialog *setting;
    createDialog * createDlg;
    QTimer * timer;                     //Таймер интервала опроса

    //---------------Калибратор------------------------------
    Calibrator * myCalibrator;          //ссылка на класс калибратора
    QThread * calibratorThread;         //Поток для калибратора

    int step;                           //вариация опроса
    bool automaticSet;                  //Признак автоматического задания генерации на эталоне


    //------------- ADomCalicration------------------------
    ADomCalibration *dom;
    AXMLCalibrationModel * XMLmodel;
    AXMLCalibrationResultModel * XMLResultsModel;
    //-----------------ALoggerModel------------------------
    ALoggerModel * logger;
    //-----------------Интерфейс задатчика ----------------
    CPAInterface * CPADriver;               //Ссылка на текущий драйвер задатчика
    QString currentCPADriver;               //Название файла текущего драйвера
    QDir *CPADriverDir;                 //Каталог доступа к драйверам задатчиков
    QMap<QString, QString> * CPADrivers;//Список имеющихся в папке CPADrivers драйверов задатчиков
    QPluginLoader *CPAPluginLoader;     //Модуль загрузки плагина задатчика
    QActionGroup *CPAActionGroup;       //Группа пунктов меню для выбора задатчика
    QThread * CPADriverThread;
    bool changeCPADriver(QString fileName);//Смена драйвера задатчика

    //----------------------Хранилище настроек-------------
    QSettings * settings;
};


#endif // MAINWINDOW_H
