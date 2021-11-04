#ifdef mqtt

#include "PubSubClient.h"

//mqtt
#define Mqtt_User "mqtt"
#define Mqtt_Pswd "mqttpass"
#define Module_Name "Light"
#define Mqtt_Base_Topic Module_Name
#define Mqtt_IP_Topic Mqtt_Base_Topic "/IP/"
//#define MQTT_JSON_RELAY_STATE_TOPIC Mqtt_Base_Topic "/relay/json_status"
#define MQTT_JSON_PCF_STATE_TOPIC Mqtt_Base_Topic "/relay/pcf_status"
#define MQTT_JSON_RELAY_COMMAND_TOPIC Mqtt_Base_Topic "/relay/json_set"
//#define MQTT_JSON_SWITCH_STATE_TOPIC Mqtt_Base_Topic "/switch/json_status"
#define MQTT_ESP_STATE Mqtt_Base_Topic "/status"
//#define MQTT_DEBUG Mqtt_Base_Topic "/rgb/debug"

#define MQTT_UP  Mqtt_Base_Topic "/active"
#define MQTT_UP_online "online"
#define MQTT_UP_offline "offline"

//IPAddress server(83, 69, 87, 179); //сервер MQTT

IPAddress server(192, 168, 1, 200); //сервер MQTT

PubSubClient client(ethClient);

unsigned long last_publish_ms = 0;
long lastReconnectAttempt = 0; // для mqtt
//mqtt/


