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
    QString getType() Q_DECL_OVERRIDE;
    QString getSN() Q_DECL_OVERRIDE;
    QString getSKN() Q_DECL_OVERRIDE;
    QString getSROK() Q_DECL_OVERRIDE;
    double getIndeterminacyGeneral(float value, measurement type_value) Q_DECL_OVERRIDE; //Получаем основную неопределенность прибора
    double getIndeterminacySecondary(float value, measurement type_value, QHash<QString, QString> conditions) Q_DECL_OVERRIDE; //Получаем дополнительную неопределенность прибора
    measurement getMeasurenentType(QList<measurement> list) Q_DECL_OVERRIDE;
    bool checkConditions(QHash<QString, QString> conditions) Q_DECL_OVERRIDE;
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
