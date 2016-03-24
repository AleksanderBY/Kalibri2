#ifndef CPAINTERFACE
#define CPAINTERFACE

#include <QString>
#include <QSettings>
#include <QObject>
#include "measurement.h"

class CPAInterface: public QObject
{
public:
    virtual ~CPAInterface() {}
    virtual void initialization(QSettings *settings) = 0;
    virtual QString getName() = 0;
    virtual QString getType() = 0;
    virtual QString getSN() = 0;
    virtual QString getSKN() = 0;
    virtual QString getSROK() = 0;
    virtual int showInfo() = 0;
    virtual void test() = 0;
    virtual bool setValue(float value, measurement type_value) = 0; //запрос на установление значения
    virtual double getIndeterminacyGeneral(float value, measurement type_value) = 0; //Получаем основную неопределенность прибора
    virtual double getIndeterminacySecondary(float value, measurement type_value, QHash<QString, QString> conditions) = 0; //Получаем дополнительную неопределенность прибора
    virtual bool setup() = 0;
    virtual measurement getMeasurenentType(QList<measurement> list) = 0;
signals:
    virtual void set_value_ok() = 0;    //Высылать когда значение успешно установлено
    virtual void set_value_error() = 0; //Высылать в случае ошибки установки значения
    virtual void log(QString message,Qt::GlobalColor category = Qt::white) = 0; //Сообщения для логирования
};

QT_BEGIN_NAMESPACE

#define CPAInterfece_iid "by.alexpozniak.kalibri2.cpainterface"

Q_DECLARE_INTERFACE(CPAInterface, CPAInterfece_iid)
QT_END_NAMESPACE

#endif // CPAINTERFACE

