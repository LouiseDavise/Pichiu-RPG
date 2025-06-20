#include "player.h"
#include "game.h"
#include "lose_scene.h"
#include "utility.h"
#include "map.h"
#include <math.h>

static bool isCollision(Player* player, Map* map);
static int weapon_switch_cooldown = 0;

bool previous_switch = false;

Player create_player(char* path, int row, int col) {
    Player player;
    memset(&player, 0, sizeof(player));
    player.hit_timer = 0;

    player.coord = (Point){
        col * TILE_SIZE,
        row * TILE_SIZE
    };

    player.speed = 4;
    player.health = 100;
    player.weapon_idx = 0;

    player.hit_audio = al_load_sample("Assets/audio/player_attacked.mp3");
    if (!player.hit_audio) {
        game_abort("Can't load coin audio");
    }


    player.image = al_load_bitmap(path);

    if (!player.image) {
        game_abort("Error Load Bitmap with path : %s", path);
    }

    player.animation_tick = 0;

    return player;
}

void update_player(Player* player, Map* map) {
    if (player->status == PLAYER_DYING) {
        player->animation_tick++;
        if (player->animation_tick >= MAX_DEATH_ANIMATION_FRAMES) {
            player->status = PLAYER_DEAD;
            game_log("Player is now DEAD.");
        }
        return; 
    }

    if (player->status == PLAYER_DEAD) {
        return;
    }

    Point original = player->coord;

    // Knockback effect
    if (player->knockback_CD > 0) {

        player->knockback_CD--;
        int next_x = player->coord.x + player->speed * cos(player->knockback_angle);
        int next_y = player->coord.y + player->speed * sin(player->knockback_angle);
        Point next;
        next = (Point){ next_x, player->coord.y };

        if (!isCollision(player, map)) {
            player->coord = next;
        }

        next = (Point){ player->coord.x, next_y };
        if (!isCollision(player, map)) {
            player->coord = next;
        }
    }

    /*
        [TODO HACKATHON 1-1]

        Player Movement
        Adjust the movement by player->speed

        if (keyState[ALLEGRO_KEY_W]) {
            player->coord.y = ...
            player->direction = ...
        }
        if (keyState[ALLEGRO_KEY_S]) {
            player->coord.y = ...
            player->direction = ...
        }
    */
    if (keyState[ALLEGRO_KEY_W] && player->health != 0) {
        player->coord.y -= player->speed;
        player->direction = UP;
    }
    if (keyState[ALLEGRO_KEY_S] && player->status != 0) {
        player->coord.y += player->speed;
        player->direction = DOWN;
    }

    // if Collide, snap to the grid to make it pixel perfect
    if (isCollision(player, map)) {
        player->coord.y = round((float)original.y / (float)TILE_SIZE) * TILE_SIZE;
    }

    /*
        [TODO HACKATHON 1-2]

        Player Movement
        Add/Subtract the movement by player->speed

        if (keyState[ALLEGRO_KEY_A]) {
            player->coord.y = ...
            player->direction = ...
        }
        if (keyState[ALLEGRO_KEY_D]) {
            player->coord.y = ...
            player->direction = ...

    }
    */
    if (weapon_switch_cooldown > 0) {
        weapon_switch_cooldown--;
    }
    if (keyState[ALLEGRO_KEY_R] && !previous_switch) {
        player->weapon_idx = (player->weapon_idx == 0) ? 1 : 0;
        game_log("Weapon switched to: %d", player->weapon_idx);
    }
    previous_switch = keyState[ALLEGRO_KEY_R];

    if (keyState[ALLEGRO_KEY_A] && player->health != 0) {
        player->coord.x -= player->speed;
        player->direction = LEFT;
    }
    if (keyState[ALLEGRO_KEY_D] && player->health != 0) {
        player->coord.x += player->speed;
        player->direction = RIGHT;
    }

    if (isCollision(player, map)) {
        player->coord.x = round((float)original.x / (float)TILE_SIZE) * TILE_SIZE;
    }

    /*
        [TODO Homework]

        Calculate the animation tick to draw animation later
    */
    // Handle player status based on movement
    if ((keyState[ALLEGRO_KEY_W] || keyState[ALLEGRO_KEY_S] || keyState[ALLEGRO_KEY_A] || keyState[ALLEGRO_KEY_D])) {
        player->status = PLAYER_WALKING;
    }
    else if (player->status != PLAYER_DYING) {
        player->status = PLAYER_IDLE;
    }
    player->animation_tick = (player->animation_tick + 1) % 64;
}

