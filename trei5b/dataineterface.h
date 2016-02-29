#ifndef DATAINETERFACE
#define DATAINETERFACE

#include <QString>
#include <QStringList>
#include <QMap>
#include <QSqlTableModel>
#include <QAbstractItemModel>
#include <QSqlRecord>
#include <QHash>

//------------------------------------------------------------------------------------------------------------------
//                        Класс для запроса параметров из устройства
//
//      В attr помещаются все параметры измерительного канала, заданые для данного типа устройства
//      Возвращаемое значение необходимо помещать в value
//------------------------------------------------------------------------------------------------------------------
class PollClass
{
public:
    double value;
    QList<double> points;
    QHash<QString,QString> attr;
};


class DataInterface: public QObject
{
public:
    virtual ~DataInterface() {}
    virtual QString getName() = 0;      //получение имени драйвера
    virtual bool initialization(QAbstractItemModel * model) = 0;        //инициализация если необходимо
    virtual QString getLastError() = 0;
    virtual bool getValues(QList<PollClass *> * pollList) = 0;              //Запрос данных от устройства
    virtual bool getFieldsDB(QVector<QString> *fieldsDB) = 0;         //Получение полей базы данных
    virtual QStringList* supportTypes() = 0;
    virtual int editDialog(int row) =0;
    virtual QList<double> getPoints(QList<PollClass *> * pollList) = 0;     //Получение точек калибровки из списка
    virtual QList<double> getParametrValue(QSqlRecord record) = 0;
    virtual QString getTagAdress(QSqlRecord record) = 0;
    //virtual bool getValue(QString device) =0;
};

QT_BEGIN_NAMESPACE

#define DataInterface_iid "by.alexpozniak.kalibri2.datainterface"

Q_DECLARE_INTERFACE(DataInterface, DataInterface_iid)
QT_END_NAMESPACE


#endif // DATAINETERFACE

