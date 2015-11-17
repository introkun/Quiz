#include "interface_manager.h"
#include <QThread>
#include <QTimer>

QInterface_Manager::QInterface_Manager(QObject *parent) :
    QThread(parent)
{
    log_pack_num = 0;
    interface_created = false;
    sending = false;
    answer_timeout = WAIT_ACK_MS;
    serial_port = NULL;
    uniq_index = 0;
    retry_count = 3;
    interfaceTimer = new QTimer();
    interfaceTimer -> start(TIME_TICK_PERIOD_MS);
    connect(interfaceTimer,SIGNAL(timeout()),this,SLOT(state_machine()));

}


//destruct
QInterface_Manager::~QInterface_Manager()
{
    disconnect(interfaceTimer,SIGNAL(timeout()),this,SLOT(state_machine()));
    if(QThread::isRunning())
    {// если поток запущен останавливаем его
            QThread::quit();; // останавливаем поток
            QThread::wait(); // дожидаемся его завершения

    }
    interfaceTimer -> stop();
    delete interfaceTimer;
}

//установить тип связи с модемом inter, подтип sub_inter(arg- аргумент типа связи)
void QInterface_Manager::connect_to_modem(QString arg, int sub_arg, rs232_port::PORT_PROTOCOL_T prot)
{
    interface_arg = arg;
    interface_sub_arg = sub_arg;
    this -> prot = prot;
}



//Отключиться от модема
void QInterface_Manager::disconnect_from_modem()
{
    QThread::exit(0);
    while(QThread::isRunning());
}


void QInterface_Manager::set_answer_timeout(int time_ms)
{
    answer_timeout = time_ms;
    if (serial_port)
        serial_port -> set_answer_timeout(answer_timeout);
}


int QInterface_Manager::get_answer_timeout() const
{
    return answer_timeout;
}


//установить количество повторных посылок в случае неудачной посылки
void QInterface_Manager::set_retry_count(int value)
{
    retry_count = value;
}


//получить количество повторных посылок в случае неудачной посылки
int QInterface_Manager::get_retry_count() const
{
    return retry_count;
}


//свободен ли порт от передачи
bool QInterface_Manager::is_port_free() const
{
    return command.getBuf().count() ? false : true;
}

//получить последнее отосланное в порт сообщение
QInterfaceMessage QInterface_Manager::get_last_sent_message() const
{
    return command_old;
}


bool QInterface_Manager::restart(void)
{
        if(!QThread::isRunning())
        {// поток еще не запущен!
            QThread::start(QThread::HighPriority);
        }
        else
        {
            QThread::exit(0);
            while(QThread::isRunning());
            QThread::start(QThread::HighPriority);
        }
        return true;
}


// обработчик потока
void QInterface_Manager::run(void)
{
    serial_port = new rs232_port();
    serial_port -> set_answer_timeout(answer_timeout);
    connect(this,SIGNAL(signal_port_proc()),serial_port,SLOT(proc()),Qt::QueuedConnection);
    if (serial_port -> connect_to_modem(interface_arg,interface_sub_arg,prot))
    {
        emit signal_log_buffer(tr("Открыт порт ") + interface_arg,QByteArray());
        set_interface_status(true);
    }
    else
    {
        emit signal_log_buffer(tr("Ошибка открытия порта ") + interface_arg,QByteArray());
        set_interface_status(false);
    }

    sending = false;
    exec();

    if (interface_created)//только если создан интерфейс
    {
        serial_port -> port_close();
        set_interface_status(false);
    }
    delete serial_port;
    serial_port = NULL;
}


bool QInterface_Manager::send_command(QInterfaceMessage & mess)
{
    if (command_to_send.getBuf().count())
        return false;
    if (mess.messageType() == 1 && mess.getBuf().count() >= 9 && mess.getBuf().at(3) == USART_PROTOCOL_EXT_DATA)
        if (!mess.setUniqueIndex((++uniq_index) % 8))
            uniq_index = (uniq_index - 1) % 8;

    command_to_send = mess;
    emit signal_log_buffer("HOST -> DEV #" + QString::number(++log_pack_num) + " ",command_to_send.getBuf());
    send_retry = 0;
    state_machine();
    return true;
}


