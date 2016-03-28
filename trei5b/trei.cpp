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
    supportTypesList.append("M732U 4-20mA");
    supportTypesList.append("M732U 0-5mA");
    supportTypesList.append("M732U +-78mV/L");
    supportTypesList.append("M732U +-78mV/K");
    supportTypesList.append("M745A +-78mV/L");
    supportTypesList.append("M745A +-78mV/K");
    supportTypesList.append("M732U 100Om/50M");
    supportTypesList.append("M745A 100Om/50M");
    supportTypesList.append("M732U 200Om/50P");
    supportTypesList.append("M745A 200Om/50P");
    dlg.setSupportType(supportTypesList);
    dlg.setModal(true);

}

QString Trei::getName()
{
    return "ISAGRAF 3.3 TCP/IP";
}

bool Trei::initialization(QAbstractItemModel *model)
{
    this->model = model;
    //устанавливаем модель в диалоге редактора
    dlg.setModel(model);
    //инициализируем winsock
    WSAData wsadata;
    int err  = WSAStartup(MAKEWORD(2,0), &wsadata);
    if (err == SOCKET_ERROR)
    {
        lastError="Не удалось инициализировать библиотеку WINSOCK2";
        return false;
    }
    return true;
}

//создание подключения к устройству
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
//    qDebug()<<hostEnt->h_name;
//    qDebug()<<hostEnt->h_addrtype;
//    qDebug()<<hostEnt->h_addr_list;
//    qDebug()<<hostEnt->h_aliases;
//    qDebug()<<hostEnt->h_length;
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



bool Trei::getValues(QList<PollClass *> *pollList)
{
    //Создаем список необходимых устройств (как правило будет одно)
    QMap<QString, int> controllerList;
    for (int i=0; i<pollList->count();i++) {
        //Проверяем существует ли поле имени контроллера в запросе
        if (pollList->at(i)->attr.contains("controller")) {
            qDebug()<<"Контроллер: "+pollList->at(i)->attr.value("controller");
            //Проверяем добавлен ли уже контроллер в список
            if (!controllerList.contains(pollList->at(i)->attr.value("controller"))) {
               //создаем соединение с контроллером
               int controllerDevice = this->createDevice(pollList->at(i)->attr.value("controller"));
               //если соединение успешно то добавляем его в список
               if (controllerDevice) {
                   controllerList.insert(pollList->at(i)->attr.value("controller"), controllerDevice);
                   qDebug()<<"Socket:"+QString::number(controllerDevice);
               }
               else
               {
                   qDebug()<<"Ошибка связи";
                   return false;
               }
            }
        }
        else
        {
            //возвращаем ошибку
            this->lastError = "Не правильный запрос: Не существует поле 'controller'";
            return false;
        }
    }
    //----------------------------------------------------------------
    //              Запрос параметров
    //--------------------------------------------------------------
    foreach (QString key, controllerList.keys()) {
        //Создаем буферы для запроса и ответа
        uchar *question_buf = new uchar[1024];
        uchar *answer_buf = new uchar[1024];
        //Подготавливаем список к отправке в контроллер
        net_open_list(question_buf,1,0);
        bool ok;
        //Помещаем адреса в список
        for (int i=0; i<pollList->count();i++) {
            //Проверяем текущий ли контроллер
            if (pollList->at(i)->attr.value("controller")==key) {
                //Проверяем существует ли поле adress
                if (pollList->at(i)->attr.contains("adress")) {
                    //Преобразуем адрес в десятичное число
                    quint16 adrr = pollList->at(i)->attr.value("adress").toInt(&ok,16);
                    //Если преобразование успешно то добавляем в список
                    if (ok) net_add_list(adrr);
                    else {
                        qDebug()<<"Неправильный формат поля адреса";
                        return false;
                    }
                }
                else {
                    qDebug()<<"Отсутствует поле adress";
                    return false;
                }
            }
        }
        //Закрываем список
        net_close_list();
        //--- Отправляем запрос контроллеру
        net_tcp_request(controllerList.value(key), question_buf, answer_buf);
        //Проверяем статус ответа
        if (!net_status_list(answer_buf)) qDebug()<<"Ответ получен";
        else {
            qDebug()<<"Получен неправильный ответ контроллера";
            return false;
        }

        //-- Извлекаем данные из буфера ответа

        for (int i=0; i<pollList->count();i++) {
            if (pollList->at(i)->attr.value("controller")==key) {
                float val;
                float deltaP;
                net_get_list(&val);
                //Добавляем поправку
                if (pollList->at(i)->attr.contains("deltaP")) {
                    deltaP = pollList->at(i)->attr.value("deltaP").toDouble();
                }
                else {
                    deltaP = 0.0;
                }
                pollList->at(i)->value=val-deltaP;
                //qDebug()<<val;
            }
        }
        //уничтожаем буферы
        delete question_buf;
        delete answer_buf;
    }
    return true;
}

