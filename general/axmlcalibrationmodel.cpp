#include "axmlcalibrationmodel.h"
#include <QDebug>
#include <math.h>


AXMLCalibrationModel::AXMLCalibrationModel(ADomCalibration *dom, int level, QObject *parent) : QAbstractTableModel(parent)
{
    this->dom = dom;
    this->modelLevel=level;
    connect(dom, SIGNAL(reset()), this, SLOT(model_reset()));
}

void AXMLCalibrationModel::change_current_channel(QModelIndex index)
{
//    if (modelLevel==1) {
//        emit this->beginResetModel();
//        dom->setCurrentChannel(index.row());
//        emit this->endResetModel();
//    }
}

void AXMLCalibrationModel::model_reset()
{
    emit beginResetModel();
    emit endResetModel();
}


int AXMLCalibrationModel::rowCount(const QModelIndex &parent) const
{
    if (!dom->isDomValid()) return 0;
    //qDebug()<<dom->getChannel(dom->getCurrentChannel()).getResultsCount();
    return dom->channelCount();
}

int AXMLCalibrationModel::columnCount(const QModelIndex &parent) const
{
    if (!dom->isDomValid()) return 0;
    return dom->getSetting("fieldscount").toInt();
}

QVariant AXMLCalibrationModel::data(const QModelIndex &index, int role) const
{
     //   return QVariant();
    if (!index.isValid()) return QVariant();
    if (!dom->isDomValid()) return QVariant();

    if (modelLevel==0){
        switch (role) {
        case Qt::DisplayRole:
            return dom->getChannel(index.row())->getChannelData().value(dom->getSetting("field"+QString::number(index.column())));
            break;
        case Qt::EditRole:
            return dom->getChannel(index.row())->getChannelData().value(dom->getSetting("field"+QString::number(index.column())));
            break;
        default:
            return QVariant();
            break;
        }
    }
//    else
//    {
//        switch (role) {
//        case Qt::DisplayRole:
//            return  dom->getChannel(dom->getCurrentChannel()).resultList.at(index.row())->resultAttributes.at(index.column()).value().toString();
//            break;
////        case Qt::EditRole:
////            return dom->getChannel(index.row())->attribute(index.column()).value().toString();
////            break;
//        default:
//            return QVariant();
//            break;
//        }
//    }


}


bool AXMLCalibrationModel::insertRows(int row, int count, const QModelIndex &parent)
{
    emit this->beginInsertRows(parent,row, row+count-1);
//    AChannelCalibration *newChannel = new AChannelCalibration();
//    //newChannel = dom->getChannel(row-1);
//    if (dom->columnCount()>0)
//    {
//        for (int i=0; i<dom->columnCount(); i++)
//        newChannel->addAttribute(dom->getBodyAttribute("field"+QString::number(i)),"");
//    }
//    dom->addChannel(newChannel);
    emit this->endInsertRows();
    return true;
}


ADomCalibration::ADomCalibration(QObject *parent) : QObject(parent)
{
    attrOpen = false;
    attrParser = false;
    reader = new QXmlStreamReader();
    writer = new QXmlStreamWriter();
    device = new QFile(this);
}

ADomCalibration::ADomCalibration(QString fileName, QObject *parent) : ADomCalibration(parent)
{
    setFileName(fileName);
    open();
}

ADomCalibration::~ADomCalibration()
{
    delete reader;
    delete writer;
    qDeleteAll(this->channelList);
    this->channelList.clear();
}

void ADomCalibration::setFileName(QString fileName)
{
    device->setFileName(fileName);
}

bool ADomCalibration::open()
{
    this->clear();
    //Пытаемся открыть файл
    if (!device->open(QIODevice::ReadOnly|QIODevice::Text)){
        attrOpen = false;
        return false;
    }
    attrOpen =true;

    reader->setDevice(device);
    //writer->setDevice(device);

    if (!parsing()) {
        log(tr("Неправильная структура файла"),Qt::red);
        attrOpen=false;
        reader->clear();
        device->close();
        return false;
    }
    device->close();
    log(tr("Файл успешно открыт"),Qt::green);

    currentChannel=0;

    emit reset();

    return true;
}

bool ADomCalibration::open(QString fileName)
{
    setFileName(fileName);
    open();
}

