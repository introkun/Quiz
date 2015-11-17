#include "qinterfacemessage.h"

QInterfaceMessage::QInterfaceMessage(QObject *parent,const QByteArray & buf) :
    QObject(parent)
{
    this -> buf = buf;
    wait_ack = false;
}

//copy-конструктор
QInterfaceMessage::QInterfaceMessage(const QInterfaceMessage & message)
{
    buf = message.buf;
    wait_ack = message.wait_ack;
    good_ack = message.good_ack;
}

//оператор присваивания
QInterfaceMessage &QInterfaceMessage::operator=(const QInterfaceMessage & message)//для операторов =
{
    buf = message.buf;
    wait_ack = message.wait_ack;
    good_ack = message.good_ack;
    return *this;
}


//очистка объекта сообщения
void QInterfaceMessage::clear()
{
    buf.clear();
    wait_ack = false;
    good_ack = 0;
}


//создать шаблон ответа для сверки(ans_value - корректный ответ)
void QInterfaceMessage::createWaitAck(uint16 ans_value)
{
    good_ack = ans_value;
    wait_ack = true;
}


//добавить в команду уникальный индекс(возвращает false в случае ошибки)
bool QInterfaceMessage::setUniqueIndex(char value)
{
    QByteArray exch;
    if (buf.count() < 9)
        return false;
    char ch = buf.at(8);
    ch &= 0x8f;
    ch |= ((value & 0x7) << 4);
    exch.append(ch);
    buf = buf.replace(8,1,&ch,1);

    return true;
}
