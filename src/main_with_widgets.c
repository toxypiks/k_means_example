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

/*
#define K 3
#define MIN_X -20.0
#define MAX_X 20.0
#define MIN_Y -20.0
#define MAX_Y 20.0
*/

#define ARRAY_LEN(xs) sizeof(xs)/sizeof(xs[0])

typedef struct KMeansParams {
    size_t num_cluster;
} KMeansParams;

typedef struct KMeansState {
  Vector2* cendroids;
  Vector2** cluster_points;
} KMeansState;

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

void kmeans_state_create(KMeansState **state, KMeansParams params, Limits limits) {
    (*state) = malloc(sizeof(KMeansState));
    for (size_t i = 0; i < params.num_cluster; ++i) {
        Vector2 new_cendroid = {
            .x = Lerp(limits.min_x,
                      limits.max_x,
                      rand_float()),
            .y = Lerp(limits.min_y,
                      limits.max_y,
                      rand_float())
        };
        arrput((*state)->cendroids, new_cendroid);
    }
}

void recluster_state(KMeansState* state, KMeansParams* params, Data* data)
{
    for (size_t j = 0; j < params->num_cluster; ++j) {
        arrsetlen(state->cluster_points[j], 0);
    }

    for (size_t i = 0; i < arrlen(data->points); ++i) {
        Vector2 p = data->points[i];
        int k = -1;
        float s = FLT_MAX;
        for (size_t j = 0; j < params->num_cluster; ++j) {
            Vector2 m = state->cendroids[j];
            float sm = Vector2LengthSqr(Vector2Subtract(p, m));
            if (sm < s) {
                s = sm;
                k = j;
            }
        }
        arrput(state->cluster_points[k], p);
    }
}

void update_means(KMeansState* state, KMeansParams* params, Data* data)
{
    for (size_t i = 0; i < params->num_cluster; ++i) {
        if (arrlen(state->cluster_points[i]) > 0) {
            state->cendroids[i] = Vector2Zero();
            for (size_t j = 0; j < arrlen(state->cluster_points[i]); ++j) {
                state->cendroids[i] = Vector2Add(state->cendroids[i], state->cluster_points[i][j]);
            }
            state->cendroids[i].x /= arrlen(state->cluster_points[i]);
            state->cendroids[i].y /= arrlen(state->cluster_points[i]);
        } else {
            // if cluster is empty just regenerate mean
            state->cendroids[i].x = Lerp(data->limits.min_x, data->limits.max_x, rand_float());
            state->cendroids[i].y = Lerp(data->limits.min_y, data->limits.max_y, rand_float());
        }
    }
}

int main(void)
{
    Data *data = NULL;
    Limits limits = {.min_x = -20.0f, .max_x = 20.0f, .min_y = -20.0f, .max_y = 20.0f};
    data_create(&data, limits);

    KMeansState *kmeans_state = NULL;
    KMeansParams params = {.num_cluster = 3};
    kmeans_state_create(&kmeans_state, params, limits);

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

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_R)) {
            free_data(&data);
            data_create(&data, limits);
        }
        if (IsKeyPressed(KEY_SPACE)) {
            recluster_state(kmeans_state, &params, data);
            update_means(kmeans_state, &params, data);
        }
        BeginDrawing();
        ClearBackground(GetColor(0x181818AA));

        layout_stack_push(&ls, LO_VERT, ui_rect(0, 0, w, h), 2, 0);
        layout_stack_push(&ls, LO_HORZ, layout_stack_slot(&ls), 2, 0);
        widget(layout_stack_slot(&ls), PINK);
        cluster_widget(layout_stack_slot(&ls), data->points, kmeans_state->cluster_points, kmeans_state->cendroids, limits);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
