#define LIBPARTIKEL_IMPLEMENTATION
//#define AM_DEBUG
#define numParticleSystems 3

#include "stdio.h"
#include "raylib.h"
#include "partikel.h"

static int screenWidth = 1000;
static int screenHeight = 800;
static unsigned long counter = 0;
static Camera2D camera;

static Texture2D texCircle;

static ParticleSystem *ps[numParticleSystems] = {NULL};
static Emitter *emitterSwirls[numParticleSystems] = {NULL};

float xPos[numParticleSystems];
float yPos[numParticleSystems];

const float minSpeed = 200;
const float maxSpeed = 500;

float speeds_X[numParticleSystems];
float speeds_Y[numParticleSystems];

const float maxLifeTime = 10.0f;

bool Particle_DeactivatorOutsideCam(Particle *p)
{
    return (p->position.y < (camera.target.y - camera.offset.y) || Particle_DeactivatorAge(p));
}

void InitSwirl(int i)
{
    ps[i] = ParticleSystem_New();
    if (ps[i] == NULL)
    {
        exit(0);
    }
    
    Color sColor;
    Color eColor;

    switch (i)
    {
    case 0:
        sColor = (Color){.r = 244, .g = 0, .b = 0, .a = 255};
        eColor = (Color){.r = 244, .g = 0, .b = 0, .a = 0};
        break;
    case 1:
        sColor = (Color){.r = 0, .g = 244, .b = 0, .a = 255};
        eColor = (Color){.r = 0, .g = 244, .b = 0, .a = 0};    
        break;
    case 2:
        sColor = (Color){.r = 0, .g = 0, .b = 244, .a = 255};
        eColor = (Color){.r = 0, .g = 0, .b = 244, .a = 0};
        break;
    default:
        break;
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
            .startColor = sColor,
            .endColor = eColor,
            .age = (FloatRange){.min = 1.0, .max = 3.0},
            .texture = texCircle,
            .blendMode = BLEND_ADDITIVE,
            .particle_Deactivator = Particle_DeactivatorOutsideCam};

    emitterSwirls[i] = Emitter_New(ecfg);
    if (emitterSwirls[i] == NULL)
    {
        exit(0);
    }

    ParticleSystem_Register(ps[i], emitterSwirls[i]);
    ParticleSystem_Start(ps[i]);
}

void DestroySwirl()
{
    for(int i=0; i<numParticleSystems; i++)
    {
        Emitter_Free(emitterSwirls[i]);
        ParticleSystem_Free(ps[i]);
    }
    
}

// Init sets up all relevant data.
void Init()
{
    InitWindow(screenWidth, screenHeight, "Raylib particle demo");
    SetTargetFPS(144);
    HideCursor();

    camera.target = (Vector2){.x = 0, .y = 0};
    camera.offset = (Vector2){.x = 0, .y = 0};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    Image imgCircle = GenImageGradientRadial(8, 8, 0.5f, WHITE, BLACK);
    texCircle = LoadTextureFromImage(imgCircle);
    UnloadImage(imgCircle);

    for (int i = 0; i < numParticleSystems; i++)
    {
        xPos[i] = GetRandomFloat(100, screenWidth - 100);
        yPos[i] = GetRandomFloat(100, screenHeight - 100);

        float sign = -1;
        if(GetRandomValue(0, 1))
        {
            sign = 1;
        }
        speeds_X[i] = sign * GetRandomFloat(minSpeed, maxSpeed);

        sign = -1;
        if(GetRandomValue(0, 1))
        {
            sign = 1;
        }
        speeds_Y[i] = sign * GetRandomFloat(minSpeed, maxSpeed);
         
        InitSwirl(i);
    }
}

void ReInitParticleSystems()
{
    for (int i = 0; i < numParticleSystems; i++)
    {
        xPos[i] = GetRandomFloat(100, screenWidth - 100);
        yPos[i] = GetRandomFloat(100, screenHeight - 100);

        float sign = -1;
        if(GetRandomValue(0, 1))
        {
            sign = 1;
        }
        speeds_X[i] = sign * GetRandomFloat(minSpeed, maxSpeed);

        sign = -1;
        if(GetRandomValue(0, 1))
        {
            sign = 1;
        }
        speeds_Y[i] = sign * GetRandomFloat(minSpeed, maxSpeed);
    }
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

    for (int i = 0; i < numParticleSystems; i++)
    {
        xPos[i] += speeds_X[i] * dt;
        yPos[i] += speeds_Y[i] * dt;
        if (xPos[i] <= 0 || xPos[i] >= screenWidth)
        {
            xPos[i] -= speeds_X[i] * dt;
            speeds_X[i] = -speeds_X[i];
        }
        if (yPos[i] <= 0 || yPos[i] >= screenHeight)
        {
            yPos[i] -= speeds_Y[i] * dt;
            speeds_Y[i] = -speeds_Y[i];
        }

        ParticleSystem_SetOrigin(ps[i], (Vector2){.x = xPos[i], .y = yPos[i]});
        counter += ParticleSystem_Update(ps[i], (float)dt);
    }
}

void Draw()
{
    BeginDrawing();
    ClearBackground(BLACK);
    // BeginMode2D(camera);
    for(int i=0; i<numParticleSystems; i++)
    {
        ParticleSystem_Draw(ps[i]);
    }
    // EndMode2D();

    DrawFPS(10, 10);

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
    float time = 0.0f;
    Init();
    while (!WindowShouldClose())
    {
        double dt = (double)GetFrameTime();
        Update(dt);
        Draw();

        time += dt;

        if(time >= maxLifeTime)
        {
            time = 0.0f;
            ReInitParticleSystems();
        }
    }
    Destroy();
    return 0;
}