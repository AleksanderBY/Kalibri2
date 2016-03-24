#ifndef DATAINETERFACE
#define DATAINETERFACE

#include <QString>
#include <QStringList>
#include <QMap>
#include <QSqlTableModel>
#include <QAbstractItemModel>
#include <QSqlRecord>
#include <QHash>
#include "measurement.h"

//enum measurement { mA, mV, V, Om, C100M1_426, C100M1_428, C50M1_426, C50M1_428, C50P, C100P, CPt100IEC385, CTypeJ, CTypeK, CTypeB, CTypeA1, CTypeS, CTypeXK };

//------------------------------------------------------------------------------------------------------------------
//                        Класс для запроса параметров из устройства
//
//      В attr помещаются все параметры измерительного канала, заданые для данного типа устройства
//      Возвращаемое значение необходимо помещать в value
//      Необходимые задержки перед и между калибровками в переменных startDelay и delay
//------------------------------------------------------------------------------------------------------------------
class PollClass
{
public:
    double value;                           //Возвращаемое значение
    int startDelay;                         //Задержка перед опросом
    int delay;                              //Задержка между опросами
    measurement measurementType;            //Тип измерения
    QHash<QString,double> points;           //Список точек калибровки %ключ% - физическое значение, %значение% - техническое значение
    QHash<QString,QString> attr;            //
};


class DataInterface: public QObject
{
public:
    virtual ~DataInterface() {}
    virtual QString getName() = 0;      //получение имени драйвера
    virtual bool initialization(QAbstractItemModel * model) = 0;        //инициализация если необходимо
    virtual QString getLastError() = 0;                                 //Получение последней ошибки
    virtual bool getValues(QList<PollClass *> * pollList) = 0;              //Запрос данных от устройства
    virtual bool getFieldsDB(QVector<QString> *fieldsDB) = 0;         //Получение полей базы данных
    virtual QStringList* supportTypes() = 0;                            //Поддерживаемые типы каналов
    virtual int editDialog(int row) =0;                                 //Диалог редактирования данных о канале
    virtual QList<double> getPoints(QList<PollClass *> * pollList) = 0;     //Получение точек калибровки из списка
    virtual QList<double> getParametrValue(QSqlRecord record) = 0;
    virtual QString getTagAdress(QSqlRecord record) = 0;
    virtual bool validationPollList(QList<PollClass *> * pollList) = 0;     //валидация каналов на совместную калибровку
    virtual QList<measurement> getMeasurementTypes(QList<PollClass *> * pollList) = 0;    //
};

QT_BEGIN_NAMESPACE

#define DataInterface_iid "by.alexpozniak.kalibri2.datainterface"

Q_DECLARE_INTERFACE(DataInterface, DataInterface_iid)
QT_END_NAMESPACE


#endif // DATAINETERFACE

