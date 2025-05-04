#include <stdio.h>
#include "raylib.h"
#include <math.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#define MIN_X -20.0
#define MAX_X 20.0
#define MIN_Y -20.0
#define MAX_Y 20.0

// function to map sample value to screen
Vector2 project_sample_to_screen(Vector2 sample)
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
float rand_float(void)
{
    return (float)rand()/RAND_MAX;
}

void generate_cluster(Vector2 center, float radius, size_t count, Vector2 **samples)
{
    for (size_t i = 0; i < count; ++i) {
        float angle = rand_float()*2*PI;
        float mag = rand_float();
        Vector2 sample = {
            .x = center.x + cosf(angle)*mag*radius,
            .y = center.y + sinf(angle)*mag*radius,
        };
        arrput(*samples, sample);

    }
}

int main(void)
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 600, "k-means");
    Vector2 *cluster = NULL;
    generate_cluster(CLITERAL(Vector2){0}, 10, 10, &cluster);
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(GetColor(0x181818AA));
        for (size_t i = 0; i < arrlen(cluster); ++i) {
            Vector2 it = cluster[i];
            DrawCircleV(project_sample_to_screen(it), 10, RED);
        }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
