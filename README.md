# esp32light2mqtt_eth
MQTT Light control

Разрабатывал для управления освещением через mqtt и выключатели
   
   Железо:
   
   - Контроллер WT32-ETH01
   - расширители портов pcf8575
   - Выключатели подключены через оптопары для гальванической развязки
   
   Софт:
   
    - IDE 1.8.13
    - ESP32 boards 1.0.6 (ESP-IDF v3.3.5)
   
      
   Задачи:
   1. Ethetnet
   2. DHCP клиент
   3. MQTT клиент
   4. Debounce выключатели
   5. Клик, Дабл, Хоулд со всех выключателей попадают в MQTT для сценариев через Home Assistant
   6. Сохранение состояний реле, при выключении электричества и повторном включении свет будет включен где был.
   7. Обновление по сети OTA
   8. Простая интеграция в Home Assistant
   
   


    Схема обмена данными:
    
                        ___________
      MQTT ->          | Нужные    |
                       | Статусы   |
                       | Реле      | -> Сравниваем с текущими -> Digital Outputs
     Digital INs  ->   |___________|
     
     
Что нужно доделать:

Проверить возможной работу контроллера при выключенном hassio

