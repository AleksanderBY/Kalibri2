#ifndef CALIBRATIONDATA
#define CALIBRATIONDATA

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


#endif // CALIBRATIONDATA

