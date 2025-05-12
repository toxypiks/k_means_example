#include "data.h"
#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <stddef.h>
#include <math.h>
#include <float.h>

#include "stb_ds.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

// function to return float number between 0 and 1
inline float rand_float(void)
{
    // random value between 0 and 1
    return (float)rand()/RAND_MAX;
}

void generate_cluster(Vector2 center, float radius, size_t count, Vector2 **samples)
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

void data_create(Data** data, Limits limits)
{
    (*data) = malloc(sizeof(Data));
    (*data)->limits = limits;
    arrsetlen((*data)->points, 0);
    // generate random clusters for drawing
    generate_cluster(CLITERAL(Vector2){0}, 10, 100, &((*data)->points));
    generate_cluster(CLITERAL(Vector2){limits.min_x*0.5f, limits.max_y*0.5f}, 5, 50, &((*data)->points));
    generate_cluster(CLITERAL(Vector2){limits.max_x*0.5f, limits.max_y*0.5f}, 5, 50, &((*data)->points));
}

void free_data(Data** data)
{
    arrfree((*data)->points);
    free(*data);
    *data = NULL;
}
