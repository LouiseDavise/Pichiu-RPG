#include <allegro5/allegro.h>
#include <stdio.h>
#include "leaderboard_scene.h"
#include "menu_scene.h"
#include "utility.h"
#include "lose_scene.h"
#include "win_scene.h"
#include "UI.h"
#include "game.h"

static Button backButton;
static Button prevButton;
static Button nextButton;
static ALLEGRO_BITMAP* slider_thumb_img;
static ALLEGRO_BITMAP* background_image = NULL;
static bool previous_switch = false;

typedef struct {
    char uid[20];
    char name[50];
    int score;
    char time[10];
} LeaderboardEntry;

static LeaderboardEntry leaderboard[100];
static int leaderboard_count = 0;
static const char* leaderboard_file = "Assets/leaderboard.txt";

static int current_page = 0;
static const int entries_per_page = 5;
static int total_pages = 0;

int load_leaderboard(const char* file_path, LeaderboardEntry* leaderboard) {
    FILE* file = fopen(file_path, "r");
    if (!file) {
        printf("Leaderboard file not found. Starting with an empty leaderboard.\n");
        return 0;
    }
    int count = 0;
    while (count < 100) {
        int result = fscanf(file, "%19[^,],%49[^,],%d,%9s\n",
            leaderboard[count].uid,
            leaderboard[count].name,
            &leaderboard[count].score,
            leaderboard[count].time);
        if (result != 4) {
            if (feof(file)) break;
            printf("Malformed entry in leaderboard file.\n");
            continue;
        }
        count++;
    }
    fclose(file);
    return count;
}

void sort_leaderboard(LeaderboardEntry* leaderboard, int size) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (leaderboard[j].score < leaderboard[j + 1].score) {
                // Swap entries
                LeaderboardEntry temp = leaderboard[j];
                leaderboard[j] = leaderboard[j + 1];
                leaderboard[j + 1] = temp;
            }
        }
    }
}

static void init(void) {
    background_image = al_load_bitmap("Assets/background_2.png");
    if (!background_image) {
        game_abort("Failed to load menu background image.");
    }

    current_page = 0;
    game_log("tessss2");
    nextButton = button_create(SCREEN_W / 2 + 230, 615, 150, 70, "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");
    prevButton = button_create(SCREEN_W / 2 - 380, 615, 150, 70, "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");
    backButton = button_create(SCREEN_W / 2 - 200, 600, 400, 100, "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");

    slider_thumb_img = al_load_bitmap("Assets/slider_thumb.png");
    if (!slider_thumb_img) {
        game_abort("Failed to load slider thumb image.");
    }
    leaderboard_count = load_leaderboard(leaderboard_file, leaderboard);

    sort_leaderboard(leaderboard, leaderboard_count);
    total_pages = (leaderboard_count + entries_per_page - 1) / entries_per_page;
}

