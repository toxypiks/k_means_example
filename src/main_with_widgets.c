#include <stdio.h>
#include "raylib.h"
#include "raymath.h"
#include <math.h>
#include <time.h>
#include <float.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "ui_stuff.c"

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#define K 3
#define MIN_X -20.0
#define MAX_X 20.0
#define MIN_Y -20.0
#define MAX_Y 20.0

#define ARRAY_LEN(xs) sizeof(xs)/sizeof(xs[0])

int read_entire_file(const char *file_path, void **data, size_t *data_size)
{
    int fd = 0;
    struct stat statbuf = {0};

    fd = open(file_path, O_RDONLY, S_IRUSR | S_IRGRP);
    if (fd == -1)
    {
        printf("failed to open %s\n", file_path);
        exit(EXIT_FAILURE);
    }

    if (fstat(fd, &statbuf) == -1)
    {
        printf("failed to fstat %s\n", file_path);
        exit(EXIT_FAILURE);
    }

    *data_size = statbuf.st_size;
    if (close(fd) == -1)
    {
        printf("failed to fclose %s\n", file_path);
        exit(EXIT_FAILURE);
    }

    FILE* fp = fopen(file_path, "rb");
    *data = malloc(*data_size);
    size_t read_bytes = fread(*data, 1, *data_size, fp);
    printf("read_bytes: %zu\n", read_bytes);

    if (fp) fclose(fp);
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

Vector2 *set = NULL;
static Vector2 *clusters[K] = {0};
static Vector2 means[K] = {0};

void generate_new_state(void)
{
    arrsetlen(set, 0);
    // generate random clusters for drawing
    generate_cluster(CLITERAL(Vector2){0}, 10, 100, &set);
    generate_cluster(CLITERAL(Vector2){MIN_X*0.5f, MAX_Y*0.5f}, 5, 50, &set);
    generate_cluster(CLITERAL(Vector2){MAX_X*0.5f, MAX_Y*0.5f}, 5, 50, &set);

    for (size_t i = 0; i < K; ++i) {
        means[i].x = Lerp(MIN_X, MAX_X, rand_float());
        means[i].y = Lerp(MIN_Y, MAX_Y, rand_float());
    }

}

void recluster_state(void)
{
    for (size_t j = 0; j < K; ++j) {
        arrsetlen(clusters[j], 0);
    }

    for (size_t i = 0; i < arrlen(set); ++i) {
        Vector2 p = set[i];
        int k = -1;
        float s = FLT_MAX;
        for (size_t j = 0; j < K; ++j) {
            Vector2 m = means[j];
            float sm = Vector2LengthSqr(Vector2Subtract(p, m));
            if (sm < s) {
                s = sm;
                k = j;
            }
        }
        arrput(clusters[k], p);
    }
}

void update_means(void)
{
    for (size_t i = 0; i < K; ++i) {
        if (arrlen(clusters[i]) > 0) {
            means[i] = Vector2Zero();
            for (size_t j = 0; j < arrlen(clusters[i]); ++j) {
                means[i] = Vector2Add(means[i], clusters[i][j]);
            }
            means[i].x /= arrlen(clusters[i]);
            means[i].y /= arrlen(clusters[i]);
        } else {
            // if cluster is empty just regenerate mean
            means[i].x = Lerp(MIN_X, MAX_X, rand_float());
            means[i].y = Lerp(MIN_Y, MAX_Y, rand_float());
        }
    }
}

int main(void)
{
    const char *leaf_path = "../leaf/leaf.csv";
    char *leaf_data = NULL;
    size_t leaf_data_size = 0;

    if (read_entire_file(leaf_path, (void**)&leaf_data, &leaf_data_size)) return 1;
    srand(time(0));

    size_t window_factor = 80;
    size_t screen_width = (16*window_factor);
    size_t screen_height = (9*window_factor);

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);
    InitWindow(screen_width, screen_height, "k-means");

    UiStuff* ui_stuff = create_ui_stuff(screen_width, screen_height);
    LayoutStack ls = {0};

    float w = GetRenderWidth();
    float h = GetRenderHeight();

    Data* data = NULL;
    Limits limits = {-10,10, -10, 10};
    gen_data(&data, 10, 4, limits);
    // generate_new_state();

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_R)) {
            free_data(&data);
            gen_data(&data, 10, 4, limits);
            // recluster_state();
        }
        if (IsKeyPressed(KEY_SPACE)) {
            // update_means();
            // recluster_state();
        }
        BeginDrawing();
        ClearBackground(GetColor(0x181818AA));

        layout_stack_push(&ls, LO_VERT, ui_rect(0, 0, w, h), 2, 0);
        layout_stack_push(&ls, LO_HORZ, layout_stack_slot(&ls), 2, 0);
        widget(layout_stack_slot(&ls), PINK);
        cluster_widget(layout_stack_slot(&ls), data->samples, clusters, means, limits);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
