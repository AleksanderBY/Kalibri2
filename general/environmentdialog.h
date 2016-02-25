#ifndef ENVIRONMENTDIALOG_H
#define ENVIRONMENTDIALOG_H

#include <QDialog>
#include <QSettings>
#include <QDate>

namespace Ui {
class EnvironmentDialog;
}

class EnvironmentDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EnvironmentDialog(QSettings * settings, QWidget *parent = 0);
    ~EnvironmentDialog();

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


    QDateTime getLastEdit() const;
    void setLastEdit(const QDateTime &value);

    void aply();                    //Принять изменения

private:
    Ui::EnvironmentDialog *ui;
    QSettings * settings;           //Указатель на хранилище настроек
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
    QDateTime lastEdit;             //Последнее редактирование

    void loadEnvironment();         //Загрузка условий
    void saveEnvironment();         //Сохранение условий

public slots:
    void accept();

};

#endif // ENVIRONMENTDIALOG_H
