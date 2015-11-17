#ifndef RS232MESSAGE_H
#define RS232MESSAGE_H
#include "typedefs.h"
#include <QByteArray>
#include <QObject>
#include "qinterfacemessage.h"
#define	USART_STAF_DATA	                0x7d
#define USART_START_DATA                0x7e
#define	USART_ADDRESS_DATA		0xFF
#define	USART_CONTROL_DATA		0x03
#define	USART_PROTOCOL_STD_DATA	        0x01
#define	USART_PROTOCOL_EXT_DATA	        0x02
#define	USART_SIZE_STD_DATA	        0x08

class QRS232Message: public QInterfaceMessage
{
    Q_OBJECT
public:

    typedef enum {
        SYSTEM_PARAM_VERSION = 0x80,
        SYSTEM_PARAM_AUTOCONNECT = 0x81,
        SYSTEM_PARAM_APPLYRESTORE = 0x85,
        SYSTEM_PARAM_MEMORY_ALL = 0x82,//общая статистика по памяти
        SYSTEM_PARAM_MEMORY_CURRENT = 0x83//статистика с привязкой к конфигурации
    } SYSTEM_PARAM_T;

    static const int FLAG_NOT_STANDALONE = 1;//флаг отсутсивя автономного режима
    static const int FLAG_CLEAR_DEV_MEM = 2;//флаг очистки памяти пультов ИУ
    static const int FLAG_CLEAR_STICK_MEM = 1;//флаг очистки памяти пультов екущей конфигурации usb-stick
    static const int FLAG_DELAYED = 2;//флаг отложенной отправки команды на ИУ(данные временно сохраняются на usb-stick)

    //коды расширенных команд Interface(общие категории)
    typedef enum {
            INTERFACE_FU_CMD = 2,           //обновление прошивки
            INTERFACE_TABLE_CMD,        //команды для работы с таблицей устройств во внешней eeprom
            INTERFACE_SYSTEM_CMD,       //системные команды модема//команда с прошивкой
            INTERFACE_PROTECT_CMD,      //команды системы защиты
            INTERFACE_LOG_CMD = 7,      //команды работы с журналами событий
            INTERFACE_RESULT_CMD = 9    //ответы на команды(расширенные)
    } INTERFACE_EXT_CMD_CODE_T;

    //типы lo команд
    typedef enum {
        INIT_WRITE_GROUP_CMD = 1,
        INIT_WRITE_SCRIPT_CMD = 2,
        INIT_WRITE_ALLOW_CMD = 3,
        INIT_WRITE_STOP_CMD = 4,

        WORK_GROUP_WRITE = 5,
        WORK_ALLOW_WRITE = 6,
        WORK_CHANGE_SCRIPT = 7,
        WORK_DEL_GROUP = 8,

        ACTION_CMD = 16,
        SCANCOD_CMD = 17,

        CONTROL_CMD = 24,
        CONTROL_MODEM_CMD = 25,
        REQUEST_CMD = 32,
        ANSWER_CMD = 33
    }USART_STD_CMD_LO_CODE_T;

    //команды usart(16 бит)
    typedef enum {
        USART_GOTO_MODE_CMD = CONTROL_CMD | ((uint16)(1 << 8)),
        USART_SET_NETRO_DEVICE_CMD = CONTROL_MODEM_CMD |((uint16)(2 << 8)),//сменить устройство подмены
        USART_OPERATION_RESULT_CMD = ANSWER_CMD | ((uint16)(15 << 8)),//ответ о выполненной команды
        USART_MODEM_ANS_MEMORY_CMD = ANSWER_CMD | ((uint16)(17 << 8)),//ответ модема о состоянии памяти
        USART_MODEM_CHANGEMODE_CMD = CONTROL_MODEM_CMD |((uint16)(1 << 8)),//смена режима работы модема
        USART_MODEM_GET_VERSION_CMD = REQUEST_CMD | ((uint16)(18 << 8)),//запрос версии прошивки модема
        USART_MODEM_GET_FUNCS_CMD = REQUEST_CMD | ((uint16)(21 << 8)),//запрос функционала usb стика
        USART_MODEM_ANS_VERSION_CMD = ANSWER_CMD | ((uint16)(18 << 8)),//ответ версии прошивки
        USART_MODEM_ANS_FUNCS_CMD = ANSWER_CMD | ((uint16)(21 << 8))//ответ функционала usb стика
    }USART_STD_CMD_CODE_T;

