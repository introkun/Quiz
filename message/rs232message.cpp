
#include "rs232message.h"

QRS232Message::QRS232Message(QObject * parent,const QByteArray & buf):QInterfaceMessage(parent,buf)
{
}


//copy-конструктор
QRS232Message::QRS232Message(const QRS232Message & message):QInterfaceMessage(message)
{
    cmd = message.cmd;
    flags = message.flags;
    netro_buf = message.netro_buf;
    std_data = message.std_data;
    std_group = message.std_group;
    dev_id = message.dev_id;
    nx = message.nx;
    cnt = message.cnt;
    init_cmd = message.init_cmd;
    ext_cmd = message.ext_cmd;
}


//создание объекта QRS232Message на основании объекта QInterfaceMessage
QRS232Message::QRS232Message(const QInterfaceMessage & message):QInterfaceMessage(message)
{
    if (buf.count() >= 5)
    {
        if (buf.at(3) == USART_PROTOCOL_EXT_DATA)
        {
            if (buf.count() != (uint8)buf.at(4) + 2 + 5)
                return;
            cmd = ((uint8)buf.at(5)) | (((uint16)((uint8)buf.at(6))) << 8);
            flags = ((uint8)buf.at(7)) | (((uint16)((uint8)buf.at(8))) << 8);
            ext_cmd = true;
        }
        else if (buf.at(3) == USART_PROTOCOL_STD_DATA)
        {
            if (buf.count() != USART_SIZE_STD_DATA + 2 + 5)
                return;
            cmd = ((uint8)buf.at(7)) | (((uint16)((uint8)buf.at(8))) << 8);
            std_group = ((uint8)buf.at(5)) | (((uint16)((uint8)buf.at(6))) << 8);
            std_data = ((uint8)buf.at(9)) | (((uint16)((uint8)buf.at(10))) << 8);
            flags = ((uint8)buf.at(11)) | (((uint16)((uint8)buf.at(12))) << 8);
            ext_cmd = false;
        }
    }
}


QRS232Message &QRS232Message::operator=(const QRS232Message & message)//для операторов =
{

    cmd = message.cmd;
    flags = message.flags;
    netro_buf = message.netro_buf;
    std_data = message.std_data;
    std_group = message.std_group;
    dev_id = message.dev_id;
    nx = message.nx;
    cnt = message.cnt;
    init_cmd = message.init_cmd;
    ext_cmd = message.ext_cmd;
    QInterfaceMessage::operator=(message);
    return *this;
}


//очистка объекта сообщения
void QRS232Message::clear()
{
    QInterfaceMessage::clear();
    cmd = 0;
    flags = 0;
    netro_buf.clear();
    std_data = 0;
    std_group = 0;
    dev_id = 0;
    nx = 0;
    cnt = 0;
    init_cmd = 0;
    ext_cmd = 0;
    wait_ack = false;
}



bool QRS232Message::create(uint16 cmd, uint16 flags, uint8 * pData,uint32 len)
{
    this -> cmd = cmd;
    this -> flags = flags;
    buf.clear();
    buf.append(USART_START_DATA);
    buf.append(USART_ADDRESS_DATA);
    buf.append(USART_CONTROL_DATA);
    buf.append(USART_PROTOCOL_EXT_DATA);
    buf.append(len + sizeof(cmd) + sizeof(flags));
    buf.append((int8)(cmd & 0xff));
    buf.append((int8)((cmd >> 8) & 0xff));
    buf.append((int8)(flags & 0xff));
    buf.append((int8)((flags >> 8) & 0xff));
    if (len)
        buf.append((const char *)pData,len);
    uint16 crc = calc_crc((uint8 *)(buf.data() + 1), buf.count() - 1);
    buf.append((int8)(crc & 0xff));
    buf.append((int8)((crc >> 8) & 0xff));
    wait_ack = false;
    ext_cmd = true;
    return true;
}


bool QRS232Message::createStd(uint16 group, uint8 cmd_lo, uint8 cmd_hi, uint8 data_lo, uint8 data_hi,uint8 attr_lo,uint8 attr_hi)
{
    this -> cmd = cmd_lo | (((uint16)cmd_hi) << 8);
    this -> flags = flags;
    buf.clear();
    buf.append(USART_START_DATA);
    buf.append(USART_ADDRESS_DATA);
    buf.append(USART_CONTROL_DATA);
    buf.append(USART_PROTOCOL_STD_DATA);
    buf.append(USART_SIZE_STD_DATA);
    buf.append((int8)(group & 0xff));
    buf.append((int8)((group >> 8) & 0xff));
    buf.append(cmd_lo);
    buf.append(cmd_hi);
    buf.append(data_lo);
    buf.append(data_hi);
    buf.append(attr_lo);
    buf.append(attr_hi);
    uint16 crc = calc_crc((uint8 *)(buf.data() + 1), buf.count() - 1);
    buf.append((int8)(crc & 0xff));
    buf.append((int8)((crc >> 8) & 0xff));
    ext_cmd = false;
    wait_ack = false;
    return true;
}


