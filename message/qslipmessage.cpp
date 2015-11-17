#include "qslipmessage.h"

QSlipMessage::QSlipMessage(QObject *parent,const QByteArray & buf) :
    QInterfaceMessage(parent,buf)
{
}


void QSlipMessage::create(uint32 addr,uint8 cmd, uint8 * pData,uint32 len)
{
    this -> cmd = cmd;
    this -> addr = addr;
    buf.clear();
    buf.append(0xc0);
    buf.append(3);
    buf.append((const char *)&addr,sizeof(addr));
    buf.append((char)0);
    buf.append(cmd);
    cmd_data.clear();
    if (len && pData)
    {
        cmd_data.append((const char *)pData,len);
        buf.append((const char *)pData,len);
    }
    buf.append((char)0);
    buf.append((char)0);
    uint16 crc = calc_crc((uint8 *)(buf.data() + 1), buf.count() - 1);
    buf.remove(buf.count() - 2,2);
    buf.append((int8)((crc >> 8) & 0xff));
    buf.append((int8)(crc & 0xff));
    buf.append(0xc0);
    return;
}


QInterfaceMessage::PARSE_RESULT QSlipMessage::parse()
{
    if (buf.count() <= 2)
        return PARSE_UNKNOWN_CMD;
    uint16 crc = calc_crc((uint8 *)(buf.data() + 1), buf.count() - 2);
    if (crc)
        return PARSE_BAD_CRC;
    cmd = 0;
    if (buf.length() >= 8)
        cmd = (uint8)buf.at(7);
    cmd_data.clear();
    if (buf.length() > 11)
        cmd_data.append((const char *)&buf.data()[8],buf.length() - 11);
    return PARSE_OK;
}


void QSlipMessage::clear()
{
    QInterfaceMessage::clear();
    addr = 0;
    cmd = 0;
    cmd_data.clear();
}


//получить данные команды
QByteArray QSlipMessage::getData()
{
    return cmd_data;
}


uint16 QSlipMessage::calc_crc(uint8 * data, uint32 len )
{
        int16 crc = 0;
        uint32 i;
        int8 a,j;

        for(i=0; i<len; i++)
        {
                a = data[i];
                for(j=0; j<8; j++)
                {
                        if ( crc& 0x8000 )
                        {
                                crc<<= 1;
                                if (a&0x80) crc |= 0x01;

                                crc ^= NSL_CRC_POLYNOM;
                        }
                        else
                        {
                                crc<<= 1;
                                if (a&0x80) crc |= 0x01;
                        }
                        a <<= 1;
                }
        }

        return crc;
}

