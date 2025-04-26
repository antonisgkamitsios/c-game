#include <stdio.h>
#include <stdbool.h>

#include <raylib.h>
#include "plug.h"
#include "hot_reload.h"

int main(void)
{

  if (!reload_plug())
    return 1;

  InitWindow(800, 600, "Test");
  SetExitKey(0);
  SetTargetFPS(60);

  plug_init();

  while (!WindowShouldClose())
  {

    if (IsKeyPressed(KEY_F11))
    {
      plug_clear();
      if (!reload_plug())
        return 1;

      plug_init();
    }

    if (IsKeyPressed(KEY_F5))
    {
      void *state = plug_pre_reload();
      if (!reload_plug())
        return 1;

      plug_post_reload(state);
    }

    plug_update();
  }

  CloseWindow();

  return 0;
}