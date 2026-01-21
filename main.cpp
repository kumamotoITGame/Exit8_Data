#include "DxLib.h"
#include <stdlib.h>
#include <time.h>

#include "FPS.h"


// 移動関連
#define MOVE_SPEED 10
#define DASH_RATE  4

// プレイヤー画像サイズ
#define PLAYER_W 64
#define PLAYER_H 64

// 背景画像サイズ
#define BG_W 1024
#define BG_H 1024

// アニメーション
#define WALK_ANIM_MAX 6
#define IDLE_ANIM_MAX 4
#define WALK_ANIM_INTERVAL 10
#define IDLE_ANIM_INTERVAL 28

// エリア最大番号
#define AREA_MAX 8
#define AREA_GOOL 9

// 足音
#define FOOTSTEP 3

// 異変
#define ANOMALY_RATE 2   // 異変発生確率

// 次の異変を決定する
void NextRoot( int* wrong , int area ){
    if( area < AREA_MAX ){
        // 次の部屋の異変を決定
        *wrong = rand() % ANOMALY_RATE;
    }else{
        *wrong = 1;
    }
}

int WINAPI WinMain( HINSTANCE hInstance , HINSTANCE hPrevInstance , LPSTR lpCmdLine , int nCmdShow ){

    const int WIDTH = 1980 , HEIGHT = 1080;			//サイズ
    SetGraphMode( WIDTH , HEIGHT , 32 );

    //SetWaitVSyncFlag(FALSE );  // モニターのリフレッシュレートに合わせて描画されます

    // ウインドウモード
    ChangeWindowMode( FALSE );

    SetWindowText( "8番出口 DX版" );

    if( DxLib_Init() == -1 ) return -1;
    SetDrawScreen( DX_SCREEN_BACK );

    srand( (unsigned int)time( NULL ) );

    // 画像読み込み
    int bg = LoadGraph( "image/BG.png" );
    int bgExit = LoadGraph( "image/BGExit.png" );

    //int bgW , bgH;
    //GetGraphSize( bg , &bgW , &bgH );

    int player[ 12 ];
    LoadDivGraph( "image/player.png" , 12 , 4 , 3 , PLAYER_W , PLAYER_H , player );

    int signNormals[ 9 ];
    LoadDivGraph( "image/add.png" , 9 , 3 , 3 , 300 , 300 , signNormals );

    int signWrongs[ 9 ];
    LoadDivGraph( "image/add_wrong0.png" , 9 , 3 , 3 , 300 , 300 , signWrongs );

    int stairs[ 16 ];
    LoadDivGraph( "image/Exit.png" , 10 , 2 , 5 , 199 , 102 , stairs );

    int footstep = LoadSoundMem( "SE/walkSE.mp3" );

    // プレイヤー初期化
    int anim = 0;
    int animCnt = 0;
    int flip = 1;
    int atlasOffset = 6;

    // ゲーム状態
    int area = 0;               // 場所
    int wrongRoot = 1;          // 異変フラグ(1以上で異変し)
    int bx = WIDTH / 2;         // 背景の位置
    int by = 0;                 // 背景の位置
    int ax = WIDTH / 2;         // エリアの位置

    // FPSデバッグ
    InitFPS();

    while( 1 ){

        ClearDrawScreen();

        // 背景
        //DrawGraph( bx - BG_W , by , bg , FALSE );
        //DrawGraph( bx        , by , bg , FALSE );
        //DrawGraph( bx + BG_W , by , bg , FALSE );
        for( int i = -1; i <= 1; i++ ){// i は -1 ～ 1 までforで繰り返す
            DrawGraph( bx + i * BG_W , by , bg , FALSE );
        }

        // 出口背景表示(ゴール)
        if( area == AREA_GOOL )DrawGraph( ax - BG_W , by , bgExit , FALSE );

        // 広告
        if( wrongRoot ){
            // 異変なし
            for( int i = 0 ; i < 9 ; i++ ){
                DrawGraph( ax + i * 400 , 300 , signNormals[ i ] , TRUE );
            }
        }
        else{
            // 異変あり
            for( int i = 0 ; i < 9 ; i++ ){
                DrawGraph( ax + i * 400 , 300 , signWrongs[ i ] , TRUE );
            }
        }

        // 出口番号
        if( area == AREA_GOOL ){
            DrawGraph( ax - 600 * 2 , 300 , stairs[ area ] , TRUE );
        }
        else{
            DrawGraph( ax - 600 , 300 , stairs[ area ] , TRUE );
        }

        // コントローラー入力
        //int key = GetJoypadInputState( DX_INPUT_KEY_PAD1 );

        // シフトボタンの確認
        bool dash = ( CheckHitKey( KEY_INPUT_LSHIFT ) || CheckHitKey( KEY_INPUT_RSHIFT ) );
        // プレイヤー移動
        int move = MOVE_SPEED * ( dash ? DASH_RATE : 1 );

        animCnt++;
        if( CheckHitKey( KEY_INPUT_A ) || CheckHitKey( KEY_INPUT_LEFT ) /* || key & PAD_INPUT_RIGHT*/ ){
            
            // BG移動
            bx += move;
            ax += move;

            // アニメ制御
            flip = -1;
            if( animCnt > WALK_ANIM_INTERVAL ){
                atlasOffset = 0;                                 // アトラステクスチャの位置を決定
                anim = ( anim + 1 ) % WALK_ANIM_MAX;
                animCnt = 0;

                // SE
                if( !( anim % FOOTSTEP ) )PlaySoundMem( footstep , DX_PLAYTYPE_BACK );// 足音
            }
        }else
        if( CheckHitKey( KEY_INPUT_D ) || CheckHitKey( KEY_INPUT_RIGHT ) /* || key & PAD_INPUT_LEFT */ ){

            // BG移動
            bx -= move;
            ax -= move;

            // アニメ制御
            flip = 1;
            if( animCnt > WALK_ANIM_INTERVAL ){
                atlasOffset = 0;                                 // アトラステクスチャの位置を決定
                anim = ( anim + 1 ) % WALK_ANIM_MAX;
                animCnt = 0;

                // SE
                if( !( anim % FOOTSTEP ) )PlaySoundMem( footstep , DX_PLAYTYPE_BACK );// 足音
            }
        }
        else{
            // アニメ制御
            if( animCnt > IDLE_ANIM_INTERVAL ){
                atlasOffset = 6;                                // アトラステクスチャの位置を決定
                anim = ( anim + 1 ) % IDLE_ANIM_MAX ;
                animCnt = 0;
            }
        }

        // 丸影
        // 半透明設定（0~255）
        //SetDrawBlendMode( DX_BLENDMODE_ALPHA , 100 );

        // 丸影
        if( animCnt % 2 ){
            DrawOval(
                WIDTH / 2 , HEIGHT / 2 + 160 ,          // 中心座標
                60 , 20 ,                               // 横半径・縦半径
                GetColor( 0 , 0 , 0 ) ,
                TRUE                                    // 塗りつぶし
            );
        }

        // ブレンド解除（重要）
        //SetDrawBlendMode( DX_BLENDMODE_NOBLEND , 0 );

        // プレイヤー
        DrawExtendGraph(
              WIDTH / 2 - PLAYER_W * 2 * flip , HEIGHT / 2 - PLAYER_H * 2 + 64  // 左上座標
            , WIDTH / 2 + PLAYER_W * 2 * flip , HEIGHT / 2 + PLAYER_H * 2 + 64  // 右下座標
            , player[  anim + atlasOffset ] , TRUE
        );

        // --------------------------- スクロール制御 ------------------------------
        // 背景移動
        if( bx < 0 ){
            bx += BG_W;
        }
        // 右端判定
        if( ax < -BG_W * 3 ){
            ax += BG_W * 6;

            if( wrongRoot ){
                area++;
            }
            else{
                // 間違えた
                area = 0;
            }

            NextRoot( &wrongRoot , area );

        }

        // 背景移動
        if( bx > BG_W ){
            bx -= BG_W;
        }
        // 左端判定
        if( ax > BG_W * 3 ){
            ax -= BG_W * 6;

            if( wrongRoot ){
                // 間違えた
                area = 0;
            }
            else{
                area++;
            }

            NextRoot( &wrongRoot , area );

        }

        // ------------------------------------- デバッグ用表示 -------------------------------------------
        UpdateFPS();
        DrawFormatString( 80 , 00   , GetColor( 255 , 255 , 255 ) , "Instant FPS: %.1f"         , instantFPS       );
        DrawFormatString( 80 , 20   , GetColor( 255 , 255 , 255 ) , "Min FPS (0.125s): %.1f"    , displayMinFPS    );
        DrawFormatString( 80 , 40   , GetColor( 255 , 255 , 255 ) , "%s %d" , "COUNT:"          , area             );
        DrawFormatString( 80 , 60   , GetColor( 255 , 255 , 255 ) , "%s %d" , "AREA:"           , wrongRoot        );
        DrawFormatString( 80 , 80   , GetColor( 255 , 255 , 255 ) , "%s %d" , "BX:"             , bx               );
        DrawFormatString( 80 , 100  , GetColor( 255 , 255 , 255 ) , "%s %d" , "AX:"             , ax               );

        ::ScreenFlip();                                               // 裏画面の内容を表画面に反映させる
       // WaitTimer( 16 );                                            // 一定時間待つ 約60FPS

        if( ProcessMessage() == -1 ) break; // Windowsから情報を受け取りエラーが起きたら終了
        if( CheckHitKey( KEY_INPUT_ESCAPE ) ) break;// ESCキーが押されたらループを抜ける
    }

    ::DxLib_End();
    return 0;
}
