#ifndef DATA_H_
#define DATA_H_

#include "raylib.h"
#include <stddef.h>

typedef struct Limits {
  float min_x;
  float max_x;
  float min_y;
  float max_y;
} Limits;

typedef struct Data {
  Vector2* samples;
  Vector2* centroids;
} Data;

void gen_data(Data** data, size_t num_samples, size_t num_centroids, Limits limits);
void free_data(Data** data);

#endif // DATA_H_