void ADomCalibration::clear()
{
    qDeleteAll(this->channelList);
    this->channelList.clear();
    this->settings.clear();
    //this->currentChannel=-1;
    this->column=0;
}

bool ADomCalibration::save(QString fileName)
{
    QFile file;
    file.setFileName("test.xml");
    if (!file.open(QIODevice::WriteOnly|QIODevice::Text)){
        return false;
    }
    writer->setAutoFormatting(true);
    writer->setDevice(&file);
    writer->writeStartDocument();
    writer->writeStartElement("body");
    //QXmlStreamAttribute attr;
    QHash<QString, QString>::iterator iter;
    QHash<QString, QString> map = this->getSettings();
    QVector<AMeasuringDevice> devices;
    for (iter = map.begin(); iter != map.end(); ++iter){
        writer->writeAttribute(iter.key(),iter.value());
    }
    for (int i=0; i<channelList.count(); i++){
        writer->writeStartElement("channel");
        map = this->getChannel(i)->getChannelData();
        for (iter = map.begin(); iter != map.end(); ++iter){
            writer->writeAttribute(iter.key(),iter.value());
        }
        for (int j=0; j<this->getChannel(i)->getResultsCount(); j++){
            writer->writeStartElement("calibration");
            map = this->getChannel(i)->getResult(j)->getOthers();
            for (iter = map.begin(); iter != map.end(); ++iter){
                writer->writeAttribute(iter.key(),iter.value());
            }

            writer->writeStartElement("results");
            map = this->getChannel(i)->getResult(j)->getResults();
            for (iter = map.begin(); iter != map.end(); ++iter){
                writer->writeAttribute(iter.key(),iter.value());
            }
            writer->writeEndElement();

            writer->writeStartElement("conditions");
            map = this->getChannel(i)->getResult(j)->getConditions();
            for (iter = map.begin(); iter != map.end(); ++iter){
                writer->writeAttribute(iter.key(),iter.value());
            }
            writer->writeEndElement();

            writer->writeStartElement("devices");

            devices=this->getChannel(i)->getResult(j)->getDevices();
            for (int k=0; k<devices.count();k++) {
                writer->writeStartElement("device");
                map = devices.at(k).getParam();
                for (iter = map.begin(); iter != map.end(); ++iter){
                    writer->writeAttribute(iter.key(),iter.value());
                }
                writer->writeEndElement();
            }

            writer->writeEndElement();


            writer->writeEndElement();
        }


        writer->writeEndElement();
    }
    writer->writeEndDocument();
    return true;
}


