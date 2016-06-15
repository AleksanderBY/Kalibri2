#ifndef CALIBRATIONDATA
#define CALIBRATIONDATA

//Измерительный прибор
class TDevice {
public:
    QVariantHash deviceInfo;                 //Информация о приборе
};
//Результаты измерений и расчетные данные для точки
class TPoint {
public:
    QVariantHash pointInfo;                  //Информация о точке калибровки
    QVariantHash results;                    //Результаты калибровки
    QVariantHash calculations;               //Расчеты для калибровки
    QVariantHash temp;                       //Временные данные
};
//Все результаты калибровки
class TCalibration {
public:
    QVariantHash calibrationInfo;            //Информация о калибровке
    QVariantHash conditions;                 //Условия проведения калибровки
    QVector<TPoint> pointList;                          //Список точек в калибровке
    QVector<TDevice> deviceList;                        //Список приборов в калибровке
};
//Канал калибровки
class TChannelCalibration {
public:
    QVariantHash channelInfo;                //Информация о канале
    QHash<QString, double> tempPoint;                   //Временные данные
    int startDelay, delay;
    QHash<QString, QString> temp;
    QVector<TCalibration> CalibtationList;              //Список всех проеведенных калибровок
};


#endif // CALIBRATIONDATA

