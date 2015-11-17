#include "typedefs.h"
#include <QtCore>
#include <QDebug>
#include "rs232_port.h"


rs232_port::rs232_port()
{
    port_name.clear();
    out_cmd_ready = false;
    out_ack_ready = false;
    logic_state = INTERFACE_IDLE;
    staf_flag = false;
    buf_index = 0;
    idle_counter = 0;
    answer_timeout = WAIT_ACK_MS;
}


rs232_port::~rs232_port()
{
}

//получить список чисел(положительных) со строки
QList<int> get_ints_from_string(QString buttons)
{
    QList<int> values;
    QString tmp;
    bool bOk,bValue = false;
    int tmp_val, val = 0;
    for (int i = 0; i < buttons.length(); i++)
    {
        tmp = buttons.at(i);
        tmp_val = tmp.toInt(&bOk);
        if (bOk)
        {
            val = val * 10 + tmp_val;
            bValue = true;
        }
        else
        {
            if (bValue)
            {
                values.append(val);
                val = 0;
                bValue = false;
            }
        }

    }

    if (bValue)
        values.append(val);
    return values;
}

//Посылка команды в порт
void rs232_port::send_command(const QByteArray & buf)
{
    if (!buf.count())
        return;//чего-то слишком короткая команда
    out_cmd = buf;
    out_cmd_ready = true;
    //proc();
}

//Посылка ответа в порт
void rs232_port::send_answer(const QByteArray & buf)
{
    if (!buf.count())
        return;//чего-то слишком короткая команда
    out_ack = buf;
    out_ack_ready = true;
    //proc();
}


//установить таймаут получения ответа
void rs232_port::set_answer_timeout(int time_ms)
{
    answer_timeout = time_ms;
}


//запросить таймаут получения ответа
int rs232_port::get_answer_timeout()
{
    return answer_timeout;
}


//ожидание команды из порта в течение времени answer_timeout
bool rs232_port::wait_for_command()
{
    if (logic_state == INTERFACE_IDLE)
    {
        logic_state = INTERFACE_GETCMD;
        timeout = answer_timeout + QDateTime::currentMSecsSinceEpoch();
        return true;
    }
    return false;
}