    //команды сниффера
    typedef enum {
            INTERFACE_FU_GET_EXDEV_FLASH_INFO_CMD = INTERFACE_FU_CMD | (((unsigned short)1) << 8),//запрос сведений о автономной прошивке для ИУ
            INTERFACE_FU_INIT_CMD = INTERFACE_FU_CMD | (((unsigned short)9) << 8),//инициализация прошивки
            INTERFACE_FU_DATA_CMD  = INTERFACE_FU_CMD | (((unsigned short)10) << 8), //данные прошивки
            INTERFACE_FU_INTEGR_CMD  = INTERFACE_FU_CMD | (((unsigned short)11) << 8),     //проверка целостности прошивки
            INTERFACE_FU_STATUS_CMD  = INTERFACE_FU_CMD | (((unsigned short)12) << 8),   //Чтение статуса прошивки
            INTERFACE_FU_APPLY_CMD  = INTERFACE_FU_CMD | (((unsigned short)13) << 8)   //Применить прошивку

    } INTERFACE_FU_CMD_T;

    //команды для работы с таблицей устройств во внешней eeprom
    typedef enum {
            INTERFACE_TABLE_SET_DEV_CMD = INTERFACE_TABLE_CMD | (((unsigned short)1) << 8),//добавить в таблицу новое устройство/изменить настройки текущего устройства
            INTERFACE_TABLE_DEL_DEV_CMD = INTERFACE_TABLE_CMD | (((unsigned short)2) << 8),//удалить устройство из таблицы

            INTERFACE_TABLE_GET_FIRST_DEV_CMD = INTERFACE_TABLE_CMD | (((unsigned short)3) << 8),//установить указатель на первое устройство в таблице и запросить его
            INTERFACE_TABLE_GET_NEXT_DEV_CMD = INTERFACE_TABLE_CMD | (((unsigned short)4) << 8),//запросить следующее устройство из таблицы
            INTERFACE_TABLE_CLEAR_TABLE_CMD = INTERFACE_TABLE_CMD | (((unsigned short)5) << 8), //Очистить таблицу устройств
            INTERFACE_TABLE_COPY_CMD = INTERFACE_TABLE_CMD | (((unsigned short)6) << 8), //копирование таблицы из/в ИУ(зависит от флагов)
            INTERFACE_TABLE_GET_HASH_CMD = INTERFACE_TABLE_CMD | (((unsigned short)7) << 8), //Запрос хэша текущей таблицы устройств
            INTERFACE_TABLE_GET_SECRET_BLOCK_CMD = INTERFACE_TABLE_CMD | (((unsigned short)8) << 8), //Запрос секретов таблицы устройств
            INTERFACE_TABLE_SET_SECRET_BLOCK_CMD = INTERFACE_TABLE_CMD | (((unsigned short)9) << 8) //Установка секретов таблицы устройств

    } INTERFACE_TABLE_CMD_T;

    //системные команды модема
    typedef enum {
            INTERFACE_SYSTEM_GET_PARAM_CMD = INTERFACE_SYSTEM_CMD | (((unsigned short)1) << 8),//запрос параметра модема/иу
            INTERFACE_SYSTEM_GET_MAC_CMD = INTERFACE_SYSTEM_CMD | (((unsigned short)2) << 8),//запрос уникального номера модема
            INTERFACE_SYSTEM_GET_PROGRESS_CMD = INTERFACE_SYSTEM_CMD | (((unsigned short)3) << 8),//запрос прогресса выполнения прошлой команды
            INTERFACE_SYSTEM_INIT_DEVICE = INTERFACE_SYSTEM_CMD | (((unsigned short)4) << 8),//записать id модема в И
            INTERFACE_SYSTEM_SET_PARAM_CMD = INTERFACE_SYSTEM_CMD | (((unsigned short)5) << 8),//запись параметра в модем/иу
            INTERFACE_SYSTEM_SET_DEF_PARAM_CMD = INTERFACE_SYSTEM_CMD | (((unsigned short)6) << 8),//сброс в заводские настройки модема/иу
            INTERFACE_SYSTEM_GET_CONFIGS = INTERFACE_SYSTEM_CMD | (((unsigned short)0xa) << 8),//запросить список существующих конфигураций
            INTERFACE_SYSTEM_GET_CONFIG_PARAM = INTERFACE_SYSTEM_CMD | (((unsigned short)0xb) << 8),//запросить параметр конфигурации
            INTERFACE_SYSTEM_SET_CURRENT_CONFIG = INTERFACE_SYSTEM_CMD | (((unsigned short)0xc) << 8),//установить текущую конфигурацию
            INTERFACE_SYSTEM_CREATE_CONFIG = INTERFACE_SYSTEM_CMD | (((unsigned short)0xd) << 8),//создать новую конфигурацию
            INTERFACE_SYSTEM_DELETE_CONFIG = INTERFACE_SYSTEM_CMD | (((unsigned short)0xe) << 8),//удалить конфигурацию
            INTERFACE_SYSTEM_SET_CONFIG_PARAM = INTERFACE_SYSTEM_CMD | (((unsigned short)0xf) << 8),//установить параметр конфигурации
            INTERFACE_SYSTEM_FORMAT_FLASH = INTERFACE_SYSTEM_CMD | (((unsigned short)0x10) << 8)//форматирование уастков флеш памяти usb-stick
    } INTERFACE_SYSTEM_CMD_T;

