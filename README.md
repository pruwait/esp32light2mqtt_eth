# esp32light2mqtt_eth
ESP32 MQTT Light control

Разрабатывал для управления освещением через mqtt и выключатели.

В текущей реализации подключено 40 выключателей, которые средствами контроллера могут переключать 40 нагрузок (через реле на дин-рейке) без участия умного дома.
Кроме того, выключатели отправляют по mqtt события клик, даблклик, хоулд для использования в сценариях через скрипты умного дома.
Примеры сценариев: 
1. По клику в гардеробной включается свет через реле, кроме того, запускается скрипт с таймером на умном доме, который выключит свет через 5 минут.
2. По удержанию выключателей бра возле кровати в спальне, запускается скрипт выключения освещения во всём доме, выключения телевизоров, закрытие входной двери на замок.
3. По двойному щелчку выключателя спальни включаются и выключаются все фонари во дворе плюс освещение балкона.
4. При удержании выключателя в прихожей включается компьютер в кабинете второго этажа и кондиционер там же, устанавливается температура +22С.
Контроллер подписывается на топики MQTT, через которые умный дом управляет освещением (нагрузками через реле) контроллёра. Реакция мгновенная.

   
   Железо:
   
   - Контроллер WT32-ETH01
   - расширители портов pcf8575
   - Выключатели подключены через оптопары для гальванической развязки
   - Лампы освещения подключены через твердотельные реле PN5-10DA к расширителям портов
   
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
     
     

