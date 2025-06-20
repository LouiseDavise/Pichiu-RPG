#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "player.h"
#include "map.h"

/*
    [OFFSET CALCULATOR FUNCTIONS]
      You might want to modify this functions if you are using different assets
      In short, this functions help us to find which asset to draw
 */
static void get_map_offset(Map* map);
static Point get_wall_offset_assets(Map* map, int i, int j);
static Point get_floor_offset_assets(Map* map, int i, int j);
static Point get_hole_offset_assets(Map* map, int i, int j);
static const int offset = 16;
extern Player player;

static bool isWall(Map* map, int i, int j);
static bool isFloor(Map* map, int i, int j);
static bool isNothing(Map* map, int i, int j);
static bool isHorDoor(Map* map, int i, int j);
static bool isVerDoor(Map* map, int i, int j);
static bool isButton(Map* map, int i, int j);
static bool isHeal(Map* map, int i, int j);
static bool isCoin(Map* map, int i, int j);
static bool isTrophy(Map* map, int i, int j);

static bool tile_collision(Point player, Point tile_coord);

Map create_map(char* path, uint8_t type) {
    Map map;
    memset(&map, 0, sizeof(Map));

    FILE* f = fopen(path, "r");
    if (!f) {
        game_abort("Can't load map file : %s", path);
    }

    game_log("Using %s to load the map", path);

    fscanf(f, "%d %d", &map.row, &map.col);
    char ch;

    //// Map Array
    //map.map = (uint8_t**)malloc(map.row * sizeof(uint8_t*));
    //map.coin_status = (COIN_STATUS**)malloc(map.row * sizeof(COIN_STATUS*)); // Allocate coin_status
    //map.coins_collected = 0;
    //map.button_status = (BUTTON_STATUS**)malloc(map.row * sizeof(BUTTON_STATUS*));

    //for (int i = 0; i < map.row; i++) {
    //    map.map[i] = (uint8_t*)malloc(map.col * sizeof(uint8_t));
    //    map.coin_status[i] = (COIN_STATUS*)malloc(map.col * sizeof(COIN_STATUS)); // Allocate coin_status row
    //    map.button_status = (BUTTON_STATUS*)malloc(map.row * sizeof(BUTTON_STATUS));
    //}

    //// Map Offset Resource Pack Array
    //map.offset_assets = (Point**)malloc(map.row * sizeof(Point*));
    //for (int i = 0; i < map.row; i++) {
    //    map.offset_assets[i] = (Point*)malloc(map.col * sizeof(Point));
    //}

    //map.total_coins = 0;
    ////int coins_collected = 0;
    //// Allocate memory for the coin_status array
    //map.coin_status = (COIN_STATUS**)malloc(map.row * sizeof(COIN_STATUS*));
    //map.coin_animation_ticks = (int**)malloc(map.row * sizeof(int*));
    //map.coin_disappear_animation_ticks = (int**)malloc(map.row * sizeof(int*));
    //map.button_animation_ticks = (int**)malloc(map.row * sizeof(int*));
    //map.button_status = (BUTTON_STATUS**)malloc(map.row * sizeof(BUTTON_STATUS*));

    //for (int i = 0; i < map.row; i++) {
    //    map.coin_status[i] = (COIN_STATUS*)malloc(map.col * sizeof(COIN_STATUS));
    //    map.coin_animation_ticks[i] = (int*)malloc(map.col * sizeof(int));
    //    map.coin_disappear_animation_ticks[i] = (int*)malloc(map.col * sizeof(int));
    //    map.button_animation_ticks[i] = (int*)malloc(map.col * sizeof(int));
    //    map.button_status = (BUTTON_STATUS*)malloc(map.row * sizeof(BUTTON_STATUS));

    //    for (int j = 0; j < map.col; j++) {
    //        map.coin_status[i][j] = APPEAR; // Default to APPEAR
    //        map.coin_animation_ticks[i][j] = 0;
    //        map.coin_disappear_animation_ticks[i][j] = 0;
    //        map.button_animation_ticks[i][j] = 0;
    //        map.button_status[i][j] = IDLE;
    //    }
    //}

    // Allocate memory for map components
    map.map = (uint8_t**)malloc(map.row * sizeof(uint8_t*));
    map.coin_status = (COIN_STATUS**)malloc(map.row * sizeof(COIN_STATUS*));
    map.button_status = (BUTTON_STATUS**)malloc(map.row * sizeof(BUTTON_STATUS*));
    map.offset_assets = (Point**)malloc(map.row * sizeof(Point*));
    map.coin_animation_ticks = (int**)malloc(map.row * sizeof(int*));
    map.coin_disappear_animation_ticks = (int**)malloc(map.row * sizeof(int*));
    map.button_animation_ticks = (int**)malloc(map.row * sizeof(int*));
    map.hori_door_status = (HORI_DOOR_STATUS**)malloc(map.row * sizeof(HORI_DOOR_STATUS*));
    map.hori_door_animation_ticks = (int**)malloc(map.row * sizeof(int*));
    map.vert_door_status = (VERT_DOOR_STATUS**)malloc(map.row * sizeof(VERT_DOOR_STATUS*));
    map.vert_door_animation_ticks = (int**)malloc(map.row * sizeof(int*));
    map.healing_status = (HEALING_POTION_STATUS**)malloc(map.row * sizeof(HEALING_POTION_STATUS*));
    map.heal_animation_ticks = (int**)malloc(map.row * sizeof(int*));
    map.trophy_status = UNCOLLECTED;
    map.trophy_animation_ticks = 0;

    map.coins_collected = 0;
    map.enemy_killed = 0;
    map.total_coins = 0;

    // Initialize and allocate rows for each component
    for (int i = 0; i < map.row; i++) {
        map.map[i] = (uint8_t*)malloc(map.col * sizeof(uint8_t));
        map.coin_status[i] = (COIN_STATUS*)malloc(map.col * sizeof(COIN_STATUS));
        map.button_status[i] = (BUTTON_STATUS*)malloc(map.col * sizeof(BUTTON_STATUS));
        map.offset_assets[i] = (Point*)malloc(map.col * sizeof(Point));
        map.coin_animation_ticks[i] = (int*)malloc(map.col * sizeof(int));
        map.coin_disappear_animation_ticks[i] = (int*)malloc(map.col * sizeof(int));
        map.button_animation_ticks[i] = (int*)malloc(map.col * sizeof(int));
        map.hori_door_status[i] = (HORI_DOOR_STATUS*)malloc(map.col * sizeof(HORI_DOOR_STATUS));
        map.hori_door_animation_ticks[i] = (int*)malloc(map.col * sizeof(int));
        map.vert_door_status[i] = (VERT_DOOR_STATUS*)malloc(map.col * sizeof(VERT_DOOR_STATUS));
        map.vert_door_animation_ticks[i] = (int*)malloc(map.col * sizeof(int));
        map.healing_status[i] = (HEALING_POTION_STATUS*)malloc(map.col * sizeof(HEALING_POTION_STATUS));
        map.heal_animation_ticks[i] = (int*)malloc(map.col * sizeof(int));

        // Initialize each cell's default values
        for (int j = 0; j < map.col; j++) {
            map.map[i][j] = 0;
            map.coin_status[i][j] = APPEAR;
            map.button_status[i][j] = IDLE;
            map.offset_assets[i][j] = (Point){ 0, 0 };
            map.coin_animation_ticks[i][j] = 0;
            map.coin_disappear_animation_ticks[i][j] = 0;
            map.button_animation_ticks[i][j] = 0;
            map.hori_door_status[i][j] = HORI_IDLE;
            map.hori_door_animation_ticks[i][j] = 0;
            map.vert_door_status[i][j] = VERT_IDLE;
            map.vert_door_animation_ticks[i][j] = 0;
            map.healing_status[i][j] = UNUSED;
            map.heal_animation_ticks[i][j] = 0;
        }
    }


    // Scan the map to the array
    int door_counter = 0;
    for (int i = 0; i < map.row; i++) {
        for (int j = 0; j < map.col; j++) {
            fscanf(f, " %c", &ch);

            switch (ch)
            {
            case '#': // Case Wall
                map.map[i][j] = WALL;
                break;

            case '.': // Case Floor
                map.map[i][j] = FLOOR;
                break;

            case 'P': // Spawn Point
                map.map[i][j] = FLOOR;
                map.Spawn = (Point){ i, j };
                break;

            case 'S': // Slime Enemy
                map.map[i][j] = FLOOR;
                map.EnemyCode[map.EnemySpawnSize] = ch;
                map.EnemySpawn[map.EnemySpawnSize++] = (Point){ i, j };
                break;

            case 'U': //Super Slime
                map.map[i][j] = FLOOR;
                map.EnemyCode[map.EnemySpawnSize] = ch;
                map.EnemySpawn[map.EnemySpawnSize++] = (Point){ i, j };
                break;

            case 'h': //HEALLLL
                map.map[i][j] = HEAL;
                break;

            case 'C': // Coins
                map.map[i][j] = COIN;   
                map.total_coins++;
                map.coin_status[i][j] = APPEAR;
                break;

            case 'B':
                map.map[i][j] = BUTTON;
                map.button_status[i][j] = IDLE;
                break;

            case 'H':
                map.map[i][j] = HORIZONTAL;
                map.hori_door_status[i][j] = HORI_IDLE;
                break;

            case 'V':
                map.map[i][j] = VERTICAL;
                map.vert_door_status[i][j] = VERT_IDLE;
                break;

            case 'T':
                map.map[i][j] = TROPHY;
                break;

            case '_': // Nothing
                map.map[i][j] = HOLE;
                break;

            default: // If not determined, set it as black
                map.map[i][j] = NOTHING;
                break;
            }
        }
    }
    map.assets = al_load_bitmap("Assets/map_packets.png");
    if (!map.assets) {
        game_abort("Can't load map assets");
    }

    map.coin_assets = al_load_bitmap("Assets/coins.png");
    if (!map.coin_assets) {
        game_abort("Can't load coin assets");
    }

    map.button_assets = al_load_bitmap("Assets/Button.png");
    if (!map.button_assets) {
        game_abort("Can't load button assets");
    }

    map.door_assets = al_load_bitmap("Assets/Door.png");
    if (!map.door_assets) {
        game_abort("Can't load door assets");
    }

    map.heal_assets = al_load_bitmap("Assets/heal_potion.png");
    if (!map.door_assets) {
        game_abort("Can't load heal assets");
    }

    map.trophy_assets = al_load_bitmap("Assets/trophy.png");
    if (!map.trophy_assets) {
        game_abort("Can't load tropy assets");
    }
    

    get_map_offset(&map);

    map.coin_audio = al_load_sample("Assets/audio/coins.mp3");
    if (!map.coin_audio) {
        game_abort("Can't load coin audio");
    }

    map.button_audio = al_load_sample("Assets/audio/button.mp3");
    if (!map.coin_audio) {
        game_abort("Can't load coin audio");
    }

    map.lose_audio = al_load_sample("Assets/audio/lose_audio.mp3");
    if (!map.lose_audio) {
        game_log("Failed to load lose audio.");
    }

    map.win_audio = al_load_sample("Assets/audio/win.mp3");
    if (!map.win_audio) {
        game_log("Failed to load win audio.");
    }

    map.heal_audio = al_load_sample("Assets/audio/heal_audio.mp3");
    if (!map.heal_audio) {
        game_log("Failed to load heal audio.");
    }

    fclose(f);

    return map;
}

