#include <allegro5/allegro.h>
#include "lose_scene.h"
#include "menu_scene.h"
#include "leaderboard_scene.h"
#include "loading_scene.h"
#include "utility.h"
#include "UI.h"
#include "game.h"
#include "player.h"
#include "map.h"

static Button backButton;
static Button leaderboardButton;
Map map;
Player player;
static const char* leaderboard_file = "Assets/leaderboard.txt";
int h, m, s, h2, m2, s2;
char game_time[30];
char time_value[20];
int player_score2 = 0;
static ALLEGRO_BITMAP* background_image = NULL;

void save_to_leaderboard(const char* uid, char* name, int score, const char* time) {
    FILE* file = fopen(leaderboard_file, "a");
    if (!file) {
        printf("Failed to open leaderboard file for writing.\n");
        return;
    }

    fprintf(file, "%s,%s,%d,%s\n", uid, name, score, time);
    fclose(file);
    printf("Saved %s to the leaderboard file.\n", name);
}

static void init(void) {
    // Stats
    total_time += map.match_time;
    h = (total_time / 3600);
    m = (total_time - (3600 * h)) / 60;
    s = (total_time - (3600 * h) - (m * 60));
    h2 = (map.match_time / 3600);
    m2 = (map.match_time - (3600 * h2)) / 60;
    s2 = (map.match_time - (3600 * h2) - (m2 * 60));
    snprintf(game_time, sizeof(game_time), "Time Taken: %02d:%02d:%02d", h2, m2, s2);
    snprintf(time_value, sizeof(time_value), "%02d:%02d:%02d", h, m, s);


    backButton = button_create(SCREEN_W / 2 - 200, 650, 400, 100, "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");
    leaderboardButton = button_create(SCREEN_W / 2 - 200, 530, 400, 100, "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");
    status_visited = 1;

    // Calculate Score
    player_score2 += map.enemy_killed * 500 + map.coins_collected * 250 - map.match_time * 50 + player.health * 2;
    player_score2 = player_score2 < 0 ? 0 : player_score2;

    background_image = al_load_bitmap("Assets/background_6.png");
    if (!background_image) {
        game_abort("Failed to load menu background image.");
    }
}

static void update(void) {

    update_button(&backButton);

    if (mouseState.buttons && backButton.hovered == true) {
        const char* player_name = nameInput;
        const char* time_taken = game_time;
        total_score += player_score2;
        save_to_leaderboard(player_uid, player_name, total_score, time_value);
        change_scene(create_menu_scene());
    }

    update_button(&leaderboardButton);

    if (mouseState.buttons && leaderboardButton.hovered == true) {
        change_scene(create_loading_scene());
    }
}

static void draw(void) {
    if (background_image) {
        al_draw_scaled_bitmap(background_image,
            0, 0,
            al_get_bitmap_width(background_image), al_get_bitmap_height(background_image),
            0, 0,
            SCREEN_W, SCREEN_H,
            0);
    }

    // Title
    al_draw_text(
        TITLE_FONT,
        al_map_rgb(200, 50, 50),
        SCREEN_W / 2,
        SCREEN_H / 4 - 50,
        ALLEGRO_ALIGN_CENTER,
        "GAME OVER"
    );

    // Subtext
    al_draw_text(
        P2_FONT,
        al_map_rgb(255, 255, 255),
        SCREEN_W / 2,
        SCREEN_H / 2.5 - 70,
        ALLEGRO_ALIGN_CENTER,
        "Don't give up, try again!"
    );
    al_draw_text(
        P2_FONT,
        al_map_rgb(255, 255, 255),
        SCREEN_W / 2,
        SCREEN_H / 2 - 100,
        ALLEGRO_ALIGN_CENTER,
        game_time
    );
    char coin_collected[30];
    snprintf(coin_collected, sizeof(coin_collected), "Coin Collected: %02d", map.coins_collected);
    
    // Add Coins Collected
    al_draw_text(
        P2_FONT,
        al_map_rgb(200, 200, 200),
        SCREEN_W / 2,
        SCREEN_H / 2 - 50,
        ALLEGRO_ALIGN_CENTER,
        coin_collected
    );

    char enemy_killed[30];
    snprintf(enemy_killed, sizeof(enemy_killed), "Enemy Defeated: %d", map.enemy_killed);

    al_draw_text(
        P2_FONT,
        al_map_rgb(200, 200, 200),
        SCREEN_W / 2,
        SCREEN_H / 2 + 0,
        ALLEGRO_ALIGN_CENTER,
        enemy_killed
    );

    char total_score2[30];
    snprintf(total_score2, sizeof(total_score2), "Total Score: %d", player_score2);
    al_draw_text(
        P2_FONT,
        al_map_rgb(200, 200, 200),
        SCREEN_W / 2,
        SCREEN_H / 2 + 50,
        ALLEGRO_ALIGN_CENTER,
        total_score2
    );


    // Leaderboard Button
    draw_button(leaderboardButton);

    // Leaderboard Text
    al_draw_text(
        P2_FONT,
        al_map_rgb(66, 76, 110),
        SCREEN_W / 2,
        530 + 28 + leaderboardButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "TRY AGAIN"
    );

    // Back Button
    draw_button(backButton);

    // Button Text
    al_draw_text(
        P2_FONT,
        al_map_rgb(66, 76, 110),
        SCREEN_W / 2,
        650 + 28 + backButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "BACK"
    );
}

static void destroy(void) {
    destroy_button(&backButton);
    if (background_image) {
        al_destroy_bitmap(background_image);
        background_image = NULL;
    }
}

Scene create_lose_scene(void) {
    Scene scene;
    memset(&scene, 0, sizeof(Scene));

    scene.name = "lose";
    scene.init = &init;
    scene.draw = &draw;
    scene.update = &update;
    scene.destroy = &destroy;

    return scene;
}
 