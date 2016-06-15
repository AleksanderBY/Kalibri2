#ifndef SERIES5CCC_H
#define SERIES5CCC_H

#include <QObject>
#include <QtPlugin>
#include "../trei5b/dataineterface.h"
#include "dialog.h"
#include "resultdialog.h"
#include "opc.h"

class Series5ccc: public DataInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "by.alexpozniak.kalibri2.datainterface" FILE "series5cccplugin.json")
    Q_INTERFACES(DataInterface)
public:
    Series5ccc();
    ~Series5ccc();
    QString getName() Q_DECL_OVERRIDE;
    bool initialization(QAbstractItemModel * model) Q_DECL_OVERRIDE;
    QString getLastError() Q_DECL_OVERRIDE;
    bool createGroup(QVector<TChannelCalibration*> * channelList) Q_DECL_OVERRIDE; //создание новой группы данных
    bool removeGroup() Q_DECL_OVERRIDE; //Удаление группы
    bool getValues(QVector<TChannelCalibration*> * channelList) Q_DECL_OVERRIDE;
    bool getFieldsDB(QVector<QString> *fieldsDB) Q_DECL_OVERRIDE;             //Получение полей базы данных
    QStringList* supportTypes() Q_DECL_OVERRIDE;
    int editDialog(int row) Q_DECL_OVERRIDE;
    QList<double> getPoints(QVector<TChannelCalibration*> * channelList, measurement measurementType) Q_DECL_OVERRIDE;
    bool validationPollList(QVector<TChannelCalibration*> * channelList) Q_DECL_OVERRIDE;
    QList<measurement> getMeasurementTypes(QVector<TChannelCalibration*> * channelList) Q_DECL_OVERRIDE;
    bool polishingResults(QVector<TChannelCalibration*> * channelList, measurement measurementType) Q_DECL_OVERRIDE;
    QVariant getParametr(QString param) Q_DECL_OVERRIDE;
private:
    QString lastError;
    QStringList supportTypesList;
    QStringList supportMeasurement;
    QAbstractItemModel * model;
    Dialog  dlg;
    ResultDialog rd;
    IOPCServer *opcServer;          //указатель на OPC-сервер
    //Настройки создания группы на сервере
    OPCHANDLE clientHandle;
    DWORD regUptRete, revisedUptRate;
    DWORD lcid;
    OPCHANDLE m_hServerGroup;
    OPCHANDLE *m_hServerItems;
    int m_numOfItems;
    IOPCItemMgt * m_IOPCItemMgt;   //Указатель на создаваемую группу
    IOPCSyncIO * m_IOPCSyncIO;
    OPCITEMRESULT *pAddResult;

};

#endif // SERIES5CCC_H
