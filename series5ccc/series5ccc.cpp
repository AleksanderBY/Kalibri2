#include "series5ccc.h"
#include <QDebug>
#include <objbase.h>
#include "windows.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <comdef.h>
#include <QAxObject>
#include <QUuid>
#include <QSet>
#include <QVariant>
#include <qaxtypes.h>

using namespace std;

Series5ccc::Series5ccc()
{
    supportTypesList.append("CM-1-335");
    supportTypesList.append("CM-1-536");
    supportTypesList.append("CM-1-547-3");
    supportTypesList.append("RCM-1-335");
    supportTypesList.append("RCM-1-536");
    supportMeasurement.append("4-20 mA");
    supportMeasurement.append("Pt-100");
    supportMeasurement.append("Type K");
    dlg.setSupportType(supportTypesList);
    dlg.setModal(true);
    rd.setModal(true);

    opcServer = NULL;
    clientHandle = 1;
    regUptRete = 5000,
    lcid = 0x409;
    m_hServerGroup = NULL;
    m_IOPCItemMgt = NULL;
    m_IOPCSyncIO = NULL;
    m_numOfItems=0;
    pAddResult = NULL;
    m_hServerItems = NULL;
}

Series5ccc::~Series5ccc()
{
    WINBOOL bbb = TRUE;
    qDebug()<<"Уничтожение драйвера Series 5";
    if (opcServer!=NULL) {
        opcServer->RemoveGroup(m_hServerGroup, bbb);
        qDebug()<<opcServer->Release();
    }
}

QString Series5ccc::getName()
{
    return "Series5 CCC";
}

bool Series5ccc::initialization(QAbstractItemModel *model)
{

    this->model = model;
    //устанавливаем модель в диалоге редактора
    dlg.setModel(model);
    //---------------------------------------------------------------------------------
    //Инициализация плагина доступа к данным контроллера CCC Series5 через OPC-сервер
    //---------------------------------------------------------------------------------

    //Переменная для получения кода ошибки
    HRESULT hr;
    //Название OPC-сервера
    LPCOLESTR olestr=L"TrainTools.S5_OPC.1";
    //уникальный идентификатор сервера
    CLSID S5CLSID;
    //Получаем идентификатор по имени
    hr = CLSIDFromProgID(olestr, &S5CLSID);
    if (SUCCEEDED(hr)) {
        qDebug()<<"OPC найден";
    }
    else {
        qDebug()<<"OPC не найден";
        return false;
    }

    //Получаем указатель на начальный интерфейс COM-сервера
    IUnknown *pUnkn = NULL;
    hr = CoCreateInstance(S5CLSID, NULL, CLSCTX_ALL, IID_IOPCServer, (LPVOID*)&opcServer);

    //Получаем указатель на OPC-сервер
    //hr = pUnkn->QueryInterface(IID_IOPCServer, (void**)&opcServer);
    if (SUCCEEDED(hr)) {
        qDebug()<<"OPC создан";
    }
    else {
        qDebug()<<"OPC не создан";
        return false;
    }
    return true;
}

QString Series5ccc::getLastError()
{
    return this->lastError;
}

bool Series5ccc::createGroup(QVector<TChannelCalibration *> *channelList)
{
    //удаляем преведущуую группу
    //this->removeGroup();
    HRESULT hr;
   //создаем новую группу
        hr = opcServer->AddGroup(L"Kalibri2",
                                 FALSE,
                                 regUptRete,
                                 clientHandle,
                                 NULL,
                                 NULL,
                                 lcid,
                                 &m_hServerGroup,
                                 &revisedUptRate,
                                 IID_IOPCItemMgt,
                                 (LPUNKNOWN*)&m_IOPCItemMgt);
        if (SUCCEEDED(hr)) {
            qDebug()<<"группа создана";

        }
        else {
            qDebug()<<"группа не создана";
            return false;
        }

        //Создаем интерфейс для опроса
        hr = m_IOPCItemMgt->QueryInterface(IID_IOPCSyncIO, (void**)&m_IOPCSyncIO);


//    HRESULT pp;

//    //чистим группу
//    if (m_numOfItems>0) {
//        m_IOPCItemMgt->RemoveItems(m_numOfItems, pAddResult->hServer, (HRESULT**)pp);
//    }



//Добавляем переменные в группу faweb.net/us/opc/sample_vc.html

     OPCITEMDEF *itemDefs = NULL;
     HRESULT *pErrors = NULL;
     m_numOfItems = channelList->count();
     _variant_t *m_itemValues;

     itemDefs = new OPCITEMDEF[m_numOfItems];
     m_itemValues = new _variant_t[m_numOfItems];

     BSTR AccessPath = ::SysAllocString(L"");
     for (DWORD i=0; i<m_numOfItems; i++) {
        std::wstring str_ = channelList->at(i)->channelInfo.value("tag").toString().toStdWString();
        BSTR ItemID = SysAllocString(str_.c_str());
        itemDefs[i].szAccessPath = AccessPath;
        itemDefs[i].szItemID = ItemID;
        itemDefs[i].bActive = FALSE;
        itemDefs[i].hClient = 1;
        itemDefs[i].dwBlobSize = 0;
        itemDefs[i].pBlob = NULL;
        itemDefs[i].vtRequestedDataType = 0;
     }
     hr = m_IOPCItemMgt->AddItems(m_numOfItems, itemDefs, &pAddResult, &pErrors);

     m_hServerItems = new OPCHANDLE[m_numOfItems];

     for (int i=0;i<m_numOfItems; i++) {
         m_hServerItems[i] = pAddResult[i].hServer;
     }

     if (SUCCEEDED(hr)) {
         qDebug()<<"переменные добавленны";

     }
     else {
         qDebug()<<"группа не создана";
         return false;
     }
    return true;
}

