#ifndef TREI_H
#define TREI_H

#include <QObject>
#include <QtPlugin>
#include "dataineterface.h"
#include "CLIE0DEF.H"
#include <QVector>
#include <QMap>
#include "dialog.h"

class Trei: public DataInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "by.alexpozniak.kalibri2.datainterface" FILE "treiplugin.json")
    Q_INTERFACES(DataInterface)
public:
    Trei();
    QString getName() Q_DECL_OVERRIDE;
    bool initialization(QAbstractItemModel * model) Q_DECL_OVERRIDE;
    QString getLastError() Q_DECL_OVERRIDE;
    bool getValues(QList<PollClass *> *pollList) Q_DECL_OVERRIDE;
    bool getFieldsDB(QVector<QString> *fieldsDB) Q_DECL_OVERRIDE;             //Получение полей базы данных
    QStringList* supportTypes() Q_DECL_OVERRIDE;
    int editDialog(int row) Q_DECL_OVERRIDE;
    QList<double> getPoints(QList<PollClass *> * pollList) Q_DECL_OVERRIDE;
    QList<double> getParametrValue(QSqlRecord record) Q_DECL_OVERRIDE;
    QString getTagAdress(QSqlRecord record) Q_DECL_OVERRIDE;
    bool validationPollList(QList<PollClass *>  * pollList) Q_DECL_OVERRIDE;
    QList<measurement> getMeasurementTypes(QList<PollClass *> * pollList) Q_DECL_OVERRIDE;
private:
    QVector<QString> *socketNameList;
    QVector<int> * socketList;
    QString lastError;
    QStringList supportTypesList;
    QAbstractItemModel * model;
    Dialog  dlg;
    int createDevice(QString desk);

};

#endif // TREI_H
