#include <string.h>
#include "game_scene.h"
#include "win_scene.h"
#include "lose_scene.h"
#include "menu_scene.h"
#include "loading_scene.h"
#include "game.h"
#include "player.h"
#include "map.h"
#include "enemy.h"
#include "weapon.h"
#include "UI.h"
#include "utility.h"
#include <math.h>

Player player;
Map map;
enemyNode* enemyList;
BulletNode* bulletList;
static Button pauseButton;

static ALLEGRO_BITMAP* pause_button_img;
static ALLEGRO_BITMAP* play_button_img;

// Weapon
Weapon weapon;
int coins_obtained;

// Time
double t0 = 0, t1 = 0;
double paused_time = 0;
double pause_start_time = 0;

// Button positions
const int button_width = 55;
const int button_height = 55;
const int button_x = 1100;
const int button_y = 10;

bool is_paused = false;
bool previous_mouse_right_click = false;
bool previous_key_p_press = false;
static double collected_time = 0;

static void init(void) {
    initEnemy();

    t0 = al_get_time();
    t1 = 0;
    paused_time = 0;
    pause_start_time = 0;
    collected_time = 0;
    if (level == 1) {
        map = create_map("Assets/map1.txt", 0);
    }
    else if (level == 2) {
        map = create_map("Assets/map2.txt", 0);
    }
    else if (level == 3) {
        map = create_map("Assets/map3.txt", 0);
    }

    player = create_player("Assets/panda2.png", map.Spawn.x, map.Spawn.y);

    enemyList = createEnemyList();
    bulletList = createBulletList();

    weapon = create_weapon("Assets/guns.png", "Assets/yellow_bullet.png", 16, 8, 10);

    ALLEGRO_BITMAP* coin_icon = al_load_bitmap("Assets/coin_icon.png");
    if (coin_icon) {
        al_draw_scaled_bitmap(
            coin_icon,
            0, 0,
            al_get_bitmap_width(coin_icon), al_get_bitmap_height(coin_icon),
            50, 10,
            32, 32,
            0
        );
        al_destroy_bitmap(coin_icon);
    }

    for (int i = 0; i < map.EnemySpawnSize; i++) {
        Enemy enemy = createEnemy(map.EnemySpawn[i].x, map.EnemySpawn[i].y, map.EnemyCode[i]);
        insertEnemyList(enemyList, enemy);
    }

    pause_button_img = al_load_bitmap("Assets/pause.png");
    if (!pause_button_img) {
        game_abort("Failed to load pause button image.");
    }

    play_button_img = al_load_bitmap("Assets/play.png");
    if (!play_button_img) {
        game_abort("Failed to load play button image.");
    }

    game_log("coord x:%d \n coords y:%d \n", map.Spawn.x, map.Spawn.y);
    change_bgm("Assets/audio/game_bgm2.mp3");
}


static void update(void) {
    //Pause & Play
    if ((keyState[ALLEGRO_KEY_P] && !previous_key_p_press) || (mouseState.buttons & 2 && !previous_mouse_right_click)) {
        int mouse_x = mouseState.x;
        int mouse_y = mouseState.y;

        if ((mouse_x >= button_x && mouse_x <= button_x + button_width &&
            mouse_y >= button_y && mouse_y <= button_y + button_height) || keyState[ALLEGRO_KEY_P]) {

            is_paused = !is_paused;

            if (is_paused) {
                pause_start_time = al_get_time();
                printf("Game Paused\n");
            }
            else {
                paused_time += al_get_time() - pause_start_time;
                printf("Game Resumed\n");
            }
        }
    }
    previous_mouse_right_click = (mouseState.buttons & 2);
    previous_key_p_press = keyState[ALLEGRO_KEY_P];


    /*
        [TODO Homework]

        Change the scene if winning/losing to win/lose scene
    */

    if (!is_paused) {
        update_player(&player, &map);

        Point Camera;
        /*
            [TODO HACKATHON 1-3]

            Calcualte the formula for the Camera
            Camera.x = ...
            Camera.y = ...

            Hint: Adjust it based on player position variable, then subtract it with half of the gameplay screen
        */
        Camera.x = (player.coord.x + TILE_SIZE) - SCREEN_W / 2;
        Camera.y = (player.coord.y + TILE_SIZE) - SCREEN_H / 2;

        int map_width = map.col * TILE_SIZE;
        int map_height = map.row * TILE_SIZE;

        Camera.x = fmax(0, fmin(Camera.x, map_width - SCREEN_W));
        Camera.y = fmax(0, fmin(Camera.y, map_height - SCREEN_H));
        updateEnemyList(enemyList, &map, &player);
        //weapon = player.weapon_idx == 0 ? create_weapon("Assets/guns.png", "Assets/yellow_bullet.png", 16, 8, 10): create_weapon("Assets/sniper.png", "Assets/orange_bullet.png", 16, 20, 10);
        if (player.weapon_idx == 0) {
            update_weapon(&weapon, "Assets/guns.png", "Assets/yellow_bullet.png", "Assets/audio/shooting.mp3", 16, 8, 10, bulletList, player.coord, Camera);
        }
        else if (player.weapon_idx == 1) {
            update_weapon(&weapon, "Assets/sniper.png", "Assets/orange_bullet.png", "Assets/audio/shooting2.mp3", 16, 20, 10, bulletList, player.coord, Camera);
        }
        //update_weapon(&weapon, bulletList, player.coord, Camera);
        updateBulletList(bulletList, enemyList, &map, &player);
        update_map(&map, player.coord, &coins_obtained);
        if (map.trophy_status == COLLECTED) {
            if (collected_time == 0) {
                collected_time = al_get_time();

                if (map.win_audio) {
                    if (!al_play_sample(map.win_audio, sfx_volume, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL)) {
                        game_log("Failed to play win audio.");
                    }
                }

                t1 = al_get_time();
                map.match_time = (t1 - t0) - paused_time;
                game_log("Time = %.f seconds\n", map.match_time);
            }

            if (al_get_time() - collected_time >= 0.5) {
                change_scene(create_win_scene());
            }
        }
        else if (player.status == PLAYER_DEAD) {
            t1 = al_get_time();
            map.match_time = (t1 - t0) - paused_time;
            game_log("Time = %.f seconds\n", (t1 - t0) - paused_time);
            if (map.lose_audio) {
                if (!al_play_sample(map.lose_audio, sfx_volume, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL)) {
                    game_log("Failed to play lose audio.");
                }
            }
            change_scene(create_lose_scene());
        }
        else {
            enemyNode* temp = enemyList->next;
            int count = 0;
            while (temp != NULL) {
                count++;
                temp = temp->next;
            }
            //game_log("Enemies remaining: %d", count);
        }
    }
}

