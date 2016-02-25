#ifndef KALIBRI2_H
#define KALIBRI2_H

#include <QObject>
#include <QDir>
#include <QtWidgets>
#include "kaliblri2setting.h"

//Главный класс приложения
class Kalibri2 : public QObject
{
    Q_OBJECT
public:
    explicit Kalibri2(QObject *parent = 0);

signals:

public slots:
    void aaa();

public:
    QDir * appDir;      //Директория приложения
    Kaliblri2Setting  * setting;     //настройки приложения




};

#endif // KALIBRI2_H
