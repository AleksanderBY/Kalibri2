#include "axmlcalibrationmodel.h"
#include <QDebug>
#include <math.h>
#include <QCryptographicHash>

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
//        dom->setCurrentChannel(index);
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
    return  dom->tChannelList.count(); //dom->channelCount();
}

int AXMLCalibrationModel::columnCount(const QModelIndex &parent) const
{
    if (!dom->isDomValid()) return 0;
    return  dom->getSetting("fieldscount").toInt();
}

QVariant AXMLCalibrationModel::data(const QModelIndex &index, int role) const
{
     //   return QVariant();
    if (!index.isValid()) return QVariant();
    if (!dom->isDomValid()) return QVariant();

    if (modelLevel==0){
        switch (role) {
        case Qt::DisplayRole:
            return dom->tChannelList.value(index.row()).channelInfo.value(dom->settings.value("field"+QString::number(index.column()))); //dom->getChannel(index.row())->getChannelData().value(dom->getSetting("field"+QString::number(index.column())));
            break;
        case Qt::EditRole:
            return dom->tChannelList.value(index.row()).channelInfo.value(dom->settings.value("field"+QString::number(index.column()))); //dom->getChannel(index.row())->getChannelData().value(dom->getSetting("field"+QString::number(index.column())));
            break;
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        default:
            return QVariant();
            break;
        }
    }
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
    this->settings.clear();
    this->column=0;
}

bool ADomCalibration::save(QString fileName)
{
    QFile file;
    file.setFileName(fileName);
    if (!file.open(QIODevice::WriteOnly|QIODevice::Text)){
        return false;
    }
    writer->setAutoFormatting(true);
    writer->setDevice(&file);
    writer->writeStartDocument();
    writer->writeStartElement("body");
    foreach (QString key, this->settings.keys()) {
        writer->writeAttribute(key, settings.value(key));
    }
    foreach (TChannelCalibration channel, tChannelList) {
        writer->writeStartElement("channel");
        foreach (QString key, channel.channelInfo.keys()) {
            writer->writeAttribute(key, channel.channelInfo.value(key).toString());
        }

        foreach (TCalibration calibration, channel.CalibtationList) {
            writer->writeStartElement("calibration");
            foreach (QString keyCal, calibration.calibrationInfo.keys()) {
                writer->writeAttribute(keyCal,calibration.calibrationInfo.value(keyCal).toString());
            }
            foreach (TPoint point, calibration.pointList) {
                writer->writeStartElement("point");
                foreach (QString keyPoint, point.pointInfo.keys()) {
                    writer->writeAttribute(keyPoint, point.pointInfo.value(keyPoint).toString());
                }
                if (point.results.count()>0) {
                    writer->writeStartElement("results");
                    foreach (QString keyRes, point.results.keys()) {
                        writer->writeAttribute(keyRes, point.results.value(keyRes).toString());
                    }
                    writer->writeEndElement();
                }
                if (point.calculations.count()>0) {
                    writer->writeStartElement("calculations");
                    foreach (QString keyCalc, point.calculations.keys()) {
                        writer->writeAttribute(keyCalc, point.calculations.value(keyCalc).toString());
                    }
                    writer->writeEndElement();
                }
                writer->writeEndElement();
            }
            if (calibration.conditions.count()>0) {
                writer->writeStartElement("conditions");
                foreach (QString keyCond, calibration.conditions.keys()) {
                    writer->writeAttribute(keyCond, calibration.conditions.value(keyCond).toString());
                }
                writer->writeEndElement();
            }
            if (calibration.deviceList.count()>0) {
                writer->writeStartElement("devices");
                foreach (TDevice device, calibration.deviceList) {
                    writer->writeStartElement("device");
                    foreach (QString dev, device.deviceInfo.keys()) {
                        writer->writeAttribute(dev, device.deviceInfo.value(dev).toString());
                    }
                    writer->writeEndElement();
                }
                writer->writeEndElement();
            }
            writer->writeEndElement();
        }

        writer->writeEndElement();
    }
    writer->writeEndDocument();
    return true;
}


