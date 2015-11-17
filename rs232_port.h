#ifndef RS232_PORT_H
#define RS232_PORT_H
#include <QObject>
#include "QtSerialPort/QSerialPort"
#define PACKAGE_MAX_SIZE 512
#define	USB_bFLAG_DATA		0x7e
#define	USB_bSTAF_DATA		0x7d

#define	USB_bSLIP_DATA		0xc0
#define	USB_bSTAF2_DATA		0xdb
#define	USB_bSTAF2_DATA_SLIP	0xdc
#define	USB_bSTAF2_DATA_STAF	0xdd
#define WAIT_ACK_MS 5000 //количество периодов таймера, после которого срабатывает таймаут ожидания ответа от модема и интерфейс переходит в исходное состояние
typedef enum {
    INTERFACE_IDLE = 0,
    INTERFACE_SENDCMD,
    INTERFACE_SENDACK,
    INTERFACE_GETCMD,
    INTERFACE_GETACK,
    INTERFACE_INCOME_ANSWER,
    INTERFACE_INCOME_COMMAND,
    INTERFACE_ERROR,
    INTERFACE_TIMEOUT
} INTERFACE_LOGIC_STATE_T;


class rs232_port : public QSerialPort
{
    Q_OBJECT
public:
    typedef enum {
        PORT_PROTOCOL_X25,
        PORT_PROTOCOL_SLIP
    }PORT_PROTOCOL_T;
    rs232_port();
    virtual ~rs232_port();
    bool connect_to_modem(const QString & s, int baud_rate,PORT_PROTOCOL_T prot = PORT_PROTOCOL_X25);
    bool port_open(const QString & port_name, QSerialPort::BaudRate baud_rate, PORT_PROTOCOL_T prot);
    bool port_close();
    //Посылка команды в порт
    void send_command(const QByteArray & buf);
    //Посылка ответа в порт
    void send_answer(const QByteArray & buf);
    //установить таймаут получения ответа
    void set_answer_timeout(int time_ms);
    //запросить таймаут получения ответа
    int get_answer_timeout();
    //ожидание команды из порта в течение времени answer_timeout
    bool wait_for_command();

    //получить массив пришедшего в порт ответа
    QByteArray get_ack();
    //получить массив пришедшей в порт команды
    QByteArray get_cmd();
    //сброс логики порта в исходное состояние
    void reset_logic(){logic_state = INTERFACE_IDLE;}
    INTERFACE_LOGIC_STATE_T get_logic(){return logic_state;}
public slots:
    INTERFACE_LOGIC_STATE_T proc();
private slots:

private:
    //первичный анализ полученного пакета
    void package_received();
    QString port_name;//текущее имя открытого порта(пусто - никакой порт не открыт)
    //int wait_ack_tics;
    qint64 timeout;//время, после которого генерируется событие таймаута
    int answer_timeout;
    int idle_counter;
    //выходные в порт буфера в виде списков
    QByteArray out_cmd;
    QByteArray out_ack;

    QByteArray in_cmd;
    QByteArray in_ack;
    bool out_cmd_ready;//есть ли у нас во внутреннем буфере out_cmd пакет на передачу
    bool out_ack_ready;//есть ли у нас во внутреннем буфере out_ack ответ на передачу
    INTERFACE_LOGIC_STATE_T logic_state;
    unsigned char buf[PACKAGE_MAX_SIZE];//буфер-приемник данных из порта
    unsigned short buf_index;//текущий индекс приемника
    bool staf_flag;//стафф флаг
    PORT_PROTOCOL_T prot;//текущий протокол обмена
};

#endif // RS232_PORT_H