//основной слот протокола
INTERFACE_LOGIC_STATE_T rs232_port::proc()
{
    char data_byte;
    //если порт не открыт, ничего не делаем
    if (port_name.isEmpty())
        return INTERFACE_ERROR;

    //анализ логического состояния порта
    switch(logic_state)
    {

    case INTERFACE_IDLE://исходное состояние интерфейса
        idle_counter++;
        if (!out_cmd_ready)
        {
            if (bytesAvailable() > 0)
                logic_state = INTERFACE_GETCMD;
            break;
        }

        if (idle_counter < 1)
            break;
        logic_state = INTERFACE_SENDCMD;
        idle_counter = 0;


    case INTERFACE_SENDCMD://отсылка буфера с данными на модем
    {
#ifdef DEBUG_LOG
        QString s;
        for (int i = 0; i < out_cmd.count(); i++)
            s += " " + QString::number((unsigned char)(out_cmd.at(i)),16);
        qDebug() << tr("Посылка команды в порт") + s;
#endif

        //учитываем  стаффинг байт
            switch (prot)
            {
            case PORT_PROTOCOL_X25:
            {
                QByteArray out_data;
                for (int i = 0; i < out_cmd.count(); i++)
                {
                    if (out_cmd.at(i) == USB_bSTAF_DATA || ((out_cmd.at(i) == USB_bFLAG_DATA) && (i != 0)))
                        out_data.append(USB_bSTAF_DATA);
                    out_data.append(out_cmd.at(i));
                }
                writeData(out_data.data(),out_data.count());
                break;
            }

            case PORT_PROTOCOL_SLIP:
                /*
                //если символ обрамления пакета встретился в середине пакета
                if (out_cmd.at(i) == USB_bSLIP_DATA && i && i != out_cmd.length() - 1)
                {
                    data_byte = USB_bSTAF2_DATA;
                    writeData(&data_byte,1);
                    data_byte = USB_bSTAF2_DATA_SLIP;
                    writeData(&data_byte,1);
                }
                else
                    if (out_cmd.at(i) == USB_bSTAF2_DATA)
                    {
                        data_byte = USB_bSTAF2_DATA;
                        writeData(&data_byte,1);
                        data_byte = USB_bSTAF2_DATA_STAF;
                        writeData(&data_byte,1);
                    }
                    else
                    {
                        data_byte = out_cmd.at(i);
                        writeData(&data_byte,1);
                    }*/
                break;
            }

        buf_index = 0;
        out_cmd_ready = false;
        logic_state = INTERFACE_GETACK;
        timeout = answer_timeout + QDateTime::currentMSecsSinceEpoch();
    }

    case INTERFACE_GETACK://Ожидание АСК от модема
    {
        //модем нам отослал достаточно данных
        while (bytesAvailable())
        {
            readData(&data_byte,1);
            switch (prot)
            {
            case PORT_PROTOCOL_X25:
                if ((data_byte != USB_bFLAG_DATA) && (buf_index == 0))
                    break;//ошибка во время начала последовательности приняли неверный айт начала последоательности. игнорируем

                if ((data_byte == USB_bSTAF_DATA) && (!staf_flag))//если мы получили первый стафсимвол
                {
                    staf_flag = true;
                    break;
                }

                 if (staf_flag)//сброс стаф-флага и запись символа, следующего за стаф-символом, в буффер
                     staf_flag = false;

                buf[buf_index] = data_byte;
                buf_index++;

                if (buf_index >= 5)
                    //в buf[4] хранится длина пакета
                    if ((buf_index >= 5 + buf[4] + 2) || (buf_index >= PACKAGE_MAX_SIZE))//получили весь пакет?
                    {
                        package_received();//первичный аанализ
                        return logic_state;
                    }
                break;

            case PORT_PROTOCOL_SLIP:
                if (((unsigned char)data_byte != USB_bSLIP_DATA) && (buf_index == 0))
                    break;//ошибка во время начала последовательности приняли неверный айт начала последоательности. игнорируем
                if (((unsigned char)data_byte == USB_bSLIP_DATA) && (buf_index == 1))
                    buf_index = 0;//ошибка во время начала последовательности приняли неверный айт начала последоательности. игнорируем

                if (((unsigned char)data_byte == USB_bSTAF2_DATA) && (!staf_flag))//если мы получили первый стафсимвол
                {
                    staf_flag = true;
                    break;
                }

                 if (staf_flag)//сброс стаф-флага и запись символа, следующего за стаф-символом, в буффер
                 {
                     staf_flag = false;
                     if ((unsigned char)data_byte == USB_bSTAF2_DATA_SLIP)
                        data_byte = (char)USB_bSLIP_DATA;
                     else
                        if ((unsigned char)data_byte == USB_bSTAF2_DATA_STAF)
                            data_byte = (char)USB_bSTAF2_DATA;
                        else
                        {
                            buf_index = 0;//ошибка, недопустимый символ после стаффинга
                            break;
                        }

                 }

                buf[buf_index] = data_byte;
                buf_index++;

                if (((unsigned char)data_byte == USB_bSLIP_DATA) && (buf_index > 1))
                {
                        // qDebug((tr("Принят пакет slip")).toAscii().data());
                        package_received();//первичный аанализ
                        return logic_state;
                }
                break;
            }
        }
        if (timeout <= QDateTime::currentMSecsSinceEpoch())
        {
            buf_index = 0;
            staf_flag = false;
            logic_state = INTERFACE_TIMEOUT;
            break;
        }
        break;
    }

    case INTERFACE_SENDACK:
        if (!out_ack_ready)//если ответа от программы пока что нету
            break;
        //учитываем  стаффинг байт
        for (int i = 0; i < out_ack.count(); i++)
            switch (prot)
            {
            case PORT_PROTOCOL_X25:
            {
                QByteArray out_data;
                for (int i = 0; i < out_cmd.count(); i++)
                {
                    if (out_cmd.at(i) == USB_bSTAF_DATA || ((out_cmd.at(i) == USB_bFLAG_DATA) && (i != 0)))
                        out_data.append(USB_bSTAF_DATA);
                    out_data.append(out_cmd.at(i));
                }
                writeData(out_data.data(),out_data.count());
                break;
            }
            case PORT_PROTOCOL_SLIP:
            /*    //если символ обрамления пакета встретился в середине пакета
                if (out_ack.at(i) == USB_bSLIP_DATA && i && i != out_ack.length() - 1)
                {
                    data_byte = USB_bSTAF2_DATA;
                    writeData(&data_byte,1);
                    data_byte = USB_bSTAF2_DATA_SLIP;
                    writeData(&data_byte,1);
                }
                else
                    if (out_ack.at(i) == USB_bSTAF2_DATA)
                    {
                        data_byte = USB_bSTAF2_DATA;
                        writeData(&data_byte,1);
                        data_byte = USB_bSTAF2_DATA_STAF;
                        writeData(&data_byte,1);
                    }
                    else
                    {
                        data_byte = out_ack.at(i);
                        writeData(&data_byte,1);
                    }*/
                break;
            }
        out_ack_ready = false;
        logic_state = INTERFACE_IDLE;
        break;

    case INTERFACE_GETCMD:
        //модем нам отослал достаточно данных
        while (bytesAvailable())
        {
            readData(&data_byte,1);
            switch (prot)
            {
            case PORT_PROTOCOL_X25:
                //emit signal_write_log("принят байт " + QString::number(data_byte,16));
                if ((data_byte != USB_bFLAG_DATA) && (buf_index == 0))
                    break;//ошибка во время начала последовательности приняли неверный айт начала последоательности. игнорируем

                if ((data_byte == USB_bSTAF_DATA) && (!staf_flag))//если мы получили первый стафсимвол
                {
                    staf_flag = true;
                    break;
                }

                 if (staf_flag)//сброс стаф-флага и запись символа, следующего за стаф-символом, в буффер
                     staf_flag = false;

                buf[buf_index] = data_byte;
                buf_index++;
                if (buf_index >= 5)
                    //в buf[4] хранится длина пакета
                    if ((buf_index >= 5 + buf[4] + 2) || (buf_index >= PACKAGE_MAX_SIZE))//получили весь пакет?
                    {
                        //emit signal_write_log("Pack received");
                        package_received();//первичный аанализ
                        return logic_state;
                    }
                break;

            case PORT_PROTOCOL_SLIP:
                if (((unsigned char)data_byte != USB_bSLIP_DATA) && (buf_index == 0))
                    break;//ошибка во время начала последовательности приняли неверный айт начала последоательности. игнорируем

                if (((unsigned char)data_byte == USB_bSTAF2_DATA) && (!staf_flag))//если мы получили первый стафсимвол
                {
                    staf_flag = true;
                    break;
                }

                 if (staf_flag)//сброс стаф-флага и запись символа, следующего за стаф-символом, в буффер
                 {
                     staf_flag = false;
                     if ((unsigned char)data_byte == USB_bSTAF2_DATA_SLIP)
                        data_byte = (char)USB_bSLIP_DATA;
                     else
                        if (data_byte == USB_bSTAF2_DATA_STAF)
                            data_byte = USB_bSTAF2_DATA;
                        else
                        {
                            buf_index = 0;//ошибка, недопустимый символ после стаффинга
                            break;
                        }

                 }

                buf[buf_index] = data_byte;
                buf_index++;

                if (((unsigned char)data_byte == USB_bSLIP_DATA) && (buf_index > 1))
                {
                        //emit signal_write_log("Pack received");
                        package_received();//первичный аанализ
                        return logic_state;
                }
                break;
            }

        }

        if (timeout <= QDateTime::currentMSecsSinceEpoch())
        {
            buf_index = 0;
            staf_flag = false;
            logic_state = INTERFACE_TIMEOUT;
            break;
        }

        break;

    case INTERFACE_INCOME_ANSWER:
    case INTERFACE_INCOME_COMMAND:
    case INTERFACE_TIMEOUT:
    case INTERFACE_ERROR:
        break;
    }
    return logic_state;
}


