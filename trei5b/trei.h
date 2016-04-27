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
    bool getValues(QVector<TChannelCalibration*> * channelList) Q_DECL_OVERRIDE;
    bool getFieldsDB(QVector<QString> *fieldsDB) Q_DECL_OVERRIDE;             //Получение полей базы данных
    QStringList* supportTypes() Q_DECL_OVERRIDE;
    int editDialog(int row) Q_DECL_OVERRIDE;
    QList<double> getPoints(QVector<TChannelCalibration*> * channelList, measurement measurementType) Q_DECL_OVERRIDE;
    bool validationPollList(QVector<TChannelCalibration*> * channelList) Q_DECL_OVERRIDE;
    QList<measurement> getMeasurementTypes(QVector<TChannelCalibration*> * channelList) Q_DECL_OVERRIDE;
    bool polishingResults(QVector<TChannelCalibration*> * channelList, measurement measurementType) Q_DECL_OVERRIDE;
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
