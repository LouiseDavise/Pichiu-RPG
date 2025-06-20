#include <allegro5/allegro.h>
#include <string.h>
#include "menu_scene.h"
#include "game_scene.h"
#include "setting_scene.h"
#include "loading_scene.h"
#include "leaderboard_scene.h"
#include "utility.h"
#include "UI.h"
#include "game.h"

static Button settingButton;
static Button playButton;
static Button leaderboardButton;
static ALLEGRO_BITMAP* background_image = NULL;

static void init(void) {
    level = 1;
    total_time = 0;
    total_score = 0;
    background_image = al_load_bitmap("Assets/background.jpg");
    if (!background_image) {
        game_abort("Failed to load menu background image.");
    }
    // Play Button
    playButton = button_create(SCREEN_W / 2 - 200, 325, 400, 100, "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");

    // Leaderboard Button
    leaderboardButton = button_create(SCREEN_W / 2 - 200, 450, 400, 100, "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");

    // Setting Button
    settingButton = button_create(SCREEN_W / 2 - 200, 575, 400, 100, "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");

    status_visited = 0;
}

static void update(void) {
    update_button(&settingButton);
    update_button(&playButton);
    update_button(&leaderboardButton);

    if (mouseState.buttons & 1 && playButton.hovered) {
        change_scene(create_loading_scene());
    }

    /*
        [TODO HACKATHON 4-3]

        Change scene to setting scene when the button is pressed
    */
    if (mouseState.buttons & 1 && leaderboardButton.hovered) {
        change_scene(create_leaderboard_scene());
    }
    if (mouseState.buttons & 1 && settingButton.hovered) {
        change_scene(create_setting_scene());
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

    // Display Player Name and UID
    char player_display[30];
    snprintf(player_display, sizeof(player_display), "Name: %s", nameInput);
    char uid_display[30];
    snprintf(uid_display, sizeof(uid_display), "UID: %s", player_uid);

    al_draw_text(
        P2_FONT,
        al_map_rgb(235, 235, 235),
        15, 20,              
        0,
        player_display
    );
    al_draw_text(
        P3_FONT,
        al_map_rgb(200, 200, 200),
        15, 60,               
        0,
        uid_display
    );

    // Title Text
    al_draw_text(
        TITLE_FONT,
        al_map_rgb(146, 161, 185),
        SCREEN_W / 2,
        SCREEN_H / 5.5, 
        ALLEGRO_ALIGN_CENTER,
        "PICHIU-RPG ADVENTURE"
    );

    draw_button(playButton);
    al_draw_text(
        P2_FONT,
        al_map_rgb(66, 76, 110),
        SCREEN_W / 2,
        325 + 28 + playButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "PLAY"
    );
    al_draw_text(
        P2_FONT,
        al_map_rgb(225, 225, 225),
        SCREEN_W / 2,
        325 + 31 + playButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "PLAY"
    );

    // Leaderboard
    draw_button(leaderboardButton);
    al_draw_text(
        P2_FONT,
        al_map_rgb(66, 76, 110),
        SCREEN_W / 2,
        450 + 28 + leaderboardButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "Leaderboard"
    );
    al_draw_text(
        P2_FONT,
        al_map_rgb(225, 225, 225),
        SCREEN_W / 2,
        450 + 31 + leaderboardButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "Leaderboard"
    );

    // button
    draw_button(settingButton);
    // button text
    al_draw_text(
        P2_FONT,
        al_map_rgb(66, 76, 110),
        SCREEN_W / 2,
        575 + 28 + settingButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "SETTING"
    );
    al_draw_text(
        P2_FONT,
        al_map_rgb(225, 225, 225),
        SCREEN_W / 2,
        575 + 31 + settingButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "SETTING"
    );
}

static void destroy(void) {
    destroy_button(&playButton);
    destroy_button(&leaderboardButton);
    if (background_image) {
        al_destroy_bitmap(background_image);
        background_image = NULL;
    }
}


Scene create_menu_scene(void) {
    Scene scene;
    memset(&scene, 0, sizeof(Scene));

    scene.name = "menu";
    scene.init = &init;
    scene.draw = &draw;
    scene.update = &update;
    scene.destroy = &destroy;

    return scene;
}
