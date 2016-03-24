#ifndef AENVIRONMENT_H
#define AENVIRONMENT_H

#include <QObject>
#include <QSettings>
#include <QDate>
#include "environmentdialog.h"

//Класс условий калибровки
class AEnvironment
{
    //Q_OBJECT
public:
    AEnvironment(QSettings * settings);

    QString getTemperature() const;
    void setTemperature(const QString &value);

    QString getHumidity() const;
    void setHumidity(const QString &value);

    QString getPressure() const;
    void setPressure(const QString &value);

    QString getVoltage() const;
    void setVoltage(const QString &value);

    QString getTemperatureDeviceType() const;
    void setTemperatureDeviceType(const QString &value);

    QString getTemperatureDeviceZN() const;
    void setTemperatureDeviceZN(const QString &value);

    QString getTemperatureDeviceSKN() const;
    void setTemperatureDeviceSKN(const QString &value);

    QDate getTemperatureDeviceSROK() const;
    void setTemperatureDeviceSROK(const QDate &value);

    QString getHumidityDeviceType() const;
    void setHumidityDeviceType(const QString &value);

    QString getHumidityDeviceZN() const;
    void setHumidityDeviceZN(const QString &value);

    QString getHumidityDeviceSKN() const;
    void setHumidityDeviceSKN(const QString &value);

    QDate getHumidityDeviceSROK() const;
    void setHumidityDeviceSROK(const QDate &value);

    QString getPressureDeviceType() const;
    void setPressureDeviceType(const QString &value);

    QString getPressureDeviceZN() const;
    void setPressureDeviceZN(const QString &value);

    QString getPressureDeviceSKN() const;
    void setPressureDeviceSKN(const QString &value);

    QDate getPressureDeviceSROK() const;
    void setPressureDeviceSROK(const QDate &value);

    QString getVoltageDeviceType() const;
    void setVoltageDeviceType(const QString &value);

    QString getVoltageDeviceZN() const;
    void setVoltageDeviceZN(const QString &value);

    QString getVoltageDeviceSKN() const;
    void setVoltageDeviceSKN(const QString &value);

    QDate getVoltageDeviceSROK() const;
    void setVoltageDeviceSROK(const QDate &value);

    QDateTime getLastDateEdit() const;
    void setLastDateEdit(const QDateTime &value);

    bool getInReportTemperature() const;
    bool getInReportHumidity() const;
    bool getInReportPressure() const;
    bool getInReportVoltage() const;

private:
    QSettings * settings;           //Указатель на хранилище настроек
    EnvironmentDialog * ed;         //Диалог редактирования условий
    QString temperature;            //Температура окружаущей среды
    QString humidity;               //Влажность окружаущей среды
    QString pressure;               //Атмосферное давление
    QString voltage;                //Напряжение сети
    QString temperatureDeviceType;  //Тип измерительного прибора температуры
    QString temperatureDeviceZN;    //Заводской номер измерителя температуры
    QString temperatureDeviceSKN;   //Номер свидетельства калибровки измерителя температуры
    QDate temperatureDeviceSROK;    //Срок действия свидетельства калибровки измерителя температуры
    QString humidityDeviceType;     //Тип измерительного прибора влажности
    QString humidityDeviceZN;       //Заводской номер измерителя влажности
    QString humidityDeviceSKN;      //Номер свидетельства калибровки измерителя влажности
    QDate humidityDeviceSROK;       //Срок действия свидетельства калибровки измерителя влажности
    QString pressureDeviceType;     //Тип измерительного прибора давления
    QString pressureDeviceZN;       //Заводской номер измерителя давления
    QString pressureDeviceSKN;      //Номер свидетельства калибровки измерителя давления
    QDate pressureDeviceSROK;       //Срок действия свидетельства калибровки измерителя давления
    QString voltageDeviceType;      //Тип измерительного прибора напряжения
    QString voltageDeviceZN;        //Заводской номер измерителя напряжения
    QString voltageDeviceSKN;       //Номер свидетельства калибровки измерителя напряжения
    QDate voltageDeviceSROK;        //Срок действия свидетельства калибровки измерителя напряжения
    QDateTime lastDateEdit;         //Последнее редактирование
    bool inReportTemperature;       //признак включения прибора температуры в отчет
    bool inReportHumidity;          //признак включения прибора влажности в отчет
    bool inReportPressure;          //признак включения прибора давления в отчет
    bool inReportVoltage;           //признак включения прибора напряжения в отчет
};

#endif // AENVIRONMENT_H