void draw_coin(Map* map, int i, int j, int dx, int dy) {
    int frame_width = 16, frame_height = 16;
    int frame_index = 0;
    int offset_y = 0;

    if (map->coin_status[i][j] == APPEAR) {
        frame_index = (map->coin_animation_ticks[i][j] / 8) % 8;
        offset_y = 0;
    }
    else if (map->coin_status[i][j] == DISAPPEARING) {
        frame_index = 8-(map->coin_disappear_animation_ticks[i][j] / 4) % 8;
        offset_y = 16;
    }

    al_draw_tinted_scaled_bitmap(
        map->coin_assets,
        al_map_rgb(255, 255, 255),
        frame_index * frame_width, offset_y, frame_width, frame_height,
        dx, dy, TILE_SIZE, TILE_SIZE,
        0
    );
}

void draw_map(Map* map, Point cam) {
    al_clear_to_color(al_map_rgb(24, 20, 37));
    int frame_width = 16;
    int frame_height = 16;
    for (int i = 0; i < map->row; i++) {
        for (int j = 0; j < map->col; j++) {
            int dy = i * TILE_SIZE - cam.y;
            int dx = j * TILE_SIZE - cam.x;

            Point offset_asset = map->offset_assets[i][j];
            al_draw_scaled_bitmap(map->assets, offset_asset.x, offset_asset.y, 16, 16, dx, dy, TILE_SIZE, TILE_SIZE, 0);

            if (map->map[i][j] == COIN) {
                if (map->coin_status[i][j] == APPEAR) {
                    draw_coin(map, i, j, dx, dy);
                }
                else if (map->coin_status[i][j] == DISAPPEARING) {
                    draw_coin(map, i, j, dx, dy);
                }
            }
            if (map->map[i][j] == BUTTON) {
                if (map->button_status[i][j] == IDLE) {
                    al_draw_tinted_scaled_bitmap(map->button_assets, al_map_rgb(255, 255, 255),
                        0, 0, 16, 16,
                        dx, dy, TILE_SIZE, TILE_SIZE,
                        1
                    );
                }
                else if (map->button_status[i][j] == PRESSING) {
                    int frame_width = 16;
                    int frame_height = 16;
                    int row_offset = 0;

                    int frame_index = (map->button_animation_ticks[i][j]/16) % 3;
                    int offset_x = frame_index * frame_width;

                    int flag = 0;

                    al_draw_tinted_scaled_bitmap(map->button_assets,
                        al_map_rgb(255, 255, 255), 
                        offset_x, row_offset,
                        frame_width, frame_height,
                        dx, dy,                
                        TILE_SIZE, TILE_SIZE,     
                        flag                  
                    );
                    //al_draw_scaled_bitmap(map->coin_assets, current_frame * 16, 16, 16, 16, dx, dy, TILE_SIZE, TILE_SIZE, 0);
                }
                else if (map->button_status[i][j] == PRESSED) {
                    al_draw_tinted_scaled_bitmap(map->button_assets, al_map_rgb(255, 255, 255),
                        32, 0, 16, 16, 
                        dx, dy, TILE_SIZE, TILE_SIZE,
                        1 
                    );
                }
            }
            if (map->map[i][j] == HEAL) {
                Point offset_asset = get_floor_offset_assets(map, i, j);
                al_draw_scaled_bitmap(map->assets, offset_asset.x, offset_asset.y, 8, 8, dx, dy, TILE_SIZE, TILE_SIZE, 0);
                if (map->healing_status[i][j] == UNUSED) {
                    int frame_width = 16;
                    int frame_height = 16; 
                    int row_offset = 0;  
                    int frame_index = (map->heal_animation_ticks[i][j] / 8) % 8; 
                    int offset_x = frame_index * frame_width;

                    float scale_factor = 0.7;
                    float small_width = TILE_SIZE * scale_factor;
                    float small_height = TILE_SIZE * scale_factor;

                    float offset_dx = (TILE_SIZE - small_width) / 2;
                    float offset_dy = (TILE_SIZE - small_height) / 2;

                    al_draw_tinted_scaled_bitmap(map->heal_assets,
                        al_map_rgb(255, 255, 255), 
                        offset_x, row_offset,     
                        frame_width, frame_height,  
                        dx + offset_dx, dy + offset_dy, 
                        small_width, small_height,
                        0                      
                    );
                }
                else {
                    Point offset_asset = get_floor_offset_assets(map, i, j);
                    al_draw_scaled_bitmap(map->assets, offset_asset.x, offset_asset.y, 16, 16, dx, dy, TILE_SIZE, TILE_SIZE, 0);
                }
            }
            if (map->map[i][j] == HORIZONTAL || map->map[i][j] == HORI_DOOR_OPEN) {
                Point offset_asset = get_floor_offset_assets(map, i, j);
                al_draw_scaled_bitmap(map->assets, offset_asset.x, offset_asset.y, 16, 16, dx, dy, TILE_SIZE, TILE_SIZE, 0);

                if (map->map[i][j] == HORIZONTAL) {
                    if (map->hori_door_status[i][j] == HORI_IDLE) {
                        al_draw_tinted_scaled_bitmap(map->door_assets, al_map_rgb(255, 255, 255),
                            0, 0, 32, 16,
                            dx, dy, TILE_SIZE, TILE_SIZE,
                            0);
                    }
                    else if (map->hori_door_status[i][j] == HORI_OPENING) {
                        int frame_width = 32;
                        int frame_index = 7-(map->hori_door_animation_ticks[i][j] / 4) % 7;
                        int offset_x = frame_index * frame_width;

                        al_draw_tinted_scaled_bitmap(map->door_assets, al_map_rgb(255, 255, 255),
                            offset_x, 0, frame_width, 16,
                            dx, dy, TILE_SIZE, TILE_SIZE,
                            0);
                    }
                }
                else if (map->map[i][j] == HORI_DOOR_OPEN) {
                    al_draw_tinted_scaled_bitmap(map->door_assets, al_map_rgb(255, 255, 255),
                        192, 0, 16, 16,
                        dx, dy, TILE_SIZE, TILE_SIZE,
                        0);
                }
            }
            if (map->map[i][j] == VERTICAL || map->map[i][j] == VERT_DOOR_OPEN) {
                // Draw the floor tile first
                Point offset_asset = get_floor_offset_assets(map, i, j);
                al_draw_scaled_bitmap(map->assets, offset_asset.x, offset_asset.y, 16, 16, dx, dy, TILE_SIZE, TILE_SIZE, 0);

                // Draw the door on top of the floor
                if (map->map[i][j] == VERTICAL) {
                    if (map->vert_door_status[i][j] == VERT_IDLE) {
                        al_draw_tinted_scaled_bitmap(map->door_assets, al_map_rgb(255, 255, 255),
                            0, 32, 32, 16,
                            dx, dy, TILE_SIZE, TILE_SIZE,
                            0);
                    }
                    else if (map->vert_door_status[i][j] == VERT_OPENING) {
                        int frame_width = 32;
                        int frame_index = 8 - (map->vert_door_animation_ticks[i][j] / 4) % 7;
                        int offset_x = frame_index * frame_width;

                        al_draw_tinted_scaled_bitmap(map->door_assets, al_map_rgb(255, 255, 255),
                            offset_x, 32, frame_width, 16,
                            dx, dy, TILE_SIZE, TILE_SIZE,
                            0);
                    }
                }
                else if (map->map[i][j] == VERT_DOOR_OPEN) {
                    al_draw_tinted_scaled_bitmap(map->door_assets, al_map_rgb(255, 255, 255),
                        192, 32, 16, 16,
                        dx, dy, TILE_SIZE, TILE_SIZE,
                        0);
                }
            }
            if (map->map[i][j] == TROPHY) {
                Point offset_asset = get_floor_offset_assets(map, i, j);
                al_draw_scaled_bitmap(map->assets, offset_asset.x, offset_asset.y, 8, 8, dx, dy, TILE_SIZE, TILE_SIZE, 0);
                if (map->trophy_status == UNCOLLECTED) {
                    int frame_width = 32;
                    int frame_height = 32;

                    int frame_index = (map->trophy_animation_ticks / 8) % 10; 
                    int offset_x = frame_index * frame_width;

                    al_draw_tinted_scaled_bitmap(
                        map->trophy_assets,              
                        al_map_rgb(255, 255, 255),      
                        offset_x, 0,                   
                        frame_width, frame_height,   
                        dx + (TILE_SIZE - (TILE_SIZE * 0.8)) / 2, dy + (TILE_SIZE - (TILE_SIZE * 0.8)) / 2,
                        TILE_SIZE*0.8, TILE_SIZE*0.8,  
                        0                    
                    );
                }
                else {
                    Point offset_asset = get_floor_offset_assets(map, i, j);
                    al_draw_scaled_bitmap(map->assets, offset_asset.x, offset_asset.y, 16, 16, dx, dy, TILE_SIZE, TILE_SIZE, 0);
                }
            }
        }
    }
}


