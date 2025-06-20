#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include "UI.h"
#include "menu_scene.h"
#include "auth_scene.h"
#include "player_uid.h"
#include "player_name.h"
#include "game.h"
#include "utility.h"

static Button newPlayerButton;
static Button oldPlayerButton;
static ALLEGRO_BITMAP* background_image = NULL;

static void init(void) {
    // Initialize buttons
    newPlayerButton = button_create(SCREEN_W / 2 - 200, SCREEN_H / 2 - 50, 400, 100, "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");
    oldPlayerButton = button_create(SCREEN_W / 2 - 200, SCREEN_H / 2 + 90, 400, 100, "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");

    background_image = al_load_bitmap("Assets/background_5.png");
    if (!background_image) {
        game_abort("Failed to load auth scene background image.");
    }
    change_bgm("Assets/audio/menu_bgm.mp3");
}

static void update(void) {
    update_button(&newPlayerButton);
    update_button(&oldPlayerButton);

    //// Handle button clicks
    if (mouseState.buttons && newPlayerButton.hovered == true) {
        change_scene(create_player_scene());
    }

    if (mouseState.buttons && oldPlayerButton.hovered == true) {
        change_scene(create_player_uid_scene());
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

    // Title Text
    al_draw_text(
        TITLE_FONT,
        al_map_rgb(255, 255, 255),
        SCREEN_W / 2,
        SCREEN_H / 4,
        ALLEGRO_ALIGN_CENTER,
        "WELCOME TO PICHIU-RPG"
    );

    // Subtitle
    al_draw_text(
        P2_FONT,
        al_map_rgb(200, 200, 200),
        SCREEN_W / 2,
        SCREEN_H / 4 + 60,
        ALLEGRO_ALIGN_CENTER,
        "Please select your option"
    );

    // Draw buttons
    draw_button(newPlayerButton);
    draw_button(oldPlayerButton);

    // Button labels
    al_draw_text(
        P2_FONT,
        al_map_rgb(66, 76, 110),
        SCREEN_W / 2,
        SCREEN_H / 2 - 20 + newPlayerButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "NEW PLAYER"
    );

    al_draw_text(
        P2_FONT,
        al_map_rgb(66, 76, 110),
        SCREEN_W / 2,
        SCREEN_H / 2 + 120 + oldPlayerButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "OLD PLAYER"
    );
}

static void destroy(void) {
    destroy_button(&newPlayerButton);
    destroy_button(&oldPlayerButton);
    if (background_image) {
        al_destroy_bitmap(background_image);
    }
}

Scene create_auth_scene(void) {
    Scene scene;
    memset(&scene, 0, sizeof(Scene));

    scene.name = "auth";
    scene.init = &init;
    scene.draw = &draw;
    scene.update = &update;
    scene.destroy = &destroy;

    return scene;
}
