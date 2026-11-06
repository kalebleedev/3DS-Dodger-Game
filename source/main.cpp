#include <3ds.h>
#include <citro2d.h>
#include <cstdio>

#include <cmath>
#include <cstdlib>
#include <ctime>
#include <vector>

constexpr float TOP_W = 400.0f; // top screen width in pixels
constexpr float SCREEN_H = 240.0f; // both screens 240 pixels tall

// Helpers
struct Player{
    float x, y, size, speed;
};

struct Block{
    float x, y, size, speed;
};


static float clampf(float v, float lo, float hi){
    if (v < lo){
        return lo;
    } else if (v > hi){
        return hi;
    } else {
        return v;
    }
}

static float randRange(float lo, float hi){
    return lo + (hi - lo) * (std::rand() / static_cast<float>(RAND_MAX));
}

int main(){
    gfxInitDefault();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();

    // top and bottom screens
    C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
    C3D_RenderTarget* bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

    // colors
    const u32 clrTop = C2D_Color32(18, 22, 44, 255);
    const u32 clrBot = C2D_Color32(30, 34, 60, 255);

    // setup
    Player player = {TOP_W / 2 - 12.0f, SCREEN_H - 40.0f, 24.0f, 3.5f};
    std::vector<Block> blocks;
    int score = 0;
    int spawnTimer = 0;
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    while (aptMainLoop()){
        hidScanInput();
        u32 kDown = hidKeysDown();
        u32 kHeld = hidKeysHeld();
        if (kDown & KEY_START) break;

        // update
        circlePosition pad;
        hidCircleRead(&pad);
        float dx = pad.dx / 154.0f;
        float dy = pad.dy / 154.0f;

        if (std::fabs(dx) < 0.15f) dx = 0;
        if (std::fabs(dy) < 0.15f) dy = 0;

        if (kHeld & KEY_DLEFT) dx -= 1.0f;
        if (kHeld & KEY_DRIGHT) dx += 1.0f;
        if (kHeld & KEY_DUP) dy += 1.0f;
        if (kHeld & KEY_DDOWN) dy -= 1.0f;

        dx = clampf(dx, -1.0f, 1.0f);
        dy = clampf(dy, -1.0f, 1.0f);

        player.x = clampf(player.x + dx * player.speed, 0.0f, TOP_W - player.size);
        player.y = clampf(player.y - dy * player.speed, 0.0f, SCREEN_H - player.size);
        // begin frame
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

        // start top screen
        C2D_TargetClear(top, clrTop);
        C2D_SceneBegin(top);
        // Draw (Top)
        C2D_DrawRectSolid(player.x, player.y, 0.0f, player.size, player.size, C2D_Color32(80, 220, 140, 255));
       

        // start bottom screen
        C2D_TargetClear(bottom, clrBot);
        C2D_SceneBegin(bottom);
        // Draw (Bottom)

        C3D_FrameEnd(0);
    }


    C2D_Fini();
    C3D_Fini();
    gfxExit();
    return 0;
}