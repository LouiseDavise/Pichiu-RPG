#define _USE_MATH_DEFINES
#include "weapon.h"
#include "game.h"
#include "utility.h"
#include "player.h"

#include <math.h>

Weapon create_weapon(char* weapon_path, char* bullet_path, int cooldown, int speed, int damage) {
    Weapon weapon;
    weapon.image = al_load_bitmap(weapon_path);
    if (!weapon.image) {
        game_abort("Failed to load Image %s", weapon_path);
    }

    const char* audio_path = "Assets/audio/shooting.mp3";
    weapon.shooting_audio = al_load_sample(audio_path);
    if (!weapon.shooting_audio) {
        game_abort("Failed to load Audio %s", audio_path);
    }

    weapon.bullet_path = bullet_path;
    weapon.cooldown = cooldown;
    weapon.cooldown_counter = 0;
    weapon.speed = speed;
    weapon.damage = damage;
    game_log("%d %d %d %d %d\n", weapon.bullet_path, weapon.cooldown, weapon.cooldown_counter, weapon.speed, weapon.damage);
    return weapon;
}

void update_weapon(Weapon* weapon, char* weapon_path, char* bullet_path, char* audio_path, int cooldown, int speed, int damage, BulletNode* bulletList, Point playerCoord, Point cam) {
    int cy = playerCoord.y - cam.y + (TILE_SIZE / 2) + 4;
    int cx = playerCoord.x - cam.x + (TILE_SIZE / 2);
    float dX = mouseState.x - cx;
    float dY = mouseState.y - cy;
    weapon->angle = -atan2(dX, dY) + M_PI_2;
    weapon->image = al_load_bitmap(weapon_path);
    weapon->shooting_audio = al_load_sample(audio_path);

    if (mouseState.buttons & 1 && weapon->cooldown_counter == 0) {
        weapon->cooldown_counter = weapon->cooldown;
        PointFloat center = (PointFloat){ playerCoord.x + TILE_SIZE / 2, playerCoord.y + TILE_SIZE / 2 };
        Bullet bullet = create_bullet(bullet_path, center, weapon->angle, speed, damage, PLAYER_BULLET);
        insertBulletList(bulletList, bullet);
        if (!al_play_sample(weapon->shooting_audio, sfx_volume, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL)) {
            game_log("Audio not playing, please increase your RESERVE_SAMPLES variable");
        }
    }
    else if (weapon->cooldown_counter > 0) {
        weapon->cooldown_counter--;
    }
}

void draw_weapon(Weapon* weapon, Point playerCoord, Point cam) {
    int dy = playerCoord.y - cam.y + (TILE_SIZE / 2) + 4;
    int dx = playerCoord.x - cam.x + (TILE_SIZE / 2);
    float scale = TILE_SIZE / 32;

    if (weapon->angle < M_PI_2) {
        al_draw_scaled_rotated_bitmap(weapon->image, 11, 16, dx - TILE_SIZE / 8, dy,
            scale, scale, weapon->angle, 0
        );
    }
    else {
        al_draw_scaled_rotated_bitmap(weapon->image, 11, 16, dx + TILE_SIZE / 8, dy,
            scale, scale, weapon->angle, 2
        );
    }
}


void delete_weapon(Weapon* weapon) {
    al_destroy_bitmap(weapon->image);
    al_destroy_sample(weapon->shooting_audio);
}