bool Trei::getFieldsDB(QVector<QString> *fieldsDB)
{
    fieldsDB->clear();
    fieldsDB->push_back("controller");
    fieldsDB->push_back("module");
    fieldsDB->push_back("pipe");
    fieldsDB->push_back("cipher");
    fieldsDB->push_back("name");
    fieldsDB->push_back("type");
    fieldsDB->push_back("min");
    fieldsDB->push_back("max");
    fieldsDB->push_back("unity");
    fieldsDB->push_back("adress");
    fieldsDB->push_back("point1");
    fieldsDB->push_back("point2");
    fieldsDB->push_back("point3");
    fieldsDB->push_back("point4");
    fieldsDB->push_back("TK");
    fieldsDB->push_back("deltaP");
    fieldsDB->push_back("note");
    return true;
}

QStringList *Trei::supportTypes()
{
    return &supportTypesList;
}

int Trei::editDialog(int row)
{
    //НЕОБХОДИМО УСТАНОВИТЬ СПИСОК КАНАЛОВ ТЕРМОКОМПЕНСАЦИИИ
    int rowCount = model->rowCount();
    QStringList TKList;
    if (rowCount>0) {
        for (int i=0; i<rowCount; i++) {
            QModelIndex index = model->index(i,5);
            QString tk = model->data(index, Qt::DisplayRole).toString();
            if (tk=="M732U 100Om/50M"||tk=="M745A 100Om/50M"||tk=="M732U 200Om/50P"||tk=="M745A 200Om/50P") {
                QString kodTK = model->data(model->index(i,1), Qt::DisplayRole).toString().rightJustified(2,'0')+model->data(model->index(i,2), Qt::DisplayRole).toString().rightJustified(2,'0');
                TKList.append(kodTK);
            }
        }
        dlg.setTKLIst(TKList);
    }

    dlg.setRow(row);

    return dlg.exec();
}

