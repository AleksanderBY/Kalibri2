#ifndef AXMLCALIBRATIONMODEL_H
#define AXMLCALIBRATIONMODEL_H

#include <QObject>
#include <QAbstractTableModel>
#include <QDomDocument>
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

//Класс измерительного прибора
//class AMeasuringDevice {
//public:
//    void addParam(QString key, QString value);              //добавление параметра прибору
//    QHash<QString, QString> getParam() const;               //Получение параметров
//    void setParam(const QHash<QString, QString> &value);    //установка всех параметров
//    void clear() {param.clear();}
//private:
//    QHash<QString, QString> param;
//};

//class AResultCalibration {
//public:
//    explicit AResultCalibration(QObject * parent = 0);
//    ~AResultCalibration();
//    //--------------Результаты измерений--------
//    void addResult(QString key, QString value) {results.insert(key, value);}            //Добавление результата
//    QHash<QString, QString> getResults() {return results;}                              //Получение всех результатов
//    QString getRes(QString key) {return results.value(key); }
//    //----------Условия калибровки--------------
//    void addCondition(QString key, QString value) {conditions.insert(key, value);}      //Добавление уловия калибровки
//    QHash<QString, QString> getConditions() {return conditions;}
//    QString getCondition(QString key) {return conditions.value(key);}
//    //-----------Расчетные параметры------------
//    void addCalulation (QString key, QString value) {calculations.insert(key, value);}
//    QHash<QString, QString> getCalculations() const;
//    QString getCalculation(QString key) {return calculations.value(key);}
//    //-----------Измерительные приборы----------
//    QVector<AMeasuringDevice> getDevices() const;
//    void addDevice(AMeasuringDevice device) { devices.push_back(device); }
//    //------------Прочие параметры--------------
//    void addOther(QString key, QString value) {others.insert(key, value) ;}
//    QHash<QString, QString> getOthers() const;
//    QString getOther(QString key) {return others.value(key);}                           //Получение параметра

//private:
//    QHash<QString, QString> results;
//    QHash<QString, QString> conditions;
//    QHash<QString, QString> calculations;
//    QVector<AMeasuringDevice> devices;
//    QHash<QString, QString> others;
//};

////----------------------------------------------------------------------------------------------------------------
//class AChannelCalibration {

//public:
//    explicit AChannelCalibration(QObject * parent = 0);
//    ~AChannelCalibration();
//    QHash<QString, QString> getChannelData() {return channelData;}
//    void addChannelData(QString key, QString value) {channelData.insert(key, value) ;}
//    void addResultCalibration(AResultCalibration * resultCalibration) {resultList.push_back(resultCalibration);}
//    AResultCalibration * getResult(int row) {return resultList.at(row);}
//    int getResultsCount() {return  resultList.count();}

//private:
//    QHash<QString, QString> channelData;
//    QVector<AResultCalibration *> resultList;
//};

//Измерительный прибор
class TDevice {
public:
    QHash<QString, QString> deviceInfo;                 //Информация о приборе
};
//Результаты измерений и расчетные данные для точки
class TPoint {
public:
    QHash<QString, QString> pointInfo;                  //Информация о точке калибровки
    QHash<QString, QString> results;                    //Результаты калибровки
    QHash<QString, QString> calculations;               //Расчеты для калибровки
};
//Все результаты калибровки
class TCalibration {
public:
    QHash<QString, QString> calibrationInfo;            //Информация о калибровке
    QHash<QString, QString> conditions;                 //Условия проведения калибровки
    QVector<TPoint> pointList;                          //Список точек в калибровке
    QVector<TDevice> deviceList;                        //Список приборов в калибровке
};
//Канал калибровки
class TChannelCalibration {
public:
    QHash<QString, QString> channelInfo;                //Информация о канале
    QVector<TCalibration> CalibtationList;              //Список всех проеведенных калибровок
};

