#ifndef DATAINETERFACE
#define DATAINETERFACE

#include <QString>
#include <QStringList>
#include <QMap>
#include <QSqlTableModel>
#include <QAbstractItemModel>
#include <QSqlRecord>
#include <QHash>

class PollClass
{
    double value;
    QHash<QString,QString> attr;
};


class DataInterface
{
public:
    virtual ~DataInterface() {}
    virtual QString getName() = 0;      //получение имени драйвера
    virtual bool initialization(QAbstractItemModel * model) = 0;
    virtual int createDevice(QString name) = 0;
    virtual QString getLastError() = 0;
    virtual bool getValues(int device, QMap<QString, float> *valuesList) = 0;
    virtual bool getValues(QList<PollClass *> poll) = 0;
    virtual QList<double> getValues(QList<QSqlRecord> *sgList) = 0;
    virtual bool getFieldsDB(QMap<QString, QString> *fieldsDB) = 0;         //Получение полей базы данных
    virtual QStringList* supportTypes() = 0;
    virtual int editDialog(int row) =0;
    virtual int getDeviceField() = 0;
    virtual int getTagField() = 0;
    virtual QList<int> getPointsFields() = 0;
    virtual QList<double> getParametrValue(QSqlRecord record) = 0;
    virtual QString getTagAdress(QSqlRecord record) = 0;
    //virtual bool getValue(QString device) =0;
};

QT_BEGIN_NAMESPACE

#define DataInterface_iid "by.alexpozniak.kalibri2.datainterface"

Q_DECLARE_INTERFACE(DataInterface, DataInterface_iid)
QT_END_NAMESPACE


#endif // DATAINETERFACE