static void draw(void) {  
    Point Camera;
    /*
        [TODO HACKATHON 1-4]

        Calcualte the formula for the Camera
        Camera.x = ...
        Camera.y = ...

        Hint: Just copy from the [TODO HACKATHON 1-3]
    */
    Camera.x = (player.coord.x + TILE_SIZE) - SCREEN_W / 2;
    Camera.y = (player.coord.y + TILE_SIZE) - SCREEN_H / 2;

    int map_width = map.col * TILE_SIZE;
    int map_height = map.row * TILE_SIZE;

    Camera.x = fmax(0, fmin(Camera.x, map_width - SCREEN_W));
    Camera.y = fmax(0, fmin(Camera.y, map_height - SCREEN_H));

    // Draw
    draw_map(&map, Camera);
    drawEnemyList(enemyList, Camera);
    drawBulletList(bulletList, Camera);
    draw_player(&player, Camera);
    draw_weapon(&weapon, player.coord, Camera);

    /*
        [TODO Homework]

        Draw the UI of Health and Total Coins
    */
    char player_health_str[20];
    snprintf(player_health_str, sizeof(player_health_str), "%02d", player.health);
    ALLEGRO_BITMAP* heart_icon = al_load_bitmap("Assets/heart.png");
    al_draw_scaled_bitmap(
        heart_icon,
        0, 0,
        al_get_bitmap_width(heart_icon), al_get_bitmap_height(heart_icon),
        10, 8,
        55, 55,
        0
    );
    al_draw_text(
        P2_FONT,
        al_map_rgb(255, 255, 255),
        63, 22,
        0,
        player_health_str
    );
    // Draw Total Coins
    char coins_collected_str[16];
    snprintf(coins_collected_str, sizeof(coins_collected_str), "%02d", map.coins_collected);
    ALLEGRO_BITMAP* coin_icon = al_load_bitmap("Assets/coin_icon.png");
    al_draw_scaled_bitmap(
        coin_icon,
        0, 0,
        al_get_bitmap_width(coin_icon), al_get_bitmap_height(coin_icon),
        10, 53,
        55, 55,
        0
    );
    al_draw_text(
        P2_FONT,
        al_map_rgb(255, 255, 255),
        63, 68,
        0,
        coins_collected_str
    );

    //Pause & Play
    if (is_paused) {
        al_draw_scaled_bitmap(pause_button_img, 0, 0,
            al_get_bitmap_width(pause_button_img), al_get_bitmap_height(pause_button_img),
            button_x, button_y, button_width, button_height, 0);
    }
    else {
        al_draw_scaled_bitmap(play_button_img, 0, 0,
            al_get_bitmap_width(play_button_img), al_get_bitmap_height(play_button_img),
            button_x, button_y, button_width, button_height, 0);
    }

    // Time
    char cal_time_str[16];
    double cal_time;
    if (is_paused) {
        cal_time = (pause_start_time - t0) - paused_time;
    }
    else {
        cal_time = (al_get_time() - t0) - paused_time;
    }

    int hours = (int)(cal_time / 3600);
    int minutes = ((int)cal_time % 3600) / 60;
    int seconds = (int)cal_time % 60;

    snprintf(cal_time_str, sizeof(cal_time_str), "%02d:%02d:%02d", hours, minutes, seconds);

    al_draw_text(
        P2_FONT,
        al_map_rgb(255, 255, 255),
        button_x - button_width - 100,
        button_y + button_height / 4,
        0,
        cal_time_str
    );
}

static void destroy(void) {
    delete_player(&player);
    delete_weapon(&weapon);
    destroy_map(&map);
    destroyBulletList(bulletList);
    destroyEnemyList(enemyList);
    terminateEnemy();

    if (pause_button_img) {
        al_destroy_bitmap(pause_button_img);
        pause_button_img = NULL;
    }

    if (play_button_img) {
        al_destroy_bitmap(play_button_img);
        play_button_img = NULL;
    }
}


Scene create_game_scene(void) {
    Scene scene;
    memset(&scene, 0, sizeof(Scene));

    scene.name = "game";
    scene.init = &init;
    scene.draw = &draw;
    scene.update = &update;
    scene.destroy = &destroy;

    return scene;
}
