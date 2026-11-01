#include <Arduino.h>
#include <Wire.h>
#include <TFT_eSPI.h>

#include "pins.h"
#include "display/display.h"
#include "hardware/touch_input.h"
#include "sensors/prox_sensor.h"
#include "sensors/imu_sensor.h"

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);

void setup() {
    Serial.begin(115200);
    
    // 1. Запуск шины I2C (SDA=8, SCL=10)
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
    Wire.setClock(400000);

    // 2. Инициализация периферии
    initDisplay(tft, sprite);
    TouchInput::init();
    ProxSensor::init();
    ImuSensor::init();
}

void loop() {
    // Обновляем датчики (без блокировок, на millis)
    ProxSensor::update();
    ImuSensor::update();

    // Рисуем диагностический экран в спрайте
    sprite.fillSprite(TFT_BLACK);
    
    // Рамка по краям экрана (проверка правильности смещения матрицы ST7735!)
    sprite.drawRect(0, 0, SCREEN_W, SCREEN_H, TFT_DARKGREEN);

    sprite.setTextColor(TFT_GREEN, TFT_BLACK);
    sprite.setTextSize(1);

    // Строка 1: Заголовок
    sprite.setCursor(6, 6);
    sprite.printf("ATON POCKET C3 TEST");

    // Строка 2: Датчик приближения APDS-9930
    sprite.setCursor(6, 20);
    if (ProxSensor::isConnected()) {
        sprite.setTextColor(TFT_CYAN, TFT_BLACK);
        sprite.printf("APDS(0x%02X): %4u", ProxSensor::getDeviceId(), ProxSensor::getProximity());
    } else {
        sprite.setTextColor(TFT_RED, TFT_BLACK);
        sprite.printf("APDS: NOT FOUND!");
    }

    // Строка 3: BMI160
    sprite.setCursor(6, 34);
    if (ImuSensor::isConnected()) {
        sprite.setTextColor(TFT_YELLOW, TFT_BLACK);
        sprite.printf("BMI(0x%02X) X:%.1f Y:%.1f", 
                      ImuSensor::getAddress(), ImuSensor::getAccX(), ImuSensor::getAccY());
    } else {
        sprite.setTextColor(TFT_RED, TFT_BLACK);
        sprite.printf("BMI160: NOT FOUND!");
    }

    // Строка 4: Сенсорные кнопки TTP223
    sprite.setCursor(6, 52);
    sprite.setTextColor(TFT_WHITE, TFT_BLACK);
    sprite.print("BTN: ");
    
    for (int i = 1; i <= 3; i++) {
        bool pressed = TouchInput::isPressed(i);
        sprite.setTextColor(pressed ? TFT_BLACK : TFT_GREEN, pressed ? TFT_GREEN : TFT_BLACK);
        sprite.printf(" [%d] ", i);
    }

    // Выводим кадр на экран
    sprite.pushSprite(0, 0);
    delay(16); // ~60 FPS
}