//первичный анализ полученного пакета
void rs232_port::package_received()
{
#ifdef DEBUG_LOG
    QString s = tr("Принята команда от сниффера ");
    for (int i = 0; i < buf_index; i++)
        s += " " + QString::number((unsigned char)(buf[i]),16);

    qDebug() << s;
#endif
    if (logic_state == INTERFACE_GETCMD)
    {
        in_cmd.clear();
        for (int i = 0; i < buf_index; i++)
            in_cmd.append(buf[i]);
        logic_state = INTERFACE_INCOME_COMMAND;
    }
    else if (logic_state == INTERFACE_GETACK)
    {
        in_ack.clear();
        for (int i = 0; i < buf_index; i++)
            in_ack.append(buf[i]);

        logic_state = INTERFACE_INCOME_ANSWER;
    }
    else
        logic_state = INTERFACE_IDLE;
    buf_index = 0;
    staf_flag = false;
}


//смена текущего com-порта на новый, зашифрованный в строке в виде "COM##" либо "NONE"
bool rs232_port::connect_to_modem(const QString & s, int baud_rate, PORT_PROTOCOL_T prot)
{

    return port_open(s,(QSerialPort::BaudRate)baud_rate,prot);

}

bool rs232_port::port_open(const QString & port_name, QSerialPort::BaudRate baud_rate, PORT_PROTOCOL_T prot)
{
    setPortName(port_name);
    setBaudRate(baud_rate); // 115200
    setFlowControl(NoFlowControl);
    setParity(NoParity);    // паритет отключен
    setDataBits(Data8);   // 8 бит
    setStopBits(OneStop);  // один стоповый
    if (!open(QIODevice::ReadWrite))
    {
        this -> port_name.clear();
        return false;
    }
    this -> port_name = port_name;
    this -> prot = prot;
    qDebug() << tr("RS232 %1 was opened").arg(port_name);
    return true;
}

bool rs232_port::port_close()
{
    //если какой-нибудь порт открыт, закрываем его
    if (isOpen())
    {
        close();
        qDebug() << tr("RS232 %1 was closed №").arg(port_name);
    }
    port_name.clear();
    return true;
}


//получить массив пришедшего в порт ответа
QByteArray rs232_port::get_ack()
{
    if (logic_state == INTERFACE_INCOME_ANSWER)
        logic_state = INTERFACE_IDLE;
    return in_ack;
}


//получить массив пришедшей в порт команды
QByteArray rs232_port::get_cmd()
{
    if (logic_state == INTERFACE_INCOME_COMMAND)
        logic_state = INTERFACE_IDLE;
    return in_cmd;
}