bool ADomCalibration::parsing()
{
    tChannelList.clear();
    TChannelCalibration readChannel;
    TCalibration readCalibration;
    TPoint readPoint;
    TDevice readDevice;
    //Удаляем ранее загруженные данные
    this->clear();
    //Вспомогательная переменная для хрнения типа тега
    int tag=-1;
    int lastTag=-1;
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
            temp.clear();
            //Читаем настройки базы данных
            if (reader->name()=="body") {
                //Получаем атрибуты стартового элемента
                temp = reader->attributes();
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
                //Проверяем версию файла
                if (settings.value("version")!="3") {
                    log(tr("Ошибка файла: неправильная версия файла"), Qt::red);
                    return false;
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
            }
            //Читаем каналы
            if (reader->name()=="channel") {
                readChannel.channelInfo.clear();
                readChannel.CalibtationList.clear();
                temp = reader->attributes();
                if (temp.count()>0)
                    foreach (QXmlStreamAttribute attr, temp) {
                        readChannel.channelInfo.insert(attr.name().toString(),attr.value().toString());
                    }
            }
            //читаем калибровки
            if (reader->name()=="calibration") {
                //Очищаем переменную для новых результатов кальбровки
                readCalibration.calibrationInfo.clear();
                readCalibration.pointList.clear();
                readCalibration.deviceList.clear();
                readCalibration.conditions.clear();
                temp = reader->attributes();
                if (temp.count()>0)
                    foreach (QXmlStreamAttribute attr, temp) {
                        readCalibration.calibrationInfo.insert(attr.name().toString(),attr.value().toString());
                    }
            }
            //читаем точки
            if (reader->name()=="point") {
                readPoint.pointInfo.clear();
                readPoint.results.clear();
                readPoint.calculations.clear();
                temp = reader->attributes();
                if (temp.count()>0)
                    foreach (QXmlStreamAttribute attr, temp) {
                        readPoint.pointInfo.insert(attr.name().toString(),attr.value().toString());
                    }
            }

            //читаем результаты
            if (reader->name()=="results") {
                readPoint.results.clear();
                temp = reader->attributes();
                if (temp.count()>0)
                    foreach (QXmlStreamAttribute attr, temp) {
                        readPoint.results.insert(attr.name().toString(),attr.value().toString());

                    }
            }
            //читаем расчеты
            if (reader->name()=="calculations") {
                readPoint.calculations.clear();
                temp = reader->attributes();
                if (temp.count()>0)
                    foreach (QXmlStreamAttribute attr, temp) {
                        readPoint.calculations.insert(attr.name().toString(),attr.value().toString());
                    }
            }
            //читаем условия
            if (reader->name()=="conditions") {
                readCalibration.conditions.clear();
                temp = reader->attributes();
                if (temp.count()>0)
                    foreach (QXmlStreamAttribute attr, temp) {
                        readCalibration.conditions.insert(attr.name().toString(),attr.value().toString());
                    }
            }
            //читаем измерительные приборы
            if (reader->name()=="device") {
                readDevice.deviceInfo.clear();
                temp = reader->attributes();
                if (temp.count()>0)
                    foreach (QXmlStreamAttribute attr, temp) {
                        readDevice.deviceInfo.insert(attr.name().toString(),attr.value().toString());
                    }
            }

        }
        if (token == QXmlStreamReader::EndElement) {
            if (reader->name()=="device") readCalibration.deviceList.push_back(readDevice);

            if (reader->name()=="point") {
                //Если нету результатов расчета то получаем их
                if (readPoint.calculations.count()<1) {
                    double summa = 0;
                    QList<double> p;
                    foreach (QString key, readPoint.results.keys()) {
                        p.push_back(readPoint.results[key].toString().replace(",",".").toDouble());
                        summa += p.last();
                    }
                    int n = p.count();
                    double av = summa/n;
                    readPoint.calculations.insert("av", QString::number(av,'g',12));
                    double delta = readPoint.pointInfo.value("value").toDouble()-av;
                    readPoint.calculations.insert("delta", QString::number(delta,'g',12));
                    //Вычисляем неопределенность измерения
                    double summa2=0;
                    foreach (double p_i, p) {
                        summa2 += pow(p_i-av, 2);
                    }
                    double koef = 1.0/(n*(n-1));

                    double neoprIzm = sqrt(koef*summa2);
                    readPoint.calculations.insert("neoprIzm", QString::number(neoprIzm,'g',12));
                }
                readCalibration.pointList.push_back(readPoint);
            }
            if (reader->name()=="calibration") {
                readChannel.CalibtationList.push_back(readCalibration);
            }
            if (reader->name()=="channel") {
                tChannelList.push_back(readChannel);
            }
        }
        if (token == QXmlStreamReader::EndDocument) {
            return true;
        }
    }
    return false;
}


