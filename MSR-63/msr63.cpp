#include "msr63.h"


MSR63Plugin::MSR63Plugin()
{

}

MSR63Plugin::~MSR63Plugin()
{

}

void MSR63Plugin::initialization(QSettings *settings)
{
    this->settings = settings;
    ZN = settings->value("msr-63/zn").toString();
    SKN = settings->value("msr-63/skn").toString();
    SROK = settings->value("msr-63/srok").toDate();
}

QString MSR63Plugin::getName()
{
    return "МСР-63";
}

int MSR63Plugin::showInfo()
{
    return 0;
}

void MSR63Plugin::test()
{

}

bool MSR63Plugin::setValue(float value, measurement type_value)
{
    return true;
}

bool MSR63Plugin::setup()
{
    setupDialog sd;
    sd.setModal(true);
    sd.setZN(ZN);
    sd.setSKN(SKN);
    sd.setSROK(SROK);
    if  (sd.exec() == QDialog::Accepted) {
        ZN = sd.getZN();
        SKN = sd.getSKN();
        SROK = sd.getSROK();
        settings->setValue("msr-63/zn", ZN);
        settings->setValue("msr-63/skn", SKN);
        settings->setValue("msr-63/srok", SROK);
    }
    return true;
}

QString MSR63Plugin::getType()
{
    return "Магизин сопротивлений МСР-63";
}

QString MSR63Plugin::getSN()
{
    return this->ZN;
}

QString MSR63Plugin::getSKN()
{
    return this->SKN;
}

QString MSR63Plugin::getSROK()
{
    return this->SROK.toString();
}

double MSR63Plugin::getIndeterminacyGeneral(float value, measurement type_value)
{

}

double MSR63Plugin::getIndeterminacySecondary(float value, measurement type_value, QHash<QString, QString> conditions)
{

}

measurement MSR63Plugin::getMeasurenentType(QList<measurement> list)
{

}
