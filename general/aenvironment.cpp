#include "aenvironment.h"


AEnvironment::AEnvironment(QSettings *settings)
{
    this->settings=settings;
    //Загружаем настройки из хранилища
    this->temperature = settings->value("environment/temperature").toString();
    this->temperatureDeviceType = settings->value("environment/temperatureDeviceType").toString();
    this->temperatureDeviceZN = settings->value("environment/temperatureDeviceZN").toString();
    this->temperatureDeviceSKN = settings->value("environment/temperatureDeviceSKN").toString();
    this->temperatureDeviceSROK = settings->value("environment/temperatureDeviceSROK").toDate();
    this->humidity = settings->value("environment/humidity").toString();
    this->humidityDeviceType = settings->value("environment/humidityDeviceType").toString();
    this->humidityDeviceZN = settings->value("environment/humidityDeviceZN").toString();
    this->humidityDeviceSKN = settings->value("environment/humidityDeviceSKN").toString();
    this->humidityDeviceSROK = settings->value("environment/temperatureDeviceSROK").toDate();
    this->pressure = settings->value("environment/pressure").toString();
    this->pressureDeviceType = settings->value("environment/pressureDeviceType").toString();
    this->pressureDeviceZN = settings->value("environment/pressureDeviceZN").toString();
    this->pressureDeviceSKN = settings->value("environment/pressureDeviceSKN").toString();
    this->pressureDeviceSROK = settings->value("environment/pressureDeviceSROK").toDate();

}

QString AEnvironment::getTemperature() const
{
    return temperature;
}

void AEnvironment::setTemperature(const QString &value)
{
    temperature = value;
}
QString AEnvironment::getHumidity() const
{
    return humidity;
}

void AEnvironment::setHumidity(const QString &value)
{
    humidity = value;
}
QString AEnvironment::getPressure() const
{
    return pressure;
}

void AEnvironment::setPressure(const QString &value)
{
    pressure = value;
}
QString AEnvironment::getVoltage() const
{
    return voltage;
}

void AEnvironment::setVoltage(const QString &value)
{
    voltage = value;
}
QString AEnvironment::getTemperatureDeviceType() const
{
    return temperatureDeviceType;
}

void AEnvironment::setTemperatureDeviceType(const QString &value)
{
    temperatureDeviceType = value;
}
QString AEnvironment::getTemperatureDeviceZN() const
{
    return temperatureDeviceZN;
}

void AEnvironment::setTemperatureDeviceZN(const QString &value)
{
    temperatureDeviceZN = value;
}
QString AEnvironment::getTemperatureDeviceSKN() const
{
    return temperatureDeviceSKN;
}

void AEnvironment::setTemperatureDeviceSKN(const QString &value)
{
    temperatureDeviceSKN = value;
}
QDate AEnvironment::getTemperatureDeviceSROK() const
{
    return temperatureDeviceSROK;
}

void AEnvironment::setTemperatureDeviceSROK(const QDate &value)
{
    temperatureDeviceSROK = value;
}
QString AEnvironment::getHumidityDeviceType() const
{
    return humidityDeviceType;
}

void AEnvironment::setHumidityDeviceType(const QString &value)
{
    humidityDeviceType = value;
}
QString AEnvironment::getHumidityDeviceZN() const
{
    return humidityDeviceZN;
}

void AEnvironment::setHumidityDeviceZN(const QString &value)
{
    humidityDeviceZN = value;
}
QString AEnvironment::getHumidityDeviceSKN() const
{
    return humidityDeviceSKN;
}

void AEnvironment::setHumidityDeviceSKN(const QString &value)
{
    humidityDeviceSKN = value;
}
QDate AEnvironment::getHumidityDeviceSROK() const
{
    return humidityDeviceSROK;
}

void AEnvironment::setHumidityDeviceSROK(const QDate &value)
{
    humidityDeviceSROK = value;
}
QString AEnvironment::getPressureDeviceType() const
{
    return pressureDeviceType;
}

void AEnvironment::setPressureDeviceType(const QString &value)
{
    pressureDeviceType = value;
}
QString AEnvironment::getPressureDeviceZN() const
{
    return pressureDeviceZN;
}

void AEnvironment::setPressureDeviceZN(const QString &value)
{
    pressureDeviceZN = value;
}
QString AEnvironment::getPressureDeviceSKN() const
{
    return pressureDeviceSKN;
}

void AEnvironment::setPressureDeviceSKN(const QString &value)
{
    pressureDeviceSKN = value;
}
QDate AEnvironment::getPressureDeviceSROK() const
{
    return pressureDeviceSROK;
}

void AEnvironment::setPressureDeviceSROK(const QDate &value)
{
    pressureDeviceSROK = value;
}
QString AEnvironment::getVoltageDeviceType() const
{
    return voltageDeviceType;
}

void AEnvironment::setVoltageDeviceType(const QString &value)
{
    voltageDeviceType = value;
}
QString AEnvironment::getVoltageDeviceZN() const
{
    return voltageDeviceZN;
}

void AEnvironment::setVoltageDeviceZN(const QString &value)
{
    voltageDeviceZN = value;
}
QString AEnvironment::getVoltageDeviceSKN() const
{
    return voltageDeviceSKN;
}

void AEnvironment::setVoltageDeviceSKN(const QString &value)
{
    voltageDeviceSKN = value;
}
QDate AEnvironment::getVoltageDeviceSROK() const
{
    return voltageDeviceSROK;
}

void AEnvironment::setVoltageDeviceSROK(const QDate &value)
{
    voltageDeviceSROK = value;
}
QDateTime AEnvironment::getLastDateEdit() const
{
    return lastDateEdit;
}

void AEnvironment::setLastDateEdit(const QDateTime &value)
{
    lastDateEdit = value;
}






















