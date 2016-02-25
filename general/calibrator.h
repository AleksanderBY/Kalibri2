#ifndef CALIBRATOR_H
#define CALIBRATOR_H

#include <QObject>
#include <QMultiMap>
#include <QTimer>
#include <QSqlRecord>

#include "../trei5b/dataineterface.h"


class Calibrator : public QObject
{
    Q_OBJECT
public:
    explicit Calibrator(QObject *parent = 0);
    ~Calibrator();
    void start(DataInterface * di);
    void addCalibrationRecord(QSqlRecord record);
    void clearCalibrationRecordList();


signals:
    start_calibration();                    //Сигнал начала калибровки
    stop_calibration();                     //Сигнал окончания калибровки
    error_calibration();                    //Сигнал ошибки в ходе калибровки
    set_next_point(double point);           //Сигнал установления нового значения на эталоне
    get_new_values(QList<double>);          //Сигнал получения новых значений

private:
    QTimer * timer;
    int counter;
    QList<QSqlRecord> * calibrationRecordList;          //список каналов подлежащих калибровке
    QMultiMap<double, int> * calibrationPointList;      //список точек поблежащих калибровке , <значение точки, номер записи в списке каналов>
    QList<double> uniqueCalibrationPointsList;
    DataInterface * data;
    int currentCalibrationPoint;                        //текущая точка калибровки
    QMultiMap<double, int>::iterator currentIterator;       //текущая иттерация точки калибровки
    QList<QSqlRecord> * currentRequest;                 //
    int currentValuePoint;                              //текущее значение номера измерения

public slots:
    void on_timer();
    void on_start_calibration();
    void on_set_next_point_complete(bool complete);
};

#endif // CALIBRATOR_H
