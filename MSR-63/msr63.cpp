#include "msr63.h"
#include <QMessageBox>
#include <math.h>
#include <QDebug>


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
    //Проверяем запрашиваемые единицы измерения
    if (type_value!=Om) {
        emit this->log("МСР-63: прибор не поддерживает запрашиваемые единицы измерения", Qt::red);
        emit this->set_value_error();
        return false;
    }
    QString str_val = QString::number(value, 'f' ,2);
    QMessageBox messageBox(QMessageBox::Question,
                "Задание значения МСР-63",
                "Задайте на входе измерительного канала значение <b>"+str_val+"</b> Ом",
                QMessageBox::Ok | QMessageBox::Cancel);
    if (messageBox.exec()==QMessageBox::Ok) {
        emit this->log("МСР-63: значение задано");
        emit this->set_value_ok();
        return true;
    }
    emit this->log("МСР-63: отменено пользователем", Qt::yellow);
    emit this->set_value_error();
    return false;
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
    return this->SROK.toString("dd.MM.yyyy");
}

double MSR63Plugin::getIndeterminacyGeneral(float value, measurement type_value)
{
    double delta = (0.05+0.000004*(111111.1/value-1))*value/100/sqrt(3.0);
    return delta;
}

double MSR63Plugin::getIndeterminacySecondary(float value, measurement type_value, QHash<QString, QString> conditions)
{
    float temperature = conditions.value("temperature").toFloat();
    double delta = (0.025+0.000004*(111111.1/value-1))*value/100;
    if (temperature>=18&&temperature<=22) {
        return 0;
    }
    if (temperature<18||(temperature>22&&temperature<=25)) {
        return delta/sqrt(3);
    }
    return 2*delta/sqrt(3);

}

measurement MSR63Plugin::getMeasurenentType(QList<measurement> list)
{
    for (int i=0; i<list.count();i++) {
        if (list.at(i)==Om) return list.at(i);
    }
    return notSupport;
}

bool MSR63Plugin::checkConditions(QHash<QString, QString> conditions)
{
    //Проверяем условия окружаующей среды на соответствие условиям эксплуатации прибора
    //температура от 15 до 30, влажность не более 80%
    bool ok;
    //Влажность
    if (conditions.contains("humidity")) {
        float humidity = conditions.value("humidity").toFloat(&ok);
        if (!ok) {
            emit this->log("МСР-63: Неверное содержание поля \"влажность\"", Qt::yellow);
            return false;
        }
        if (humidity>80) {
            emit this->log("МСР-63: Влажность превышает допустимую", Qt::yellow);
            return false;
        }
    }
    //Температура
    if (conditions.contains("temperature")) {
        float temperature = conditions.value("temperature").toFloat(&ok);
        if (!ok) {
            emit this->log("МСР-63: Неверное содержание поля \"температура\"", Qt::yellow);
            return false;
        }
        if (temperature>30||temperature<15) {
            emit this->log("МСР-63: Температура выходит за пределы допустимой", Qt::yellow);
            return false;
        }
    }
    return true;
}

double MSR63Plugin::getErrorGeneral(float value, measurement type_value)
{
    double delta = (0.05+0.000004*(111111.1/value-1))*value/100;
    return delta;
}

double MSR63Plugin::getErrorSecondary(float value, measurement type_value, QHash<QString, QString> conditions)
{
    float temperature = conditions.value("temperature").toFloat();
    double delta = (0.025+0.000004*(111111.1/value-1))*value/100;
    if (temperature>=18&&temperature<=22) {
        return 0;
    }
    if (temperature<18||(temperature>22&&temperature<=25)) {
        return delta;
    }
    return 2*delta;
}
