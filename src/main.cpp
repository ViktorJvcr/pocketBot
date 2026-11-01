#include <Arduino.h>
#include <Wire.h>

#include "pins.h"
#include "display/display.h"
#include "hardware/touch_input.h"
#include "sensors/prox_sensor.h"
#include "sensors/imu_sensor.h"

Adafruit_ST7735 tft = Adafruit_ST7735(&SPI, TFT_CS_PIN, TFT_DC_PIN, TFT_RST_PIN);
PocketSprite sprite(SCREEN_W, SCREEN_H);

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
    ProxSensor::update();
    ImuSensor::update();

    sprite.fillSprite(0x0000);
    
    // Зеленая рамка по краям экрана
    sprite.drawRect(0, 0, SCREEN_W, SCREEN_H, 0x03E0);

    sprite.setTextColor(0x07E0); // Зеленый
    sprite.setTextSize(1);

    // Строка 1: Заголовок
    sprite.setCursor(6, 6);
    sprite.printf("ATON POCKET C3 TEST");

    // Строка 2: Датчик приближения APDS-9930
    sprite.setCursor(6, 20);
    if (ProxSensor::isConnected()) {
        sprite.setTextColor(0x07FF); // Cyan
        sprite.printf("APDS(0x%02X): %4u", ProxSensor::getDeviceId(), ProxSensor::getProximity());
    } else {
        sprite.setTextColor(0xF800); // Red
        sprite.printf("APDS: NOT FOUND!");
    }

    // Строка 3: BMI160
    sprite.setCursor(6, 34);
    if (ImuSensor::isConnected()) {
        sprite.setTextColor(0xFFE0); // Yellow
        sprite.printf("BMI(0x%02X) X:%.1f Y:%.1f", 
                      ImuSensor::getAddress(), ImuSensor::getAccX(), ImuSensor::getAccY());
    } else {
        sprite.setTextColor(0xF800); // Red
        sprite.printf("BMI160: NOT FOUND!");
    }

    // Строка 4: Сенсорные кнопки TTP223
    sprite.setCursor(6, 52);
    sprite.setTextColor(0xFFFF);
    sprite.print("BTN: ");
    
    for (int i = 1; i <= 3; i++) {
        bool pressed = TouchInput::isPressed(i);
        sprite.setTextColor(pressed ? 0x0000 : 0x07E0, pressed ? 0x07E0 : 0x0000);
        sprite.printf(" [%d] ", i);
    }

    // Выводим буфер кадра на экран без мерцания!
    pushSpriteToDisplay(tft, sprite);
    delay(16);
}