void update_map(Map* map, Point player_coord, int* total_coins) {
    /*
        Hint: To check if it's collide with object in map, you can use tile_collision function
        e.g. to update the coins if you touch it
    */
    // Get player's tile coordinates
    //int player_row = player_coord.y / TILE_SIZE;
    //int player_col = player_coord.x / TILE_SIZE;

    //if (player_row >= 0 && player_row <= map->row && player_col >= 0 && player_col <= map->col) {
    //    if (map->map[player_row][player_col] == COIN &&
    //        map->coin_status[player_row][player_col] == APPEAR) {

    //        // Debug before state change

    //        map->coin_status[player_row][player_col] = DISAPPEARING;
    //        map->coins_collected++;
    //        if (!al_play_sample(map->coin_audio, SFX_VOLUME, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL)) {
    //            game_log("Failed to play coin audio");
    //        }
    //        map->coin_animation_ticks[player_row][player_col] = 32; // Reset ticks
    //        map->coin_disappear_animation_ticks[player_row][player_col] = 32;
    //        // Debug after state change

    //        //map->map[player_row][player_col] = FLOOR;
    //        (*total_coins)++;
    //    }
    //}

    for (int i = 0; i < map->row; i++) {
        for (int j = 0; j < map->col; j++) {
            if (map->map[i][j] == COIN) {
                Point p;
                p.x = j * TILE_SIZE;
                p.y = i * TILE_SIZE;

                if (map->coin_status[i][j] == APPEAR) {
                   
                    map->coin_animation_ticks[i][j]++;
                }

                if (tile_collision(player_coord, p) && map->coin_status[i][j] == APPEAR) {
                    map->coin_status[i][j] = DISAPPEARING;
                    map->coins_collected++;

                    if (!al_play_sample(map->coin_audio, sfx_volume, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL)) {
                        game_log("Failed to play coin audio");
                    }

                    map->coin_disappear_animation_ticks[i][j] = 32;
                    (*total_coins)++;
                }

                if (map->coin_status[i][j] == DISAPPEARING) {
                    map->coin_disappear_animation_ticks[i][j]--;

                    if (map->coin_disappear_animation_ticks[i][j] <= 0) {
                        map->coin_status[i][j] = DISAPPEAR;
                    }
                }
            }
            else if (map->map[i][j] == BUTTON) {
                Point button_pos = { j * TILE_SIZE, i * TILE_SIZE };

                if (tile_collision(player_coord, button_pos) && map->button_status[i][j] == IDLE) {
                    if (!al_play_sample(map->button_audio, sfx_volume, 0.0, 1.5, ALLEGRO_PLAYMODE_ONCE, NULL)) {
                        game_log("Failed to play button audio");
                    }

                    map->button_status[i][j] = PRESSING;
                    map->button_animation_ticks[i][j] = 32; 

                    Point closest_door = { -1, -1 };
                    int min_distance = INT_MAX;
                    int is_horizontal = 0;

                    for (int x = 0; x < map->row; x++) {
                        for (int y = 0; y < map->col; y++) {
                            int dx = j - y;
                            int dy = i - x;
                            int distance_squared = dx * dx + dy * dy;

                            if (distance_squared <= 25) { 
                                if (map->map[x][y] == HORIZONTAL && map->hori_door_status[x][y] == HORI_IDLE) {
                                    map->hori_door_status[x][y] = HORI_OPENING;
                                    map->hori_door_animation_ticks[x][y] = 28;
                                }
                                else if (map->map[x][y] == VERTICAL && map->vert_door_status[x][y] == VERT_IDLE) {
                                    map->vert_door_status[x][y] = VERT_OPENING;
                                    map->vert_door_animation_ticks[x][y] = 28;
                                }
                            }
                        }
                    }

                    if (closest_door.x != -1 && closest_door.y != -1) {
                        if (is_horizontal) {
                            map->hori_door_status[closest_door.x][closest_door.y] = HORI_OPENING;
                            map->hori_door_animation_ticks[closest_door.x][closest_door.y] = 28;
                        }
                        else {
                            map->vert_door_status[closest_door.x][closest_door.y] = VERT_OPENING;
                            map->vert_door_animation_ticks[closest_door.x][closest_door.y] = 28;
                        }
                    }
                }
            }else if (map->map[i][j] == HEAL) {
                Point healing_pos = { j * TILE_SIZE, i * TILE_SIZE }; 
                if (map->healing_status[i][j] == UNUSED) {
                    map->heal_animation_ticks[i][j]++;
                }
                if (tile_collision(player_coord, healing_pos) && map->healing_status[i][j] == UNUSED) {
                    game_log("Healing potion collected at tile (%d, %d)", i, j);

                    map->healing_status[i][j] = USED;

                    player.health += 20;
                    if (player.health > 100) {
                        player.health = 100;
                    }

                    game_log("Player health after healing: %d", player.health);

                    if (map->heal_audio) {
                        if (!al_play_sample(map->heal_audio, sfx_volume, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL)) {
                            game_log("Failed to play lose audio.");
                        }
                    }
                }
            }
            else if (map->map[i][j] == TROPHY) {
                Point trophy_pos = { j * TILE_SIZE, i * TILE_SIZE };
                if (map->trophy_status == UNCOLLECTED) {
                    map->trophy_animation_ticks++;
                }
                if (tile_collision(player_coord, trophy_pos) && map->trophy_status == UNCOLLECTED) {
                    game_log("Trophy collected at tile (%d, %d)", i, j);

                    map->trophy_status = COLLECTED;
                }
            }

            // Button and door animation
            if (map->button_status[i][j] == PRESSING) {
                if (--map->button_animation_ticks[i][j] <= 0) {
                    map->button_status[i][j] = PRESSED;
                }
            }

            if (map->hori_door_status[i][j] == HORI_OPENING) {
                if (--map->hori_door_animation_ticks[i][j] <= 0) {
                    map->hori_door_status[i][j] = HORI_OPENED;
                    map->map[i][j] = HORI_DOOR_OPEN;
                }
            }

            if (map->vert_door_status[i][j] == VERT_OPENING) {
                if (--map->vert_door_animation_ticks[i][j] <= 0) {
                    map->vert_door_status[i][j] = VERT_OPENED;
                    map->map[i][j] = VERT_DOOR_OPEN;
                }
            }

        }
    }
}

