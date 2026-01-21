#ifndef FPS_H
#define FPS_H

#include "DxLib.h"

// === 設定 ===
const  int TARGET_FPS = 60;
const  int FRAME_INTERVAL = 1000 / TARGET_FPS;  // 16ms/フレーム
const  int MIN_FPS_INTERVAL = 250;             // 0.25秒区間の最小FPS計算

// === FPS用変数 ===
extern int prevTime;
extern int intervalStartTime;
extern int maxFrameTime;
extern double displayMinFPS;
extern double instantFPS;

// === FPS初期化関数 ===
void InitFPS();

// === FPS更新関数 ===
void UpdateFPS();

#endif