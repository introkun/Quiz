#ifndef RS232_THREAD_H
#define RS232_THREAD_H

#include <QThread>
#include <QTimer>
#include "rs232_port.h"
#include "message/rs232message.h"

class QInterface_Manager : public QThread
{
    Q_OBJECT
public:
    explicit QInterface_Manager(QObject *parent = 0);
    virtual ~QInterface_Manager();
    bool restart(void);
    //arg- аргумент типа связи
    void connect_to_modem(QString arg,int sub_arg,rs232_port::PORT_PROTOCOL_T prot = rs232_port::PORT_PROTOCOL_X25);
    //Отключиться от модема
    void disconnect_from_modem();

    void set_answer_timeout(int time_ms);
    int get_answer_timeout() const;

    //установить/получить количество повторных посылок в случае неудачной посылки
    void set_retry_count(int value);
    int get_retry_count() const;
    //свободен ли порт от передачи
    bool is_port_free() const;
    //получить последнее отосланное в порт сообщение
    QInterfaceMessage get_last_sent_message() const;
    int get_send_retry() const{return send_retry;}
    void clear_send_retry(){send_retry = 0;}
protected:
        virtual void run(void);
signals:
    //сигнал о том, создан ли или уничтожен экземпляр интерфейса(включены ли его слоты и т.п.)
    void signal_interface_status(bool on_off);
    //сигнал программе о пришедшем в порт ответе/команде, аргументы которой находятся в QByteArray
    void signal_proc_incoming_message(QByteArray);
    void signal_ack_timeout();
    void signal_port_proc();
    void signal_log_buffer(const QString &,const QByteArray &);
public slots:
    //Послать новую команду mess в иинтерфейс
    bool send_command(QInterfaceMessage & mess);
    bool send_answer(QInterfaceMessage & mess);
    //повторить прошлую команду
    bool repeat_command();
    //остановить текущую команду
    bool stop_command();
    //ожидать команду от модема
    bool wait_for_command();
    INTERFACE_LOGIC_STATE_T logic_state();
private slots:
    //автомат отсылки сообщений на верхнем логическом уровне
    //имеет 2 состояния- IDLE и ожидание подтверждения
    void state_machine();
private:
    static const int TIME_TICK_PERIOD_MS = 20;
    QInterfaceMessage command_to_send;
    QInterfaceMessage command;//текущая передаваемая команда
    QInterfaceMessage command_old;//последняя переданная команда
    QInterfaceMessage answer;//текущий передаваемый ответ
    int answer_timeout;//время таймаута при ожидании ответа
    int retry_count;//максимальное количество повторов посылки
    bool interface_created;//создан ли интерфейс
    QString interface_arg;
    int interface_sub_arg;
    rs232_port * serial_port;//экземпляр объекта com порта
    rs232_port::PORT_PROTOCOL_T prot;
    int uniq_index;//уникальный индекс команды
    // таймер задержки на TIME_TICK_PERIOD_MS мс
    QTimer * interfaceTimer;
    //Установить включенность\выключенность интерфейса обмена с модемом(посылает соответсвующий сигнал)
    //(когда он удаляется, очищается очередь
    void set_interface_status(bool on_off);
    bool sending;
    int send_retry;//номер повтора
    unsigned int log_pack_num;

};

#endif // RS232_THREAD_H