void destroy_map(Map* map) {
    for (int i = 0; i < map->row; i++) {
        free(map->map[i]);
        free(map->offset_assets[i]);
        free(map->coin_status[i]);  
        free(map->coin_animation_ticks[i]);
        free(map->coin_disappear_animation_ticks[i]);
        free(map->button_animation_ticks[i]);     
        free(map->button_status[i]);  
        free(map->hori_door_status[i]);
        free(map->hori_door_animation_ticks[i]);
        free(map->vert_door_status[i]);
        free(map->vert_door_animation_ticks[i]);
        free(map->healing_status[i]);
    }

    free(map->map);
    free(map->offset_assets);
    free(map->coin_status);                 
    free(map->coin_animation_ticks);
    free(map->coin_disappear_animation_ticks);    
    free(map->button_animation_ticks);         
    free(map->button_status);                       
    free(map->hori_door_status);
    free(map->hori_door_animation_ticks);
    free(map->vert_door_status);
    free(map->vert_door_animation_ticks);
    free(map->healing_status);

    // Destroy other allocated resources
    al_destroy_bitmap(map->assets);
    al_destroy_bitmap(map->coin_assets);
    al_destroy_sample(map->coin_audio);
    al_destroy_sample(map->button_audio);
}

bool isWalkable(BLOCK_TYPE block) {
    if (block == FLOOR || block == COIN || block == BUTTON || block == HORI_DOOR_OPEN || block == VERT_DOOR_OPEN || block == HEAL || block == TROPHY) return true;
    return false;
}

