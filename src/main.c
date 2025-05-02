#include <stdio.h>
#include "raylib.h"

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
    return CLITERAL(Vector2) {
        .x = GetScreenWidth()*nx,
        .y = GetScreenHeight()*ny,
    };
}

int main(void)
{
    InitWindow(800, 600, "k-means");
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(GetColor(0x181818AA));
        DrawCircle(GetScreenWidth()/2, GetScreenHeight()/2, 30, RED);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
