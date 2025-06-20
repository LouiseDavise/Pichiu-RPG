#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include "player_name.h"
#include "menu_scene.h"
#include "utility.h"
#include "UI.h"
#include "game.h"
#include "auth_scene.h"
#include <stdio.h>
#include <string.h>

static Button nextButton;
static Button backButton;
static ALLEGRO_BITMAP* slider_thumb_img;
static ALLEGRO_BITMAP* background_image = NULL;

// Generate UID
unsigned long hash(const char* str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    unsigned long seed = (unsigned long)time(NULL);
    hash += seed;
    return hash;
}

void generate_player_uid(const char* player_name, char* uid_buffer) {
    unsigned long hashed_value = hash(player_name);
    snprintf(uid_buffer, 20, "%lu", hashed_value);
}

void save_player_uid_and_name(const char* uid, const char* player_name) {
    FILE* file = fopen("Assets/player_uid.txt", "a");
    if (file) {
        fprintf(file, "%s,%s\n", uid, player_name);
        fclose(file);
        printf("Player UID and name appended to player_uid.txt\n");
    }
    else {
        printf("Failed to append Player UID and name to file.\n");
    }
}

static void init(void) {
    nextButton = button_create(SCREEN_W / 2 - 200, 520, 400, 100, "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");
    backButton = button_create(SCREEN_W / 2 - 200, 650, 400, 100, "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");
    background_image = al_load_bitmap("Assets/background_5.png");
    if (!background_image) {
        game_abort("Failed to load menu background image.");
    }

}

static void update(void) {
    update_button(&nextButton);
    update_button(&backButton);

    if (mouseState.buttons && nextButton.hovered == true && nameInputLen >= 3) {
        generate_player_uid(nameInput, player_uid);
        printf("Player UID: %s\n", player_uid);
        onEnterNameScene = false;
        save_player_uid_and_name(player_uid, nameInput);
        change_scene(create_menu_scene());
    }
    if (mouseState.buttons && backButton.hovered == true) {
        change_scene(create_auth_scene());
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
        al_map_rgb(240, 240, 240),
        SCREEN_W / 2,
        SCREEN_H / 6,
        ALLEGRO_ALIGN_CENTER,
        "Enter Your Name"
    );

    // Subtitle
    al_draw_text(
        P2_FONT,
        al_map_rgb(180, 180, 180),
        SCREEN_W / 2,
        SCREEN_H / 6 + 60,
        ALLEGRO_ALIGN_CENTER,
        "This will be displayed in the game."
    );

    // Input Box
    float input_x = SCREEN_W / 2 - 250;
    float input_y = SCREEN_H / 2 - 50;
    float input_w = 500;
    float input_h = 90;

    // Background of the input box
    al_draw_filled_rounded_rectangle(
        input_x, input_y,
        input_x + input_w, input_y + input_h,
        10, 10, 
        al_map_rgb(240, 240, 240)
    );

    // Border of the input box
    al_draw_rounded_rectangle(
        input_x, input_y,
        input_x + input_w, input_y + input_h,
        10, 10,
        al_map_rgb(50, 50, 50), 3
    );

    // Placeholder Text
    al_draw_text(
        P2_FONT,
        al_map_rgb(150, 150, 150),
        SCREEN_W / 2,
        SCREEN_H / 2 - 20,
        ALLEGRO_ALIGN_CENTER,
        nameInput
    );

    draw_button(nextButton);
    al_draw_text(P2_FONT, al_map_rgb(66, 76, 110), SCREEN_W / 2, 520 + 28 + nextButton.hovered * 11, ALLEGRO_ALIGN_CENTER, "NEXT");
    al_draw_text(P2_FONT, al_map_rgb(225, 225, 225), SCREEN_W / 2, 520 + 31 + nextButton.hovered * 11, ALLEGRO_ALIGN_CENTER, "NEXT");
    // Submit Button

    draw_button(backButton);
    al_draw_text(P2_FONT, al_map_rgb(66, 76, 110), SCREEN_W / 2, 650 + 28 + backButton.hovered * 11, ALLEGRO_ALIGN_CENTER, "BACK");
    al_draw_text(P2_FONT, al_map_rgb(225, 225, 225), SCREEN_W / 2, 650 + 31 + backButton.hovered * 11, ALLEGRO_ALIGN_CENTER, "BACK");
}

static void destroy(void) {
    destroy_button(&nextButton);
    destroy_button(&backButton);
    if (background_image) {
        al_destroy_bitmap(background_image);
        background_image = NULL;
    }
}

Scene create_player_scene(void) {
    Scene scene;
    memset(&scene, 0, sizeof(Scene));

    scene.name = "player_name";
    scene.init = &init;
    scene.draw = &draw;
    scene.update = &update;
    scene.destroy = &destroy;

    return scene;
}