/*
    DON'T CHANGE CODE BELOW UNLESS YOU ARE UNDERSTAND WELL
    OR WANT TO CHANGE TO DIFFERENT ASSETS
 */
static bool isWall(Map* map, int i, int j);
//static bool isFloorHole(Map * map, int i, int j);
static bool isFloor(Map* map, int i, int j);
static bool isNothing(Map* map, int i, int j);

static Point get_floor_offset_assets(Map* map, int i, int j) {
    bool up = isWall(map, i - 1, j);
    bool left = isWall(map, i, j - 1);
    bool right = isWall(map, i, j + 1);
    bool top_left = isWall(map, i - 1, j - 1);
    bool top_right = isWall(map, i - 1, j + 1);

    if (up && left && right) {
        return (Point) { offset * 12, offset * 3 };
    }
    if (up && left) {
        return (Point) { offset * 11, 0 };
    }
    if (up && right) {
        return (Point) { offset * 13, 0 };
    }
    if (left) {
        if (top_left)
            return (Point) { offset * 11, offset * 1 };
        else
            return (Point) { offset * 15, offset * 1 };

    }
    if (right) {
        if (top_right)
            return (Point) { offset * 13, offset * 1 };
        else
            return (Point) { offset * 14, offset * 1 };
    }
    if (up) {
        return (Point) { offset * 12, 0 };
    }
    if (top_left) {
        return (Point) { offset * 11, offset * 3 };
    }
    if (top_right) {
        return (Point) { offset * 13, offset * 3 };
    }

    return (Point) { 12 * offset, 4 * offset };
}

