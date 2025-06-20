#ifndef map_h
#define map_h

#include <stdint.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

#include "utility.h"


// IF YOU WANT TO INCREASE YOUR MAP SIZE OR ENEMY SIZE MAKE SURE TO INCREASE DEFINE VARIABLE BELOW IF IT EXCEED THE MAXIMUM
#define MAX_MAP_ROW 100
#define MAX_MAP_COL 100
#define MAX_ENEMY_SPAWN 100

/*
    Map Tiles type
    Hint : Some type might need a background, to configure it try to modify "get_map_offset(Map * map)"
*/
typedef enum _BLOCK_TYPE {
    FLOOR,
    WALL,
    DOOR_CLOSE,
    HOLE,
    COIN,
    BUTTON,
    HORIZONTAL,
    HORI_DOOR_OPEN,
    VERTICAL,
    HEAL,
    VERT_DOOR_OPEN,
    TROPHY,
    NOTHING
} BLOCK_TYPE;

typedef enum _BUTTON_STATUS {
    IDLE,
    PRESSING,
    PRESSED
} BUTTON_STATUS;

typedef enum _HORI_DOOR_STATUS {
    HORI_IDLE,
    HORI_OPENING,
    HORI_OPENED
} HORI_DOOR_STATUS;

typedef enum _VERT_DOOR_STATUS {
    VERT_IDLE,
    VERT_OPENING,
    VERT_OPENED
} VERT_DOOR_STATUS;

typedef enum _COIN_STATUS {
    APPEAR,
    DISAPPEARING,
    DISAPPEAR
} COIN_STATUS;

typedef enum _HEALING_POTION_STATUS {
    UNUSED,
    USED
} HEALING_POTION_STATUS;

typedef enum _TROPHY_STATUS {
    UNCOLLECTED,
    COLLECTED
} TROPHY_STATUS;

// Map Struct
typedef struct Map_ {
    uint8_t** map; // Map array
    Point** offset_assets; // Assets

    int row, col;

    // Map assets
    ALLEGRO_BITMAP* assets;
    ALLEGRO_BITMAP* coin_assets;
    ALLEGRO_BITMAP* button_assets;
    ALLEGRO_BITMAP* door_assets;
    ALLEGRO_BITMAP* heal_assets;
    ALLEGRO_BITMAP* trophy_assets;

    // TROPHY Properties
    TROPHY_STATUS trophy_status;
    int trophy_animation_ticks;

    // DOOR Properties
    BUTTON_STATUS** button_status;
    int** button_animation_ticks;
    HORI_DOOR_STATUS** hori_door_status;
    int** hori_door_animation_ticks;
    VERT_DOOR_STATUS** vert_door_status;
    int** vert_door_animation_ticks;

    //Game properties
    double match_time;
    int enemy_killed;


    // Audio
    ALLEGRO_SAMPLE* coin_audio;
    ALLEGRO_SAMPLE* button_audio;
    ALLEGRO_SAMPLE* lose_audio;
    ALLEGRO_SAMPLE* win_audio;
    ALLEGRO_SAMPLE* heal_audio;

    // Coin Properties
    COIN_STATUS** coin_status;
    int** coin_animation_ticks; // Animation progress for disappearing coins
    int** coin_disappear_animation_ticks;
    int total_coins;
    int coins_collected;

    //Healing properties
    HEALING_POTION_STATUS** healing_status;
    int** heal_animation_ticks;

    // Spawn Coordinate
    Point Spawn;
    Point EnemySpawn[MAX_ENEMY_SPAWN];
    char EnemyCode[MAX_ENEMY_SPAWN];
    uint8_t EnemySpawnSize;

} Map;

/*
    MAP FUNCTION
    Feel free to add more if you have some idea or interaction with the map
 */
Map create_map(char* path, uint8_t type); // Create a map based on given file path
void draw_map(Map* map, Point cam); // Draw the map
void update_map(Map* map, Point player_coord, int* total_coins); // Update map : you might want add some parameter here
void destroy_map(Map* map); // Destroy map

bool isWalkable(BLOCK_TYPE block);

#endif /* map_h */
