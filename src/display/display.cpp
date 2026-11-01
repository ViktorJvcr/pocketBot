#include "display.h"
#include "pins.h"

void initDisplay(TFT_eSPI &tft, TFT_eSprite &sprite) {
    pinMode(TFT_BL_PIN, OUTPUT);
    digitalWrite(TFT_BL_PIN, HIGH); // Включаем подсветку

    tft.init();
    tft.setRotation(1); // Альбомная ориентация: 160x80
    tft.fillScreen(TFT_BLACK);

    sprite.setColorDepth(16);
    sprite.createSprite(SCREEN_W, SCREEN_H);
    sprite.fillSprite(TFT_BLACK);
}