// Calculate the offset from the source assets
static Point get_wall_offset_assets(Map* map, int i, int j) {
    bool up = isWall(map, i - 1, j);
    bool down = isWall(map, i + 1, j);
    bool left = isWall(map, i, j - 1);
    bool right = isWall(map, i, j + 1);

    if (up && down && left && right) {
        return (Point) { 3 * offset, 5 * offset };
    }
    if (up && down && right) {
        bool left_floor = isFloor(map, i, j - 1) || isHorDoor(map, i, j - 1) || isVerDoor(map, i, j - 1) || isButton(map, i, j - 1) || isHeal(map, i, j - 1) || isCoin(map, i, j - 1) || isTrophy(map,i, j -1);
        bool right_down = isWall(map, i + 1, j + 1);
        bool right_up = isWall(map, i - 1, j + 1);
        if (right_down && right_up)
            return (Point) { 2 * offset, 5 * offset };
        if (left_floor) {
            return (Point) { 1 * offset, 3 * offset };
        } else {
            return (Point) { 4 * offset, 5 * offset };
        }
    }
    if (up && down && left) {
        bool right_floor = isFloor(map, i, j + 1) || isHorDoor(map, i, j + 1) || isVerDoor(map, i, j + 1) || isButton(map, i, j + 1) || isHeal(map, i, j + 1) || isCoin(map, i, j + 1) || isTrophy(map, i, j + 1);
        bool left_down = isWall(map, i + 1, j - 1);
        bool left_up = isWall(map, i - 1, j - 1);
        if (left_down && left_up)
            return (Point) { 4 * offset, 5 * offset };
        if (right_floor) {
            return (Point) { 1 * offset, 3 * offset };
        } else {
            return (Point) { 2 * offset, 5 * offset };
        }
    }
    if (down && right && left) {
        bool down_right_wall = isWall(map, i + 1, j + 1);
        bool down_left_wall = isWall(map, i + 1, j - 1);
        bool down_right = isFloor(map, i + 1, j + 1) || isHorDoor(map, i + 1, j + 1) || isVerDoor(map, i + 1, j + 1) || isButton(map, i + 1, j + 1) || isHeal(map, i + 1, j + 1) || isCoin(map, i + 1, j + 1) || isTrophy(map, i + 1, j + 1);
        bool down_left = isFloor(map, i + 1, j - 1) || isHorDoor(map, i + 1, j - 1) || isVerDoor(map, i + 1, j - 1) || isButton(map, i + 1, j - 1) || isHeal(map, i + 1, j - 1) || isCoin(map, i + 1, j - 1) || isTrophy(map, i + 1, j - 1);
        if (down_right_wall && down_left_wall)
            return (Point) { 3 * offset, 4 * offset };
        if ((down_right ^ down_left) == 0)
            return (Point) { 8 * offset, 5 * offset };
        if (down_right)
            return (Point) { 4 * offset, 4 * offset };
        if (down_left)
            return (Point) { 2 * offset, 4 * offset };
    }
    if (left && right) {
        if (isFloor(map, i + 1, j) || isHorDoor(map, i + 1, j) || isVerDoor(map, i + 1, j) || isButton(map, i + 1, j) || isHeal(map, i + 1, j) || isCoin(map, i + 1, j) || isTrophy(map, i + 1, j))
            return (Point) { 7 * offset, 5 * offset };
        else
            return (Point) { 7 * offset, 7 * offset };
    }
    if (down && up) {
        bool left_ = isFloor(map, i, j - 1) || isHorDoor(map, i, j - 1) || isVerDoor(map, i, j - 1) || isButton(map, i, j - 1) || isHeal(map, i, j - 1) || isCoin(map, i, j - 1) || isTrophy(map, i, j - 1);
        bool right_ = isFloor(map, i, j + 1) || isHorDoor(map, i, j + 1) || isVerDoor(map, i, j + 1) || isButton(map, i, j + 1) || isHeal(map, i, j + 1) || isCoin(map, i, j + 1) || isTrophy(map, i, j + 1);
        if ((left_ ^ right_) == 0)
            return (Point) { 1 * offset, 3 * offset };
        if (left_)
            return (Point) { 2 * offset, 5 * offset };
        return (Point) { 4 * offset, 5 * offset };
    }
    if (down && right) {
        if (isFloor(map, i + 1, j + 1) || isHorDoor(map, i + 1, j + 1) || isVerDoor(map, i + 1, j + 1) || isButton(map, i + 1, j + 1) || isHeal(map, i + 1, j + 1) || isCoin(map, i + 1, j + 1) || isTrophy(map, i + 1, j + 1))
            return (Point) { 4 * offset, 5 * offset };
        else
            return (Point) { 2 * offset, 4 * offset };
    }
    if (down && left) {
        if ((isFloor(map, i, j + 1) || isHorDoor(map, i, j + 1) || isVerDoor(map, i, j + 1) || isButton(map, i, j + 1) || isHeal(map, i, j + 1) || isCoin(map, i, j + 1) || isTrophy(map, i, j + 1)) &&
            (isFloor(map, i + 1, j - 1) || isHorDoor(map, i + 1, j - 1) || isVerDoor(map, i + 1, j - 1) || isButton(map, i + 1, j - 1) || isHeal(map, i + 1, j - 1) || isCoin(map, i + 1, j - 1) || isTrophy(map, i + 1, j - 1)))
            return (Point) { 1 * offset, 2 * offset };
        else if (isFloor(map, i + 1, j - 1) || isHorDoor(map, i + 1, j - 1) || isVerDoor(map, i + 1, j - 1) || isButton(map, i + 1, j - 1) || isHeal(map, i + 1, j - 1) || isCoin(map, i + 1, j - 1) || isTrophy(map, i + 1, j - 1))
            return (Point) { 2 * offset, 5 * offset };
        else
            return (Point) { 4 * offset, 4 * offset };
    }
    if (up && right) {
        if (isFloor(map, i + 1, j) || isHorDoor(map, i + 1, j) || isVerDoor(map, i + 1, j) || isButton(map, i + 1, j) || isHeal(map, i + 1, j) || isCoin(map, i + 1, j) || isTrophy(map, i + 1, j))
            return (Point) { 2 * offset, 6 * offset };
        else
            return (Point) { 5 * offset, 6 * offset };
    }
    if (up && left) {
        if (isFloor(map, i + 1, j) || isHorDoor(map, i + 1, j) || isVerDoor(map, i + 1, j) || isButton(map, i + 1, j) || isHeal(map, i + 1, j) || isCoin(map, i + 1, j) || isTrophy(map, i + 1, j))
            return (Point) { 4 * offset, 6 * offset };
        else
            return (Point) { 6 * offset, 6 * offset };
    }
    if (left || right) {
        if (isFloor(map, i + 1, j) || isHorDoor(map, i + 1, j) || isVerDoor(map, i + 1, j) || isButton(map, i + 1, j) || isHeal(map, i + 1, j) || isCoin(map, i + 1, j) || isTrophy(map, i + 1, j))
            return (Point) { 7 * offset, 5 * offset };
        else
            return (Point) { 7 * offset, 7 * offset };
    }
    if (down) {
        return (Point) { 1 * offset, 2 * offset };
    }
    if (up) {
        return (Point) { 1 * offset, 4 * offset };
    }

    return (Point) { 0, 0 };
}