static void update(void) {
    update_button(&nextButton);
    update_button(&prevButton);
    update_button(&backButton);

    if (mouseState.buttons && backButton.hovered == true) {
        if (status_visited == 0) {
            change_scene(create_menu_scene());
        }
        else if (status_visited == 1) {
            change_scene(create_lose_scene());
        }
        else if (status_visited == 2) {
            change_scene(create_win_scene());
        }
    }
    // Handle Prev Button
    if ((mouseState.buttons && prevButton.hovered && current_page > 0) && !previous_switch) {
        current_page--;
    }

    // Handle Next Button
    if ((mouseState.buttons && nextButton.hovered && current_page < total_pages - 1) && !previous_switch) {
        current_page++;
    }
    previous_switch = mouseState.buttons;

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

   
    float box_x = 50;
    float box_y = SCREEN_H / 8 - 30;
    float box_width = SCREEN_W - 100; 
    float box_height = SCREEN_H / 1.6;
    al_draw_filled_rectangle(
        box_x,
        box_y,
        box_x + box_width,
        box_y + box_height,
        al_map_rgb(30, 30, 60)
    );

    al_draw_rectangle(
        box_x,
        box_y,
        box_x + box_width,
        box_y + box_height,
        al_map_rgb(255, 223, 0),
        4
    );

    // Title
    al_draw_text(
        TITLE_FONT,
        al_map_rgb(255, 255, 255),
        SCREEN_W / 2,
        box_y + 50,
        ALLEGRO_ALIGN_CENTER,
        "LEADERBOARD"
    );

    float col_rank = box_x + 60;
    float col_name = box_x + box_width / 3 + 20;
    float col_score = box_x + (box_width / 3) * 2 - 20;
    float col_time = box_x + box_width - 90;

    // Draw Column Headers
    al_draw_text(
        P2_FONT,
        al_map_rgb(180, 180, 180),
        col_rank,
        box_y + 130,
        ALLEGRO_ALIGN_CENTER,
        "Rank"
    );
    al_draw_text(
        P2_FONT,
        al_map_rgb(180, 180, 180),
        col_name,
        box_y + 130,
        ALLEGRO_ALIGN_CENTER,
        "Player Name"
    );
    al_draw_text(
        P2_FONT,
        al_map_rgb(180, 180, 180),
        col_score,
        box_y + 130,
        ALLEGRO_ALIGN_CENTER,
        "Score"
    );
    al_draw_text(
        P2_FONT,
        al_map_rgb(180, 180, 180),
        col_time,
        box_y + 130,
        ALLEGRO_ALIGN_CENTER,
        "Time"
    );

    // Line
    al_draw_line(
        box_x + 10,
        box_y + 170,
        box_x + box_width - 10,
        box_y + 170,
        al_map_rgb(255, 255, 255),
        2
    );
    int start_idx = current_page * entries_per_page;
    int end_idx = start_idx + entries_per_page;
    if (end_idx > leaderboard_count) {
        end_idx = leaderboard_count;
    }

    for (int i = start_idx; i < end_idx; i++) {
        float row_y = box_y + 200 + (i - start_idx) * 50;

        bool is_current_player = strcmp(leaderboard[i].uid, player_uid) == 0;
        ALLEGRO_COLOR entry_color = is_current_player ? al_map_rgb(255, 223, 0) : al_map_rgb(255, 255, 255);

        char rank[5];
        snprintf(rank, sizeof(rank), "%d", i + 1);
        al_draw_text(P2_FONT, entry_color, col_rank, row_y, ALLEGRO_ALIGN_CENTER, rank);

        // Name
        al_draw_text(P2_FONT, entry_color, col_name, row_y, ALLEGRO_ALIGN_CENTER, leaderboard[i].name);

        // Score
        char score[10];
        snprintf(score, sizeof(score), "%d", leaderboard[i].score);
        al_draw_text(P2_FONT, entry_color, col_score, row_y, ALLEGRO_ALIGN_CENTER, score);

        // Time
        al_draw_text(P2_FONT, entry_color, col_time, row_y, ALLEGRO_ALIGN_CENTER, leaderboard[i].time);
    }
    // Prev Button

    draw_button(prevButton);
    al_draw_text(
        P2_FONT,
        al_map_rgb(66, 76, 110),
        prevButton.x + prevButton.w / 2,
        prevButton.y + 15 + prevButton.hovered * 3,
        ALLEGRO_ALIGN_CENTER,
        "Prev"
    );
    al_draw_text(
        P2_FONT,
        al_map_rgb(225, 225, 225),
        prevButton.x + prevButton.w / 2,
        prevButton.y + 19 + prevButton.hovered * 3,
        ALLEGRO_ALIGN_CENTER,
        "Prev"
    );

    // Next Button
    draw_button(nextButton);
    al_draw_text(
        P2_FONT,
        al_map_rgb(66, 76, 110),
        nextButton.x + nextButton.w / 2,
        nextButton.y + 15 + nextButton.hovered * 3,
        ALLEGRO_ALIGN_CENTER,
        "Next"
    );
    al_draw_text(
        P2_FONT,
        al_map_rgb(225, 225, 225),
        nextButton.x + nextButton.w / 2,
        nextButton.y + 19 + nextButton.hovered * 3,
        ALLEGRO_ALIGN_CENTER,
        "Next"
    );

    // Back Button
    draw_button(backButton);
    al_draw_text(
        P2_FONT,
        al_map_rgb(66, 76, 110),
        SCREEN_W / 2,
        600 + 28 + backButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "BACK"
    );
    al_draw_text(
        P2_FONT,
        al_map_rgb(225, 225, 225),
        SCREEN_W / 2,
        600 + 31 + backButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "BACK"
    );
}

static void destroy(void) {
    destroy_button(&nextButton);
    destroy_button(&prevButton);
    destroy_button(&backButton);
    if (slider_thumb_img) {
        al_destroy_bitmap(slider_thumb_img);
        slider_thumb_img = NULL;
    }
    if (background_image) {
        al_destroy_bitmap(background_image);
        background_image = NULL;
    }
}

Scene create_leaderboard_scene(void) {
    Scene scene;
    memset(&scene, 0, sizeof(Scene));

    scene.name = "leaderboard";
    scene.init = &init;
    scene.draw = &draw;
    scene.update = &update;
    scene.destroy = &destroy;

    return scene;
}
