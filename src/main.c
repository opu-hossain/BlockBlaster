/*



##########################################
#                                        #
# ---> Project Name: Block Blaster <---  #
#                                        #
#      Name: Azizur Rahaman              #
#      ID:0112330455                     #
#                                        #
#                                        #
##########################################


*/

#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define WIDTH 800
#define HEIGHT 600

#define SPEED 500.0f
#define PLAYER_SPEED 1.5 * SPEED
#define BALL_SPEED SPEED * 0.2
#define BLOCK_COUNT 60
#define BLOCK_WIDTH 40
#define BLOCK_HEIGHT 20
#define ITERATIONS 4
#define BOUNCE_WAIT 300

typedef struct {
  Vector2 pos;
  Vector2 size;
} Entity;

typedef struct {
  Entity entity;
  bool is_destroyed;
} Block;

typedef struct {
  int score;
  int high_score;
  size_t destroyed_count;
  size_t bounce_wait;
  Vector2 ball_initial_pos;
  Vector2 ball_speed;
  float delta_time;
  Entity player;
  Entity ball;
} Game;

Game game = {0};
Music bg_music;
Sound impact_sound;

Block block_stack[BLOCK_COUNT];

const char *get_database_path() {
  static char path[512];
  snprintf(path, sizeof(path), "%sdatabase.txt", GetApplicationDirectory());
  return path;
}

void read_from_file() {
  const char *path = get_database_path();
  FILE *file = fopen(path, "a+");
  if (file == NULL) {
    fprintf(stderr, "error: could not open file %s\n", path);
    game.score = 0;
    return;
  }

  int maxi = 0;
  int temp;

  while (fscanf(file, "%d\n", &temp) != EOF) {
    printf("%d\n", temp);
    if (temp > maxi)
      maxi = temp;
  }
  fclose(file);

  game.high_score = maxi;
}

void write_to_file() {
  const char *path = get_database_path();
  FILE *file = fopen(path, "a");
  if (file == NULL) {
    fprintf(stderr, "error: could not open file %s\n", path);
    return;
  }

  fprintf(file, "\n%d", game.score);
  fclose(file);
}

Entity init_entity(float x, float y, float width, float height) {
  Entity entity = {.pos =
                       {
                           .x = x,
                           .y = y,
                       },
                   .size = {
                       .x = width,
                       .y = height,
                   }};
  return entity;
}

bool check_collision(Entity *e1, Entity *e2) {
  if ((e1->pos.x) <= (e2->pos.x + e2->size.x) &&
      (e1->pos.x + e1->size.x) >= e2->pos.x &&
      (e1->pos.y) <= (e2->pos.y + e2->size.y) &&
      ((e1->pos.y + e1->size.y) >= e2->pos.y)) {
    return true;
  }
  return false;
}

void reset_blocks() {
  size_t block_stack_pos = 0;
  for (size_t layers = 0; layers <= ITERATIONS; layers++) {
    for (size_t i = 0; i < BLOCK_COUNT / ITERATIONS; i++) {
      Block block = {
          .entity = init_entity(((BLOCK_WIDTH + 10) * i + (BLOCK_WIDTH - 5)),
                                ((BLOCK_HEIGHT + 5) * layers) + 20, BLOCK_WIDTH,
                                BLOCK_HEIGHT),
          .is_destroyed = false,
      };
      block_stack[block_stack_pos++] = block;
    }
  }
}

void reset_ball(Entity *ball, const Game *game) {
  ball->pos.x = game->ball_initial_pos.x;
  ball->pos.y = game->ball_initial_pos.y;
}

float random_x() {
  srand(time(NULL));
  return SPEED - (float)rand() / (float)(RAND_MAX / 0.19f);
}

void initialize(Game *game) {
  read_from_file();
  printf("%d\n", game->high_score);

  game->score = 0;
  game->destroyed_count = 0;
  Vector2 ball_speed = {
      .x = random_x(),
      .y = BALL_SPEED,
  };
  game->ball_speed = ball_speed;
  Vector2 ball_initial_pos = {
      .x = WIDTH / 2,
      .y = HEIGHT / 2,
  };
  game->ball_initial_pos = ball_initial_pos;
  game->bounce_wait = 0;

  reset_blocks();
}

void MENU_TITLE(char title[], float posX, float posY) {
  DrawRectangle(posX, posY, 200, 40, BLACK);
  DrawText(title, posX + 30, posY + 10, 20, RAYWHITE);
}

void BUILD_BUTTON(char title[], float posX, float posY) {
  DrawRectangle(posX, posY, 40, 40, BLACK);
  DrawText(title, posX + 15, posY + 10, 20, RAYWHITE);
}

