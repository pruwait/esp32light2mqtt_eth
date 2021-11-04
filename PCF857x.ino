#ifdef pcf

/*
  В ESP32 для Wire по умолчанию использованы пины:
  SDA (default is GPIO 21)
  SCL (default is GPIO 22)
  Эти же пины использует eth, поэтому придется соединить PCF8575 с Wire2
  Иначе сеть отваливается

  Как выход, pcf может нормально работать только с LOW LEVEL TGIGGER!!!



   PCF8575(1,2)    ----- Esp32
   A0              ----- GRD
   A1              ----- GRD
   A2              ----- GRD
   VSS             ----- GRD
   VDD             ----- 5V/3.3V
   SDA             ----- IO5_RXD
   SCL             ----- IO17_TXD
   INT1            ----- 32 (CFG)
   INT2            ----- 33 (485_EN)
   INT3            ----- 39 (Input only)

   P0     ----------------- SW0
   P1     ----------------- SW1
   P2     ----------------- SW2
   P3     ----------------- SW3
   P4     ----------------- SW4
   P5     ----------------- SW5
   P6     ----------------- SW6
   P7     ----------------- SW7



  Выбор адреса
  A0 A1 A2 ID
  -  -  -  0x20
  +  -  -  0x21
  -  +  -  0x22
  +  +  -  0x23
  -  -  +  0x24
  +  -  +  0x25
  -  +  +  0x26
  +  +  +  0x27




   (from figure 4 datasheet
  Place a pull up resistor 4K7 between pin and 5V
  Place a capacitor 10-400pF between pin and GND
  Для всех входов обязательны подтягивающие резисторы, иначе происходит ложное срабатывание

*/




#include "PCF8575.h"  // https://github.com/RobTillaart/PCF8575 Rob Tillaart
/*
  В этой библиотеке вывод PCF8575 можно использовать как выход и вход одновременно!
  Для кнопок используем readButton16() или readButton16(mask)
  Эти команды временно подтягивают к Vcc пины расширителя на момент чтения, в итоге корректно читается кнопка, замыкающая пин на GND.
*/


//Вариант2 библиотека https://github.com/xreef/PCF8575_library v1.0.2 < для использования раскомметнировать в *.h // #define PCF8575_LOW_MEMORY и изменить синтаксис
// Отключается сеть. Нужно использовать wire2


//Все GPIO могут быть настроены для обработки прерываний. По прерываниям обновляются статусы виртуальных входов
#define ESP32_INTERRUPTED_PIN1 32
#define ESP32_INTERRUPTED_PIN2 33
#define ESP32_INTERRUPTED_PIN3 39


//для Wire2
#define SDA_2 5
#define SCL_2 17

//адреса расширителей для входов и выходов
#define addrIn1 0x20
#define addrIn2 0x21
#define addrIn3 0x27
#define addrOut1 0x22
#define addrOut2 0x24
#define addrOut3 0x26


//количество микросхем для входов и выходов
#define pcfOutNum 3
#define pcfInNum 3






// Устанавливаем адреса i2c расширителей, пины и функции прерываний
PCF8575 in1(addrIn1,  &Wire1);
PCF8575 in2(addrIn2,  &Wire1);
PCF8575 in3(addrIn3,  &Wire1);
PCF8575 out1(addrOut1,  &Wire1);
PCF8575 out2(addrOut2,  &Wire1);
PCF8575 out3(addrOut3,  &Wire1);

//uint32_t click_st, dbl_st, hold_st;  // переменные для текуших статусов входных сигналов

// пока не знаю, пригодится ли эта структура
/*
  struct Button {
  //  const uint8_t PIN;
  bool pressed;
  bool dbl;
  bool holding;
  };
*/

void   IRAM_ATTR keyChangedOnPCF1() ;
void   IRAM_ATTR keyChangedOnPCF2() ;
void   IRAM_ATTR keyChangedOnPCF3() ;



void setup_pcf () {

  Wire1.begin(SDA_2, SCL_2, 100000); // запускаем wire2 интерфейс на выбранных пинах с указанной частотой

  Serial.println(__FILE__);
  Serial.print("PCF8575_LIB_VERSION: ");
  Serial.println(PCF8575_LIB_VERSION);

  if (!in1.begin())
  {
    Serial.println("Нет инициализации PCF857x IN1...");
  }
  if (!in1.isConnected())
  {
    Serial.println("=> Нет соединения с PCF857x IN1");
  }
  else
  {
    Serial.println("=> PCF857x IN1 соединен!");
    In1Con = true;
  }


  if (!in2.begin())
  {
    Serial.println("Нет инициализации PCF857x IN2...");
  }
  if (!in2.isConnected())
  {
    Serial.println("=> Нет соединения с PCF857x IN2");
  }
  else
  {
    Serial.println("=> PCF857x IN2 соединен!");
    In2Con = true;
  }

  if (!in3.begin())
  {
    Serial.println("Нет инициализации PCF857x IN3...");
  }
  if (!in3.isConnected())
  {
    Serial.println("=> Нет соединения с PCF857x IN3");
  }
  else
  {
    Serial.println("=> PCF857x IN3 соединен!");
    In3Con = true;
  }

  if (!out1.begin())
  {
    Serial.println("Нет инициализации PCF857x OUT1...");
  }
  if (!out1.isConnected())
  {
    Serial.println("=> Нет соединения с PCF857x OUT1");
  }
  else
  {
    Serial.println("=> PCF857x OUT1 соединен!");
    Out1Con = true;
  }


  if (!out2.begin())
  {
    Serial.println("Нет инициализации PCF857x OUT2...");
  }
  if (!out2.isConnected())
  {
    Serial.println("=> Нет соединения с PCF857x OUT2");
  }
  else
  {
    Serial.println("=> PCF857x OUT2 соединен!");
    Out2Con = true;
  }

  if (!out3.begin())
  {
    Serial.println("Нет инициализации PCF857x OUT3...");
  }
  if (!out3.isConnected())
  {
    Serial.println("=> Нет соединения с PCF857x OUT3");
  }
  else
  {
    Serial.println("=> PCF857x OUT3 соединен!");
    Out3Con = true;
  }


  // пинмод прерываний
  pinMode(ESP32_INTERRUPTED_PIN1, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ESP32_INTERRUPTED_PIN1), keyChangedOnPCF1, FALLING);

  pinMode(ESP32_INTERRUPTED_PIN2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ESP32_INTERRUPTED_PIN2), keyChangedOnPCF2, FALLING);

  pinMode(ESP32_INTERRUPTED_PIN3, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ESP32_INTERRUPTED_PIN3), keyChangedOnPCF3, FALLING);

  // Устанавливаем pinMode расширителей
  /*
    for (uint8_t i = 0; i < 16; i++) {
    in1.pinMode(i, INPUT);
    in2.pinMode(i, INPUT);
    out1.pinMode(i, OUTPUT);
    out2.pinMode(i, OUTPUT);
    }
  */
  // выключаем свет
  out1.write16(0xFFFF);
  out2.write16(0xFFFF);
  out3.write16(0xFFFF);

  //подтягиваем к плюсу входы

}

/*
void loop_pcf () {

  uint32_t now = millis();
    if (keyChanged1)
    {
     keyChanged1 = false;
     uint16_t x = in1.readButton16();
     Serial.print("READ But16:\t");
     Serial.print('\t');
     Serial.print(now);
     Serial.print('\t');
     Serial.println(x, BIN);
    }
  
}
*/



#endif