bool Series5ccc::removeGroup()
{
    if (m_IOPCSyncIO !=NULL) m_IOPCSyncIO->Release();
    if (m_IOPCItemMgt !=NULL) m_IOPCItemMgt->Release();

    if (m_hServerGroup!=NULL) {
        opcServer->RemoveGroup(m_hServerGroup, TRUE);
        //delete m_hServerItems;
    }
    if (m_hServerItems !=NULL) delete [] m_hServerItems;
    return true;
}

bool Series5ccc::getValues(QVector<TChannelCalibration *> *channelList)
{
    //Читаем данные с сервера
    HRESULT hr;
    OPCITEMSTATE *pItemValues = NULL;
    HRESULT *pErrors = NULL;

    hr = m_IOPCSyncIO->Read(OPC_DS_DEVICE, m_numOfItems, m_hServerItems, &pItemValues, &pErrors);
    QVariant val;
    for (int i=0; i<channelList->count();i++) {
        val = VARIANTToQVariant(pItemValues[i].vDataValue, "double");
        qDebug()<<val;
        int currentItem = channelList->at(i)->CalibtationList.last().pointList.last().results.count()+1;
        channelList->at(i)->CalibtationList.last().pointList.last().results.insert("V"+QString::number(currentItem), val.toString());
    }

    return true;
}

bool Series5ccc::getFieldsDB(QVector<QString> *fieldsDB)
{
    fieldsDB->clear();
    fieldsDB->push_back("controller");
    fieldsDB->push_back("FTA_RFTA");
//    fieldsDB->push_back("module");
//    fieldsDB->push_back("pipe");
//    fieldsDB->push_back("cipher");
//    fieldsDB->push_back("name");
//    fieldsDB->push_back("type");
//    fieldsDB->push_back("min");
//    fieldsDB->push_back("max");
//    fieldsDB->push_back("unity");
//    fieldsDB->push_back("tag");
//    fieldsDB->push_back("point1");
//    fieldsDB->push_back("point2");
//    fieldsDB->push_back("point3");
//    fieldsDB->push_back("point4");
//    fieldsDB->push_back("note");
    return true;
}

QStringList *Series5ccc::supportTypes()
{
    return new QStringList();
}

int Series5ccc::editDialog(int row)
{
    dlg.setRow(row);
    return dlg.exec();
}

