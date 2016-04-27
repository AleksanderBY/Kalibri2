#include "calibrator.h"
#include <QDebug>
#include <QTime>

Calibrator::Calibrator( QObject *parent) : QObject(parent)
{
    counter = 0;
    //Создаем список каналов калибровки QList<QSqlRecord>
    //в QSqlRecord вся информация по калибровке не зависимо от типа базы данных
    calibrationRecordList = new QList<QSqlRecord>;
    //Создаем список точек для калибровки
    calibrationPointList = new QMultiMap<double, int>;
    //uniqueCalibrationPointsList = new QList<double>;
    currentRequest = new QList<QSqlRecord>;
    timer = new QTimer();
    timer->setInterval(10000);
    connect(timer, SIGNAL(timeout()), this, SLOT(on_timer()));
    connect(this, SIGNAL(start_calibration()),SLOT(on_start_calibration()));

}

Calibrator::~Calibrator()
{
    qDebug()<<"разрушение в отдельном потоке";
}

void Calibrator::start(DataInterface *di)
{
    data = di;

    emit this->start_calibration();

    if (calibrationRecordList->count()<1) {
        emit error_calibration();
        return;
    }
    //получаем списки точек калибровки
    calibrationPointList->clear();
    QList<double> currentList;
    for (int i=0; i<calibrationRecordList->count(); i++)
    {
        currentList.clear();
        //currentList = data->getParametrValue(calibrationRecordList->at(i));
        if (currentList.count()<1) continue;
        for (int j=0; j<currentList.count(); j++)
        {

            calibrationPointList->insert(currentList.at(j), i);
        }
    }

    uniqueCalibrationPointsList = calibrationPointList->uniqueKeys();
    //устанавливаем значение на первую точку калибровки
    currentCalibrationPoint = 0;

    //устанавливаем указатель на первый канал калибровки
    currentIterator = calibrationPointList->begin();


    emit set_next_point(uniqueCalibrationPointsList.at(currentCalibrationPoint));



    //qDebug()<<calibrationPointList->value(0,05);
    //qDebug()<<calibrationPointList->find(0,05).value();
    //timer->start();
}

void Calibrator::addCalibrationRecord(QSqlRecord record)
{
    calibrationRecordList->push_back(record);
}

void Calibrator::clearCalibrationRecordList()
{
    calibrationRecordList->clear();
}

void Calibrator::on_timer()
{
    qDebug()<<QTime::currentTime().toString() + " - " + QString::number(counter);
    ++counter;
}

void Calibrator::on_start_calibration()
{
    //QMultiMap<double, int>::iterator iterator = calibrationPointList->begin();
    //currentCalibrationPoint = iterator.key();
}

void Calibrator::on_set_next_point_complete(bool complete)
{
    if (complete) {
        //формируем запрос к контроллеру
       currentRequest->clear();
        qDebug()<<currentIterator.key();
        qDebug()<<uniqueCalibrationPointsList.at(currentCalibrationPoint);
        qDebug()<<currentCalibrationPoint;
        qDebug()<<calibrationRecordList->at(currentCalibrationPoint);
        while (currentIterator.key()==uniqueCalibrationPointsList.at(currentCalibrationPoint));
        {
            currentRequest->push_back(calibrationRecordList->at(currentCalibrationPoint));
            qDebug()<<"sdf";
            currentIterator++;
        }
       //отправляем запрос

        //data->getValues(currentRequest);


        qDebug()<< "Точка задана калибруем";
        currentCalibrationPoint++;
        if (currentCalibrationPoint<uniqueCalibrationPointsList.count())  emit set_next_point(uniqueCalibrationPointsList.at(currentCalibrationPoint));

    }
    else
        qDebug()<< "Отмена калибровки";
}

