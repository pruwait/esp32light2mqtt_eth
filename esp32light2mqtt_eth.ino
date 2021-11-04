/*
   Разрабатывал для управления освещением через mqtt и выключатели
   Железо:
   Контроллер WT32-ETH01
   расширители портов pcf8575
   Выключатели подключены через оптопары
   Софт:
   IDE 1.8.13
   ESP32 boards 1.0.6 (ESP-IDF v3.3.5)

   LAN8720 можно сбросить в случае зависания, подтягивая линию nRST
   при этом Ethernet PHY меняется на 0 и ESP32 перестает видеть модуль сети


   Задачи:
   Ethetnet работает
   DHCP клиент работает
   MQTT клиент работает
   Debounce выключатели работает
   Клик, Дабл, Хоулд работает
   Сохранение состояний реле работает

    Схема обмена данными:

                        ___________
      MQTT ->          | Нужные    |
                       | Статусы   |
                       | Реле      | -> Сравниваем с текущими -> Digital Outputs
     Digital INs  ->   |___________|


Что нужно переделать:
1. Добавить вебсервер с обновлением ota +
2. Перенести статусы расширителей портов в отдельный топик +
3. Сделать отдельные топики для всех реле +
4. Изменить состояния от контроллера на ON/OFF +
5. Изменить команды для контроллера на ON/OFF +
6. Сделать возможной работу контроллера при выключенном hassio
7. Уходим от JSON схемы MQTT на схему по-умолчанию для выключателей

*/




#define pcf
#define mqtt

#define button
#define relays
#define lan


#ifdef lan
#include <ETH.h>
#endif


#define RELAY_NUM 40
#ifdef relays

uint8_t TargetRelay[RELAY_NUM]; // массив требуемых состояний реле
uint8_t StatusRelay[RELAY_NUM]; // массив текущих состояний реле
const char* strRelay[] = { "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10",
                           "r11", "r12", "r13", "r14", "r15", "r16", "r17", "r18", "r19", "r20",
                           "r21", "r22", "r23", "r24", "r25", "r26", "r27", "r28", "r29", "r30",
                           "r31", "r32", "r33", "r34", "r35", "r36", "r37", "r38", "r39", "r40"
                         }; //объявляем массив имён реле для использования в циклах заполнения json
// Serial.println(strRelay[0]); выведет  Relay1

const char* strSwitch[] = { "sw1", "sw2", "sw3", "sw4", "sw5", "sw6", "sw7", "sw8", "sw9", "sw10",
                           "sw11", "sw12", "sw13", "sw14", "sw15", "sw16", "sw17", "sw18", "sw19", "sw20",
                           "sw21", "sw22", "sw23", "sw24", "sw25", "sw26", "sw27", "sw28", "sw29", "sw30",
                           "sw31", "sw32", "sw33", "sw34", "sw35", "sw36", "sw37", "sw38", "sw39", "sw40"
                         };



#endif




#ifdef pcf
// флаги изменения состояний входов для каждого расширителя, должны меняться в прерываниях, поэтому  volatile
//#include "esp_attr.h"

//volatile bool  keyChanged1 = false;
bool  keyChanged1 = false;
bool  keyChanged2 = false;
bool  keyChanged3 = false;

unsigned long time_click1; // Переменная для хранения точки отсчета клика
unsigned long time_click2;
unsigned long time_click3;




#endif


bool In1Con = false ;
bool In2Con = false ;
bool In3Con = false ;
bool Out1Con = false ;
bool Out2Con = false ;
bool Out3Con = false ;


#include <ArduinoJson.h>



void setup() {
  Serial.begin(115200);



#ifdef pcf
  setup_pcf();
#endif

#ifdef relays
  setup_relay();
#endif


#ifdef lan
  setup_eth ();
  // delay (500);
#endif



#ifdef mqtt
  setup_mqtt ();
  // delay (500);
#endif






}// setup

void loop() {

/*
#ifdef pcf
  loop_pcf ();
#endif
*/
#ifdef relays
  loop_relay();
#endif

#ifdef button
  loop_buttons();
#endif

#ifdef mqtt
  loop_mqtt();
#endif



} // loop
