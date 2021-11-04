#ifdef relays
/*
  Как выход, pcf может нормально работать только с LOW LEVEL TGIGGER!!!

*/



#include "EEPROM.h" // для сохранения состояния реле и считывания при перезагрузке

bool readConf = false; // настройки считаны при загрузке?


void changeRelayState() //меняем состояние реле согласно целевому массиву
{
  for (uint8_t p = 0; p < pcfOutNum; p++) { // цикл по каждой микросхеме  pcf pcfOutNum
    for (uint8_t i = 0; i < 16; i++) { // цикл по пинам  pcf (было первые 16 реле  )
      if (i + p * 16 == RELAY_NUM) {
        break; // проверили все контакты с реле?
      }
      if (TargetRelay[i + p * 16] == ! StatusRelay[i + p * 16]) { // если требуемый статус реле отличается от текущего
#ifdef pcf
        bool out_stat;
        out_stat = (TargetRelay[i + p * 16] == 0) ?  HIGH : LOW;
        if (p == 0) {
          out1.write(i, out_stat); // меняем состояние пина pcf1 на требуемое
        }
        if (p == 1) {
          out2.write(i, out_stat); // меняем состояние пина pcf2 на требуемое
        }
        if (p == 2) {
          out3.write(i, out_stat); // меняем состояние пина pcf3 на требуемое
        }
        Serial.print("Переключили r"); Serial.println(i + p * 16 + 1);
        //  printHex(x);
#endif
        StatusRelay[i + p * 16] = TargetRelay[i + p * 16]; // и запоминаем, что все изменения применились
        if (readConf ) { // если конфигурация уже была считана ранее из памяти, нужно записать изменения в память
          Serial.println("Сохраненное значение отличается от требуемого. Сохраняем...");
          EEPROM.write(i + p * 16, TargetRelay[i + p * 16]); // сохраняем состояние в eeprom
          Serial.print("Записано состояние реле");
          Serial.print(i + p * 16 + 1);
          Serial.print(" ");
          Serial.println(TargetRelay[i + p * 16]);

        } //if

        /*
          //и публикуем через MQTT новый статус реле в формате JSON
          StaticJsonDocument<16> root;
          root[strRelay[i+p*16]] = StatusRelay[i+p*16] ? 1 : 0;
          char buffer[measureJson(root) + 1];
          serializeJson(root, buffer, sizeof(buffer));
          #ifdef mqtt
          client.publish(MQTT_JSON_RELAY_STATE_TOPIC, buffer, true);
          #endif
        */
        //publishRelayState(); // публикуем состояние всех реле
        //и публикуем через MQTT новый статус реле
        publishRelayNumState(i+p*16);        
      } //if

    } //for по пинам
  } //for по микросхемам



  EEPROM.commit();
  readConf = true;
}

void setup_relay() {

  if (!EEPROM.begin(RELAY_NUM))
  {
    Serial.println("Ошибка чтения EEPROM");
  }
  Serial.println(" Считываем состояние реле из памяти:");
  for (int i = 0; i < RELAY_NUM; i++)
  {
    Serial.print(byte(EEPROM.read(i))); Serial.print(" ");
  }
  Serial.println();

  // восстанавливаем сохраненное состояние реле до перезагрузки

  for (int j = 0; j < RELAY_NUM; j++) { //цикл2 по битам регистра с состояниями реле

    if (EEPROM.read(j) == 0  || EEPROM.read(j) == 1)  {  // если было ранее сохранено состояние реле, считываем его
      TargetRelay[j] = EEPROM.read(j); //Сохраненное в памяти состояние реле запишем в бите j переменной с требуемыми состояниями  реле
      StatusRelay[j] = 0; // заполняем нулями текущее состояние реле, чтобы менять в лупе
    }//if
    else {
      EEPROM.write(j, 0);
    }
    //   Serial.println(TargetRelay[j]);
  } //for
  EEPROM.commit(); // применяем запись

}
void loop_relay() {
  changeRelayState(); // применяем требуемые изменения в статусах реле
}

#endif
