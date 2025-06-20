#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "player_uid.h"
#include "menu_scene.h"
#include "loading_welcome.h"
#include "utility.h"
#include "auth_scene.h"
#include "UI.h"
#include "game.h"

static Button submitButton;
static Button backButton;
static ALLEGRO_BITMAP* background_image = NULL;
char uidInput[21] = "";
int uidInputLen = 0;
static bool isInvalidUID = false;
static bool switch_key = false;
static char name[50];

bool validate_uid(const char* uid) {
    FILE* file = fopen("Assets/player_uid.txt", "r");
    if (!file) {
        printf("UID file not found.\n");
        return false;
    }

    char file_uid[20];

    while (true) {
        int result = fscanf(file, "%19[^,],%49[^\n]\n", file_uid, name);


        if (result != 2) { 
            if (feof(file)) break;
            printf("Malformed entry in leaderboard file.\n");
            continue; 
        }

        if (strcmp(uid, file_uid) == 0) {
            fclose(file);
            return true;
        }
    }

    fclose(file);
    return false; // UID not found
}

void load_player_data(const char* uid) {
    game_log("Loading player data for UID: %s", uid);
}

static void init(void) {
    submitButton = button_create(SCREEN_W / 2 - 200, 520, 400, 100, "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");
    backButton = button_create(SCREEN_W / 2 - 200, 650, 400, 100, "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");
    background_image = al_load_bitmap("Assets/background_5.png");
    if (!background_image) {
        game_abort("Failed to load background image.");
    }
}

static void update(void) {
    update_button(&submitButton);
    update_button(&backButton);

    if (mouseState.buttons && submitButton.hovered == true && uidInputLen > 0) {
        if (validate_uid(uidInput)) {
            load_player_data(uidInput);
            strcpy_s(player_uid, sizeof(player_uid), uidInput);
            strcpy_s(nameInput, sizeof(nameInput), name);
            change_scene(create_welcome_scene());
        }
        else {
            isInvalidUID = true; 
        }
    }

    if (isInvalidUID && uidInputLen == 0) {
        isInvalidUID = false; 
    }

    ALLEGRO_KEYBOARD_STATE keyboardState;
    al_get_keyboard_state(&keyboardState);

    bool key_pressed = false;

    for (int key = ALLEGRO_KEY_0; key <= ALLEGRO_KEY_9; key++) {
        if (al_key_down(&keyboardState, key) && uidInputLen < 20 && !switch_key) {
            uidInput[uidInputLen++] = '0' + (key - ALLEGRO_KEY_0);
            uidInput[uidInputLen] = '\0';
            key_pressed = true;
            break;
        }
    }

    if (al_key_down(&keyboardState, ALLEGRO_KEY_BACKSPACE) && uidInputLen > 0 && !switch_key) {
        uidInput[--uidInputLen] = '\0';
        key_pressed = true;
    }

    if (key_pressed) {
        switch_key = true;
    }
    else if (!al_key_down(&keyboardState, ALLEGRO_KEY_BACKSPACE) &&
        !(al_key_down(&keyboardState, ALLEGRO_KEY_0) ||
            al_key_down(&keyboardState, ALLEGRO_KEY_1) ||
            al_key_down(&keyboardState, ALLEGRO_KEY_2) ||
            al_key_down(&keyboardState, ALLEGRO_KEY_3) ||
            al_key_down(&keyboardState, ALLEGRO_KEY_4) ||
            al_key_down(&keyboardState, ALLEGRO_KEY_5) ||
            al_key_down(&keyboardState, ALLEGRO_KEY_6) ||
            al_key_down(&keyboardState, ALLEGRO_KEY_7) ||
            al_key_down(&keyboardState, ALLEGRO_KEY_8) ||
            al_key_down(&keyboardState, ALLEGRO_KEY_9))) {
        switch_key = false; 
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
            0
        );
    }

    // Title
    al_draw_text(TITLE_FONT, al_map_rgb(255, 255, 255), SCREEN_W / 2, 100, ALLEGRO_ALIGN_CENTER, "Enter Your UID");
    // Subtitle
    al_draw_text(
        P2_FONT,
        al_map_rgb(180, 180, 180),
        SCREEN_W / 2,
        SCREEN_H / 6 + 60,
        ALLEGRO_ALIGN_CENTER,
        "Enter your UID to access your game data."
    );
    // Input box
    float input_x = SCREEN_W / 2 - 250;
    float input_y = SCREEN_H / 2 - 50;
    float input_w = 500;
    float input_h = 90;

    al_draw_filled_rounded_rectangle(
        input_x, input_y,
        input_x + input_w, input_y + input_h,
        10, 10,
        al_map_rgb(240, 240, 240)
    );

    al_draw_rounded_rectangle(
        input_x, input_y,
        input_x + input_w, input_y + input_h,
        10, 10,
        al_map_rgb(50, 50, 50), 3
    );

    // UID Text
    al_draw_text(P2_FONT, al_map_rgb(0, 0, 0), SCREEN_W / 2, SCREEN_H / 2 - 20, ALLEGRO_ALIGN_CENTER, uidInput);
    draw_button(submitButton);
    al_draw_text(P2_FONT, al_map_rgb(66, 76, 110), SCREEN_W / 2, 520 + 28 + submitButton.hovered * 11, ALLEGRO_ALIGN_CENTER, "NEXT");
    al_draw_text(P2_FONT, al_map_rgb(225, 225, 225), SCREEN_W / 2, 520 + 31 + submitButton.hovered * 11, ALLEGRO_ALIGN_CENTER, "NEXT");
    // Submit Button
    draw_button(backButton);

    al_draw_text(P2_FONT, al_map_rgb(66, 76, 110), SCREEN_W / 2, 650 + 28 + backButton.hovered * 11, ALLEGRO_ALIGN_CENTER, "BACK");
    al_draw_text(P2_FONT, al_map_rgb(225, 225, 225), SCREEN_W / 2, 650 + 31 + backButton.hovered * 11, ALLEGRO_ALIGN_CENTER, "BACK");

    if (isInvalidUID) {
        al_draw_text(
            P2_FONT,
            al_map_rgb(255, 0, 0),
            SCREEN_W / 2,
            SCREEN_H / 2 + 70,
            ALLEGRO_ALIGN_CENTER,
            "Invalid UID. Please try again."
        );
    }
}

static void destroy(void) {
    destroy_button(&submitButton);
    destroy_button(&backButton);
    if (background_image) {
        al_destroy_bitmap(background_image);
    }
}

Scene create_player_uid_scene(void) {
    Scene scene;
    memset(&scene, 0, sizeof(Scene));

    scene.name = "player_uid";
    scene.init = &init;
    scene.update = &update;
    scene.draw = &draw;
    scene.destroy = &destroy;

    return scene;
}