QList<double> Trei::getPoints(QList<PollClass *> *pollList)
{
    //Создаем список точек
    QList<double> listPoint;
    for (int i=0; i<pollList->count();i++) {
        pollList->at(i)->points.clear();
        //Проверяем наличие поля тип измерительного канала
        if (!pollList->at(i)->attr.contains("type")) {
            qDebug()<<"Отсутствует поле типа канала";
            continue;
        }
        //Проверяем поддержку типа измерительного канала
        if (!supportTypesList.contains(pollList->at(i)->attr.value("type"))) {
            qDebug()<<"Тип измерительного канала не поддерживается";
            continue;
        }
        QString mType = pollList->at(i)->attr.value("type");
        //Проверяем наличие поля min
        if (!pollList->at(i)->attr.contains("min")) {
            qDebug()<<"Отсутствует поле min";
            continue;
        }
        //Проверяем наличие поля max
        if (!pollList->at(i)->attr.contains("max")) {
            qDebug()<<"Отсутствует поле max";
            continue;
        }
        //Проверяем правильность полей min, max
        double PMin,PMax;
        bool ok;
        PMin = pollList->at(i)->attr["min"].replace(',','.').toDouble(&ok);
        if (!ok) {
            qDebug()<<"Неправильное поле min";
            continue;
        }
        PMax = pollList->at(i)->attr["max"].replace(',','.').toDouble(&ok);
        if (!ok) {
            qDebug()<<"Неправильное поле max";
            continue;
        }
        if (PMin>=PMax) {
            qDebug()<<"Неверный диапазон";
            continue;
        }



        measurement measurementType = pollList->at(i)->measurementType;
        double EMin, EMax;
        switch (measurementType) {
        case mA:
            if (mType== "M745A 0-5mA"||mType=="M732U 0-5mA") {
                EMin = 0; EMax = 5;
            }
            if (mType== "M745A 4-20mA"||mType=="M732U 4-20mA") {
                EMin = 4; EMax = 20;
            }
            for (int j=1; j<=4; j++) {
                //Проверяем наличие поля тип измерительного канала
                if (!pollList->at(i)->attr.contains("point"+QString::number(j))) {
                    qDebug()<<"Точка:"+QString::number(j)+" отсутствует";
                    continue;
                }
                //Проверяем корректность поля точки канала
                double PPoint = pollList->at(i)->attr["point"+QString::number(j)].replace(',','.').toDouble(&ok);
                if (!ok) {
                    qDebug()<<"Неправильное значение поля точки "+QString::number(j);
                    continue;
                }
                double EPoint = (EMax-EMin)*(PPoint-PMin)/(PMax-PMin)+EMin;
                pollList->at(i)->points.insert(pollList->at(i)->attr.value("point"+QString::number(j)), EPoint);
                listPoint.push_back(EPoint);
            }
            break;
        case CTypeXK:
        case CTypeS:
        case CTypeB:
        case CTypeK:
        case CTypeJ:
        case C50M1_428:
        case C50M1_426:
        case C100M1_428:
        case C100M1_426:
        case C50P:
        case C100P:
            for (int j=1; j<=4; j++) {
                //Проверяем наличие поля тип измерительного канала
                if (!pollList->at(i)->attr.contains("point"+QString::number(j))) {
                    qDebug()<<"Точка:"+QString::number(j)+" отсутствует";
                    continue;
                }
                //Проверяем корректность поля точки канала
                double PPoint = pollList->at(i)->attr["point"+QString::number(j)].replace(',','.').toDouble(&ok);
                if (!ok) {
                    qDebug()<<"Неправильное значение поля точки "+QString::number(j);
                    continue;
                }
                pollList->at(i)->points.insert(pollList->at(i)->attr.value("point"+QString::number(j)), PPoint);
                listPoint.push_back(PPoint);
            }
            break;
        case Om:
            if (mType== "M732U 100Om/50M"||mType=="M745A 100Om/50M"||mType=="M732U 200Om/50P"||mType == "M745A 200Om/50P") {
                for (int j=1; j<=4; j++) {
                    //Проверяем наличие поля тип измерительного канала
                    if (!pollList->at(i)->attr.contains("point"+QString::number(j))) {
                        qDebug()<<"Точка:"+QString::number(j)+" отсутствует";
                        continue;
                    }
                    //Проверяем корректность поля точки канала
                    double PPoint = pollList->at(i)->attr["point"+QString::number(j)].replace(',','.').toDouble(&ok);
                    if (!ok) {
                        qDebug()<<"Неправильное значение поля точки "+QString::number(j);
                        continue;
                    }
                    int indexK;
                    double EPoint;
                    //Расчитываем сопротивление для медного термосопротивления 50М
                    if (mType== "M732U 100Om/50M"||mType=="M745A 100Om/50M") {
                        indexK = static_cast<int>(PPoint)+201;
                        EPoint = K1428[indexK]*50;
                    }
                    //Расчитываем сопротивление для платинового термосопротивления 50П
                    if (mType== "M732U 200Om/50P"||mType=="M745A 200Om/50P") {
                        indexK = static_cast<int>(PPoint);
                        EPoint = P1385[indexK]*50;
                        qDebug()<<"Платиновый термометр, точка - " + QString::number(EPoint);
                    }

                    pollList->at(i)->points.insert(pollList->at(i)->attr.value("point"+QString::number(j)), EPoint);
                    listPoint.push_back(EPoint);
                }
            }
            break;
        default:
            break;
        }

        //Передаем в основную программу паузы между запросами
        if (mType=="M745A 4-20mA"||mType== "M745A 0-5mA"||mType== "M745A +-78mV/L"||mType== "M745A 100Om/50M") {
            pollList->at(i)->startDelay = 3000;
            pollList->at(i)->delay = 3000;
        }

        if (mType=="M732U 4-20mA"||mType== "M732U 0-5mA"||mType== "M732U +-78mV/L"||mType== "M732U 100Om/50M") {
            pollList->at(i)->startDelay = 1000;
            pollList->at(i)->delay = 1000;
        }


    }
    QSet<double> temp = listPoint.toSet();
    listPoint = temp.toList();
    qSort(listPoint.begin(), listPoint.end());
    return listPoint;
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

//Проверяем возможность совместной калибровки каналов из списка
bool Trei::validationPollList(QList<PollClass *>  * pollList)
{

    //список пуст
    if (pollList->count()<=0) return false;
    //в списке одно значение
    if (pollList->count()==1) return true;
    //много значений
    QString firstType = pollList->at(0)->attr.value("type");
    for (int i=1; i<pollList->count();i++) {
        if (firstType!=pollList->at(i)->attr.value("type")) return false;
    }
    //qDebug()<<"Валидация прошла успешно";
    return true;
}
//Получаем список поддерживаемых типов задатчиков
QList<measurement> Trei::getMeasurementTypes(QList<PollClass *> *pollList)
{
    QList<measurement> list;
    if (pollList->count()<=0) return list;
    QString mType = pollList->at(0)->attr.value("type");
    if (mType=="M732U 4-20mA"||mType== "M732U 0-5mA"||mType=="M745A 4-20mA"||mType== "M745A 0-5mA"){
        list<<mA;
    }
    if (mType=="M732U +-78mV/L"||mType== "M745A +-78mV/L"){
        list<<CTypeXK;
        list<<mV;
    }
    if (mType=="M732U +-78mV/K"||mType== "M745A +-78mV/K"){
        list<<CTypeK;
        list<<mV;
    }
    if (mType=="M732U 100Om/50M"||mType=="M745A 100Om/50M"){
        list<<C50M1_428;
        list<<Om;
    }
    if (mType=="M732U 200Om/50P"||mType=="M745A 200Om/50P"){
        list<<C50P;
        list<<Om;
    }

    return list;
}





