#include "kaliblri2setting.h"
#include <QFile>
#include <QDebug>

Kaliblri2Setting::Kaliblri2Setting(QDir *appDir, QObject *parent) : QObject(parent)
{
    //Получаем путь к директории программы
    QFile settingFile;
    QDomDocument domDoc;
    settingFile.setFileName("settings.xml");
    qDebug()<<appDir->dirName()+"  11111111111111111";
    appDir->cdUp();
    qDebug()<<appDir->dirName()+"  11111111111111111";
    if (settingFile.exists()){
        qDebug()<<"file found";
        //открываем файл
        if (settingFile.open(QIODevice::ReadOnly))
        {
            domDoc.setContent(&settingFile);
            QDomElement domElement= domDoc.documentElement();
            qDebug()<<domElement.nodeName();
        }
    }
}

