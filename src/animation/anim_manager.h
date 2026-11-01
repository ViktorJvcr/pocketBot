#pragma once
#include "../display/display.h

enum class AnimType {
    IDLE,
    TRANSITION_TO_SLEEP,
    SLEEP,
    JOY,
    ANGER,
    SAD,
    DIZZY,
    SCARED,
    WIND,
    WIND_EASY,
    SWEAR
};

namespace AnimManager {
    void init(TFT_eSPI &tft);
    void runCurrentAnimation(Adafruit_ST7735 &tft, PocketSprite &sprite);
    void setAnimation(AnimType type, bool blink = true);
    AnimType getCurrentAnimation();

    void triggerScared();
    void triggerDizzy();
    void resetActivityTimer();
    void nextState();
}
