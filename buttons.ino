#ifdef button



#include <EncButton.h> // для отслеживания кликов, даблкликов, лонгпрессов, v 1.8 https://github.com/GyverLibs/EncButton

EncButton<EB_TICK, VIRT_BTN> * buttons = new EncButton<EB_TICK, VIRT_BTN> [RELAY_NUM]; // массив виртуальных кнопок


unsigned long timing; // Переменная для хранения точки отсчета


// Функции прерываний

void   IRAM_ATTR keyChangedOnPCF1() {
  // Interrupt called (No Serial no read no wire in this function, and DEBUG disabled on PCF library)
  keyChanged1 = true;
  time_click1 = millis();
}

void  IRAM_ATTR  keyChangedOnPCF2() {
  // Interrupt called (No Serial no read no wire in this function, and DEBUG disabled on PCF library)
  keyChanged2 = true;
  time_click2 = millis();
}

void  IRAM_ATTR keyChangedOnPCF3() {
  // Interrupt called (No Serial no read no wire in this function, and DEBUG disabled on PCF library)
  keyChanged3 = true;
  time_click3 = millis();
}





void loop_buttons() {

  if (millis() - timing > 10) { // Проверяем кнопки каждые 10мс
    timing = millis();

    //если нет подключенного расширителя, то флаг состояния может сбрасываться при перезагрузке
    // чтобы исключить лишние тики неподключенных расширителей с ложными событиями, проверяем подключение

    //обработка выключателей
    if (((keyChanged1) || (millis() - time_click1 < 5000)) && (In1Con)) { //при изменении входов первого расширителя или в течение 5 секунд после
      uint16_t di = in1.readButton16(); // здесь состояния всех входов после изменения как 11100011110001, в прерывании эта функция вызывает перезагрузку
      //разбираем байты и тикаем виртуальные кнопки в цикле
      /*
         p0 = ((di & bit(0))>0)?HIGH:LOW;
         p1 = ((di & bit(1))>0)?HIGH:LOW;
         p2 = ((di & bit(2))>0)?HIGH:LOW;
         p3 = ((di & bit(3))>0)?HIGH:LOW;
         p4 = ((di & bit(4))>0)?HIGH:LOW;
         p5 = ((di & bit(5))>0)?HIGH:LOW;
         p6 = ((di & bit(6))>0)?HIGH:LOW;
         p7 = ((di & bit(7))>0)?HIGH:LOW;
      */
      for (uint8_t i = 0; i < 16; i++) { // тикаем первые 16 виртуальных кнопок, т.е. buttons[0-15]
        buttons[i].tick(((di & bit(i)) > 0) ? LOW : HIGH); // кнопки подтянуты к питанию и прижимаем к GND в нажатии, поэтому HIGH = отпущена

        // публикуем событие
        if (buttons[i].isHolded()) {
          Serial.print("holded ");
          Serial.println(i + 1);
          publishSwitchJsonState(i, "holded");
        }

        if (buttons[i].isPress()) {
          Serial.print("press ");
          Serial.println(i + 1);
          publishSwitchJsonState(i, "press");
          TargetRelay[i] = ! StatusRelay[i];
        }

        if (buttons[i].hasClicks(1)) {
          Serial.print("click ");
          Serial.println(i + 1);
          publishSwitchJsonState(i, "click");
        }
        if (buttons[i].hasClicks(2)) {
          Serial.print("2 click ");
          Serial.println(i + 1);
          publishSwitchJsonState(i, "double");
        }

        //    if (buttons[i].hasClicks()) Serial.println(buttons[i].clicks);
        //     if (buttons[i].isHold()) Serial.println("hold");

      } //for

      keyChanged1 = false; //сбрасываем флаг изменённых входов расширителя
    } //if


    if (((keyChanged2) || (millis() - time_click2 < 5000))  && (In2Con)) { //при изменении входов второго расширителя
      uint16_t di = in2.readButton16(); // здесь состояния всех входов после изменения как 11100011110001
      for (uint8_t i = 0; i < 16; i++) { // тикаем вторые 16 виртуальных кнопок, т.е. buttons[16-31]
        buttons[i + 16].tick(((di & bit(i)) > 0) ? LOW : HIGH);

        // публикуем событие
        if (buttons[i + 16].isHolded()) {
          Serial.print("holded ");
          Serial.println(i + 17);
          publishSwitchJsonState(i + 16, "holded");
        }

        if (buttons[i + 16].isPress()) {
          Serial.print("press ");
          Serial.println(i + 17);
          publishSwitchJsonState(i + 16, "press");
          TargetRelay[i + 16] = ! StatusRelay[i + 16];
        }

        if (buttons[i + 16].hasClicks(1)) {
          Serial.print("click ");
          Serial.println(i + 17);
          publishSwitchJsonState(i + 16, "click");
        }
        if (buttons[i + 16].hasClicks(2)) {
          Serial.print("2 click ");
          Serial.println(i + 17);
          publishSwitchJsonState(i + 16, "double");
        }

      } //for

      keyChanged2 = false; //сбрасываем флаг изменённых входов расширителя

    } //if

    if (((keyChanged3) || (millis() - time_click3 < 5000))  && (In3Con)) { //при изменении входов третьего расширителя
      uint16_t di = in3.readButton16(); // здесь состояния всех входов после изменения как 11100011110001
      for (uint8_t i = 0; i < 8; i++) { // тикаем последние 8 виртуальных кнопок, т.е. buttons[32-39]
        buttons[i + 32].tick(((di & bit(i)) > 0) ? LOW : HIGH);

        // публикуем событие
        if (buttons[i + 32].isHolded()) {
          Serial.print("holded ");
          Serial.println(i + 33);
          publishSwitchJsonState(i + 32, "holded");
        }

        if (buttons[i + 32].isPress()) {
          Serial.print("press ");
          Serial.println(i + 33);
          publishSwitchJsonState(i + 32, "press");
          TargetRelay[i + 32] = ! StatusRelay[i + 32];
        }

        if (buttons[i + 32].hasClicks(1)) {
          Serial.print("click ");
          Serial.println(i + 33);
          publishSwitchJsonState(i + 32, "click");
        }
        if (buttons[i + 32].hasClicks(2)) {
          Serial.print("2 click ");
          Serial.println(i + 33);
          publishSwitchJsonState(i + 32, "double");
        }

      } //for

      keyChanged3 = false; //сбрасываем флаг изменённых входов расширителя

    } //if

  }
} //loop
#endif
