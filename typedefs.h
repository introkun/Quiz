#ifndef _TYPEDEFS_H_
#define _TYPEDEFS_H_

#include <sys/types.h>
#define MAX_OWNER_LENGTH 59 //максимальное количество символов в имени владельца пульта
#define EMPTY_MAC_ADDR 0xffffffff
#define STICK_PASSWORD_SIZE 20

#define RC_ACTIVE_COLOR_NUMBER 0xA7E8C1
#define RC_BLOCK_COLOR_NUMBER 0xF5B7B7
#define RC_ACTIVE_COLOR QColor(RC_ACTIVE_COLOR_NUMBER)
#define RC_BLOCK_COLOR QColor(RC_BLOCK_COLOR_NUMBER)
//typedefs
#ifndef uint8
typedef unsigned char uint8;
#endif

#ifndef int8
typedef signed char int8;
#endif

#ifndef uint16
typedef unsigned short uint16;
#endif

#ifndef int16
typedef signed short int16;
#endif

#ifndef uint32
typedef unsigned int uint32;
#endif

#ifndef int32
typedef signed int int32;
#endif

#ifndef uint64
typedef unsigned long long uint64;
#endif

#ifndef int64
typedef signed long long int64;
#endif

//#ifndef __int64
//#define long long __int64;
//#endif

// bool type
//#ifndef bool
//typedef unsigned int bool
//#endif

//ячейки псевдобитового массива.
//typedef unsigned short bit_t;
typedef unsigned char bit_t;


#ifndef BOOL
typedef int BOOL;
#endif 

//беззнаковый нормальной рязрядности
#ifndef uint
typedef unsigned uint;
#endif

#ifndef true
#define false (0)
#define true  (!0)
#endif

// структура комплексных значений
typedef struct _complex_t {
    float Re;     
    float Im; 
} complex_t;


//последний источник, откуда были загружены данные
typedef enum {
    DB_SOURCE_NONE,
    DB_SOURCE_FILE,
    DB_SOURCE_MODEM,
    DB_SOURCE_RESERVED,
    DB_SOURCE_EXDEV,
} DB_SOURCE_T;

//тип операции копирования конфигурации между устройствами
typedef enum {
    COPY_MODEM_TO_DEVICE,//запись конфигурации из стика в БУ
    COPY_DEVICE_TO_MODEM,//запись конфигурации из БУ в стик
    COPY_GUI_TO_MODEM,//запись конфигурации из GUI в стик
    COPY_GUI_TO_DEVICE//запись конфигурации из GUI в БУ
} COPY_CONFIG_T;
#endif /*_TYPEDEFS_H_*/