//анализирует сообщение in_mess как ответ для текущего сообщения
//возвращает true, если ответ соответствует сообщению либо ответ "в прогрессе"
//поэтому следует также анализировать ans_value на значение INTERFACE_RESULT_PROGRESS_CONST
bool QRS232Message::isGoodAck(const QRS232Message & in_mess, uint16 * ans_value)
{
    QRS232Message tmp_mess = in_mess;
    if (ans_value)
        *ans_value = (uint16)(-1);
    if (tmp_mess.parse() != PARSE_OK)
        return false;
    //если ответ в расширенной команде
    if (tmp_mess.isExtCmd())
    {
        QByteArray tmp_data = tmp_mess.netroCommand();
        // если ответ о результате операции
        if (tmp_mess.getCommand() == INTERFACE_RESULT_STATUS_CMD)
        {
            if (tmp_data.count() != sizeof(good_ack))
                return false;
            if ((((uint8)tmp_data.at(0)) | (((uint16)((uint8)tmp_data.at(1)))) << 8) == good_ack)
            {
                if (ans_value)
                    *ans_value = good_ack;
                return true;
            }
            else
            {
                if (((uint8)tmp_data.at(0)) == INTERFACE_RESULT_PROGRESS_CONST)
                {
                    if (ans_value)
                        *ans_value = INTERFACE_RESULT_PROGRESS_CONST | ((((uint16)((uint8)tmp_data.at(1)))) << 8);
                    return true;
                }

                if ((((uint8)tmp_data.at(0)) | (((uint16)((uint8)tmp_data.at(1)))) << 8) == INTERFACE_RESULT_AUTH_REQ_CONST)
                {
                    if (ans_value)
                        *ans_value = INTERFACE_RESULT_AUTH_REQ_CONST;
                    return true;
                }
                //копируем результат, какой бы он плохой не был, в ответ
                if (ans_value)
                    *ans_value = (((uint8)tmp_data.at(0)) | (((uint16)((uint8)tmp_data.at(1)))) << 8);
                return false;
            }
        }
    }
    else
    {
        //если пришел не ответ
        if ((tmp_mess.getCommand() & 0xff) != ANSWER_CMD)
            return false;


        //если ответ не соответствует команде при условии что был запрос, либо была обычная команда а result !=0
        if (((((tmp_mess.getCommand() >> 8) & 0xff) != ((getCommand() >> 8) & 0xff)) && ((getCommand() & 0xff) == REQUEST_CMD))
                || (((getCommand() & 0xff) != REQUEST_CMD) && (((tmp_mess.getStdData() & 0xff) != INTERFACE_RESULT_OK_CONST) && ((tmp_mess.getStdData() & 0xff) != INTERFACE_RESULT_PROGRESS_CONST))))
        {
            //если был не запрос, а пришел результат, копируем его в ответ
            if (ans_value && (getCommand() & 0xff) != REQUEST_CMD)
                *ans_value = tmp_mess.getStdData() & 0xff;

            return false;
        }
        else
        {
            if (ans_value)
                *ans_value = tmp_mess.getStdData();
            return true;
        }
    }

    return false;
}


QRS232Message::PARSE_RESULT QRS232Message::parse()
{
    if (buf.count() < 7)
        return PARSE_UNKNOWN_CMD;
    uint16 crc = calc_crc((uint8 *)(buf.data() + 1), buf.count() - 3);
    uint16 cmd_crc = ((uint8)buf.at(buf.count() - 2)) | (((uint16)((uint8)buf.at(buf.count() - 1))) << 8);
    if (crc != cmd_crc)
        return PARSE_BAD_CRC;

    netro_buf.clear();
    if (buf.at(3) == USART_PROTOCOL_EXT_DATA)//протокол с расширенными командами?
    {
        ext_cmd = true;
        cmd = (((uint8)buf.at(5)) | (((uint16)((uint8)buf.at(6))) << 8));
        flags = ((uint8)buf.at(7)) | (((uint16)((uint8)buf.at(8))) << 8);
        switch (cmd)
        {
        case QRS232Message::INTERFACE_RESULT_STATUS_CMD://результат выполнения команды
            if (buf.count() < 13)
                return PARSE_UNKNOWN_CMD;
            for (int i = 9; i < buf.count() - 2; i++)
                netro_buf.append(buf.at(i));
        default:
            for (int i = 9; i < buf.count() - 2; i++)
                netro_buf.append(buf.at(i));
            return PARSE_OK;
        }
    }
    else if (buf.at(3) == USART_PROTOCOL_STD_DATA)
    {
        if (buf.count() < 15)
            return PARSE_UNKNOWN_CMD;
        ext_cmd = false;
        std_group =((uint8)buf.at(5)) | (((uint16)((uint8)buf.at(6))) << 8);
        cmd =((uint8)buf.at(7)) | (((uint16)((uint8)buf.at(8))) << 8);
        std_data =((uint8)buf.at(9)) | (((uint16)((uint8)buf.at(10))) << 8);
        flags =((uint8)buf.at(11)) | (((uint16)((uint8)buf.at(12))) << 8);
        return PARSE_OK;
    }

    return PARSE_UNKNOWN_CMD;
}


//возвращает лишь буфер с расширенными данными(после флагов, не включая crc)
QByteArray QRS232Message::getExtDataBuf()
{
    QByteArray result;
    if (!ext_cmd)
        return result;
    for (int i = 9; i < buf.count() - 2; i++)
        result.append(buf.at(i));
    return result;
}


uint16 QRS232Message::calc_crc(uint8 * data, uint32 len)
{
    register uint16 crc = 0x0000;
    register uint8 i;
    register uint8 temp;
    register uint8 * pcBlock = data;//почему-то crc ищется с первого байта пакета, а не с нулевого
    while (len--)
    {
        temp = *pcBlock++;

        for (i = 0; i < 8; i++)
        {
            if ( crc & 0x8000 )
            {
                crc = crc << 1;
                if (temp & 0x0001)
                    crc |= 0x0001;
                crc ^= 0x1021;
            }
            else
            {
                crc = crc << 1;
                if (temp & 0x0001)
                    crc |= 0x0001;
            }

            temp = temp >> 1;
        }

   }

       return crc;
}
