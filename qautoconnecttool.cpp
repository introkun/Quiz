#include <QApplication>
#include <QtSerialPort/QSerialPortInfo>
#include "qautoconnecttool.h"
#include "message/rs232message.h"

QAutoConnectTool::QAutoConnectTool(QObject *parent, QInterface_Manager * manager) :
    QObject(parent)
{
    port_opened = false;
    this -> manager = manager;
    port_index = 0;
    version_soft = 0;
    version_hard = 0;
    if (manager)
    {
        //connect(this,SIGNAL(signalPostCommand(QRS232Message&)),this -> manager,SLOT(send_command(QRS232Message&)));
        connect(this -> manager,SIGNAL(signal_proc_incoming_message(QByteArray)),this,SLOT(slotIncomeMessage(QByteArray)));
        connect(this -> manager,SIGNAL(signal_ack_timeout()),this,SLOT(slotTimeout()));
        connect(this -> manager,SIGNAL(signal_interface_status(bool)),this,SLOT(slotOpenStatus(bool)));
    }

}


QAutoConnectTool::~QAutoConnectTool()
{
    disconnect(this -> manager,SIGNAL(signal_proc_incoming_message(QByteArray)),this,SLOT(slotIncomeMessage(QByteArray)));
    disconnect(this -> manager,SIGNAL(signal_ack_timeout()),this,SLOT(slotTimeout()));
    disconnect(this -> manager,SIGNAL(signal_interface_status(bool)),this,SLOT(slotOpenStatus(bool)));
    manager -> set_answer_timeout(WAIT_ACK_MS);
    manager -> set_retry_count(3);
}


//получить список доступных COM портов
QStringList QAutoConnectTool::getSystemSerials()
{
    QStringList tmp_port_list;
    QList <QSerialPortInfo> ports = QSerialPortInfo::availablePorts();

    foreach (QSerialPortInfo port,ports)
        if (!port.isBusy())
            tmp_port_list.append(port.portName());
    return tmp_port_list;
}



//определить порт для устройства. Функция не возвращает номер порта. Его возвращает сигнал signalPortFound(только номер первого найденного порта!!!)
//keep_connect- оставить ли подключенным порт после того, как устройство найдено
bool QAutoConnectTool::findDevicePort(bool keep_connect)
{
    if (!manager)
        return false;
    manager -> set_answer_timeout(500);
    manager -> set_retry_count(0);
    //если вдруг есть подключение к порту
    if (port_opened)
    {
        port_found = true;//имитируем найденный порт для отключения
        manager -> disconnect_from_modem();
        do {
            //ждем закрытия порта(срабатывания slotOpenStatus(false))
            QApplication::processEvents();
        } while(port_opened);
    }

    //получаем список портов
    port_list = getSystemSerials();
    if (!port_list.count())
    {
        emit signalNoPortFound();
        return true;
    }
    port_index = 0;
    this -> keep_connect = keep_connect;
    port_found = false;
    connectToCurrentPort();
    return true;
}


//получен ответ от устройства buf
void QAutoConnectTool::slotIncomeMessage(const QByteArray & buf)
{
    QRS232Message mess = QRS232Message(NULL,buf);
    if (mess.parse() != QRS232Message::PARSE_OK)
    {
        QTimer::singleShot(100,this,SLOT(slotTimeout()));
        return;
    }
    if  (!mess.isExtCmd())
    {
       switch(mess.getCommand())
       {
       case QRS232Message::USART_MODEM_ANS_VERSION_CMD:
       {

            version_soft = (mess.getStdData() & 0xff) | (mess.getFlags() & 0xff00);
            version_hard = ((mess.getStdData() & 0xff00) >> 8) | ((mess.getFlags() & 0xff) << 8);
            if ((version_hard & 0xff) != 88)
            {
                QTimer::singleShot(100,this,SLOT(slotTimeout()));
                return;
            }
            manager -> set_answer_timeout(WAIT_ACK_MS);
            manager -> set_retry_count(3);

            port_found = true;
             //если сохранять подключение не надо
            if (!keep_connect && port_opened)
            {
                manager -> disconnect_from_modem();
                do {
                    //ждем закрытия порта(срабатывания slotOpenStatus(false))
                    QApplication::processEvents();
                } while(port_opened);
            }
            emit signalPortFound(port_list.at(port_index));
            return;
       }
       default:
           QTimer::singleShot(100,this,SLOT(slotTimeout()));
           return;
       }
    }
    else
    {
        QTimer::singleShot(100,this,SLOT(slotTimeout()));
        return;
    }
}


//таймаут ожидания ответа от порта
void QAutoConnectTool::slotTimeout()
{
    //если мы подсоеденены к порту
    if (port_opened)
    {
        port_opened = false;
        manager -> disconnect_from_modem();
        return;
    }

    port_index++;
    if (port_index >= port_list.count())
    {
        emit signalNoPortFound();
        return;
    }
    connectToCurrentPort();
}


//послать сообщение запроса версии устройства
void QAutoConnectTool::postGetVersionMsg()
{
    QRS232Message mess;
    mess.createStd(0,QRS232Message::USART_MODEM_GET_VERSION_CMD & 0xff ,(QRS232Message::USART_MODEM_GET_VERSION_CMD >> 8) & 0xff,0,0,0,0);
    manager -> send_command(mess);
}


//подключиться к текущему порту
void QAutoConnectTool::connectToCurrentPort()
{
    manager -> connect_to_modem(port_list.at(port_index),QSerialPort::Baud115200,rs232_port::PORT_PROTOCOL_X25);
    manager -> restart();
}


//слот об успешном/неудачном открытии порта
void QAutoConnectTool::slotOpenStatus(bool value)
{
    //подключенность к порту?
    if (!value)
    {
        port_opened = false;
        if (!port_found)//если порт не найден
            QTimer::singleShot(100,this,SLOT(slotTimeout()));
    }
    else
    {
        port_opened = true;
        postGetVersionMsg();
    }

}
