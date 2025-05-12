#ifndef DATA_H_
#define DATA_H_

#include "raylib.h"
#include <stddef.h>

typedef struct Limits {
    float min_x, max_x, min_y, max_y;
} Limits;

typedef struct Data {
    Vector2* points;
    Limits limits;
} Data;

void data_create(Data** data, Limits limits);
void free_data(Data** data);
// function to return float number between 0 and 1
float rand_float(void);

#endif // DATA_H_
