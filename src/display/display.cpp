#include "display.h"

void initDisplay(Adafruit_ST7735 &tft, PocketSprite &sprite) {
    pinMode(TFT_BL_PIN, OUTPUT);
    digitalWrite(TFT_BL_PIN, HIGH); // Включаем подсветку

    // Инициализируем аппаратный SPI на наших пинах: SCK=6, MISO=-1, MOSI=7, CS=5
    SPI.begin(TFT_SCLK_PIN, -1, TFT_MOSI_PIN, TFT_CS_PIN);

    // Инициализация матрицы 0.96" 80x160 (MINI160x80)
    tft.initR(INITR_MINI160x80);
    tft.setRotation(3); // Альбомная ориентация 160x80 (или 1)
    tft.fillScreen(ST77XX_BLACK);

    sprite.fillSprite(0x0000);
}

void pushSpriteToDisplay(Adafruit_ST7735 &tft, PocketSprite &sprite) {
    tft.drawRGBBitmap(0, 0, sprite.getBuffer(), SCREEN_W, SCREEN_H);
}
