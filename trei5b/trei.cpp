#include "trei.h"
#include "winsock2.h"
#include "QDebug"
#include <QMessageBox>

Trei::Trei()
{
    //создаем список подкдючений к разным устройствам
    this->socketNameList = new QVector<QString>;
    this->socketList = new QVector<int>;
    //this->supportTypesList = new QStringList();
    supportTypesList.append("M745A 4-20mA");
    supportTypesList.append("M745A 0-5mA");
    dlg.setSupportType(supportTypesList);
    dlg.setModal(true);

}

QString Trei::getName()
{
    return "ISAGRAF 3.3 TCP/IP";
}

bool Trei::initialization(QAbstractItemModel *model)
{
    dlg.setModel(model);
    //инициализируем winsock
    WSAData wsadata;
    int err  = WSAStartup(MAKEWORD(2,0), &wsadata);
    if (err == SOCKET_ERROR)
    {
        lastError="Не удалось инициализировать библиотеку WINSOCK2";
        return err;
    }
    return true;
}

int Trei::createDevice(QString desk)
{
    //Проверяем наличие данного устройства в списке
    if (this->socketNameList->contains(desk))
    {
        lastError = "Устройство было создано ранее";
        return this->socketList->at(this->socketNameList->indexOf(desk));
    }

    //Создаем новое подключение к устройству
    //Получаем информацию о подключении из хоста
    LPHOSTENT hostEnt;
    hostEnt = gethostbyname(desk.toLocal8Bit().data());
    qDebug()<<hostEnt->h_name;
    qDebug()<<hostEnt->h_addrtype;
    qDebug()<<hostEnt->h_addr_list;
    qDebug()<<hostEnt->h_aliases;
    qDebug()<<hostEnt->h_length;
    //gethostbyaddr("192.168.10.241",4,2);
    //устанавливаем соединение с контроллером
    int socket;
    int i = tcp_isa_connect(&socket, hostEnt, 1100);

    //Если соединение установленно
    if (!i) {
        //сохраняем его в списке и возвращаем его хэндл в качестве уникального параметра для дальнейшего использования
        socketNameList->push_back(desk);
        socketList->push_back(socket);
        return socket;
    }
    else
    {
        lastError="Не удалось установить соединение с контроллером.";
        return 0;
    }

    return 0;
}

QString Trei::getLastError()
{
    return lastError;
}

bool Trei::getValues(int device, QMap<QString, float> *valuesList)
{
    //Проверяем наличие устройства в списке
    if (!socketList->contains(device)) {
        lastError = "Неизвестное устройство";
        return false;
    }
    //Создаем буферы для запроса и ответа
    uchar *question_buf = new uchar[1024];
    uchar *answer_buf = new uchar[1024];
    //Подготавливаем список к отправке в контроллер
    net_open_list(question_buf,1,0);

    bool ok;
    //Помещаем адреса в список
    foreach (QString key, valuesList->keys()) {
        quint16 adrr = key.toInt(&ok,16);
        net_add_list(adrr);
    }

    net_close_list();
    //отправляем запрос
    net_tcp_request(device, question_buf, answer_buf);
    //Проверяем статус ответа
    net_status_list(answer_buf);
    //извлекаем данные из ответа и помещаем в список
    foreach (QString key, valuesList->keys()) {
        float val;
        net_get_list(&val);
        valuesList->insert(key, val);
    }

    return true;
}

bool Trei::getValues(QList<PollClass *> poll)
{

}

QList<double> Trei::getValues(QList<QSqlRecord> *sgList)
{
    //Получаем имя устройства
    QMap<QString, QString> devPoints;
    QList<double> s;
    for (int i=0; i<sgList->count(); i++)
    {

        qDebug()<<sgList->at(i).value(0).toString();
    }
    return s ;
}

bool Trei::getFieldsDB(QMap<QString, QString> *fieldsDB)
{
    fieldsDB->clear();
    fieldsDB->insert("01_controller", "STRING(20)");
    fieldsDB->insert("02_module", "STRING(20)");
    fieldsDB->insert("03_pipe", "STRING(20)");
    fieldsDB->insert("04_cipher", "STRING(20)");
    fieldsDB->insert("05_name", "STRING(60)");
    fieldsDB->insert("06_type", "STRING(20)");
    fieldsDB->insert("07_min", "STRING(20)");
    fieldsDB->insert("08_max", "STRING(20)");
    fieldsDB->insert("09_unit", "STRING(20)");
    fieldsDB->insert("10_adress", "STRING(20)");
    fieldsDB->insert("11_point1", "STRING(20)");
    fieldsDB->insert("12_point2", "STRING(20)");
    fieldsDB->insert("13_point3", "STRING(20)");
    fieldsDB->insert("14_point4", "STRING(20)");
    return true;
}

QStringList *Trei::supportTypes()
{
    return &supportTypesList;
}

int Trei::editDialog(int row)
{
    dlg.setRow(row);
    return dlg.exec();
}

int Trei::getDeviceField()
{
    return 1;
}

int Trei::getTagField()
{
    return 10;
}

QList<int> Trei::getPointsFields()
{
    QList<int> points;
    points<<11<<12<<13<<14;
    return points;
}

QList<double> Trei::getParametrValue(QSqlRecord record)
{
    QList<double> list;
    if (record.count()!=15) return list;
    //qDebug()<<record.value("type");
    QVariant _type = record.value("type");
    //qDebug()<<_type.typeName();
    if(!_type.isValid()) {lastError="Не верно поле TYPE"; return list;}
    double kTr;
    double Pmax, Pmin, Amax, Amin, Point;
    QString mType = _type.toString();

    if (record.value("max").isValid()) Pmax=record.value("max").toDouble();
    else {
        lastError="Неверно поле MAX";
        return list;
    }

    if (record.value("min").isValid()) Pmin=record.value("min").toDouble();
    else {
        lastError="Неверно поле MIN";
        return list;
    }

    for (int i=1; i<=4; i++)
    {
        Point=record.value("point"+QString::number(i)).toDouble();

        if (mType == "M745A 0-5mA") {
            Amax = 5; Amin = 0;
        }

        if (mType == "M745A 4-20mA") {
            Amax = 20; Amin = 4;
        }

        kTr=(Amax-Amin)*Point/(Pmax-Pmin)+Amin;
        list.push_back(kTr);
        //qDebug()<<kTr;
    }


    return list;
}

QString Trei::getTagAdress(QSqlRecord record)
{
    return record.value("adress").toString();
}





