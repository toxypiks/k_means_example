#ifndef UI_STUFF_H_
#define UI_STUFF_H_

#include <raylib.h>
#include <raymath.h>
#include <stddef.h>

#include "data.h"

#define SAMPLE_RADIUS 4.0f
#define MEAN_RADIUS (2*SAMPLE_RADIUS)

#define DA_INIT_CAP 256
#define da_append(da, item)                                                      \
  do {                                                                           \
    if ((da)->count >= (da)->capacity) {                                         \
      (da)->capacity = ((da)->capacity == 0) ? DA_INIT_CAP : (da)->capacity * 2; \
      (da)->items =                                                              \
          realloc((da)->items, (da)->capacity * sizeof(*(da)->items));           \
      assert((da)->items != NULL && "Buy more RAM lol");                         \
    }                                                                            \
                                                                                 \
    (da)->items[(da)->count++] = (item);                                         \
  } while (0)

#define layout_stack_slot(ls)                                                    \
  (assert((ls)->count > 0),                                                      \
   layout_slot_loc(&(ls)->items[(ls)->count - 1], __FILE__, __LINE__))
#define layout_stack_pop(ls)                                                     \
  do {                                                                           \
    assert((ls)->count > 0);                                                     \
    (ls)->count -= 1;                                                            \
  } while (0)

typedef struct UiRect {
  float x;
  float y;
  float w;
  float h;
} UiRect;

typedef enum LayoutOrient {
  LO_HORZ,
  LO_VERT,
} LayoutOrient;

typedef struct Layout {
  LayoutOrient orient;
  UiRect rect;
  size_t count;
  size_t i;
  float gap;
} Layout;

typedef struct LayoutStack {
  Layout *items;
  size_t count;
  size_t capacity;
} LayoutStack;

typedef struct UiStuff {
  RenderTexture2D screen;
} UiStuff;

UiStuff* create_ui_stuff(size_t screen_width, size_t screen_height);
void ui_stuff_clear(UiStuff*);

UiRect ui_rect(float x, float y, float w, float h);
UiRect layout_slot_loc(Layout *l, const char *file_path, int line);
void layout_stack_push(LayoutStack *ls, LayoutOrient orient, UiRect rect, size_t count, float gap);
void layout_stack_delete(LayoutStack *ls);
void widget(UiRect r, Color c);
void cluster_widget(UiRect r, Vector2 *set, Vector2 *clusters[], Vector2 means[], Limits limits);
#endif // UI_STUFF_H