void splash_screen() {
  float posX = (WIDTH / 2) - 70;
  float posY = HEIGHT - (HEIGHT / 4);

  bool isClose = false;

  while (!(IsKeyPressed(KEY_ENTER))) {
    UpdateMusicStream(bg_music);
    BeginDrawing();
    ClearBackground(LIGHTGRAY);

    DrawText("BLOCK BLASTER", 50, HEIGHT / 2 - 20, 80, RED);

    // Enter Button
    DrawRectangle(posX, posY, 140, 40, BLACK);
    DrawText("HIT ENTER!", posX + 10, posY + 10, 20, RAYWHITE);
    EndDrawing();

    if (WindowShouldClose()) {
      isClose = true;
      break;
    }
  }

  if (isClose)
    CloseWindow();
  printf("key pressed\n");
}

void GamePlay();
void ScoreBoard();

void show_menu() {
  float posX = (WIDTH / 2) - 70;
  float posY = 200;

  while (!(WindowShouldClose())) {
    UpdateMusicStream(bg_music);
    BeginDrawing();
    ClearBackground(RAYWHITE);

    BUILD_BUTTON("P", posX - 60, posY);
    MENU_TITLE("PLAY", posX, posY);

    BUILD_BUTTON("S", posX - 60, posY + 50);
    MENU_TITLE("SCORE BOARD", posX, posY + 50);

    BUILD_BUTTON("E", posX - 60, posY + 100);
    MENU_TITLE("EXIT", posX, posY + 100);
    EndDrawing();

    if (IsKeyPressed(KEY_P)) {
      GamePlay();
    } else if (IsKeyPressed(KEY_S)) {
      ScoreBoard();
    } else if (IsKeyPressed(KEY_E)) {
      break;
    }
  }
}

int main() {

  InitWindow(WIDTH, HEIGHT, "Block Blaster");
  InitAudioDevice();

  bg_music = LoadMusicStream(
      TextFormat("%sassets/music.mp3", GetApplicationDirectory()));
  if (!IsMusicValid(bg_music)) {
    fprintf(stderr, "error: failed to load backgroud music\n");
  } else {
    PlayMusicStream(bg_music);
  }

  impact_sound =
      LoadSound(TextFormat("%sassets/impact_1.ogg", GetApplicationDirectory()));
  if (!IsSoundValid(impact_sound)) {
    fprintf(stderr, "error: failed to load impact_sound\n");
  }

  splash_screen();
  show_menu();

  UnloadSound(impact_sound);
  UnloadMusicStream(bg_music);
  CloseAudioDevice();

  CloseWindow();
  return 0;
}