    typedef enum {
            INTERFACE_PROTECT_GET_STATUS_CMD = INTERFACE_PROTECT_CMD | (((unsigned short)1) << 8),//запрос состояния авторизированности и присутствия защиты
            INTERFACE_PROTECT_AUTH_CMD = INTERFACE_PROTECT_CMD | (((unsigned short)2) << 8),//авторизация
            INTERFACE_PROTECT_SET_PASS_CMD = INTERFACE_PROTECT_CMD | (((unsigned short)3) << 8),//установка/сброс пароля
            INTERFACE_PROTECT_FORGET_PASS_CMD = INTERFACE_PROTECT_CMD | (((unsigned short)4) << 8)//забыть пароль с помощью БУ

    } INTERFACE_PROTECT_CMD_T;
    typedef enum {
            INTERFACE_LOG_GET_INFO_CMD = INTERFACE_LOG_CMD | (((unsigned short)1) << 8),//запрос состояния журналов
            INTERFACE_LOG_GET_EVENTS_CMD = INTERFACE_LOG_CMD | (((unsigned short)2) << 8)//запрос событий журналов
    } INTERFACE_LOG_CMD_T;

    //команды ответов на расширенные команы
    typedef enum {
            INTERFACE_RESULT_STATUS_CMD = INTERFACE_RESULT_CMD | (((unsigned short)0) << 8),//результат выполнения команды
            INTERFACE_RESULT_TABLE_DEV_CMD = INTERFACE_RESULT_CMD | (((unsigned short)0x31) << 8),//устройство в таблице
            INTERFACE_RESULT_TABLE_STATUS_CMD = INTERFACE_RESULT_CMD | (((unsigned short)0x35) << 8),//состояние памяти usb стика
            INTERFACE_RESULT_SYSTEM_PARAM_CMD = INTERFACE_RESULT_CMD | (((unsigned short)0x41) << 8),//параметр
            INTERFACE_RESULT_SYSTEM_MAC_CMD = INTERFACE_RESULT_CMD | (((unsigned short)0x42) << 8),//мак адрес модема
            INTERFACE_RESULT_PROTECT_STATUS_CMD = INTERFACE_RESULT_CMD | (((unsigned short)0x51) << 8),//состояние защиты модема
            INTERFACE_RESULT_HELLO_CMD = INTERFACE_RESULT_CMD | (((unsigned short)0x62) << 8),//состояние hello
            INTERFACE_RESULT_DEVICE_ANSWER_CMD = INTERFACE_RESULT_CMD | (((unsigned short)0xff) << 8),//ответ от устройства
            INTERFACE_RESULT_BUVO_CMD = INTERFACE_RESULT_CMD | (((unsigned short)0x15) << 8),//ответ о принятой команде сниффера
            INTERFACE_RESULT_SYSTEM_CONFIGS = INTERFACE_RESULT_CMD | (((unsigned short)0x4a) << 8),//ответ о существующих конфигурациях
            INTERFACE_RESULT_SYSTEM_CONFIG_PARAM = INTERFACE_RESULT_CMD | (((unsigned short)0x4b) << 8),//ответ о параметре конфигурации
            INTERFACE_RESULT_SYSTEM_INIT_DEVICE = INTERFACE_RESULT_CMD | (((unsigned short)0x44) << 8),//ответ о результате операции инициализации устройства
            INTERFACE_RESULT_SNIFFER_LAST_RSSI_CMD = INTERFACE_RESULT_CMD | (((unsigned short)0x16) << 8),//ответ о результате уровня rssi
            INTERFACE_RESULT_TABLE_HASH_CMD = INTERFACE_RESULT_CMD | (((unsigned short)0x37) << 8),//хэш памяти пультов
            INTERFACE_RESULT_TABLE_SECRET_BLOCK_CMD = INTERFACE_RESULT_CMD | (((unsigned short)0x38) << 8),//секреты памяти пультов
            INTERFACE_RESULT_EXDEV_FLASH_INFO_CMD = INTERFACE_RESULT_CMD | (((unsigned short)0x21) << 8), //ответ о сведениях автономной прошивки ИУ в стике
            INTERFACE_RESULT_LOG_INFO = INTERFACE_RESULT_CMD | (((unsigned short)0x71) << 8), //ответ о сведениях журналов событий
            INTERFACE_RESULT_LOG_EVENTS = INTERFACE_RESULT_CMD | (((unsigned short)0x72) << 8), //ответ о событиях из журнала
            INTERFACE_RESULT_FORGET_PASS = INTERFACE_RESULT_CMD | (((unsigned short)0x54) << 8) //ответ о забытии пароля
    } INTERFACE_RESULT_CMD_T;


