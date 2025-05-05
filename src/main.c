#include <stdio.h>
#include "raylib.h"
#include "raymath.h"
#include <math.h>
#include <time.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#define K 3
#define MIN_X -20.0
#define MAX_X 20.0
#define MIN_Y -20.0
#define MAX_Y 20.0

#define SAMPLE_RADIUS 4.0f
#define MEAN_RADIUS (2*SAMPLE_RADIUS)

// function to map sample value to screen
static Vector2 project_sample_to_screen(Vector2 sample)
{
    // -20..20 => 0..40 => 0..1
    float nx = (sample.x - MIN_X)/(MAX_X - MIN_X);
    float ny = (sample.y - MIN_Y)/(MAX_Y - MIN_Y);
    float w = GetScreenWidth();
    float h = GetScreenHeight();
    return CLITERAL(Vector2) {
        .x = w*nx,
        .y = h - (h*ny),
    };
}

// function to return float number between 0 and 1
static inline float rand_float(void)
{
    // random value between 0 and 1
    return (float)rand()/RAND_MAX;
}

static void generate_cluster(Vector2 center, float radius, size_t count, Vector2 **samples)
{
    for (size_t i = 0; i < count; ++i) {
        float angle = rand_float()*2*PI;
        float mag = pow(1.0 - rand_float(),4);
        Vector2 sample = {
            .x = center.x + cosf(angle)*mag*radius,
            .y = center.y + sinf(angle)*mag*radius,
        };
        arrput(*samples, sample);
    }
}

static Vector2 clusters[K] = {0};
static Vector2 means[K] = {0};

int main(void)
{
    srand(time(0));
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 600, "k-means");
    Vector2 *set = NULL;
    // generate random clusters for drawing
    generate_cluster(CLITERAL(Vector2){0}, 10, 100, &set);
    generate_cluster(CLITERAL(Vector2){MIN_X*0.5f, MAX_Y*0.5f}, 5, 50, &set);
    generate_cluster(CLITERAL(Vector2){MAX_X*0.5f, MAX_Y*0.5f}, 5, 50, &set);

    for (size_t i = 0; i < K; ++i) {
        means[i].x = Lerp(MIN_X, MAX_X, rand_float());
        means[i].y = Lerp(MIN_Y, MAX_Y, rand_float());
    }

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_R)) {
            arrsetlen(set, 0);
            generate_cluster(CLITERAL(Vector2){0}, 10, 100, &set);
            generate_cluster(CLITERAL(Vector2){MIN_X*0.5f, MAX_Y*0.5f}, 5, 50, &set);
            generate_cluster(CLITERAL(Vector2){MAX_X*0.5f, MAX_Y*0.5f}, 5, 50, &set);

            for (size_t i = 0; i < K; ++i) {
                means[i].x = Lerp(MIN_X, MAX_X, rand_float());
                means[i].y = Lerp(MIN_Y, MAX_Y, rand_float());
            }
        }
        BeginDrawing();
        ClearBackground(GetColor(0x181818AA));
        for (size_t i = 0; i < arrlen(set); ++i) {
            Vector2 it = set[i];
            DrawCircleV(project_sample_to_screen(it), SAMPLE_RADIUS, RED);
        }
        for (size_t i = 0; i < K; ++i) {
            Vector2 it = means[i];
            DrawCircleV(project_sample_to_screen(it), MEAN_RADIUS, YELLOW);
        }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
