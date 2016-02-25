#ifndef MANUAL_H
#define MANUAL_H

#include <QObject>
#include <QtPlugin>
#include <QDate>
#include <QMessageBox>
#include "cpainterface.h"
#include "setupdialog.h"


class ManualPlugin: public CPAInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "by.alexpozniak.kalibri2.cpainterface" FILE "manualplugin.json")
    Q_INTERFACES(CPAInterface)
public:
    ManualPlugin();
    void initialization(QSettings *settings) Q_DECL_OVERRIDE;
    QString getName() Q_DECL_OVERRIDE;
    int showInfo() Q_DECL_OVERRIDE;
    void test() Q_DECL_OVERRIDE;
    bool setValue(float value, measurement type_value) Q_DECL_OVERRIDE;
    bool setup() Q_DECL_OVERRIDE;

signals:
    void set_value_ok();
    void set_value_error();
    void log(QString message,Qt::GlobalColor category = Qt::white);
private:
    QSettings *settings;    //хранилище настроек
    QString ZN;             //Заводской номер
    QString SKN;            //Номер свидетельства калибровки
    QDate SROK;           //Срок действия свидетельства до ...
    QString TypeDevice;     //Тип измерительного прибора
};

#endif // MANUAL_H
