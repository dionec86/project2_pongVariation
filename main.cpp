#include "CS3113/Entity.h"


// Global Constants
constexpr int SCREEN_WIDTH  = 1000,
              SCREEN_HEIGHT = 600,
              FPS           = 120,
              WINNING_SCORE = 3;

constexpr float PADDLE_POS = 450.0f, // 490.0f
                FIXED_TIMESTEP   = 1.0f / 60.0f;

constexpr char  STARTSCREEN[] = "assets/game/background-desert.jpg",
                BACKGROUND[] = "assets/game/background-clean.png",
                ENDSCREEN[] = "assets/game/background-folders.png",
                LEFTWINSCREEN[] = "assets/game/background-split.jpg",
                RIGHTWINSCREEN[] = "assets/game/background-split copy.jpg",
                SCROLLBAR[] = "assets/game/scrollbar.png",
                SCREENSHOT[] = "assets/game/screenshot.png";
constexpr Vector2 ORIGIN      = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };


// Global Variables
AppStatus gAppStatus   = RUNNING;
GameStatus gGameStatus = NOT_STARTED;
DifficultyLevel gDifficultyLevel = EASY;
BotMode gBotMode = OFF;

float gPreviousTicks   = 0.0f,
      gTimeAccumulator = 0.0f;

int p1Score = 0,
    p2Score = 0;

Entity* gLeftPaddle = nullptr;
Entity* gRightPaddle = nullptr;
std::vector<Entity*> gPaddles;
// Entity* gBalls = nullptr;
std::vector<Entity*> gBalls;

Texture2D gStartScreenTexture;
Texture2D gBackgroundTexture;
Texture2D gEndScreenTexture;
Texture2D gLeftWinScreenTexture;
Texture2D gRightWinScreenTexture;

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();

int getScreenWidth() { return SCREEN_WIDTH; }
int getScreenHeight() { return SCREEN_HEIGHT; }
float getSpawnRange() { return SPAWN_RANGE; }
DifficultyLevel getDifficultyLevel() { return gDifficultyLevel; }
GameStatus getGameStatus() { return gGameStatus; }
Vector2 getOrigin() { return ORIGIN; }
int getP1Score() { return p1Score; }
int getP2Score() { return p2Score; }
void addP1Score() { ++p1Score; }
void addP2Score() { ++p2Score; }


void renderBackground(const Texture2D *texture)
{
    Rectangle textureArea = {
        0.0f, 0.0f,
        static_cast<float>(texture->width),
        static_cast<float>(texture->height)
    };

    Rectangle destinationArea = {
        ORIGIN.x,
        ORIGIN.y,
        static_cast<float>(SCREEN_WIDTH),
        static_cast<float>(SCREEN_HEIGHT)
    };

    Vector2 originOffset = {
        static_cast<float>(SCREEN_WIDTH) / 2.0f,
        static_cast<float>(SCREEN_HEIGHT) / 2.0f
    };

    DrawTexturePro(
        *texture,
        textureArea, destinationArea, originOffset,
        0.0f, WHITE
    );
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "pongVariation");

    gStartScreenTexture = LoadTexture(STARTSCREEN);
    gBackgroundTexture = LoadTexture(BACKGROUND);
    gEndScreenTexture = LoadTexture(ENDSCREEN);
    gLeftWinScreenTexture = LoadTexture(LEFTWINSCREEN);
    gRightWinScreenTexture = LoadTexture(RIGHTWINSCREEN);
    gGameStatus = NOT_STARTED;

    LOG("Welcome to desktop pong (:");
    LOG("Type 'E' to start the game!");


    // /*
    gLeftPaddle = new Entity(
        {ORIGIN.x - PADDLE_POS, ORIGIN.y}, // position
        {8.0f, 200.0f},  // scale
        SCROLLBAR,        // texture file address
        PADDLE,                         // entity type
        UNUSED
    );
    gLeftPaddle->setSpeed(300.0f);

    gRightPaddle = new Entity(
        {ORIGIN.x + PADDLE_POS, ORIGIN.y}, // position
        {8.0f, 200.0f},  // scale
        SCROLLBAR,        // texture file address
        PADDLE,                         // entity type
        UNUSED
    );
    gRightPaddle->setSpeed(300.0f);
    gPaddles = {gLeftPaddle, gRightPaddle};


    for (int i = 0; i < 3; ++i) {
        gBalls.push_back(new Entity(
            {ORIGIN.x, static_cast<float>(GetRandomValue(ORIGIN.y - SPAWN_RANGE, ORIGIN.y + SPAWN_RANGE))}, // position
            {50.0f, 50.0f},  // scale
            SCREENSHOT,        // texture file address
            BALL,                         // entity type
            UNUSED
        ));
    }

    gBalls[0]->use();
    gBalls[0]->spawn(SPAWN_RANGE);

    gBalls[1]->deactivate();
    gBalls[2]->deactivate();
    // */


    SetTargetFPS(FPS);
}

