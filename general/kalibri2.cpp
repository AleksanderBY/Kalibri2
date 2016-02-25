#include "kalibri2.h"
#include <QSqlDatabase>
#include <QSqlQuery>


Kalibri2::Kalibri2(QObject *parent) : QObject(parent)
{
    //Создаем или открываем базу настроек программы
    QFile sqliteFile;
    sqliteFile.setFileName("kalibri2.db3");
    if (sqliteFile.exists()){
        qDebug()<<"База данных найдена";
    }
    else {
        qDebug()<<"Создание базы данных";
        sqliteFile.open(QIODevice::WriteOnly);
        sqliteFile.close();
        QSqlDatabase SettingDB = QSqlDatabase::addDatabase("QSQLITE");
        SettingDB.setDatabaseName("kalibri2.db3");
        //открываев базу
        if (SettingDB.open()) {
            qDebug()<<"База открыта";
            QSqlQuery *query = new QSqlQuery("CREATE TABLE users (id INTEGER PRIMARY KEY, name STRING(20))", SettingDB);
                    query->exec();
        }


    }
    //Подключаемся к базе данных настроек программы

    appDir = new QDir(qApp->applicationDirPath());
    setting = new Kaliblri2Setting(appDir);
}

void Kalibri2::aaa()
{
    ;
}

