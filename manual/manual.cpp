#include "manual.h"


ManualPlugin::ManualPlugin()
{
}

void ManualPlugin::initialization(QSettings *settings)
{
    this->settings = settings;
    TypeDevice = settings->value("manual/typeDevice").toString();
    ZN = settings->value("manual/zn").toString();
    SKN = settings->value("manual/skn").toString();
    SROK = settings->value("manual/srok").toDate();
}

QString ManualPlugin::getName()
{
    return "Ручной ввод";
}

int ManualPlugin::showInfo()
{
    return 0;
}

void ManualPlugin::test()
{

}

bool ManualPlugin::setValue(float value, measurement type_value)
{
    QString str_val = QString::number(value, 'g' ,6);
    QMessageBox messageBox(QMessageBox::Question,
                "Задание значения",
                "Задайте на входе измерительного канала значение <b>"+str_val+"</b> ед.",
                QMessageBox::Ok | QMessageBox::Cancel);
    if (messageBox.exec()==QMessageBox::Ok) {
        emit this->set_value_ok();
        return true;
    }
    else emit this->set_value_error();
    return false;
}

bool ManualPlugin::setup()
{
    emit this->log("Изменение настроек");
    SetupDialog sd;
    sd.setModal(true);
    sd.setTypeDevice(TypeDevice);
    sd.setZN(ZN);
    sd.setSKN(SKN);
    sd.setSROK(SROK);
    if  (sd.exec() == QDialog::Accepted) {
        TypeDevice = sd.getTypeDevice();
        ZN = sd.getZN();
        SKN = sd.getSKN();
        SROK = sd.getSROK();
        settings->setValue("manual/typeDevice", TypeDevice);
        settings->setValue("manual/zn", ZN);
        settings->setValue("manual/skn", SKN);
        settings->setValue("manual/srok", SROK);
    }
}

QString ManualPlugin::getType()
{
    return TypeDevice;
}

QString ManualPlugin::getSN()
{
    return ZN;
}

QString ManualPlugin::getSKN()
{
    return SKN;
}

QString ManualPlugin::getSROK()
{
    return SROK.toString("dd.MM.yyyy");
}

double ManualPlugin::getIndeterminacyGeneral(float value, measurement type_value)
{
    return 0;
}

double ManualPlugin::getIndeterminacySecondary(float value, measurement type_value, QHash<QString, QString> conditions)
{
    return 0;
}

measurement ManualPlugin::getMeasurenentType(QList<measurement> list)
{

}

bool ManualPlugin::checkConditions(QHash<QString, QString> conditions)
{
    return true;
}

