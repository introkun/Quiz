#ifndef QAUTOCONNECTTOOL_H
#define QAUTOCONNECTTOOL_H
/*
Описание класса утилиты для автоматического
выбора порта com, на котором виден модем
Е.А.Дроздов, 2013
*/
#include <QObject>
#include <QStringList>
#include "interface_manager.h"

class QAutoConnectTool : public QObject
{
    Q_OBJECT
public:
    explicit QAutoConnectTool(QObject *parent = 0,QInterface_Manager * manager = NULL);
    virtual ~QAutoConnectTool();
    //получить список доступных COM портов
    QStringList getSystemSerials();
    //определить порт для устройства. Функция не возвращает номер порта. Его возвращает сигнал signalPortFound(только номер первого найденного порта!!!)
    //keep_connect- оставить ли подключенным порт после того, как устройство найдено
    bool findDevicePort(bool keep_connect = false);

    uint16 getVersionSoft(){return version_soft;}
    uint16 getVersionHard(){return version_hard;}
signals:

    void signalOpenPort(const QString & port);
    void signalClosePort(const QString & port);
    //сигнал нахождения на порте port нужного нам устройства
    void signalPortFound(const QString & port);
    //сигнал "не найден порт устройства"
    void signalNoPortFound();
private slots:
    //получен ответ от устройства buf
    void slotIncomeMessage(const QByteArray & buf);
    //таймаут ожидания ответа от порта
    void slotTimeout();
    //слот об успешном/неудачном открытии порта
    void slotOpenStatus(bool value);
private:
    QInterface_Manager * manager;
    QStringList port_list;
    int port_index;
    bool port_opened;//состояние открытости порта
    bool keep_connect;//сохранять ли подключение после того, как устройство было обнаружено
    bool port_found;//найден ли порт
    //послать сообщение запроса версии устройства
    void postGetVersionMsg();
    //подключиться к текущему порту
    void connectToCurrentPort();
    uint16 version_soft;
    uint16 version_hard;
};

#endif // QAUTOCONNECTTOOL_H
