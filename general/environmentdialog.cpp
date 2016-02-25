#include "environmentdialog.h"
#include "ui_environmentdialog.h"
#include <QDebug>

EnvironmentDialog::EnvironmentDialog(QSettings *settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EnvironmentDialog)
{
    ui->setupUi(this);
    this->settings = settings;
    this->loadEnvironment();
}

EnvironmentDialog::~EnvironmentDialog()
{
    delete ui;
}
QString EnvironmentDialog::getTemperature() const
{
    return temperature;
}

void EnvironmentDialog::setTemperature(const QString &value)
{
    temperature = value;
}
QString EnvironmentDialog::getHumidity() const
{
    return humidity;
}

void EnvironmentDialog::setHumidity(const QString &value)
{
    humidity = value;
}
QString EnvironmentDialog::getPressure() const
{
    return pressure;
}

void EnvironmentDialog::setPressure(const QString &value)
{
    pressure = value;
}
QString EnvironmentDialog::getVoltage() const
{
    return voltage;
}

void EnvironmentDialog::setVoltage(const QString &value)
{
    voltage = value;
}
QString EnvironmentDialog::getTemperatureDeviceType() const
{
    return temperatureDeviceType;
}

void EnvironmentDialog::setTemperatureDeviceType(const QString &value)
{
    temperatureDeviceType = value;
}
QString EnvironmentDialog::getTemperatureDeviceZN() const
{
    return temperatureDeviceZN;
}

void EnvironmentDialog::setTemperatureDeviceZN(const QString &value)
{
    temperatureDeviceZN = value;
}
QString EnvironmentDialog::getTemperatureDeviceSKN() const
{
    return temperatureDeviceSKN;
}

void EnvironmentDialog::setTemperatureDeviceSKN(const QString &value)
{
    temperatureDeviceSKN = value;
}
QDate EnvironmentDialog::getTemperatureDeviceSROK() const
{
    return temperatureDeviceSROK;
}

void EnvironmentDialog::setTemperatureDeviceSROK(const QDate &value)
{
    temperatureDeviceSROK = value;
}
QString EnvironmentDialog::getHumidityDeviceType() const
{
    return humidityDeviceType;
}

void EnvironmentDialog::setHumidityDeviceType(const QString &value)
{
    humidityDeviceType = value;
}
QString EnvironmentDialog::getHumidityDeviceZN() const
{
    return humidityDeviceZN;
}

void EnvironmentDialog::setHumidityDeviceZN(const QString &value)
{
    humidityDeviceZN = value;
}
QString EnvironmentDialog::getHumidityDeviceSKN() const
{
    return humidityDeviceSKN;
}

void EnvironmentDialog::setHumidityDeviceSKN(const QString &value)
{
    humidityDeviceSKN = value;
}
QDate EnvironmentDialog::getHumidityDeviceSROK() const
{
    return humidityDeviceSROK;
}

void EnvironmentDialog::setHumidityDeviceSROK(const QDate &value)
{
    humidityDeviceSROK = value;
}
QString EnvironmentDialog::getPressureDeviceType() const
{
    return pressureDeviceType;
}

void EnvironmentDialog::setPressureDeviceType(const QString &value)
{
    pressureDeviceType = value;
}
QString EnvironmentDialog::getPressureDeviceZN() const
{
    return pressureDeviceZN;
}

void EnvironmentDialog::setPressureDeviceZN(const QString &value)
{
    pressureDeviceZN = value;
}
QString EnvironmentDialog::getPressureDeviceSKN() const
{
    return pressureDeviceSKN;
}

void EnvironmentDialog::setPressureDeviceSKN(const QString &value)
{
    pressureDeviceSKN = value;
}
QDate EnvironmentDialog::getPressureDeviceSROK() const
{
    return pressureDeviceSROK;
}

void EnvironmentDialog::setPressureDeviceSROK(const QDate &value)
{
    pressureDeviceSROK = value;
}
QString EnvironmentDialog::getVoltageDeviceType() const
{
    return voltageDeviceType;
}

void EnvironmentDialog::setVoltageDeviceType(const QString &value)
{
    voltageDeviceType = value;
}
QString EnvironmentDialog::getVoltageDeviceZN() const
{
    return voltageDeviceZN;
}

void EnvironmentDialog::setVoltageDeviceZN(const QString &value)
{
    voltageDeviceZN = value;
}
QString EnvironmentDialog::getVoltageDeviceSKN() const
{
    return voltageDeviceSKN;
}

