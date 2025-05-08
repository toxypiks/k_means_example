#include "ui_stuff.h"
#include <stdio.h>
#include <assert.h>
#include <rlgl.h>
#include <stdlib.h>

#include "stb_ds.h"

#define ARRAY_LEN(xs) sizeof(xs)/sizeof(xs[0])

UiRect ui_rect(float x, float y, float w, float h) {
  UiRect r = {0};
  r.x = x;
  r.y = y;
  r.w = w;
  r.h = h;
  return r;
}

UiRect layout_slot_loc(Layout *l, const char *file_path, int line)
{
  if (l->i >= l->count) {
    fprintf(stderr, "%s:%d: ERROR: Layout overflow\n", file_path, line);
    exit(1);
  }

  UiRect r = {0};

  switch (l->orient) {
  case LO_HORZ:
    r.w = l->rect.w / l->count;
    r.h = l->rect.h;
    r.x = l->rect.x + l->i * r.w;
    r.y = l->rect.y;

    if (l->i == 0) { // First
      r.w -= l->gap / 2;
    } else if (l->i >= l->count - 1) { // Last
      r.x += l->gap / 2;
      r.w -= l->gap / 2;
    } else { // Middle
      r.x += l->gap / 2;
      r.w -= l->gap;
    }

    break;

  case LO_VERT:
    r.w = l->rect.w;
    r.h = l->rect.h / l->count;
    r.x = l->rect.x;
    r.y = l->rect.y + l->i * r.h;

    if (l->i == 0) { // First
      r.h -= l->gap / 2;
    } else if (l->i >= l->count - 1) { // Last
      r.y += l->gap / 2;
      r.h -= l->gap / 2;
    } else { // Middle
      r.y += l->gap / 2;
      r.h -= l->gap;
    }

    break;

  default:
    assert(0 && "Unreachable");
  }

  l->i += 1;

  return r;
}

void layout_stack_push(LayoutStack *ls, LayoutOrient orient, UiRect rect, size_t count, float gap)
{
  Layout l = {0};
  l.orient = orient;
  l.rect = rect;
  l.count = count;
  l.gap = gap;
  da_append(ls, l);
}

void layout_stack_delete(LayoutStack *ls){
    ls->capacity = 0;
    ls->count = 0;
    free(ls->items);
}

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

void widget(UiRect r, Color c) { DrawRectangle(r.x, r.y, r.w, r.h, c); }

// function to map sample value to screen
static Vector2 project_sample_to_screen(UiRect r, Vector2 sample)
{
    // -20..20 => 0..40 => 0..1
    float MIN_X = r.x;
    float MAX_X = r.x + r.w;
    float MIN_Y = r.y;
    float MAX_Y = r.y + r.h;
    float nx = (sample.x - MIN_X)/(MAX_X - MIN_X);
    float ny = (sample.y - MIN_Y)/(MAX_Y - MIN_Y);
    float w = GetScreenWidth();
    float h = GetScreenHeight();
    return CLITERAL(Vector2) {
        .x = w*nx,
        .y = h - (h*ny),
    };
}

void cluster_widget(UiRect r, Vector2 *set, Vector2 *clusters[], Vector2 means[])
{
    for (size_t i = 0; i < arrlen(set); ++i) {
        Vector2 it = set[i];
        DrawCircleV(project_sample_to_screen(r, it), SAMPLE_RADIUS, RED);
    }
    for (size_t i = 0; i < 3; ++i) {
        Color color = colors[i%(ARRAY_LEN(colors))];

        for (size_t j = 0; j < arrlen(clusters[i]); ++j) {
            Vector2 it = clusters[i][j];
            DrawCircleV(project_sample_to_screen(r, it), SAMPLE_RADIUS, color);
        }

        DrawCircleV(project_sample_to_screen(r, means[i]), MEAN_RADIUS, color);
    }
}

UiStuff* create_ui_stuff(size_t screen_width, size_t screen_height){
  UiStuff* ui_stuff = (UiStuff*)malloc(sizeof(UiStuff));
  // init part
  ui_stuff->screen = LoadRenderTexture(screen_width, screen_height);
  return ui_stuff;
}

void ui_stuff_clear(UiStuff* ui_stuff) {
  free(ui_stuff);
  return;
}