    // результат полученной команды USART, который передается в ответе
    typedef enum {
            INTERFACE_RESULT_OK_CONST		=	1,
            INTERFACE_RESULT_CRCERROR_CONST	=	2,
            INTERFACE_RESULT_OVERFLOW_CONST	=	3,
            INTERFACE_RESULT_WAIT_CONST	=	4,
            INTERFACE_RESULT_DONTWAIT_CONST	=	5,
            INTERFACE_RESULT_APPLY_OK_CONST   = 6,
            INTERFACE_RESULT_CONERROR_CONST   = 7,
            INTERFACE_RESULT_PROGRESS_CONST     =       8,
            INTERFACE_RESULT_AUTH_REQ_CONST     =       9,
            INTERFACE_RESULT_DEV_ERR_1_CONST = 10

    } INTERFACE_RESULT_T;



    // результат полученной команды USART, который передается в ответе(для стандартной команды)
    typedef enum {
            NET_WRITEALLOW_OK_CONST		=	1,
            NET_WRITEALLOW_CRCERROR_CONST	=	2,
            NET_WRITEALLOW_OVERFLOW_CONST	=	3,
            NET_WRITEALLOW_WAIT_CONST	=	4,
            NET_WRITEALLOW_DONTWAIT_CONST	=	5,
            NET_WRITEALLOW_APPLY_OK_CONST   = 6
    } NET_ANSWER_T;
    //Флаги
    typedef enum {
       INTERFACE_TABLE_CLEAR_MODEM_FLAG = 0x01, //флаг очистки памяти на модеме
       INTERFACE_TABLE_CLEAR_DEVICE_FLAG = 0x02 //флаг очистки памяти на устройстве

    } INTERFACE_TABLE_CLEAR_FLAGS;


    explicit QRS232Message(QObject * parent = NULL,const QByteArray & buf = QByteArray());
    //copy-конструктор
    QRS232Message(const QRS232Message & message);
    QRS232Message(const QInterfaceMessage & message);
    QRS232Message &operator=(const QRS232Message &);//для операторов =
    PARSE_RESULT parse();
    bool create(uint16 cmd, uint16 flags, uint8 * pData,uint32 len);
    bool createStd(uint16 group, uint8 cmd_lo, uint8 cmd_hi, uint8 data_lo, uint8 data_hi,uint8 attr_lo,uint8 attr_hi);
    //очистка объекта сообщения
    virtual void clear();
    //анализирует сообщение in_mess как ответ для текущего сообщения
    //возвращает true, если ответ соответствует сообщению либо ответ "в прогрессе"
    //поэтому следует также анализировать ans_value на значение INTERFACE_RESULT_PROGRESS_CONST
    bool isGoodAck(const QRS232Message & in_mess, uint16 * ans_value);
    //возвращает true, если текущая команда расширенная. Если стандартная, возвращает false
    bool isExtCmd(){return ext_cmd;}
    //возвращает буфер netro если он содержится в команде
    QByteArray netroCommand(){return netro_buf;}
    uint16  getCommand(){return cmd;}
    //Запрос Id(для расширеннвх команд, иначе возвращает (-1))
    uint32 getId(){return ext_cmd ? dev_id : (-1);}
    uint16 getNx(){return nx;}
    uint16 getCnt(){return cnt;}
    bool getInit(){return init_cmd;}
    uint16 getFlags(){return flags;}
    uint16 getStdData(){return std_data;}
    uint16 getStdGroup(){return std_group;}
    QByteArray getExtDataBuf();//возвращает лишь буфер с расширенными данными(после флагов, не включая crc)
    //тип сообщения
    virtual int messageType()const{return 1;}
private:
    uint16 calc_crc(uint8 * data, uint32 len);
    bool ext_cmd;       //расширенная ли команда или стандартная
    uint16 cmd;         //команда(стандартная или расширенная)
    uint16 flags;       //флаги в расширенных командах и атрибуты в стандартных командах
    uint16 std_data;    //данные в стандартной команде
    uint16 std_group;   //группа в стандартных командах

    QByteArray netro_buf;
    //расшифровка параметров команды
    uint32 dev_id;
    uint16 nx;
    uint16 cnt;
    bool init_cmd;
};

#endif // RS232MESSAGE_H