void EnvironmentDialog::setVoltageDeviceSKN(const QString &value)
{
    voltageDeviceSKN = value;
}
QDate EnvironmentDialog::getVoltageDeviceSROK() const
{
    return voltageDeviceSROK;
}

void EnvironmentDialog::setVoltageDeviceSROK(const QDate &value)
{
    voltageDeviceSROK = value;
}
QDateTime EnvironmentDialog::getLastEdit() const
{
    return lastEdit;
}

void EnvironmentDialog::setLastEdit(const QDateTime &value)
{
    lastEdit = value;
}



void EnvironmentDialog::loadEnvironment()
{
    //Загружаем настройки из хранилища
    this->temperature = settings->value("environment/temperature").toString();
    ui->temperature->setText(temperature);
    this->temperatureDeviceType = settings->value("environment/temperatureDeviceType").toString();
    ui->temperatureDeviceType->setText(temperatureDeviceType);
    this->temperatureDeviceZN = settings->value("environment/temperatureDeviceZN").toString();
    ui->temperatureDeviceZN->setText(temperatureDeviceZN);
    this->temperatureDeviceSKN = settings->value("environment/temperatureDeviceSKN").toString();
    ui->temperatureDeviceSKN->setText(temperatureDeviceSKN);
    this->temperatureDeviceSROK = settings->value("environment/temperatureDeviceSROK").toDate();
    ui->temperatureDeviceSROK->setDate(temperatureDeviceSROK);
    this->humidity = settings->value("environment/humidity").toString();
    ui->humidity->setText(humidity);
    this->humidityDeviceType = settings->value("environment/humidityDeviceType").toString();
    ui->humidityDeviceType->setText(humidityDeviceType);
    this->humidityDeviceZN = settings->value("environment/humidityDeviceZN").toString();
    ui->humidityDeviceZN->setText(humidityDeviceZN);
    this->humidityDeviceSKN = settings->value("environment/humidityDeviceSKN").toString();
    ui->humidityDeviceSKN->setText(humidityDeviceSKN);
    this->humidityDeviceSROK = settings->value("environment/temperatureDeviceSROK").toDate();
    ui->humidityDeviceSROK->setDate(humidityDeviceSROK);
    this->pressure = settings->value("environment/pressure").toString();
    ui->pressure->setText(pressure);
    this->pressureDeviceType = settings->value("environment/pressureDeviceType").toString();
    ui->pressureDeviceType->setText(pressureDeviceType);
    this->pressureDeviceZN = settings->value("environment/pressureDeviceZN").toString();
    ui->pressureDeviceZN->setText(pressureDeviceZN);
    this->pressureDeviceSKN = settings->value("environment/pressureDeviceSKN").toString();
    ui->pressureDeviceSKN->setText(pressureDeviceSKN);
    this->pressureDeviceSROK = settings->value("environment/pressureDeviceSROK").toDate();
    ui->pressureDeviceSROK->setDate(pressureDeviceSROK);
    this->voltage = settings->value("environment/voltage").toString();
    ui->voltage->setText(voltage);
    this->voltageDeviceType = settings->value("environment/voltageDeviceType").toString();
    ui->voltageDeviceType->setText(voltageDeviceType);
    this->voltageDeviceZN = settings->value("environment/voltageDeviceZN").toString();
    ui->voltageDeviceZN->setText(voltageDeviceZN);
    this->voltageDeviceSKN = settings->value("environment/voltageDeviceSKN").toString();
    ui->voltageDeviceSKN->setText(voltageDeviceSKN);
    this->voltageDeviceSROK = settings->value("environment/voltageDeviceSROK").toDate();
    ui->voltageDeviceSROK->setDate(voltageDeviceSROK);

    this->lastEdit = settings->value("environment/lastEdit").toDateTime();
}

void EnvironmentDialog::saveEnvironment()
{
    this->temperature = ui->temperature->text();
    settings->setValue("environment/temperature", temperature);
    this->humidity = ui->humidity->text();
    this->pressure = ui->pressure->text();
    this->voltage = ui->voltage->text();
    this->temperatureDeviceType = ui->temperatureDeviceType->text();
    this->temperatureDeviceZN = ui->temperatureDeviceZN->text();
    this->temperatureDeviceSKN = ui->temperatureDeviceSKN->text();
    this->temperatureDeviceSROK = ui->temperatureDeviceSROK->date();
}

void EnvironmentDialog::accept()
{

    qDebug()<<"Демонстрация настроек";
}























