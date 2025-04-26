#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include <raylib.h>
#include <raymath.h>

#include "nobuild.h"

typedef enum
{
  TOP,
  TOP_RIGHT,
  TOP_LEFT,
  BOTTOM,
  BOTTOM_RIGHT,
  BOTTOM_LEFT,
  LEFT,
  RIGHT
} Direction;

typedef enum
{
  PLAYING,
  PAUSED
} Sate;

typedef struct
{
  int w;
  int h;
  Vector2 pos;
  Vector2 rel_pos;
  float speed;
  Color color;
  Direction dir;

  int bullet_w;
  int bullet_h;
  float fire_rate;
  float fire_speed;
  float fire_damage;
  double last_fired;
} Player;

typedef struct
{
  int w;
  int h;
  Vector2 pos;
  Vector2 rel_pos;
  float speed;
  Color color;
  Direction dir;
  float life;
  float max_life;
} Enemy;

typedef struct
{
  int w;
  int h;
  Vector2 pos;
  Vector2 rel_pos;
  float speed;
  Direction dir;
  bool fire;
} Bullet;

typedef struct
{
  Bullet *data;
  size_t length;
  size_t capacity;
} Bullets;

typedef struct
{
  Player player;
  Bullets bullets;
  Enemy enemy;
  Sate state;
  int score;
} Plug;

Plug *p = NULL;

Vector2 get_vector(Direction dir)
{
  switch (dir)
  {
  case TOP:
    return (Vector2){0, -1};
  case TOP_LEFT:
    return Vector2Normalize((Vector2){-1, -1});
  case TOP_RIGHT:
    return Vector2Normalize((Vector2){1, -1});
  case BOTTOM:
    return (Vector2){0, 1};
  case BOTTOM_LEFT:
    return Vector2Normalize((Vector2){-1, 1});
  case BOTTOM_RIGHT:
    return Vector2Normalize((Vector2){1, 1});
    break;
  case LEFT:
    return (Vector2){-1, 0};
    break;
  case RIGHT:
    return (Vector2){1, 0};
  default:
    break;
  }
}

Direction get_direction(Vector2 vec)
{
  if (vec.x == 0 && vec.y == -1)
    return TOP;
  if (vec.x == -1 && vec.y == -1)
    return TOP_LEFT;
  if (vec.x == 1 && vec.y == -1)
    return TOP_RIGHT;
  if (vec.x == 0 && vec.y == 1)
    return BOTTOM;
  if (vec.x == -1 && vec.y == 1)
    return BOTTOM_LEFT;
  if (vec.x == 1 && vec.y == 1)
    return BOTTOM_RIGHT;
  if (vec.x == -1 && vec.y == 0)
    return LEFT;
  if (vec.x == 1 && vec.y == 0)
    return RIGHT;
}

void plug_clear(void)
{
  free(p->bullets.data);
  free(p);
}

void plug_init(void)
{
  p = malloc(sizeof(*p));
  assert(p != NULL && "Bro buy some RAM");
  memset(p, 0, sizeof(*p));

  int w = GetScreenWidth();
  int h = GetScreenHeight();
  float dt = GetFrameTime();

  // Player stuff
  Player player = {0};

  player.pos.x = w / 2;
  player.pos.y = h / 2;
  player.rel_pos.x = player.pos.x / w;
  player.rel_pos.y = player.pos.y / h;
  player.w = 35;
  player.h = 35;
  player.speed = 400;
  player.color = BLACK;
  player.dir = BOTTOM;

  player.fire_rate = 1.0f;
  player.last_fired = 0;
  player.fire_speed = 500;
  player.bullet_w = 5;
  player.bullet_h = 5;
  player.fire_damage = 10;

  p->player = player;

  // Enemy stuff
  Enemy enemy = {0};

  enemy.pos.x = 0.0f;
  enemy.pos.y = h / 2;
  enemy.rel_pos.x = enemy.pos.x / w;
  enemy.rel_pos.y = enemy.pos.y / h;
  enemy.w = 35;
  enemy.h = 35;
  enemy.speed = 100;
  enemy.color = YELLOW;
  enemy.dir = BOTTOM;
  enemy.max_life = 30;
  enemy.life = enemy.max_life;

  p->enemy = enemy;

  // Bullet stuff
  Bullets bullets = {0};

  p->bullets = bullets;

  p->state = PLAYING;
}

void *plug_pre_reload()
{
  return p;
}

void plug_post_reload(Plug *prev_p)
{
  p = prev_p;
}

