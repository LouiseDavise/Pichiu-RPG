#include <allegro5/allegro.h>
#include <string.h>
#include "menu_scene.h"
#include "loading_welcome.h"
#include "game_scene.h"
#include "setting_scene.h"
#include "utility.h"
#include "UI.h"
#include "game.h"

typedef struct {
    char uid[20];
    char name[50];
    int score;
    char time[10];
} LeaderboardEntry;

static int timer_countdown;
static LeaderboardEntry filteredEntries[100];
static int filteredCount = 0;
static Button continueButton;

static void load_player_history(const char* file_path, const char* player_uid) {
    FILE* file = fopen(file_path, "r");
    if (!file) {
        printf("Leaderboard file not found.\n");
        return;
    }

    char uid[20], name[50], time[10];
    int score;
    filteredCount = 0;

    while (filteredCount < 100) {
        int result = fscanf(file, "%19[^,],%49[^,],%d,%9s\n", uid, name, &score, time);
        if (result != 4) {
            if (feof(file)) break;
            printf("Malformed leaderboard entry.\n");
            continue;
        }

        if (strcmp(uid, player_uid) == 0) {
            strcpy_s(filteredEntries[filteredCount].uid, sizeof(filteredEntries[filteredCount].uid), uid);
            strcpy_s(filteredEntries[filteredCount].name, sizeof(filteredEntries[filteredCount].name), name);
            filteredEntries[filteredCount].score = score;
            strcpy_s(filteredEntries[filteredCount].time, sizeof(filteredEntries[filteredCount].time), time);
            filteredCount++;
        }
    }

    fclose(file);
}

static void init(void) {
    continueButton = button_create(
        SCREEN_W / 2 - 200,
        650,
        400, 100,
        "Assets/UI_Button.png",
        "Assets/UI_Button_hovered.png"
    );
    load_player_history("Assets/leaderboard.txt", player_uid);
}

static void update(void) {
    update_button(&continueButton);

    if (mouseState.buttons && continueButton.hovered) {
        change_scene(create_menu_scene());
    }
}

static void draw(void) {
    char welcomeText[30];
    snprintf(welcomeText, sizeof(welcomeText), "Welcome back %s!", nameInput);

    al_draw_text(
        TITLE_FONT,
        al_map_rgb(146, 161, 185),
        SCREEN_W / 2,
        50,
        ALLEGRO_ALIGN_CENTER,
        welcomeText
    );
    al_draw_text(
        P2_FONT,
        al_map_rgb(180, 180, 180),
        SCREEN_W / 2,
        120,
        ALLEGRO_ALIGN_CENTER,
        "Take a look at your previous records"
    );
    if (filteredCount == 0) {
        al_draw_text(
            P2_FONT,
            al_map_rgb(255, 0, 0),
            SCREEN_W / 2,
            SCREEN_H / 2,
            ALLEGRO_ALIGN_CENTER,
            "No Record Found"
        );
    }
    else {
        al_draw_text(P2_FONT, al_map_rgb(255, 255, 255), SCREEN_W / 4, 190, ALLEGRO_ALIGN_LEFT, "Name");
        al_draw_text(P2_FONT, al_map_rgb(255, 255, 255), SCREEN_W / 2, 190, ALLEGRO_ALIGN_CENTER, "Score");
        al_draw_text(P2_FONT, al_map_rgb(255, 255, 255), 3 * SCREEN_W / 4, 190, ALLEGRO_ALIGN_RIGHT, "Time");

        // Display player history
        for (int i = 0; i < filteredCount; i++) {
            int y = 240 + i * 40; 
            al_draw_text(P3_FONT, al_map_rgb(200, 200, 200), SCREEN_W / 4, y, ALLEGRO_ALIGN_LEFT, filteredEntries[i].name);
            char scoreText[20];
            snprintf(scoreText, sizeof(scoreText), "%d", filteredEntries[i].score);
            al_draw_text(P3_FONT, al_map_rgb(200, 200, 200), SCREEN_W / 2, y, ALLEGRO_ALIGN_CENTER, scoreText);
            al_draw_text(P3_FONT, al_map_rgb(200, 200, 200), 3 * SCREEN_W / 4, y, ALLEGRO_ALIGN_RIGHT, filteredEntries[i].time);
        }
    }
    draw_button(continueButton);
    al_draw_text(P2_FONT, al_map_rgb(66, 76, 110), SCREEN_W / 2, 650 + 28 + continueButton.hovered * 11, ALLEGRO_ALIGN_CENTER, "CONTINUE");
    al_draw_text(P2_FONT, al_map_rgb(225, 225, 225), SCREEN_W / 2, 650 + 31 + continueButton.hovered * 11, ALLEGRO_ALIGN_CENTER, "CONTINUE");
}

static void destroy(void) {
    destroy_button(&continueButton);
}


Scene create_welcome_scene(void) {
    Scene scene;
    memset(&scene, 0, sizeof(Scene));

    scene.name = "welcome";
    scene.init = &init;
    scene.draw = &draw;
    scene.update = &update;
    scene.destroy = &destroy;

    return scene;
}