QList<double> Series5ccc::getPoints(QVector<TChannelCalibration *> *channelList, measurement measurementType)
{
    //Создаем список точек
    QList<double> listPoint;
    for (int i=0; i<channelList->count();i++) {
        //Проверяем наличие поля тип измерительного канала
        if (!channelList->at(i)->channelInfo.contains("type")) {
            qDebug()<<"Отсутствует поле типа канала";
            continue;
        }
        //Проверяем поддержку типа измерительного канала
        if (!supportMeasurement.contains(channelList->at(i)->channelInfo.value("type").toString())) {
            qDebug()<<"Тип измерительного канала не поддерживается";
            continue;
        }
        QString mType = channelList->at(i)->channelInfo.value("type").toString();
        //Проверяем наличие поля min
        if (!channelList->at(i)->channelInfo.contains("min")) {
            qDebug()<<"Отсутствует поле min";
            continue;
        }
        //Проверяем наличие поля max
        if (!channelList->at(i)->channelInfo.contains("max")) {
            qDebug()<<"Отсутствует поле max";
            continue;
        }
        //Проверяем правильность полей min, max
        double PMin,PMax;
        bool ok;
        PMin = channelList->at(i)->channelInfo["min"].toString().replace(',','.').toDouble(&ok);
        if (!ok) {
            qDebug()<<"Неправильное поле min";
            continue;
        }
        PMax = channelList->at(i)->channelInfo["max"].toString().replace(',','.').toDouble(&ok);
        if (!ok) {
            qDebug()<<"Неправильное поле max";
            continue;
        }
        if (PMin>=PMax) {
            qDebug()<<"Неверный диапазон";
            continue;
        }

        double EMin, EMax;

        switch (measurementType) {
        case mA:
            EMin = 4; EMax = 20;
            for (int j=1; j<=4; j++) {
                //Проверяем наличие поля тип измерительного канала
                if (!channelList->at(i)->channelInfo.contains("point"+QString::number(j))) {
                    qDebug()<<"Точка:"+QString::number(j)+" отсутствует";
                    continue;
                }
                //Проверяем корректность поля точки канала
                double PPoint = channelList->at(i)->channelInfo["point"+QString::number(j)].toString().replace(',','.').toDouble(&ok);
                if (!ok) {
                    qDebug()<<"Неправильное значение поля точки "+QString::number(j);
                    continue;
                }
                double EPoint = (EMax-EMin)*(PPoint-PMin)/(PMax-PMin)+EMin;
                channelList->at(i)->tempPoint.insert(channelList->at(i)->channelInfo.value("point"+QString::number(j)).toString(), EPoint);
                double coef = (PMax-PMin)/(EMax-EMin);
                channelList->at(i)->temp.insert("coefEtOsn", QString::number(coef, 'g', 12));
                channelList->at(i)->temp.insert("coefEtDop", QString::number(coef, 'g', 12));
                //pollList->at(i)->points.insert(pollList->at(i)->attr.value("point"+QString::number(j)), EPoint);
                listPoint.push_back(EPoint);
            }
            break;
        case CTypeK:
        case C100P:
            for (int j=1; j<=4; j++) {
                if (!channelList->at(i)->channelInfo.contains("point"+QString::number(j))) {
                    qDebug()<<"Точка:"+QString::number(j)+" отсутствует";
                    continue;
                }
                //Проверяем корректность поля точки канала
                double PPoint = channelList->at(i)->channelInfo["point"+QString::number(j)].toString().replace(',','.').toDouble(&ok);
                if (!ok) {
                    qDebug()<<"Неправильное значение поля точки "+QString::number(j);
                    continue;
                }
                channelList->at(i)->tempPoint.insert(channelList->at(i)->channelInfo.value("point"+QString::number(j)).toString(), PPoint);
                channelList->at(i)->temp.insert("coefEtOsn", QString::number(1));
                channelList->at(i)->temp.insert("coefEtDop", QString::number(1));
                listPoint.push_back(PPoint);
            }
            break;
        case Om:
            if (mType== "Pt-100") {
                for (int j=1; j<=4; j++) {
                    //Проверяем наличие поля тип измерительного канала
                    if (!channelList->at(i)->channelInfo.contains("point"+QString::number(j))) {
                        qDebug()<<"Точка:"+QString::number(j)+" отсутствует";
                        continue;
                    }
                    //Проверяем корректность поля точки канала
                    double PPoint = channelList->at(i)->channelInfo["point"+QString::number(j)].toString().replace(',','.').toDouble(&ok);
                    if (!ok) {
                        qDebug()<<"Неправильное значение поля точки "+QString::number(j);
                        continue;
                    }
                    double EPoint;
                    EPoint = OmToC::getTSP391(100,PPoint);
                    qDebug()<<"Платиновый термометр, точка - " + QString::number(EPoint);

                    channelList->at(i)->tempPoint.insert(channelList->at(i)->channelInfo.value("point"+QString::number(j)).toString(), EPoint);
                    double coef = PPoint/EPoint;
                    channelList->at(i)->temp.insert("coefEtOsn", QString::number(coef, 'g', 12));
                    channelList->at(i)->temp.insert("coefEtDop", QString::number(coef, 'g', 12));
                    //pollList->at(i)->points.insert(pollList->at(i)->attr.value("point"+QString::number(j)), EPoint);
                    listPoint.push_back(EPoint);
                }
            }
            break;
        default:
            break;
        }
        //задаем задержку перед опросом
        channelList->at(i)->startDelay = 5000;
        //задержка между опросами
        channelList->at(i)->delay = 1000;
        }
    QSet<double> temp = listPoint.toSet();
    listPoint = temp.toList();
    qSort(listPoint.begin(), listPoint.end());
    return listPoint;
}

bool Series5ccc::validationPollList(QVector<TChannelCalibration *> *channelList)
{
    return true;
}

//Получаем список поддерживаемых типов задатчиков
QList<measurement> Series5ccc::getMeasurementTypes(QVector<TChannelCalibration *> *channelList)
{
    QList<measurement> list;
    if (channelList->count()<=0) return list;
    QString mType = channelList->at(0)->channelInfo.value("type").toString();
    if (mType=="4-20 mA"){
        list<<mA;
    }
    if (mType=="Type K"){
        list<<CTypeK;
        list<<mV;
    }
    if (mType=="Pt-100"){
        list<<C100P;
        list<<Om;
    }
    return list;
}

bool Series5ccc::polishingResults(QVector<TChannelCalibration *> *channelList, measurement measurementType)
{
    return true;
}

QVariant Series5ccc::getParametr(QString param)
{
    QString str;
    return str;
}