void GamePlay() {

  initialize(&game);
  game.ball =
      init_entity(game.ball_initial_pos.x, game.ball_initial_pos.y, 20, 20);
  game.player = init_entity(WIDTH / 2, HEIGHT - (HEIGHT / 10), 100, 20);

  // InitWindow(WIDTH, HEIGHT, "breakout");

  bool isPause = false;

  while (!WindowShouldClose()) {
    SetMusicVolume(bg_music, 0.3f);
    UpdateMusicStream(bg_music);
    game.delta_time = GetFrameTime();
    // crash if all blocks are destroyed
    if (game.destroyed_count >= BLOCK_COUNT || game.ball.pos.y >= HEIGHT - 20) {

      Vector2 font_size =
          MeasureTextEx(GetFontDefault(), "GAME OVER", 100, 10.0f);
      DrawText("GAME OVER", WIDTH / 2 - font_size.x / 2,
               HEIGHT / 2 - font_size.y / 2, 100, RED);
      EndDrawing();

      write_to_file();

      reset_ball(&game.ball, &game);
      initialize(&game);
      game.score += 1;
      WaitTime(3.0);
      SetMusicVolume(bg_music, 1.0f);
      break;
    }

    // block rendering and collision handling
    for (size_t i = 0; i < BLOCK_COUNT; i++) {
      Block *enemy = &block_stack[i];
      if (!enemy->is_destroyed) {

        if (i <= 14) {
          DrawRectangleV(enemy->entity.pos, enemy->entity.size, RED);
        } else if (i <= 29) {
          DrawRectangleV(enemy->entity.pos, enemy->entity.size, BLUE);
        } else if (i <= 44) {
          DrawRectangleV(enemy->entity.pos, enemy->entity.size, GREEN);
        } else if (i <= 59) {
          DrawRectangleV(enemy->entity.pos, enemy->entity.size, YELLOW);
        }

        if (check_collision(&enemy->entity, &game.ball)) {
          game.destroyed_count++;
          block_stack[i].is_destroyed = true;
          game.score += 1;
          game.ball_speed.y = BALL_SPEED;
          PlaySound(impact_sound);
        }
      }
    }

    if (GetKeyPressed() == KEY_SPACE) {
      isPause = !isPause;
    }

    // PAUSE

    if (isPause) {
      MENU_TITLE("PAUSE", WIDTH / 2 - 50, HEIGHT / 2 - 50);
      EndDrawing();
      continue;
    }

    if (game.ball.pos.x < (0 - 25))
      game.ball.pos.x = 0;
    if (game.ball.pos.x > (WIDTH - game.ball.size.x + 25))
      game.ball.pos.x = (WIDTH - game.ball.size.x);

    if (game.ball.pos.y < (0 - 25))
      game.ball.pos.y = 0;
    if (game.ball.pos.y > (HEIGHT - game.ball.size.y + 25))
      game.ball.pos.y = (HEIGHT - game.ball.size.y);

    if (game.player.pos.x < 0)
      game.player.pos.x = 0;
    if (game.player.pos.x > (WIDTH - game.player.size.x))
      game.player.pos.x = (WIDTH - game.player.size.x);

    // Key presses
    //
    //
    if (IsKeyDown(KEY_LEFT))
      game.player.pos.x -= PLAYER_SPEED * game.delta_time;
    if (IsKeyDown(KEY_RIGHT))
      game.player.pos.x += PLAYER_SPEED * game.delta_time;

    // game.ball speed
    game.ball.pos.y += (1 * game.ball_speed.y) * game.delta_time;
    game.ball.pos.x += (1 * game.ball_speed.x) * game.delta_time;

    // game.player game.ball collision
    if (game.bounce_wait == 0) {
      if ((game.player.pos.x) <= (game.ball.pos.x + game.ball.size.x) &&
          (game.player.pos.x + game.player.size.x) >= game.ball.pos.x &&
          (game.player.pos.y) <= (game.ball.pos.y + game.ball.size.y) &&
          (game.player.pos.y + 5) >= (game.ball.pos.y + game.ball.size.y) &&
          (game.player.pos.y + game.player.size.y) >= (game.ball.pos.y)) {
        game.ball_speed.y *= -1;
        game.bounce_wait = BOUNCE_WAIT;
      } else if ((game.player.pos.x) <= (game.ball.pos.x + game.ball.size.x) &&
                 (game.player.pos.x + game.ball.size.x * 0.25) >=
                     (game.ball.pos.x + game.ball.size.x) &&
                 (game.player.pos.x + game.player.size.x) >= game.ball.pos.x &&
                 (game.player.pos.y) <= (game.ball.pos.y + game.ball.size.y) &&
                 (game.player.pos.y + game.player.size.y) >=
                     (game.ball.pos.y)) {
        game.ball_speed.y *= -1;
        game.ball_speed.x = -random_x();
        game.bounce_wait = BOUNCE_WAIT;
      } else if ((game.player.pos.x) <= (game.ball.pos.x + game.ball.size.x) &&
                 (game.player.pos.x + game.player.size.x) >= game.ball.pos.x &&
                 (game.player.pos.x + game.player.size.x) <=
                     (game.ball.pos.x + game.ball.size.x * 0.25) &&
                 (game.player.pos.y) <= (game.ball.pos.y + game.ball.size.y) &&
                 (game.player.pos.y + game.player.size.y) >=
                     (game.ball.pos.y)) {
        game.ball_speed.y *= -1;
        game.ball_speed.x = random_x();
        game.bounce_wait = BOUNCE_WAIT;
      }
    }

    // game.ball window collision
    if (game.ball.pos.y > HEIGHT) {
      game.score -= 1;
      reset_ball(&game.ball, &game);
    }
    if (game.ball.pos.y < 0)
      game.ball_speed.y *= -1;
    if (game.ball.pos.x > WIDTH)
      game.ball_speed.x *= -1;
    if (game.ball.pos.x < 0)
      game.ball_speed.x *= -1;

    // init things
    BeginDrawing();
    ClearBackground(LIGHTGRAY);
    DrawRectangleV(game.player.pos, game.player.size, BLACK);
    DrawRectangleV(game.ball.pos, game.ball.size, BLACK);
    char *score_str = malloc(45);
    sprintf(score_str, "Score: %d", game.score);
    DrawText(score_str, (WIDTH / 2) - 80, 0, 20, BLUE);
    free(score_str);
    EndDrawing();
    if (game.bounce_wait > 0)
      game.bounce_wait--;
  }
}

void ScoreBoard() {

  read_from_file();
  while (!WindowShouldClose()) {
    UpdateMusicStream(bg_music);
    char textBuffer[20];

    BeginDrawing();
    ClearBackground(RAYWHITE);

    // BACK BUTTON

    BUILD_BUTTON("<", 10, 10);

    DrawText("SCORE BOARD: ", (WIDTH / 2) - 150, 50, 40, BLACK);
    sprintf(textBuffer, "%d", game.high_score);

    DrawText("Score: ", (WIDTH / 2) - 100, 100, 30, BLACK);
    DrawText(textBuffer, (WIDTH / 2) + 20, 100, 30, BLACK);
    EndDrawing();

    if (IsKeyPressed(KEY_LEFT)) {
      break;
    }
  }
}
