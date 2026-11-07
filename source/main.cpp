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

static bool overlap(const Player& p, const Block& b){
    return p.x < b.x + b.size && p.x + p.size > b.x && p.y < b.y + b.size && p.y + p.size > b.y;
}

static void drawLabel(C2D_TextBuf buf, const char* s, float x, float y, float scale, u32 color){
    C2D_Text t;
    C2D_TextParse(&t, buf, s);
    C2D_TextOptimize(&t);
    C2D_DrawText(&t, C2D_WithColor, x, y, 0.5f, scale, scale, color);
}

static int loadBest(){
    FILE* f = fopen("smdc:/dodger_best.txt", "r");
    if (!f) return 0;
    int v = 0;
    if (fscanf(f, "%d", &v) != 1) v = 0;
    fclose(f);
    return v;
}

static void saveBest(int v){
    FILE* f = fopen("smdc:/dodger_best.txt", "w");
    if (!f) return;
    fprintf(f, "%d\n", v);
    fclose(f);
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
    C2D_TextBuf textBuf = C2D_TextBufNew(512);
    int best = loadBest();
    
    int score = 0;
    int spawnTimer = 0;
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    bool alive = true;
    

    auto restart = [&](){
        player = {TOP_W / 2 - 12.0f, SCREEN_H - 40.0f, 24.0f, 3.5f};
        blocks.clear();
        score = 0;
        spawnTimer = 0;
        alive = true;
    };

    restart();

    while (aptMainLoop()){
        hidScanInput();
        u32 kDown = hidKeysDown();
        u32 kHeld = hidKeysHeld();
        if (kDown & KEY_START) break;

        // update
        touchPosition touch;
        hidTouchRead(&touch);
        bool touching = (kHeld & KEY_TOUCH) != 0;
        bool tappedRestart = touching && touch.px >= 100 && touch.px <= 220 && touch.py >= 150 && touch.py <= 190;

        if (alive){

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

                // player movement
                player.x = clampf(player.x + dx * player.speed, 0.0f, TOP_W - player.size);
                player.y = clampf(player.y - dy * player.speed, 0.0f, SCREEN_H - player.size);

                // blocks
                if (--spawnTimer <= 0){
                    float size = randRange(14.0f, 34.0f);
                    blocks.push_back({randRange(0.0f, TOP_W - size), -size, size, randRange(1.5f, 3.0f) + score * 0.005f});
                    
                    int interval = 45 - score; // as you score, lower frames
                    if (interval < 12) interval = 12;
                    spawnTimer = interval;
                }

                // block speed, dodging, and score
                for (size_t i = 0; i < blocks.size();){
                    blocks[i].y += blocks[i].speed;
                    // collision overlap 
                    if (overlap(player, blocks[i])) {
                        alive = false;
                        // score system
                        if (score > best){
                            best = score;
                            saveBest(best);
                        }
                    }
                    if (blocks[i].y > SCREEN_H){
                        blocks.erase(blocks.begin() + i); // dodging mechanic
                        score++;
                    } else {
                        ++i;
                    }
                }
                } else if ((kDown & KEY_A) || ((kDown & KEY_TOUCH) && tappedRestart)){
                    restart();
                }
                // begin frame
                C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
                C2D_TextBufClear(textBuf);

                // start top screen
                C2D_TargetClear(top, clrTop);
                C2D_SceneBegin(top);
                // Draw (Top)
                for (const Block& b: blocks){
                    C2D_DrawRectSolid(b.x, b.y, 0.0f, b.size, b.size, C2D_Color32(240, 90, 90, 255));
                }
                C2D_DrawRectSolid(player.x, player.y, 0.0f, player.size, player.size, C2D_Color32(80, 220, 140, 255));
                // Game Over UI
                if(!alive){
                    drawLabel(textBuf, "GAME OVER", 110.0f, 90.0f, 1.2f, C2D_Color32(255, 255, 255, 255));
                }
            

                // start bottom screen
                C2D_TargetClear(bottom, clrBot);
                C2D_SceneBegin(bottom);
                // Draw (Bottom)
                char line[64];
                const u32 white = C2D_Color32(255, 255, 255, 255);

                drawLabel(textBuf, "DODGER", 100.0f, 10.0f, 1.2f, white);
                snprintf(line, sizeof line, "Score: %d", score);
                drawLabel(textBuf, line, 20.0f, 60.0f, 0.9f, white);
                snprintf(line, sizeof line, "Best: %d", best);
                drawLabel(textBuf, line, 20.0f, 90.0f, 0.9f, white);

                if (!alive){
                    C2D_DrawRectSolid(100.0f, 150.0f, 0.0f, 120.0f, 40.0f, C2D_Color32(80, 220, 140, 255));
                    drawLabel(textBuf, "RESTART", 122.0f, 158.0f, 0.8f, C2D_Color32(10, 20, 20, 255));
                }
                if (touching){
                    C2D_DrawCircleSolid(touch.px, touch.py, 0.0f, 6.0f, C2D_Color32(255, 220, 80, 255));
                }





                C3D_FrameEnd(0);
        
    }

    C2D_TextBufDelete(textBuf);
    C2D_Fini();
    C3D_Fini();
    gfxExit();
    return 0;
}