bool ADomCalibration::parsing()
{
    //Удаляем ранее загруженные данные
    this->clear();
    //Вспомогательная переменная для хрнения типа тега
    int tag=-1;
    int lastTag=-1;
    AChannelCalibration * channel;
    AResultCalibration * result;
    QXmlStreamAttributes temp;
    while (!reader->atEnd()&&!reader->hasError())
    {
        QXmlStreamReader::TokenType token = reader->readNext();
        //qDebug()<<token ;
        if (token == QXmlStreamReader::StartDocument) {
            //Читаем данные заголовка файла
            version = reader->documentVersion();
            encoding = reader->documentEncoding();
        }
        if (token == QXmlStreamReader::StartElement) {

            //Проверяем стартовый тег на правильность названия
            if (reader->name()=="body") tag=0;
            if (reader->name()=="channel") tag=1;
            if (reader->name()=="results1"||reader->name() == "results2"||reader->name() == "results3"||reader->name() == "results4") tag=2;
            if (reader->name()=="event_time") tag=3;
            if (reader->name()=="conditions") tag=4;
            if (reader->name()=="generalSI") tag=6;
            if (reader->name()=="secondarySI") tag=7;
            if (reader->name()=="SI") tag=5;

            //Получаем атрибуты стартового элемента
            temp = reader->attributes();
            switch (tag) {
            case 0:{
                //Проверяем правильную последовательность тэгов
                //qDebug()<<lastTag;
                if (lastTag!=-1) {
                    log(tr("Ошибка файла: неправильная вложенность тегов"), Qt::red);
                    return false;
                }

                if (temp.count()>0)
                //переписываем аттрибуты в переменную настроек дома
                    foreach (QXmlStreamAttribute attr, temp) {
                        settings.insert(attr.name().toString(), attr.value().toString());
                    }
                //проверяем наличие обязательных полей и правильность их заполнения
                if (!settings.contains("driver")) {
                    log(tr("Ошибка файла: отсутствует информация о драйвере опроса"), Qt::red);
                    return false;
                }
                else if (settings.value("driver")=="") {
                    log(tr("Ошибка файла: отсутствует информация о драйвере опроса"), Qt::red);
                    return false;
                }
                if (!settings.contains("fieldscount")){
                    log(tr("Ошибка файла: отсутствует информация о количестве полей БД"), Qt::red);
                    return false;
                }
                else
                    if (settings.value("fieldscount").toInt()<=0){
                        log(tr("Ошибка файла: не корректная информация о количестве полей БД"), Qt::red);
                        return false;
                    }
                    else {
                        column = settings.value("fieldscount").toInt();
                        for (int i=0;i<column;i++) {
                            if (!settings.contains("field"+QString::number(i))){
                                log("Ошибка файла: отсутствует поле field"+QString::number(i), Qt::red);
                                return false;
                            }
                            else if (settings.value("field"+QString::number(i))=="") {
                                log("Ошибка файла: пустое поле field"+QString::number(i), Qt::red);
                                return false;
                            }
                        }
                    }
                lastTag = tag;
                break;
            }

            case 1:{
                AChannelCalibration * channel = new AChannelCalibration();
                temp = reader->attributes();
                if (temp.count()>0)
                    foreach (QXmlStreamAttribute attr, temp) {
                        channel->addChannelData(attr.name().toString(),attr.value().toString());
                    }
                this->channelList.push_back(channel);
                break;
            }


            case 2:{
                AResultCalibration * result = new AResultCalibration();
                AChannelCalibration * channel = getChannel(channelCount()-1);
                temp = reader->attributes();
                if (temp.count()>0)
                    foreach (QXmlStreamAttribute attr, temp) {
                        //qDebug()<<attr.name().toString();
                        result->addResult(attr.name().toString(), attr.value().toString());
                    }
                //Добавляем точку калибровки если её нет
                if (!result->getOthers().contains("point")) result->addOther("point", channel->getChannelData().value("point"+QString::number(channel->getResultsCount()+1)));
                //Добавляем среднее значение если его нет
                if (!result->getCalculations().contains("av")) {
                    double summa = 0;
                    for (int i=1; i<=10; i++) {
                        summa = summa+result->getRes("V"+QString::number(i)).replace(',','.').toDouble();
                    }
                    double av = summa/10;
                    //qDebug()<<summa;
                    result->addCalulation("av", QString::number(av, 'g', 12));
                }
                //Добавляем отклонение если его нет
                if (!result->getCalculations().contains("delta")) {
                    double av = result->getCalculation("av").toDouble();
                    //qDebug()<<QString::number(av,'g',12);
                    double point = result->getOther("point").toDouble();
                    double delta = point - av;
                    //qDebug()<<delta;
                    result->addCalulation("delta", QString::number(delta, 'g', 12));
                }

                //Добавляем неопределенность измерения если его нет
                if (!result->getCalculations().contains("neoprIzm"))
                {
                    double av = result->getCalculation("av").toDouble();
                    double summa = 0;
                    for (int i=1; i<=10; i++) {
                        summa += pow(result->getRes("V"+QString::number(i)).toDouble()-av, 2);
                    }
                    summa = summa/90;
                    summa = sqrt(summa);
                    result->addCalulation("neoprIzm", QString::number(summa, 'g', 12));
                }
                channel->addResultCalibration(result);

                break;
            }

            case 3:{
                AChannelCalibration * channel = getChannel(channelCount()-1);
                temp = reader->attributes();
                //Временно для старого формата файлов
                AResultCalibration * result;
                for (int i=0; i<channel->getResultsCount();i++)
                {
                    result=channel->getResult(i);
                    foreach (QXmlStreamAttribute attr, temp) {
                        //qDebug()<<attr.name().toString();
                        result->addOther(attr.name().toString(), attr.value().toString());
                    }
                }
                break;
            }
            case 4:{
                AChannelCalibration * channel = getChannel(channelCount()-1);
                temp = reader->attributes();
                //Временно для старого формата файлов
                AResultCalibration * result;
                for (int i=0; i<channel->getResultsCount();i++)
                {
                    result=channel->getResult(i);
                    foreach (QXmlStreamAttribute attr, temp) {
                        //qDebug()<<attr.name().toString();
                        result->addCondition(attr.name().toString(), attr.value().toString());
                    }
                }
                break;}
            case 5:{
                AChannelCalibration * channel = getChannel(channelCount()-1);
                temp = reader->attributes();
                //Временно для старого формата файлов
                AResultCalibration * result;
                for (int i=0; i<channel->getResultsCount();i++)
                {
                    result=channel->getResult(i);
                    AMeasuringDevice MDevice;
                    foreach (QXmlStreamAttribute attr, temp) {
                        qDebug()<<attr.name().toString();
                        QString key;
                        if (attr.name().toString()=="Tip") key="Type";
                        if (attr.name().toString()=="NZ") key="SN";
                        if (attr.name().toString()=="NP") key="SKN";
                        if (attr.name().toString()=="DO") key="SROK";
                        MDevice.addParam(key, attr.value().toString());
                    }
                    if (lastTag==6) {
                        MDevice.addParam("role", "general");
                    }
                    if (lastTag==7) {
                        MDevice.addParam("role", "secondary");
                    }
                    result->addDevice(MDevice);
                }


            }
                break;
//            default:{
//                return false;
//                break;
//            }

            }
            if (lastTag!=5&&(tag==6||tag==7)) lastTag=tag;




            if (reader->name() == "channel") {
//                channel = new AChannelCalibration();
//                channel->setAttributes(reader->attributes());
//                channelList.push_back(channel);
            }
            if (reader->name() == "results1"||reader->name() == "results2"||reader->name() == "results3"||reader->name() == "results4") {
//                result = new AResultCalibration();
//                result->setResults(reader->attributes());
//                QXmlStreamAttribute attr("point" , channel->getChannelAttributes().at(channel->resultList.count()+10).value().toString());
//                result->resultAttributes.insert(0, attr);
//                channel->addResultCalibration(result);
            }

            if (reader->name() == "event_time") {
//                temp = reader->attributes();
//                if (temp.count()>0)
//                {
//                    int i = channel->resultList.count();
//                    if (i>0)
//                        for (int j=0;j<i;j++){
//                            for(int k=0;k<temp.count();k++) {
//                                channel->resultList.at(j)->resultAttributes.insert(k,temp.at(k));
//                            }
//                        }
//                }

            }
        }
        if (token == QXmlStreamReader::EndElement) {
            //qDebug()<<"/"+reader->name().toString();
        }

        if (token == QXmlStreamReader::EndDocument) {
            return true;
        }
    }
    return false;
}


