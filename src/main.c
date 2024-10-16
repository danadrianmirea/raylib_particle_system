#define LIBPARTIKEL_IMPLEMENTATION
#define AM_DEBUG

#include "stdio.h"
#include "raylib.h"
#include "partikel.h"

static int screenWidth = 1000;
static int screenHeight = 800;
static unsigned long counter = 0;
static Camera2D camera;

static Texture2D texCircle;

static ParticleSystem *ps = NULL;
static Emitter *emitterSwirl = NULL;
static Emitter *emitterSwirl2 = NULL;
static Emitter *emitterSwirl3 = NULL;

float xPos;
float yPos;

float speedX = 200.0f;
float speedY = 200.0f;

bool Particle_DeactivatorOutsideCam(Particle *p)
{
    return (p->position.y < (camera.target.y - camera.offset.y) || Particle_DeactivatorAge(p));
}

void InitSwirl()
{
    ps = ParticleSystem_New();
    if (ps == NULL)
    {
        exit(0);
    }

    EmitterConfig ecfg = 
    {
        .capacity = 25000,
        .emissionRate = 5000,
        .origin = (Vector2){.x = 0, .y = 0},
        .originAcceleration = (FloatRange){.min = 400, .max = 500},
        .offset = (FloatRange){.min = 30, .max = 40},
        .direction = (Vector2){.x = 0, .y = -1}, // go up
        .directionAngle = (FloatRange){.min = -180, .max = 180},
        .velocityAngle = (FloatRange){.min = -90, .max = 90},
        .velocity = (FloatRange){.min = 200, .max = 400},
        .startColor = (Color){.r = 244, .g = 0, .b = 0, .a = 255},
        .endColor = (Color){.r = 244, .g = 244, .b = 0, .a = 0},
        .age = (FloatRange){.min = 2.5, .max = 5.0},
        .texture = texCircle,
        .blendMode = BLEND_ADDITIVE,
        .particle_Deactivator = Particle_DeactivatorOutsideCam
    };

    emitterSwirl = Emitter_New(ecfg);
    if (emitterSwirl == NULL)
    {
        exit(0);
    }

    ParticleSystem_Register(ps, emitterSwirl);
    ParticleSystem_Start(ps);
}

void DestroySwirl()
{
    Emitter_Free(emitterSwirl);
    ParticleSystem_Free(ps);
}

// Init sets up all relevant data.
void Init()
{
    InitWindow(screenWidth, screenHeight, "Raylib particle demo");
    SetTargetFPS(144);
    HideCursor();

    xPos = screenWidth/2.0f;
    yPos = screenHeight/2.0f;

    camera.target = (Vector2){.x = 0, .y = 0};
    camera.offset = (Vector2){.x = 0, .y = 0};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    Image imgCircle = GenImageGradientRadial(8, 8, 0.5f, WHITE, BLACK);
    texCircle = LoadTextureFromImage(imgCircle);
    UnloadImage(imgCircle);

    InitSwirl();
}

void Destroy()
{
    DestroySwirl();
    UnloadTexture(texCircle);
    CloseWindow();
}

void Update(double dt)
{
    counter = 0;

    xPos += speedX * dt;
    yPos += speedY * dt;
    if(xPos <= 0 || xPos >= screenWidth)
    {
        speedX = -speedX;
    }
    if(yPos <= 0 || yPos >= screenHeight)
    {
        speedY = -speedY;
    }

    ParticleSystem_SetOrigin(ps, (Vector2){.x = xPos, .y = yPos});
    counter += ParticleSystem_Update(ps, (float)dt);
}

void Draw()
{
    BeginDrawing();
    ClearBackground(BLACK);
    //BeginMode2D(camera);
    ParticleSystem_Draw(ps);
    //EndMode2D();

#ifdef AM_DEBUG
    DrawFPS(10, 10);
    char pcount[40];
    sprintf(pcount, "Particles: %lu", counter);
    DrawText(pcount, 10, 40, 20, DARKGREEN);
#endif
    EndDrawing();
}

int main()
{
    Init();
    while (!WindowShouldClose()) 
    {
        double dt = (double)GetFrameTime();
        Update(dt);
        Draw();
    }
    Destroy();
    return 0;
}