#ifndef KALIBLRI2SETTING_H
#define KALIBLRI2SETTING_H

#include <QObject>
#include <QDir>
#include <QtXml>

class Kaliblri2Setting : public QObject
{
    Q_OBJECT
public:
    explicit Kaliblri2Setting(QDir *appDir, QObject *parent = 0);

signals:

public slots:
};

#endif // KALIBLRI2SETTING_H