AChannelCalibration::AChannelCalibration(QObject *parent)
{

}

AChannelCalibration::~AChannelCalibration()
{
    qDeleteAll(this->resultList);
    this->resultList.clear();
    this->channelData.clear();
}



bool AXMLCalibrationModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
       dom->getChannel(index.row())->addChannelData(dom->getSetting("field"+QString::number(index.column())), value.toString());
    }
        //dom->getChannel(index.row())->setAttribute(index.column(), value.toString());
    return true;
}


Qt::ItemFlags AXMLCalibrationModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEnabled|Qt::ItemIsSelectable|Qt::ItemIsEditable;
}


QVariant AXMLCalibrationModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (!dom->isDomValid()) return QVariant();
    if (role==Qt::DisplayRole)
        switch (orientation) {
        case Qt::Horizontal:
            return dom->getSettings().value("field"+QString::number(section));
            break;
        case Qt::Vertical:
            return section+1;
            break;
        default:
            return QVariant();
            break;
        }
    return QVariant();
}


bool AXMLCalibrationModel::removeRows(int row, int count, const QModelIndex &parent)
{
    emit this->beginRemoveRows(parent,row, row+count-1);
//    for (int i=0; i<count; i++){
//      AChannelCalibration *channel = dom->getChannel(row);
//      delete channel;
//      dom->removeChannel(row);
//    }
    emit this->endRemoveRows();
    return true;
}

//Создание модели для результатов калибровки
AXMLCalibrationResultModel::AXMLCalibrationResultModel(ADomCalibration *dom, QObject *parent): QAbstractTableModel(parent)
{
    this->dom = dom;
    connect(dom, SIGNAL(reset()), this, SLOT(model_reset()));
}

void AXMLCalibrationResultModel::change_current_channel(QModelIndex index, QModelIndex prev)
{
    this->dom->setCurrentChannel(index);
    this->model_reset();
}