static Point get_hole_offset_assets(Map* map, int i, int j) {
    bool up = isNothing(map, i - 1, j);
    bool U_Floor = isFloor(map, i - 1, j) || isHorDoor(map, i - 1, j) || isVerDoor(map, i - 1, j) || isButton(map, i - 1, j) || isHeal(map, i - 1, j) || isCoin(map, i - 1, j) || isTrophy(map, i - 1, j);

    if (up) {
        return (Point) { 0, 0 };
    }
    if (U_Floor) {
        bool UL = isFloor(map, i - 1, j - 1) || isHorDoor(map, i - 1, j - 1) || isVerDoor(map, i - 1, j - 1) || isButton(map, i - 1, j - 1) || isHeal(map, i - 1, j - 1) || isCoin(map, i - 1, j - 1) || isTrophy(map, i - 1, j - 1);
        bool UR = isFloor(map, i - 1, j + 1) || isHorDoor(map, i - 1, j + 1) || isVerDoor(map, i - 1, j + 1) || isButton(map, i - 1, j + 1) || isHeal(map, i - 1, j + 1) || isCoin(map, i - 1, j + 1) || isTrophy(map, i - 1, j + 1);

        if (UL && UR) return (Point) { offset * 13, offset * 11 };
        if (UL) return (Point) { offset * 14, offset * 11 };
        if (UR) return (Point) { offset * 12, offset * 11 };
        return (Point) { offset * 10, offset * 12 };
    }
    return (Point) { 0, 0 };
}

