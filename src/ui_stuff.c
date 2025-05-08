#include "ui_stuff.h"
#include <stdio.h>
#include <assert.h>
#include <rlgl.h>
#include <stdlib.h>

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

void widget(UiRect r, Color c) { DrawRectangle(r.x, r.y, r.w, r.h, c); }

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
