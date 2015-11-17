#ifndef QSLIPMESSAGE_H
#define QSLIPMESSAGE_H

#include "qinterfacemessage.h"
#define	USART_SLIP_DATA	                0xc0
#define NSL_CRC_POLYNOM   	0x8005
class QSlipMessage : public QInterfaceMessage
{
    Q_OBJECT
public:

    explicit QSlipMessage(QObject *parent = 0,const QByteArray & buf = QByteArray());
    void create(uint32 addr,uint8 cmd, uint8 * pData,uint32 len);
    PARSE_RESULT parse();
    virtual void clear();
    uint16 calc_crc(uint8 * data, uint32 len );
    uint8 getCmd(){return cmd;}
    //получить данные команды
    QByteArray getData();
    //тип сообщения
    virtual int messageType()const{return 2;}
signals:

public slots:
private:
    uint32 addr;
    uint8 cmd;
    QByteArray cmd_data;
};

#endif // QSLIPMESSAGE_H
