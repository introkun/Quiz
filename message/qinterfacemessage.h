#ifndef QINTERFACEMESSAGE_H
#define QINTERFACEMESSAGE_H
#include "typedefs.h"
#include <QObject>
//базовый класс для сообщений интерфейса
class QInterfaceMessage : public QObject
{
    Q_OBJECT
public:
    typedef enum {
        PARSE_OK = 0,
        PARSE_BAD_CRC,
        PARSE_UNKNOWN_CMD
    }PARSE_RESULT;

    explicit QInterfaceMessage(QObject *parent = 0,const QByteArray & buf = QByteArray());
    QInterfaceMessage(const QInterfaceMessage & message);
    virtual QInterfaceMessage &operator=(const QInterfaceMessage &);//для операторов =
    virtual PARSE_RESULT parse(){return PARSE_UNKNOWN_CMD;}//распарсивание буфера buf
    //очистка объекта сообщения
    virtual void clear();
    QByteArray getBuf() const{return buf;}
    void setBuf(const QByteArray & buf){this -> buf = buf;}
    //тип сообщения
    virtual int messageType()const{return 0;}
    //создать шаблон ответа для сверки(ans_value - корректный ответ)
    virtual void createWaitAck(uint16 ans_value);
    //ожидается ли ответ на команду
    virtual bool isWaitAck() const{return wait_ack;}
    //добавить в команду уникальный индекс(возвращает false в случае ошибки)
    virtual bool setUniqueIndex(char value);

    bool isEmpty(){return buf.isEmpty();}
protected:
    //подсчет crc - абстрактная функция
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
    virtual uint16 calc_crc(uint8 * data, uint32 len){return 0;}
#pragma GCC diagnostic pop
    //буфер сообщения
    QByteArray buf;
    uint16 good_ack;
    bool wait_ack;//ожидать ли ответа
};

#endif // QINTERFACEMESSAGE_H
