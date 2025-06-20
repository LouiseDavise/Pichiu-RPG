#include <allegro5/allegro.h>
#include "win_scene.h"
#include "menu_scene.h"
#include "leaderboard_scene.h"
#include "utility.h"
#include "loading_scene.h"
#include "map.h"
#include "player.h"
#include "UI.h"
#include "game.h"

static Button backButton;
static Button nextLevelButton;
Map map;
Player player;
static const char* leaderboard_file = "Assets/leaderboard.txt";
int h, m, s, h2, m2, s2;
char game_time[30];
char time_value[20];
int player_score = 0;
bool saveLeaderboard = false;
static ALLEGRO_BITMAP* background_image = NULL;

void save_to_leaderboard2(const char* uid, char* name, int score, const char* time) {
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
    player_score = 0;
    if (!saveLeaderboard) {
        total_time += map.match_time;
    }
    h = (total_time / 3600);
    m = (total_time - (3600 * h)) / 60;
    s = (total_time - (3600 * h) - (m * 60));
    h2 = (map.match_time / 3600);
    m2 = (map.match_time - (3600 * h2)) / 60;
    s2 = (map.match_time - (3600 * h2) - (m2 * 60));
    snprintf(game_time, sizeof(game_time), "Time Taken: %02d:%02d:%02d", h2, m2, s2);
    snprintf(time_value, sizeof(time_value), "%02d:%02d:%02d", h, m, s);
    backButton = button_create(SCREEN_W / 2 - 200, 650, 400, 100, "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");
    nextLevelButton = button_create(SCREEN_W / 2 - 200, 530, 400, 100, "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");
    status_visited = 2;
    //Calculate Score
    player_score += map.enemy_killed * 500 + map.coins_collected * 250 - map.match_time * 50 + player.health * 2 + level * 200;

    if (!saveLeaderboard) {
        total_score += player_score;
    }
    
    background_image = al_load_bitmap("Assets/background_6.png");
    if (!background_image) {
        game_abort("Failed to load menu background image.");
    }
}

static void update(void) {

    update_button(&backButton);
    if (mouseState.buttons && backButton.hovered == true) {
        
        if (!saveLeaderboard){
            const char* player_name = nameInput;
            const char* time_taken = game_time;
            saveLeaderboard = true;
            save_to_leaderboard2(player_uid, player_name, total_score, time_value);
        }
        change_scene(create_menu_scene());
    }

    update_button(&nextLevelButton);
    if (mouseState.buttons && nextLevelButton.hovered == true) {
        if (level < 3) {
            level++;
            saveLeaderboard = false;
            change_scene(create_loading_scene());
        }
        else {
            if (!saveLeaderboard)
            {
                const char* player_name = nameInput;
                const char* time_taken = game_time;
                saveLeaderboard = true;
                save_to_leaderboard2(player_uid, player_name, total_score, time_value);
            }
            change_scene(create_leaderboard_scene());
        }
        
    }

    /*update_button(&leaderboardButton);
    if (mouseState.buttons && leaderboardButton.hovered == true) {
        if (!saveLeaderboard)
        {
            const char* player_name = nameInput;
            const char* time_taken = game_time;
            total_score += player_score;
            saveLeaderboard = true;
            save_to_leaderboard2(player_name, total_score, time_value);
        }
        change_scene(create_leaderboard_scene());
    }*/
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
        al_map_rgb(255, 223, 0),
        SCREEN_W / 2,
        SCREEN_H / 4 - 50,
        ALLEGRO_ALIGN_CENTER,
        "CONGRATULATIONS!"
    );

    // Subtext
    al_draw_text(
        P2_FONT,
        al_map_rgb(255, 255, 255),
        SCREEN_W / 2,
        SCREEN_H / 2.5 - 70,
        ALLEGRO_ALIGN_CENTER,
        "You've achieved greatness!"
    );

    // Stats
    /*al_draw_text(
        P2_FONT,
        al_map_rgb(200, 200, 200),
        SCREEN_W / 2,
        SCREEN_H / 2 -30,
        ALLEGRO_ALIGN_CENTER,
        "Time Taken: 00:02:34"
    );*/
    //game_log("value => %.d %.d %.d", h, m, s);
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
    snprintf(total_score2, sizeof(total_score2), "Total Score: %d", player_score);
    al_draw_text(
        P2_FONT,
        al_map_rgb(200, 200, 200),
        SCREEN_W / 2,
        SCREEN_H / 2 + 50,
        ALLEGRO_ALIGN_CENTER,
        total_score2
    );

   
    char buttonValue[20];
    snprintf(buttonValue, sizeof(buttonValue), "%s", level < 3 ? "NEXT LEVEL" : "LEADERBOARD");

    draw_button(nextLevelButton);
    al_draw_text(
        P2_FONT,
        al_map_rgb(66, 76, 110),
        SCREEN_W / 2,
        530 + 28 + nextLevelButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        buttonValue
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
    destroy_button(&nextLevelButton);
    al_destroy_bitmap(background_image);
}

Scene create_win_scene(void) {
    Scene scene;
    memset(&scene, 0, sizeof(Scene));

    scene.name = "win";
    scene.init = &init;
    scene.draw = &draw;
    scene.update = &update;
    scene.destroy = &destroy;

    return scene;
}