bool AXMLCalibrationModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        dom->tChannelList[index.row()].channelInfo.insert(dom->settings.value("field"+QString::number(index.column())), value.toString());
        //dom->getChannel(index.row())->addChannelData(dom->getSetting("field"+QString::number(index.column())), value.toString());
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
    if (dom->tChannelList[dom->getCurrentChannel()].CalibtationList.count()<=0) return 0;
    return dom->tChannelList[dom->getCurrentChannel()].CalibtationList.last().pointList.count(); //dom->getChannel(dom->getCurrentChannel())->getResultsCount();

}

int AXMLCalibrationResultModel::columnCount(const QModelIndex &parent) const
{
    if (!dom->isDomValid()) return 0;
    return 16;
}

QVariant AXMLCalibrationResultModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();
    if (!dom->isDomValid()) return QVariant();
    switch (role) {
    case Qt::DisplayRole:

        if (index.column()>2&&index.column()<13) {
            return dom->tChannelList[dom->getCurrentChannel()].CalibtationList.last().pointList[index.row()].results.value("V"+QString::number(index.column()-2));
        }
        if (index.column()==0) return dom->tChannelList[dom->getCurrentChannel()].CalibtationList.last().pointList[index.row()].pointInfo.value("value");
        if (index.column()==1) return dom->tChannelList[dom->getCurrentChannel()].CalibtationList.last().calibrationInfo.value("date");
        if (index.column()==2) return dom->tChannelList[dom->getCurrentChannel()].CalibtationList.last().calibrationInfo.value("time");
        if (index.column()==13) return dom->tChannelList[dom->getCurrentChannel()].CalibtationList.last().pointList[index.row()].calculations.value("av");
        if (index.column()==14) return dom->tChannelList[dom->getCurrentChannel()].CalibtationList.last().pointList[index.row()].calculations.value("delta");
        if (index.column()==15) return dom->tChannelList[dom->getCurrentChannel()].CalibtationList.last().pointList[index.row()].calculations.value("neoprIzm");
//        if (index.column()==13) return dom->getChannel(dom->getCurrentChannel())->getResult(index.row())->getCalculation("av");
//        if (index.column()==14) return dom->getChannel(dom->getCurrentChannel())->getResult(index.row())->getCalculation("delta");
//        if (index.column()==15) return dom->getChannel(dom->getCurrentChannel())->getResult(index.row())->getCalculation("neoprIzm");
//        if (index.column()==16) return dom->getChannel(dom->getCurrentChannel())->getResult(index.row())->getCondition("temperature");
//        if (index.column()==17) return dom->getChannel(dom->getCurrentChannel())->getResult(index.row())->getCondition("humidity");
//        if (index.column()==18) return dom->getChannel(dom->getCurrentChannel())->getResult(index.row())->getCondition("pressure");
//        if (index.column()==19) return dom->getChannel(dom->getCurrentChannel())->getResult(index.row())->getCondition("voltage");
//        return 1;//dom->getChannel(index.row()).getChannelData().value(dom->getSetting("field"+QString::number(index.column())));
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


bool AXMLCalibrationResultModel::insertRows(int row, int count, const QModelIndex &parent)
{
    emit this->beginInsertRows(parent,row, row+count-1);

    emit this->endInsertRows();
    return true;
}


AXMLCalibrationListModel::AXMLCalibrationListModel(ADomCalibration *dom, QObject *parent)
{
    this->dom=dom;
}

int AXMLCalibrationListModel::rowCount(const QModelIndex &parent) const
{
    return dom->tChannelList.value(dom->getCurrentChannel()).CalibtationList.count();
}

QVariant AXMLCalibrationListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();
    TCalibration calibration = dom->tChannelList.value(dom->getCurrentChannel()).CalibtationList.value(index.row());
    if (role == Qt::DisplayRole)
            return calibration.calibrationInfo.value("date").toString()+"/"+calibration.calibrationInfo.value("time").toString()+" - "+calibration.calibrationInfo.value("uuid").toString();
        else
            return QVariant();
}


Qt::ItemFlags AXMLCalibrationListModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEnabled|Qt::ItemIsSelectable;
}


int AXMLCalibrationListModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}