void update_game()
{
  int w = GetScreenWidth();
  int h = GetScreenHeight();
  float dt = GetFrameTime();

  if (IsKeyPressed(KEY_ESCAPE))
  {
    p->state = PAUSED;
    return;
  }

  // Update player stuff
  Player *player = &p->player;

  bool moved = false;
  Vector2 v = {0, 0};
  if (IsKeyDown(KEY_W))
  {
    v.y = -1;
    moved = true;
  }
  if (IsKeyDown(KEY_S))
  {
    v.y = 1;
    moved = true;
  }
  if (IsKeyDown(KEY_A))
  {
    v.x = -1;
    moved = true;
  }
  if (IsKeyDown(KEY_D))
  {
    v.x = 1;
    moved = true;
  }

  if (moved)
  {
    Vector2 n_v = Vector2Normalize(v);
    float p_s = player->speed * dt;
    float new_x = (player->pos.x + n_v.x * p_s);
    float new_y = (player->pos.y + n_v.y * p_s);

    bool within_x_bounds = new_x - player->w >= 0 && new_x + player->w <= w;
    bool within_y_bounds = new_y - player->h >= 0 && new_y + player->h <= h;

    if (within_x_bounds)
    {
      player->rel_pos.x = new_x / w;
    }
    if (within_y_bounds)
    {
      player->rel_pos.y = new_y / h;
    }

    player->dir = get_direction(v);
  }

  // Update enemy stuff
  Enemy *enemy = &p->enemy;

  if (enemy->life > 0)
  {
    Vector2 dir = Vector2Normalize(Vector2Subtract(player->pos, enemy->pos));

    enemy->rel_pos.x = (enemy->pos.x + dir.x * enemy->speed * dt) / w;
    enemy->rel_pos.y = (enemy->pos.y + dir.y * enemy->speed * dt) / h;
  }

  // Update Bullet stuff
  Bullets *bullets = &p->bullets;
  bool firing = false;
  Direction bullet_direction;

  player->last_fired += dt;
  bool can_fire = player->last_fired >= player->fire_rate;

  if (IsKeyDown(KEY_UP))
  {
    bullet_direction = TOP;
    firing = true;
  }
  if (IsKeyDown(KEY_DOWN))
  {
    bullet_direction = BOTTOM;
    firing = true;
  }
  if (IsKeyDown(KEY_LEFT))
  {
    bullet_direction = LEFT;
    firing = true;
  }
  if (IsKeyDown(KEY_RIGHT))
  {
    bullet_direction = RIGHT;
    firing = true;
  }

  if (firing && can_fire)
  {
    Bullet bullet = {0};
    bullet.speed = player->fire_speed;
    bullet.w = player->bullet_w;
    bullet.h = player->bullet_h;

    bullet.dir = bullet_direction;
    Vector2 move = get_vector(bullet.dir);

    bullet.pos.x = player->pos.x + move.x * player->w;
    bullet.pos.y = player->pos.y + move.y * player->h;
    bullet.fire = true;
    player->last_fired = 0;

    da_append(bullets, bullet);
  }

  for (size_t i = 0; i < bullets->length; i++)
  {
    Bullet *bullet = &bullets->data[i];

    if (bullet->fire)
    {
      if (bullet->pos.x - bullet->w < 0 || bullet->pos.x + bullet->w >= w ||
          bullet->pos.y - bullet->h < 0 || bullet->pos.y + bullet->h >= h)
      {
        bullet->fire = false;
      }
      else
      {
        Vector2 move = get_vector(bullet->dir);
        float speed = bullet->speed * dt;
        bullet->rel_pos.x = (bullet->pos.x + move.x * speed) / w;
        bullet->rel_pos.y = (bullet->pos.y + move.y * speed) / h;
      }
      // Detect collision
      if (enemy->life > 0 && Vector2Distance(enemy->pos, bullet->pos) <= enemy->w + bullet->w)
      {
        bullet->fire = false;
        enemy->life -= player->fire_damage;
        p->score += 1;
      }
    }
  }
}

void draw_game()
{

  int w = GetScreenWidth();
  int h = GetScreenHeight();
  float dt = GetFrameTime();

  DrawText(TextFormat("CURRENT FPS: %i", (int)(1.0f / dt)), w - 220, 40, 20, GREEN);
  DrawText(TextFormat("SCORE: %i", p->score), 10, 40, 20, BLACK);

  // Draw player stuff
  Player *player = &p->player;
  player->pos.x = player->rel_pos.x * w;
  player->pos.y = player->rel_pos.y * h;
  DrawCircleV(player->pos, player->w, player->color);
  DrawCircleV(player->pos, 1, YELLOW);

  // Draw enemy stuff
  Enemy *enemy = &p->enemy;
  enemy->pos.x = enemy->rel_pos.x * w;
  enemy->pos.y = enemy->rel_pos.y * h;
  if (enemy->life > 0)
  {
    DrawCircleV(enemy->pos, enemy->w, enemy->color);
    float life_p = enemy->life / enemy->max_life;
    DrawRectangle(enemy->pos.x - enemy->w / 2, enemy->pos.y - enemy->h - 15, enemy->w * life_p, 10, GREEN);
  }

  // Draw bullets stuff
  Bullets bullets = p->bullets;
  for (size_t i = 0; i < bullets.length; i++)
  {
    Bullet *bullet = &bullets.data[i];
    bullet->pos.x = bullet->rel_pos.x * w;
    bullet->pos.y = bullet->rel_pos.y * h;
    if (bullet->fire)
    {
      DrawCircleV(bullet->pos, bullet->w, PURPLE);
    }
  }
}

void plug_update(void)
{
  int w = GetScreenWidth();
  int h = GetScreenHeight();

  float dt = GetFrameTime();

  BeginDrawing();

  ClearBackground(RAYWHITE);

  draw_game();

  if (p->state == PLAYING)
  {
    update_game();
  }
  else if (p->state == PAUSED)
  {
    const char *label = "PAUSED";
    Color paused_bg_color = (Color){0, 0, 0, 100};

    BeginBlendMode(BLEND_ALPHA);
    DrawRectangle(0, 0, w, h, paused_bg_color);
    EndBlendMode();

    int label_width = MeasureText(label, 69);
    int label_height = 69;
    DrawText("PAUSED", w / 2 - label_width / 2, h / 2 - label_height / 2, 69, DARKGRAY);

    if (IsKeyPressed(KEY_ESCAPE))
    {
      p->state = PLAYING;
    }
  }
  EndDrawing();
}