#pragma once
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include "pins.h"

// Создаем класс-наследник холста, чтобы у него был метод fillSprite() как в TFT_eSPI!
class PocketSprite : public GFXcanvas16 {
public:
    PocketSprite(uint16_t w, uint16_t h) : GFXcanvas16(w, h) {}
    void fillSprite(uint16_t color) { fillScreen(color); }
};

void initDisplay(Adafruit_ST7735 &tft, PocketSprite &sprite);
void pushSpriteToDisplay(Adafruit_ST7735 &tft, PocketSprite &sprite);