void callback_mqtt(char* topic, byte* payload, unsigned int length) {
  Serial.print("Получено сообщение [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  // Serial.println(length);
  String topicStr(topic); // получили топик

  uint8_t indexR = RELAY_NUM + 1; // найденный номер реле

  if (topicStr.indexOf("/r") >= 0) {
    topicStr.remove(0, topicStr.indexOf("/r") + 1); // отсекаем в топике начало
    topicStr.remove(topicStr.indexOf('/')); // отсекаем в топике конец
    Serial.println(topicStr); // здесь номер реле
    topicStr.remove(0, 1); // удаляем первый символ, чтобы осталось число
    if (topicStr.toInt () > 0) {
      indexR = topicStr.toInt (); //если получилось преобразовать в число - мы знаем индекс реле!
    }


    if (indexR < RELAY_NUM + 1) { // если нашли реле, можем искать команду в нагрузке
      //   Serial.print ("indexR = "); Serial.println (indexR); // нашли номер реле

      // Просматриваем символы полученной нагрузки
      if ((char)payload[0] == 'O') { // если начинается с O
        if (length == 2) { //если в нагрузке 2 символа
          if ((char)payload[1] == 'N') {
            Serial.println("Включаем");
            TargetRelay[indexR - 1] = 1;
          }
        }
        else if (length == 3) { //если в нагрузке 3 символа
          if (((char)payload[1] == 'F') && ((char)payload[2] == 'F')) {
            Serial.println("Выключаем");
            TargetRelay[indexR - 1]  = 0;
          }
        }


      } //если O


    }
    else Serial.println ("Номер реле не найден");
  }





} //callback


/*
  void callback_mqtt(char* p_topic, byte* p_payload, unsigned int p_length) {
  // обработчик полученного сообщения

  Serial.print("Получено сообщение [");
  Serial.print(p_topic);
  Serial.println("] ");






  //парсим сообщение средствами json
  StaticJsonDocument<768> root;
  DeserializationError error = deserializeJson(root, p_payload, p_length);

  if (error) {
   Serial.print(F("deserializeJson() ошибка: "));
   Serial.println(error.c_str());
   return;
  } //if
  // получаем значения
  JsonObject documentRoot = root.as<JsonObject>(); // преобразуем json в объект и получим сылку на него
  //После того, как у нас есть JsonObject , мы можем перебирать его пары ключ-значение, используя синтаксис цикла for на основе диапазона, представленный в C ++ 11
  for (JsonPair keyValue : documentRoot) {  //перебор элементов json
   Serial.print(keyValue.key().c_str()); // печатаем полученный ключ
   String myKey = keyValue.key().c_str(); // здесь получили ключ, по которому нужно найти номер реле
   uint8_t indexR = RELAY_NUM + 1; // найденный номер реле

   if (myKey.startsWith("r"))  { // ключ начинается с r?
     myKey.remove(0, 1); // удаляем первый символ, чтобы осталось число
     if (myKey.toInt () > 0) {
       indexR = myKey.toInt (); //если получилось преобразовать в число - мы знаем индекс реле!
     }
   }


   if (indexR < RELAY_NUM + 1) {
     //   Serial.print ("indexR = "); Serial.println (indexR); // нашли номер реле
   }
   else Serial.println ("Номер реле не найден");

   // Проверяем значение JsonVariant. Это строка?
   if (keyValue.value().is<const char*>()) {
     // Да!
     // Тогда можно получить значение так:
     //const char* s = keyValue.value();
     // или так:
     auto s = keyValue.value().as<const char*>();
     Serial.print("Строка: ");
     Serial.println(s); // печатаем полученное значение-строку
   }
   if (keyValue.value().is<int>()) { // это число?
     // Да!
     // Тогда можно получить значение так:
     auto s = keyValue.value().as<int>(); //здесь получили значение, которое нужно присвоить элементу массива требуемого состояния реле
     Serial.print(": ");
     Serial.println(s); // печатаем полученное значение-число
     if (indexR < RELAY_NUM + 1) {
       TargetRelay[indexR - 1] = s;
       Serial.print("Целевое значение: ");
       Serial.println(TargetRelay[indexR - 1]);
       Serial.print("Текущее значение: ");
       Serial.println(StatusRelay[indexR - 1]);
     }


   }
   if (keyValue.value().is<bool>()) { // это истина/ложь?
     // Да!
     // Тогда можно получить значение так:
     auto s = keyValue.value().as<bool>();
     Serial.print("Булен: ");
     Serial.println(s); // печатаем полученное значение-булево
   }


  }//for перебор элементов json


  } //callback

*/



//для всех реле
void publishRelayState() {

  //  root [strRelay[4]] = StatusRelay[4]; // работает!
  //  root [strRelay[5]] = StatusRelay[5];
  /*
    //через JSON
    StaticJsonDocument<1024> root;

    for (uint8_t i = 0; i < RELAY_NUM; i++) { // заполняем документ json текущими статусами реле

     root[strRelay[i]] = StatusRelay[i] ? "on" : "off";

    } //for
    char buffer[measureJson(root) + 1];
    serializeJson(root, buffer, sizeof(buffer));
    client.publish(MQTT_JSON_RELAY_STATE_TOPIC, buffer, true);
  */
  // без JSON
  for (uint8_t i = 0; i < RELAY_NUM; i++) {// цикл по всем реле для публикации статуса
    //создаём топик из констант и переменных
    // объявляем топик с нужной длиной символов
    char* sub_topic  = new char [strlen(Mqtt_Base_Topic) + strlen("/") + strlen(strRelay[i]) + strlen("/status") + 1];
    strcpy( sub_topic , Mqtt_Base_Topic); // копируем в строку топика начало
    strcat(sub_topic, "/"); // добавляем к топику продолжение
    strcat(sub_topic, strRelay[i]);
    strcat(sub_topic, "/status");
    //   Serial.println(sub_topic);
    //создаём нагрузку для топика
    uint8_t len_pload = StatusRelay[i] ? strlen("ON") : strlen("OFF"); // считаем длину
    char* sub_pload  = new char [len_pload + 1]; // объявляем указатель на нагрузку
    StatusRelay[i] ? strcpy(sub_pload, "ON") : strcpy(sub_pload, "OFF"); //заполняем нагрузку значением

    client.publish(sub_topic, sub_pload, true); //публикуем статус
    delete sub_topic; // освобождение памяти
    delete sub_pload; // освобождение памяти
  } //for
}

// для одного реле
void publishRelayNumState( uint8_t rnum) {

  // объявляем топик с нужной длиной символов
  char* sub_topic  = new char [strlen(Mqtt_Base_Topic) + strlen("/") + strlen(strRelay[rnum]) + strlen("/status") + 1];
  strcpy( sub_topic , Mqtt_Base_Topic); // копируем в строку топика начало
  strcat(sub_topic, "/"); // добавляем к топику продолжение
  strcat(sub_topic, strRelay[rnum]);
  strcat(sub_topic, "/status");
  // Serial.println(sub_topic);
  //создаём нагрузку для топика
  uint8_t len_pload = StatusRelay[rnum] ? strlen("ON") : strlen("OFF"); // считаем длину
  char* sub_pload  = new char [len_pload + 1]; // объявляем указатель на нагрузку
  StatusRelay[rnum] ? strcpy(sub_pload, "ON") : strcpy(sub_pload, "OFF"); //заполняем нагрузку значением

  client.publish(sub_topic, sub_pload, true); //публикуем статус
  delete sub_topic; // освобождение памяти
  delete sub_pload; // освобождение памяти
}

void publishESPJsonState() {
  StaticJsonDocument<32> root;
  root["freeheap"] = ESP.getFreeHeap();
  root["uptime"] = millis() / 1000;
  char buffer[measureJson(root) + 1];
  serializeJson(root, buffer, sizeof(buffer));

  client.publish(MQTT_ESP_STATE, buffer, true);

}

void publishPcfJsonState() {
  StaticJsonDocument<192> root;
  root["In1Connected"] = In1Con;
  root["In2Connected"] = In2Con;
  root["In3Connected"] = In3Con;
  root["Out1Connected"] = Out1Con;
  root["Out2Connected"] = Out2Con;
  root["Out3Connected"] = Out3Con;
  //root["freeheap"] = ESP.getFreeHeap();
  //root["uptime"] = millis() / 1000;
  char buffer[measureJson(root) + 1];
  serializeJson(root, buffer, sizeof(buffer));

  client.publish(MQTT_JSON_PCF_STATE_TOPIC , buffer, true);

}

void publishSwitchJsonState(uint8_t i, const char* stat) {
  /*StaticJsonDocument<64> root;
    root[strSwitch[i]] = stat;

    char buffer[measureJson(root) + 1];
    serializeJson(root, buffer, sizeof(buffer));
  */
  // объявляем топик с нужной длиной символов
  char* sub_topic  = new char [strlen(Mqtt_Base_Topic) + strlen("/") + strlen(strSwitch[i]) + strlen("/status") + 1];
  strcpy( sub_topic , Mqtt_Base_Topic); // копируем в строку топика начало
  strcat(sub_topic, "/"); // добавляем к топику продолжение
  strcat(sub_topic, strSwitch[i]);
  strcat(sub_topic, "/status");
  //  Serial.println(sub_topic);



  client.publish(sub_topic, stat, true);
  delete sub_topic; // освобождение памяти
}




boolean reconnectMQTT() {
  //  if (client.connect(Module_Name)) { // без пароля

  if (client.connect(Module_Name, Mqtt_User, Mqtt_Pswd)) {
    // Once connected, publish an announcement...
    Serial.println("MQTT подключен");

    client.publish(MQTT_UP, MQTT_UP_online, true);
    publishRelayState();
    publishPcfJsonState();
    // ... and resubscribe

    //client.subscribe(MQTT_JSON_RELAY_COMMAND_TOPIC);
    for (uint8_t i = 0; i < RELAY_NUM; i++) {// подписываемся на топики команд для всех реле
      //создаём топик из констант и переменных
      // объявляем топик с нужной длиной символов
      char* sub_topic  = new char [strlen(Mqtt_Base_Topic) + strlen("/") + strlen(strRelay[i]) + strlen("/set") + 1];

      strcpy( sub_topic , Mqtt_Base_Topic); // копируем в строку топика начало
      strcat(sub_topic, "/"); // добавляем к топику продолжение
      strcat(sub_topic, strRelay[i]);
      strcat(sub_topic, "/set");
      //  Serial.println(sub_topic);
      client.subscribe(sub_topic);
    } //for


  }
  return client.connected();
}

void setup_mqtt() {
  //mqtt
  client.setServer(server, 1886);
  client.setCallback(callback_mqtt);
  client.setBufferSize(512); // увеличиваем буфер для json от 32-х реле
  lastReconnectAttempt = 0; // для mqtt

}

void loop_mqtt() {
  //mqtt
  if (!client.connected()) {
    long now = millis();
    if (now - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = now;
      // Attempt to reconnect
      if (reconnectMQTT()) {
        lastReconnectAttempt = 0;
      }
    }
  } else {
    // Client connected

    client.loop();

  } //else
  // Отправляем статус каждую минуту
  unsigned long now = millis();
  if (now - last_publish_ms > 60000) {
    last_publish_ms = now;

  /*  Serial.print("Куча: ");
    Serial.println(ESP.getFreeHeap()); */

    publishRelayState();
    publishESPJsonState();

  } //if
}




#endif