static Point get_nothing_offset_assets(Map* map, int i, int j) {
    bool U_Floor = isFloor(map, i - 1, j) || isHorDoor(map, i - 1, j) || isVerDoor(map, i - 1, j) || isButton(map, i - 1, j) || isHeal(map, i - 1, j) || isCoin(map, i - 1, j) || isTrophy(map, i - 1, j);
    if (U_Floor) {
        bool UL = isFloor(map, i - 1, j - 1) || isHorDoor(map, i - 1, j - 1) || isVerDoor(map, i - 1, j - 1) || isButton(map, i - 1, j - 1) || isHeal(map, i - 1, j - 1) || isCoin(map, i - 1, j - 1) || isTrophy(map, i - 1, j - 1);
        bool UR = isFloor(map, i - 1, j + 1) || isHorDoor(map, i - 1, j + 1) || isVerDoor(map, i - 1, j + 1) || isButton(map, i - 1, j + 1) || isHeal(map, i - 1, j + 1) || isCoin(map, i - 1, j + 1) || isTrophy(map, i - 1, j + 1);
        if (UL && UR) return (Point) { offset * 13, offset * 11 };
        if (UL) return (Point) { offset * 14, offset * 11 };
        if (UR) return (Point) { offset * 12, offset * 11 };
        return (Point) { offset * 10, offset * 12 };
    }
    return (Point) { 0, 0 };
}

static void get_map_offset(Map* map) {
    // Calculate offsets for all tiles in the map
    for (int i = 0; i < map->row; i++) {
        for (int j = 0; j < map->col; j++) {
            switch (map->map[i][j]) {
            case WALL:
                // Get wall-specific offset
                map->offset_assets[i][j] = get_wall_offset_assets(map, i, j);
                break;

            case FLOOR:
            case COIN:
                // Use floor offset for floor and coins
                map->offset_assets[i][j] = get_floor_offset_assets(map, i, j);
                break;

            case BUTTON:
                // Use floor offset for buttons
                map->offset_assets[i][j] = get_floor_offset_assets(map, i, j);
                break;

            case HORIZONTAL:
            case HORI_DOOR_OPEN:
                // Use floor offset for horizontal doors
                map->offset_assets[i][j] = get_floor_offset_assets(map, i, j);
                break;

            case VERTICAL:
            case VERT_DOOR_OPEN:
                // Use floor offset for vertical doors
                map->offset_assets[i][j] = get_floor_offset_assets(map, i, j);
                break;

            case HEAL:
                // Use floor offset for healing items
                map->offset_assets[i][j] = get_floor_offset_assets(map, i, j);
                break;

            case HOLE:
                // Get offset for holes
                map->offset_assets[i][j] = get_hole_offset_assets(map, i, j);
                break;

            case TROPHY:
                map->offset_assets[i][j] = get_hole_offset_assets(map, i, j);
                break;

            case NOTHING:
            default:
                // Default to no offset
                map->offset_assets[i][j] = (Point){ 0, 0 };
                break;
            }
        }
    }
}


static bool isWall(Map* map, int i, int j) {
    if (i < 0 || j < 0 || i >= map->row || j >= map->col) return false;
    if (map->map[i][j] == WALL) return true;
    return false;
}

static bool isFloor(Map* map, int i, int j) {
    if (i < 0 || j < 0 || i >= map->row || j >= map->col) return false;
    if (map->map[i][j] == FLOOR) return true;
    return false;
}

static bool isNothing(Map* map, int i, int j) {
    if (i < 0 || j < 0 || i >= map->row || j >= map->col) return true;
    if (map->map[i][j] == NOTHING || map->map[i][j] == HOLE) return true;
    return false;
}

static bool isHorDoor(Map* map, int i, int j) {
    if (i < 0 || j < 0 || i >= map->row || j >= map->col) return true;
    if (map->map[i][j] == HORIZONTAL || map->map[i][j] == HORI_DOOR_OPEN) return true;
    return false;
}

static bool isVerDoor(Map* map, int i, int j) {
    if (i < 0 || j < 0 || i >= map->row || j >= map->col) return true;
    if (map->map[i][j] == VERTICAL || map->map[i][j] == VERT_DOOR_OPEN) return true;
    return false;
}

static bool isButton(Map* map, int i, int j) {
    if (i < 0 || j < 0 || i >= map->row || j >= map->col) return true;
    if (map->map[i][j] == BUTTON) return true;
    return false;
}

static bool isHeal(Map* map, int i, int j) {
    if (i < 0 || j < 0 || i >= map->row || j >= map->col) return true;
    if (map->map[i][j] == HEAL) return true;
    return false;
}

static bool isCoin(Map* map, int i, int j) {
    if (i < 0 || j < 0 || i >= map->row || j >= map->col) return true;
    if (map->map[i][j] == COIN) return true;
    return false;
}

static bool isTrophy(Map* map, int i, int j) {
    if (i < 0 || j < 0 || i >= map->row || j >= map->col) return true;
    if (map->map[i][j] == TROPHY) return true;
    return false;
}


/*
    Collision
 */
static bool tile_collision(Point player, Point tile_coord) {

    if (player.x < tile_coord.x + TILE_SIZE &&
        player.x + TILE_SIZE > tile_coord.x &&
        player.y < tile_coord.y + TILE_SIZE &&
        player.y + TILE_SIZE > tile_coord.y) {
        return true; // Rectangles collide
    }
    else {
        return false; // Rectangles do not collide
    }
}