void draw_player(Player* player, Point cam) {
    int dy = player->coord.y - cam.y;
    int dx = player->coord.x - cam.x;

    int flag = 0;

    /*
        [TODO Homework]

        Draw Animation of Dying, Walking, and Idle
    */
    ALLEGRO_COLOR tint = al_map_rgb(255, 255, 255); 
    if (player->hit_timer > 0) {
        tint = al_map_rgb(255, 0, 0); 
    }
    if (player->status == PLAYER_WALKING) {
        int frame_width = 32; 
        int frame_height = 32;
        int row_offset = 32; 

        int frame_index = (player->animation_tick / 8) % 4;
        int offset_x = frame_index * frame_width;

        int flag = player->direction == LEFT ? 0 : 1;

        al_draw_tinted_scaled_bitmap(player->image,
            tint,
            offset_x, row_offset,      
            frame_width, frame_height,   
            dx, dy,                  
            TILE_SIZE, TILE_SIZE,    
            flag                      
        );
    }
    else if (player->status == PLAYER_IDLE) {
        int frame_width = 32;
        int frame_height = 32;

        al_draw_tinted_scaled_bitmap(player->image,
            tint,
            0, 0,
            frame_width, frame_height,
            dx, dy,
            TILE_SIZE, TILE_SIZE,
            0
        );
    }

    else if (player->status == PLAYER_DYING) {
        int frame_width = 32;
        int frame_height = 32;
        int row_offset = 64; 

        int frame_index = (player->animation_tick / 8) % 4;
        int offset_x = frame_index * frame_width;

        int flag = player->direction == LEFT ? 0 : 1;

        al_draw_tinted_scaled_bitmap(player->image,
            al_map_rgb(255, 255, 255), 
            offset_x, row_offset,     
            frame_width, frame_height, 
            dx, dy,                   
            TILE_SIZE, TILE_SIZE,  
            flag                   
        );
    }
    if (player->hit_timer > 0) {
        player->hit_timer--;
    }

#ifdef DRAW_HITBOX
    al_draw_rectangle(
        dx, dy, dx + TILE_SIZE, dy + TILE_SIZE,
        al_map_rgb(255, 0, 0), 1
    );
#endif

}

void delete_player(Player* player) {
    al_destroy_bitmap(player->image);
}

static bool isCollision(Player* player, Map* map) {
    int top_left_tile_x = (int)(player->coord.x / TILE_SIZE);
    int top_left_tile_y = (int)(player->coord.y / TILE_SIZE);

    int top_right_tile_x = (int)((player->coord.x + TILE_SIZE - 1) / TILE_SIZE);
    int top_right_tile_y = (int)(player->coord.y / TILE_SIZE);

    int bottom_left_tile_x = (int)(player->coord.x / TILE_SIZE);
    int bottom_left_tile_y = (int)((player->coord.y + TILE_SIZE - 1) / TILE_SIZE);

    int bottom_right_tile_x = (int)((player->coord.x + TILE_SIZE - 1) / TILE_SIZE);
    int bottom_right_tile_y = (int)((player->coord.y + TILE_SIZE - 1) / TILE_SIZE);

    if (player->coord.x < 0 ||
        player->coord.y < 0 ||
        (player->coord.x + TILE_SIZE - 1) / TILE_SIZE >= map->col ||
        (player->coord.y + TILE_SIZE - 1) / TILE_SIZE >= map->row)
        return true;

    /*

        [TODO HACKATHON 2-1]

        Check every corner of enemy if it's collide or not

        We check every corner if it's collide with the wall/hole or not
        You can activate DRAW_HITBOX in utility.h to see how the collision work

        if(!isWalkable(map->map[...][...])) return true;
        if(!isWalkable(map->map[...][...])) return true;
        if(!isWalkable(map->map[...][...])) return true;
        if(!isWalkable(map->map[...][...])) return true;

    */
    if (!isWalkable(map->map[top_left_tile_y][top_left_tile_x])) return true; 
    if (!isWalkable(map->map[top_right_tile_y][top_right_tile_x])) return true; 
    if (!isWalkable(map->map[bottom_left_tile_y][bottom_left_tile_x])) return true;
    if (!isWalkable(map->map[bottom_right_tile_y][bottom_right_tile_x])) return true;
    return false;
}

void hitPlayer(Player* player, Point enemy_coord, int damage, bool apply_knockback) {
    if (apply_knockback && player->knockback_CD == 0) {
        float dY = player->coord.y - enemy_coord.y;
        float dX = player->coord.x - enemy_coord.x;
        float angle = atan2(dY, dX);

        /*
            [TODO Homework]

            Game Logic when the player get hit or die

            player->knockback_angle = angle;
            player->knockback_CD = 32;

            player->health = ...
            if(player->health <= 0){

                player->health = 0;
                player->status = ...

                ...
            }
        */
        player->knockback_angle = angle;
        player->knockback_CD = 32;
    }
    if (player->hit_audio) {
        al_play_sample(player->hit_audio, sfx_volume, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
    }
    player->health -= damage;
    player->hit_timer = 30;
    if (player->health <= 0) {
        player->health = 0;
        player->status = PLAYER_DYING;
        game_log("Player has died.");
    }
    else {
        game_log("Player hit! Remaining health: %d", player->health);
    }
}

void shootPlayer(Player* player, int damage) {
    if (player->knockback_CD == 0) {
        player->health -= damage;
        player->hit_timer = 30;
        if (player->health <= 0) {
            player->health = 0;
            player->status = PLAYER_DYING;
            game_log("Player has died.");
        }
        else {
            game_log("Player hit! Remaining health: %d", player->health);
        }
    }
}