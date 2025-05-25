#include <stdio.h>
#include "raylib.h"
#include "raymath.h"
#include <math.h>
#include <time.h>
#include <float.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#define K 3
#define MIN_X -20.0
#define MAX_X 20.0
#define MIN_Y -20.0
#define MAX_Y 20.0

#define SAMPLE_RADIUS 4.0f
#define MEAN_RADIUS (2*SAMPLE_RADIUS)

#define ARRAY_LEN(xs) sizeof(xs)/sizeof(xs[0])

void print_array(float **array)
{
    for (size_t i = 0; i < arrlen(array); ++i) {
        for (size_t j = 0; j < arrlen(array[i]); ++j) {
            printf("%f\n", array[i][j]);
        }
        printf("\n");
    }
}

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

void read_csv_file(const char *file_path, void ***all_data, size_t *data_size)
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
    FILE *file_handler = fopen(file_path, "r");

    char *char_in_row = NULL; // 1 char
    char *float_char = NULL;
    float *data_in_row = NULL;

    while (true) {
        char buf[2] = {0};
        fgets(buf, 2, file_handler);
        buf[1] = 0;
        if (!(buf[0] == '\n' || buf[0] == '\0')) {
            arrput(char_in_row, buf[0]);
        } else {
            for (size_t i = 0; i < arrlen(char_in_row); ++i) {
                if (char_in_row[i] != ',') { // ['0 ', '.', '1','2' , ',' ...]
                    arrput(float_char, char_in_row[i]);
                }
                else { // float_char ['0','.','1','2',0]
                    arrput(float_char, 0);
                    float value = atof(float_char);
                    arrput(data_in_row, value);
                    arrfree(float_char);
                }
            }
            // take last values add to data_in_row (no comma issue)
            arrput(float_char, 0);
            float value = atof(float_char);
            arrput(data_in_row, value);
            arrfree(float_char);
            // add data_in_row to all_data
            arrput(*all_data, data_in_row);
            arrfree(char_in_row);
            char_in_row = NULL;
            data_in_row = NULL;
            // check if EOF
            if (buf[0] == '\0') { // EOF => '\0'
                break;
            }
       }
   }
   if (file_handler) fclose(file_handler);
}

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

Vector2 *set = NULL;
static Vector2 *clusters[K] = {0};
static Vector2 means[K] = {0};
static Color colors[] = {
    PINK,
    YELLOW,
    RED,
    BLUE,
    MAROON,
    GREEN,
    LIME,
    SKYBLUE,
    GOLD,
    PURPLE,
    VIOLET,
    BEIGE,
    BROWN,
};

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

    float **all_data = NULL;
    size_t data_size = 0;
    read_csv_file(leaf_path, (void***)&all_data, &data_size);
    //printf("size: %f\n", sizeof(all_data));
    print_array(all_data);

    srand(time(0));
    /*SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 600, "k-means");

    generate_new_state();*/

    /*while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_R)) {
            generate_new_state();
            recluster_state();
        }
        if (IsKeyPressed(KEY_SPACE)) {
            update_means();
            recluster_state();
        }
        BeginDrawing();
        ClearBackground(GetColor(0x181818AA));
        for (size_t i = 0; i < arrlen(set); ++i) {
            Vector2 it = set[i];
            DrawCircleV(project_sample_to_screen(it), SAMPLE_RADIUS, RED);
        }
        for (size_t i = 0; i < K; ++i) {
            Color color = colors[i%(ARRAY_LEN(colors))];

            for (size_t j = 0; j < arrlen(clusters[i]); ++j) {
                Vector2 it = clusters[i][j];
                DrawCircleV(project_sample_to_screen(it), SAMPLE_RADIUS, color);
            }

            DrawCircleV(project_sample_to_screen(means[i]), MEAN_RADIUS, color);
        }
        EndDrawing();
    }
    CloseWindow();*/
    return 0;
}