void AXMLCalibrationResultModel::model_reset()
{
    emit beginResetModel();
    emit endResetModel();
}

int AXMLCalibrationResultModel::rowCount(const QModelIndex &parent) const
{
    if (!dom->isDomValid()) return 0;
//    qDebug()<<"Калибровок";
//    qDebug()<<dom->getChannel(dom->getCurrentChannel()).getResultsCount();
    return dom->getChannel(dom->getCurrentChannel())->getResultsCount();

}

int AXMLCalibrationResultModel::columnCount(const QModelIndex &parent) const
{
    if (!dom->isDomValid()) return 0;
    return 20;
}

QVariant AXMLCalibrationResultModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();
    if (!dom->isDomValid()) return QVariant();

    switch (role) {
    case Qt::DisplayRole:
        if (index.column()>2&&index.column()<13) {
            return dom->getChannel(dom->getCurrentChannel())->getResult(index.row())->getRes("V"+QString::number(index.column()-2));
        }
        if (index.column()==0) return dom->getChannel(dom->getCurrentChannel())->getResult(index.row())->getOther("point");
        if (index.column()==1) return dom->getChannel(dom->getCurrentChannel())->getResult(index.row())->getOther("date");
        if (index.column()==2) return dom->getChannel(dom->getCurrentChannel())->getResult(index.row())->getOther("time");
        if (index.column()==13) return dom->getChannel(dom->getCurrentChannel())->getResult(index.row())->getCalculation("av");
        if (index.column()==14) return dom->getChannel(dom->getCurrentChannel())->getResult(index.row())->getCalculation("delta");
        if (index.column()==15) return dom->getChannel(dom->getCurrentChannel())->getResult(index.row())->getCalculation("neoprIzm");
        if (index.column()==16) return dom->getChannel(dom->getCurrentChannel())->getResult(index.row())->getCondition("temperature");
        if (index.column()==17) return dom->getChannel(dom->getCurrentChannel())->getResult(index.row())->getCondition("humidity");
        if (index.column()==18) return dom->getChannel(dom->getCurrentChannel())->getResult(index.row())->getCondition("pressure");
        if (index.column()==19) return dom->getChannel(dom->getCurrentChannel())->getResult(index.row())->getCondition("voltage");
        return 1;//dom->getChannel(index.row()).getChannelData().value(dom->getSetting("field"+QString::number(index.column())));
        break;
    case Qt::EditRole:
        return 1;//dom->getChannel(index.row()).getChannelData().value(dom->getSetting("field"+QString::number(index.column())));
        break;
    default:
        return QVariant();
        break;
    }
}

Qt::ItemFlags AXMLCalibrationResultModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEnabled|Qt::ItemIsSelectable|Qt::ItemIsEditable;
}

QVariant AXMLCalibrationResultModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (!dom->isDomValid()) return QVariant();
    QString header;
    if (role==Qt::DisplayRole)
        switch (orientation) {
        case Qt::Horizontal:
            switch (section) {
            case 0: header="Точка";
                break;
            case 1: header="Дата";
                break;
            case 2: header="Время";
                break;
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
                header="N"+QString::number(section-2);
                break;
            case 13: header="Ср.Знач.";
                break;
            case 14: header="Откл.";
                break;
            case 15: header="Неопр.";
                break;
            default:
                header="def";
                break;
            }
            return header;
            break;
        case Qt::Vertical:
            return section+1;
            break;
        default:
            return QVariant();
            break;
        }
    return QVariant();
}


AResultCalibration::AResultCalibration(QObject *parent)
{

}

AResultCalibration::~AResultCalibration()
{
    this->results.clear();
    this->conditions.clear();
}
QHash<QString, QString> AResultCalibration::getCalculations() const
{
    return calculations;
}

QVector<AMeasuringDevice> AResultCalibration::getDevices() const
{
    return devices;
}

QHash<QString, QString> AResultCalibration::getOthers() const
{
    return others;
}

void AMeasuringDevice::addParam(QString key, QString value)
{
    if (key=="Type"||key=="SN"||key=="SKN"||key=="SROK"||key=="role")
        this->param.insert(key, value);
}

QHash<QString, QString> AMeasuringDevice::getParam() const
{
    return param;
}
void AMeasuringDevice::setParam(const QHash<QString, QString> &value)
{
    param = value;
}