void processInput() 
{
    // Game Settings
    if      (IsKeyDown(KEY_E))  { gDifficultyLevel = EASY; gGameStatus = STARTED; }
    else if (IsKeyDown(KEY_M)) { gDifficultyLevel = MEDIUM; gGameStatus = STARTED; }
    else if (IsKeyDown(KEY_H)) { gDifficultyLevel = HARD; gGameStatus = STARTED; }

    if      (IsKeyDown(KEY_T)) { gBotMode = AUTO; }

    if      (IsKeyDown(KEY_ONE)) { for (Entity* e : gBalls) { e->stopUse(); e->deactivate(); }
                                    gBalls[0]->use();}
    else if (IsKeyDown(KEY_TWO)) { for (Entity* e : gBalls) { e->stopUse(); e->deactivate(); }
                                    gBalls[0]->use();
                                    gBalls[1]->use();}
    else if (IsKeyDown(KEY_THREE)) { for (Entity* e : gBalls) { e->stopUse(); e->deactivate();  }
                                    gBalls[0]->use();
                                    gBalls[1]->use();
                                    gBalls[2]->use();}

    switch (gGameStatus) {
        case STARTED:
            // Game control
            if (gBotMode == OFF) {
                gLeftPaddle->resetMovement();
                if      (IsKeyDown(KEY_W)) gLeftPaddle->moveUp();
                else if (IsKeyDown(KEY_S)) gLeftPaddle->moveDown();
            }
            else {
                gLeftPaddle->setMovement({0,1});
            }
            gRightPaddle->resetMovement();
            if      (IsKeyDown(KEY_UP)) gRightPaddle->moveUp();
            else if (IsKeyDown(KEY_DOWN)) gRightPaddle->moveDown();

    }
    
    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
    // if (WindowShouldClose()) gAppStatus = TERMINATED;

}

void update() 
{
    // Delta time
    float ticks = (float) GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;

    // Fixed timestep
    deltaTime += gTimeAccumulator;

    if (deltaTime < FIXED_TIMESTEP)
    {
        gTimeAccumulator = deltaTime;
        return;
    }


    while (deltaTime >= FIXED_TIMESTEP)
    {
        switch (gGameStatus) {
            case STARTED:
                // Updates paddle & ball, only when game is running
                // if (gGameStatus != RUNNING) continue;
                // LOG("started");

                if (gBotMode == AUTO) {
                    gLeftPaddle->autoPlay();
                }

                gLeftPaddle->update(FIXED_TIMESTEP);
                gRightPaddle->update(FIXED_TIMESTEP);
                for (Entity* e : gBalls) {
                    e->update(FIXED_TIMESTEP, gPaddles);
                }
                // gBalls->update(FIXED_TIMESTEP, gPaddles);

                // Check score for win-condition
                if (p1Score >= WINNING_SCORE || p2Score >= WINNING_SCORE) {
                    gGameStatus = FINISHED;
                    gLeftPaddle->deactivate();
                    gRightPaddle->deactivate();
                    for (Entity* e : gBalls) {
                        e->deactivate();
                    }
                    // gBalls->deactivate();
                }
            break;
        }

        deltaTime -= FIXED_TIMESTEP;
    }
}

void render()
{
    BeginDrawing();
    // ClearBackground(ColorFromHex(BG_COLOUR));
    switch (gGameStatus){
        case NOT_STARTED:
            ClearBackground(PINK);
            renderBackground(&gStartScreenTexture);
            break;
        case STARTED:
            // LOG("started");
            renderBackground(&gBackgroundTexture);

            gLeftPaddle->render();
            gLeftPaddle->displayCollider();
            gRightPaddle->render();
            gRightPaddle->displayCollider();

            for (Entity* e : gBalls) {
                e->render();
                // e->displayCollider();
            }
            // gBalls->render();
            // gBalls->displayCollider();
            break;

        case FINISHED:
            // LOG("finished");
            // renderBackground(&gEndScreenTexture);
            if (p1Score > p2Score) {
                renderBackground(&gLeftWinScreenTexture);
            }
            else {
                renderBackground(&gRightWinScreenTexture);
            }
            break;
        default:
            LOG("default");
            ClearBackground(BLUE);
            // break;
    }

    EndDrawing();
}

void shutdown() 
{ 
    CloseWindow();
}

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();
        render();
    }

    shutdown();

    return 0;
}