//---------------------------------------------------------------------------------------------------------------
//
//Класс предназначен для:
//              -парсинга XML-файла хранящего информацию о каналах контроллеров и результатам калибровки
//              -создания структуры для доступа к данным из модели
//              -сохранения результатов
//
//----------------------------------------------------------------------------------------------------------------
class ADomCalibration : public QObject {
    Q_OBJECT
public:
    explicit ADomCalibration(QObject *parent = 0);
    ADomCalibration(QString fileName, QObject *parent = 0);
    ~ADomCalibration();
    void setFileName(QString fileName);
    bool open();
    bool open(QString fileName);
    bool isOpen() {return attrOpen;}
    void clear();                                                                   //очистка данных
    bool save(QString fileName);

    //----------------------------------------------------------------
    QHash<QString, QString> getSettings() {return settings;}
    QString getDriverFile() {return settings.value("driver");}
    int columnCount() {return column;}                                              //Возвращает число колонок в таблице
    QString getSetting(QString name) {return settings.value(name);}
    void addSetting(QString key, QString value) {settings.insert(key, value);}
    void setCurrentChannel(QModelIndex index) {currentChannel=index.row();}
    int getCurrentChannel() {return currentChannel;}
    //----------------------------------------------------------------
    bool isDomValid() {return attrOpen;}

    //----------------------------------------------------------------
    QHash<QString, QString> settings;                                //Установки базы
   //----------------------------------------------------------------

    //------------------------------------------------------------------
    //          тесты по структуре данных
    //------------------------------------------------------------------
    QVector<TChannelCalibration> tChannelList;

signals:
    reset();
    log(QString message, Qt::GlobalColor category = Qt::white);

private:
    QFile * device;                                                //Файл данных
    QXmlStreamReader * reader;
    QXmlStreamWriter * writer;


    int column;
    bool attrOpen;
    bool attrParser;
    //QStringList *fields;                                          //Названия полей в базе
    QStringRef version, encoding;                                   //Версия и кодировка файла
    int currentChannel;                                             //Текущий канал калибровки
    bool parsing();                                                 //Функция парсинга файла


};


//-------------------------------------------------------------------------------------------------------------------
//
//                 Модель даступа к данным в файле XML через класс ADomCalibration(каналы калибровки)
//
//-------------------------------------------------------------------------------------------------------------------
class AXMLCalibrationModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    AXMLCalibrationModel(ADomCalibration * dom, int level = 0, QObject *parent = 0);
    ADomCalibration * getDom() {return dom;}
    void setLevel(int level) {modelLevel=level;}
    //void setCurrentChannel(int a) {currentChannel=a;}
    //int getCurrentChannel() {return currentChannel;}
private:
    ADomCalibration *dom;
    int modelLevel;
    int currentChannel;
signals:
    log(QString message, Qt::GlobalColor category = Qt::white);

public slots:
    void change_current_channel(QModelIndex index);
    void model_reset();




    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool insertRows(int row, int count, const QModelIndex &parent);
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool removeRows(int row, int count, const QModelIndex &parent);
};

//-------------------------------------------------------------------------------------------------------------------
//
//                 Модель даступа к данным в файле XML через класс ADomCalibration(результаты калибровки)
//
//-------------------------------------------------------------------------------------------------------------------
class AXMLCalibrationResultModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    AXMLCalibrationResultModel(ADomCalibration * dom, QObject *parent = 0);
    ADomCalibration * getDom() {return dom;}
private:
    ADomCalibration *dom;
signals:
    log(QString message, Qt::GlobalColor category = Qt::white);

public slots:
    void change_current_channel(QModelIndex index, QModelIndex prev);
    void model_reset();



    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    //bool insertRows(int row, int count, const QModelIndex &parent);
    //bool setData(const QModelIndex &index, const QVariant &value, int role);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    //bool removeRows(int row, int count, const QModelIndex &parent);

    // QAbstractItemModel interface
public:
    bool insertRows(int row, int count, const QModelIndex &parent);
};

class AXMLCalibrationListModel: public QAbstractListModel
{
    Q_OBJECT
public:
    AXMLCalibrationListModel(ADomCalibration * dom, QObject *parent = 0);
    ADomCalibration * getDom() {return dom;}
private:
    ADomCalibration *dom;

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

    // QAbstractItemModel interface
public:
    Qt::ItemFlags flags(const QModelIndex &index) const;

    // QAbstractItemModel interface
public:
    int columnCount(const QModelIndex &parent) const;
};

#endif // AXMLCALIBRATIONMODEL_H
