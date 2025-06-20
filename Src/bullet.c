#include "bullet.h"

#include <math.h>

Bullet create_bullet(char* bullet_path, PointFloat coord, float angle, float speed, float damage, BulletSource source) {
    Bullet bullet;
    bullet.coord = coord;
    bullet.angle = angle;
    bullet.speed = speed;
    bullet.damage = damage;
    bullet.source = source;

    bullet.image = al_load_bitmap(bullet_path);
    if (!bullet.image) {
        exit(1); // Terminate the game safely
    }

    bullet.coord.x += (float)(TILE_SIZE / 2) * cos(bullet.angle);
    bullet.coord.y += (float)(TILE_SIZE / 2) * sin(bullet.angle);

    return bullet;
} 


// Return true if the bullet collides, so it will be deleted from the list
#include <math.h>  // For cos and sin functions

// Function to update the bullet position and check for collisions
bool update_bullet(Bullet* bullet, enemyNode* enemyList, Map* map) {
    bullet->coord.x += bullet->speed * cos(bullet->angle);
    bullet->coord.y += bullet->speed * sin(bullet->angle);

    // Calculate the tile coordinates the bullet is currently in
    int tile_y = (int)(bullet->coord.x / TILE_SIZE);
    int tile_x = (int)(bullet->coord.y / TILE_SIZE);

    // Check if the bullet is out of bounds (off-screen)
    if (tile_x < 0 || tile_x >= map->row || tile_y < 0 || tile_y >= map->col) {
        return true;
    }

    switch (map->map[tile_x][tile_y]) {
    case WALL:
        return true;
    case HOLE:
        return true;
    case HORIZONTAL:
        return true;
    case VERTICAL:
        return true;
    default:
        break;  // No collision with tile
    }

    // Check if the bullet collides with any enemies
    enemyNode* cur = enemyList->next;
    while (cur != NULL) {
        // Get the coordinates of the current enemy
        Point enemyCoord = cur->enemy.coord;

        // Check if the bullet collides with this enemy
        // Bullet and enemy are both TILE_SIZE by TILE_SIZE, so we just check for overlap
        if (bullet->coord.x < enemyCoord.x + TILE_SIZE &&
            bullet->coord.x > enemyCoord.x &&
            bullet->coord.y < enemyCoord.y + TILE_SIZE &&
            bullet->coord.y > enemyCoord.y &&
            bullet->source == PLAYER_BULLET) {

            hitEnemy(&cur->enemy, bullet->damage, bullet->angle);
            return true;
        }

        // Move to the next enemy in the list
        cur = cur->next;
    }

    // Bullet did not collide with anything, continue
    return false;
}


void draw_bullet(Bullet* bullet, Point camera) {
    float scale = TILE_SIZE / 16;
    //al_draw_filled_circle(bullet->coord.x - camera.x, bullet->coord.y - camera.y, scale, al_map_rgb(255, 255, 0));
    al_draw_bitmap(bullet->image, bullet->coord.x - camera.x - 16, bullet->coord.y - camera.y - 16, 0);
}

void destroy_bullet(Bullet* bullet) {
    // In case you are using immage asset for the bullet, free it here
    al_destroy_bitmap(bullet->image);
}

/*
    LINKED LIST IMPLEMENTATION FOR BULLET
    THERE ARE :
        (1) CREATE      : INITIALIZING DUMMY HEAD
        (2) INSERT      : INSERT NEW NODE OF BULLET
        (3) UPDATE      : ITERATE EVERYTHING AND UPDATE THE BULLET
        (4) DRAW        : ITERATE EVERYTHING AND DRAW THE BULLET
        (5) DESTROY     : DESTROY THE LINKED LIST
 */

BulletNode* createBulletList(void) {
    BulletNode* dummyhead = (BulletNode*)malloc(sizeof(BulletNode));
    dummyhead->next = NULL;
    return dummyhead;
}

void insertBulletList(BulletNode* dummyhead, Bullet bullet) {
    BulletNode* tmp = (BulletNode*)malloc(sizeof(BulletNode));
    tmp->bullet = bullet;
    tmp->next = dummyhead->next;
    dummyhead->next = tmp;
}

bool isCollisionWithPlayer(Bullet* bullet, Player* player) {
    // Player bounding box
    int player_left = player->coord.x;
    int player_right = player->coord.x + TILE_SIZE;
    int player_top = player->coord.y;
    int player_bottom = player->coord.y + TILE_SIZE;

    // Bullet position
    int bullet_x = bullet->coord.x;
    int bullet_y = bullet->coord.y;

    // Check if bullet is within player bounds
    return (bullet_x > player_left && bullet_x < player_right &&
        bullet_y > player_top && bullet_y < player_bottom);
}

void updateBulletList(BulletNode* dummyhead, enemyNode* enemyList, Map* map, Player* player) {
    BulletNode* cur = dummyhead->next;
    BulletNode* prev = dummyhead;

    while (cur != NULL) {
        bool shouldDelete = false;

        // Update bullet movement and check collisions with enemies or map
        shouldDelete = update_bullet(&cur->bullet, enemyList, map);

        // Check if the bullet hits the player
        if (!shouldDelete && cur->bullet.source != PLAYER_BULLET && isCollisionWithPlayer(&cur->bullet, player)) {
            hitPlayer(player, (Point) { cur->bullet.coord.x, cur->bullet.coord.y }, cur->bullet.damage, false);
            shouldDelete = true; // Mark bullet for deletion
        }

        // Remove bullet if it needs to be deleted
        if (shouldDelete) {
            prev->next = cur->next;
            destroy_bullet(&cur->bullet);
            free(cur);
            cur = prev;
        }

        prev = cur;
        cur = cur->next;
    }
}

void drawBulletList(BulletNode* dummyhead, Point camera) {
    BulletNode* cur = dummyhead->next;

    while (cur != NULL) {
        draw_bullet(&cur->bullet, camera);
        cur = cur->next;
    }
}

void destroyBulletList(BulletNode* dummyhead) {
    BulletNode* cur = dummyhead->next;
    while (cur != NULL) {
        BulletNode* del = cur;
        cur = cur->next;
        destroy_bullet(&del->bullet);
        free(del);
    }
    free(dummyhead);
}
