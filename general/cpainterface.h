#ifndef CPAINTERFACE
#define CPAINTERFACE

#include <QString>
#include <QSettings>
#include <QObject>

enum measurement { mA, mV, V, Om, C100M1_426, C100M1_428, C50M1_426, C50M1_428, C50P, C100P, CPt100IEC385, CTypeJ, CTypeK, CTypeB, CTypeA1, CTypeS, CTypeXK };

class CPAInterface: public QObject
{
public:
    virtual ~CPAInterface() {}
    virtual void initialization(QSettings *settings) = 0;
    virtual QString getName() = 0;
    virtual int showInfo() = 0;
    virtual void test() = 0;
    virtual bool setValue(float value, measurement type_value) = 0; //запрос на установление значения
    virtual bool setup() = 0;
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

