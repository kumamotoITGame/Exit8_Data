#include "FPS.h"

int prevTime;
int intervalStartTime;
int maxFrameTime;
double displayMinFPS;
double instantFPS;


// === FPS‰Šú‰»ŠÖ” ===
void InitFPS(){
    prevTime = GetNowCount();
    intervalStartTime = prevTime;
    maxFrameTime = 0;
    displayMinFPS = 0.0;
    instantFPS = 0.0;
}

// === FPSXVŠÖ” ===
void UpdateFPS(){
    int nowTime = GetNowCount();
    int frameTime = nowTime - prevTime;
    prevTime = nowTime;

    if( frameTime > 0 ) instantFPS = 1000.0 / frameTime;

    if( frameTime > maxFrameTime ) maxFrameTime = frameTime;

    if( nowTime - intervalStartTime >= MIN_FPS_INTERVAL ){
        if( maxFrameTime > 0 ) displayMinFPS = 1000.0 / maxFrameTime;
        maxFrameTime = 0;
        intervalStartTime = nowTime;
    }
}