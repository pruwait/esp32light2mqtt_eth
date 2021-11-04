# esp32light2mqtt_eth
MQTT Light control

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