bool QInterface_Manager::send_answer(QInterfaceMessage & mess)
{
    answer = mess;
    state_machine();
    return true;
}


//повторить прошлую команду
bool QInterface_Manager::repeat_command()
{
    send_retry++;
    if (command.getBuf().count())
        return false;
    if (!command_old.getBuf().count())
        return false;
    command_to_send = command_old;
    emit signal_log_buffer("<b>RETRY</b> HOST -> DEV #" + QString::number(log_pack_num) + " ",command_to_send.getBuf());
    state_machine();
    return true;
}


//ожидать команду от модема
bool QInterface_Manager::wait_for_command()
{
    INTERFACE_LOGIC_STATE_T port_event = serial_port -> get_logic();
    if (port_event == INTERFACE_IDLE && !sending)
    {
        return serial_port -> wait_for_command();
    }
    else
        return false;
}


//остановить текущую команду
bool QInterface_Manager::stop_command()
{
    if (!command.getBuf().count())
        return false;
    command_to_send.clear();
    command.clear();
    command_old.clear();
    if (serial_port -> get_logic() == INTERFACE_SENDCMD || serial_port -> get_logic() == INTERFACE_SENDACK)
    {
        serial_port -> reset_logic();
        sending = false;
        state_machine();
    }
    return true;
}


INTERFACE_LOGIC_STATE_T QInterface_Manager::logic_state()
{
    return serial_port -> get_logic();
}

//автомат отсылки сообщений на верхнем логическом уровне
//имеет 2 состояния- IDLE и ожидание подтверждения
void QInterface_Manager::state_machine()
{
    if (!interface_created)
        return;

    //получаем текущее состояние порта
    INTERFACE_LOGIC_STATE_T port_event = INTERFACE_ERROR;

    port_event = serial_port -> get_logic();

    //если слот порта вышел из idle при старте передачи
    if (sending && port_event != INTERFACE_IDLE)
        sending = false;

    switch(port_event)
    {
    case INTERFACE_ERROR://нерабочий интерфейс
        return;
    case INTERFACE_TIMEOUT:
        //снимаем таймаут        
        serial_port -> reset_logic();
        if (!command.isEmpty())
            command_old = command;
        else
            command_old.clear();
        command.clear();
        if (send_retry < retry_count)
            repeat_command();
        else
        {
            send_retry = 0;
            emit signal_ack_timeout();
        }
        return;

       //переходим на idle!
    case INTERFACE_IDLE:

        //есть ли команды на передачу
        if (command_to_send.getBuf().count() && !sending)
        {
            sending = true;
            command = command_to_send;
            command_to_send.clear();//послали в порт, команда перекочевывает в command
            serial_port -> send_command(command.getBuf());
            if (interfaceTimer -> interval() >= TIME_TICK_PERIOD_MS)
                interfaceTimer -> setInterval(0);
        }
        else
        {
            if (interfaceTimer -> interval() < TIME_TICK_PERIOD_MS)
                interfaceTimer -> setInterval(TIME_TICK_PERIOD_MS);

        }
        break;

    case INTERFACE_INCOME_COMMAND:
        if (!command.isEmpty())
            command_old = command;
        else
            command_old.clear();
        command.clear();
        emit signal_proc_incoming_message(serial_port -> get_cmd());
        break;

    case INTERFACE_INCOME_ANSWER:
    {
        command_old = command;
        command.clear();
        emit signal_proc_incoming_message(serial_port -> get_ack());        
        break;
    }

    default:
        break;
    }
    emit signal_port_proc();//вызываем в другом потоке слот порта
}

//Установить включенность\выключенность интерфейса обмена с модемом(посылает соответсвующий сигнал)
//(когда он удаляется, очищается очередь
void QInterface_Manager::set_interface_status(bool on_off)
{
    interface_created = on_off;
    if (!interface_created)
    {
        if (interfaceTimer -> interval() < TIME_TICK_PERIOD_MS)
            interfaceTimer -> setInterval(TIME_TICK_PERIOD_MS);
        command.clear();
        command_old.clear();
    }
    emit signal_interface_status(on_off);
}

