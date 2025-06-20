#include <allegro5/allegro.h>
#include "setting_scene.h"
#include "menu_scene.h"
#include "utility.h"
#include "UI.h"
#include "game.h"

static Button backButton;
static ALLEGRO_BITMAP* slider_thumb_img;
static ALLEGRO_BITMAP* background_image = NULL;

static int bgm_slider_x, bgm_slider_y, slider_width, slider_height;
static int sfx_slider_x, sfx_slider_y;
static bool is_dragging_bgm = false;
static bool is_dragging_sfx = false;
static int initial_mouse_x;   
static float initial_bgm_volume;
static float initial_sfx_volume; 


static void init(void) {
    backButton = button_create(SCREEN_W / 2 - 200, 650, 400, 100, "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");
    // Initialize slider positions
    slider_width = 500;
    slider_height = 10;
    bgm_slider_x = SCREEN_W / 2 - slider_width / 2; 
    bgm_slider_y = 300; 
    sfx_slider_x = SCREEN_W / 2 - slider_width / 2; 
    sfx_slider_y = 450; 

    slider_thumb_img = al_load_bitmap("Assets/slider_thumb.png");
    if (!slider_thumb_img) {
        game_abort("Failed to load slider thumb image.");
    }
    background_image = al_load_bitmap("Assets/space_background.jpg");
    if (!background_image) {
        game_abort("Failed to load menu background image.");
    }
}

static void update(void) {
    update_button(&backButton);

    if (mouseState.buttons && backButton.hovered == true) {
        change_scene(create_menu_scene());
    }

    Point mouse = { mouseState.x, mouseState.y };

    if (mouseState.buttons & 1) { 
        if (mouse.y >= bgm_slider_y && mouse.y <= bgm_slider_y + slider_height) { 
            bgm_volume = (float)(mouse.x - bgm_slider_x) / (slider_width);
            if (bgm_volume < 0.0f) bgm_volume = 0.0f; 
            if (bgm_volume > 1.0f) bgm_volume = 1.0f; 
        }

        if (mouse.y >= sfx_slider_y && mouse.y <= sfx_slider_y + slider_height) { 
            sfx_volume = (float)(mouse.x - sfx_slider_x) / (slider_width);
            if (sfx_volume < 0.0f) sfx_volume = 0.0f; 
            if (sfx_volume > 1.0f) sfx_volume = 1.0f; 
        }
    }

    if ((mouseState.buttons & 1) && !is_dragging_sfx) {
        float bgm_thumb_x = bgm_slider_x + slider_width * bgm_volume - 30;
        if (mouse.x >= bgm_thumb_x - 30 && mouse.x <= bgm_thumb_x + 30 &&
            mouse.y >= bgm_slider_y - 30 && mouse.y <= bgm_slider_y + 30) {
            is_dragging_bgm = true;
            initial_mouse_x = mouse.x;
            initial_bgm_volume = bgm_volume;
        }
    }

    if ((mouseState.buttons & 1) && !is_dragging_bgm) {
        float sfx_thumb_x = sfx_slider_x + slider_width * sfx_volume - 30;
        if (mouse.x >= sfx_thumb_x - 30 && mouse.x <= sfx_thumb_x + 30 &&
            mouse.y >= sfx_slider_y - 30 && mouse.y <= sfx_slider_y + 30) {
            is_dragging_sfx = true;
            initial_mouse_x = mouse.x;
            initial_sfx_volume = sfx_volume;
        }
    }

    if (!(mouseState.buttons & 1)) {
        is_dragging_bgm = false;
        is_dragging_sfx = false;
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
    
    // Draw BGM Slider
    al_draw_filled_rectangle(
        bgm_slider_x, bgm_slider_y,
        bgm_slider_x + slider_width, bgm_slider_y + slider_height,
        al_map_rgb(100, 100, 100)
    ); 
    // Track
    al_draw_scaled_bitmap(
        slider_thumb_img,
        0, 0, al_get_bitmap_width(slider_thumb_img), al_get_bitmap_height(slider_thumb_img),
        bgm_slider_x + slider_width * bgm_volume - 30,
        bgm_slider_y + slider_height / 2 - 30,
        60, 60,
        0
    );

    // Draw SFX Slider
    al_draw_filled_rectangle(
        sfx_slider_x, sfx_slider_y,
        sfx_slider_x + slider_width, sfx_slider_y + slider_height,
        al_map_rgb(100, 100, 100)
    ); // Track
    al_draw_scaled_bitmap(
        slider_thumb_img,
        0, 0, al_get_bitmap_width(slider_thumb_img), al_get_bitmap_height(slider_thumb_img),
        sfx_slider_x + slider_width * sfx_volume - 30, // Corrected thumb position
        sfx_slider_y + slider_height / 2 - 30,
        60, 60,
        0
    );

    // Draw Labels
    al_draw_text(P2_FONT, al_map_rgb(255, 255, 255), bgm_slider_x, bgm_slider_y - 50, ALLEGRO_ALIGN_LEFT, "BGM");
    al_draw_text(P2_FONT, al_map_rgb(255, 255, 255), sfx_slider_x, sfx_slider_y - 50, ALLEGRO_ALIGN_LEFT, "SFX");

    // Button
    draw_button(backButton);
    al_draw_text(
        P2_FONT,
        al_map_rgb(66, 76, 110),
        SCREEN_W / 2,
        650 + 28 + backButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "BACK"
    );
    al_draw_text(
        P2_FONT,
        al_map_rgb(225, 225, 225),
        SCREEN_W / 2,
        650 + 31 + backButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "BACK"
    );
}

static void destroy(void) {
    destroy_button(&backButton);
    if (slider_thumb_img) {
        al_destroy_bitmap(slider_thumb_img);
    }
    if (background_image) {
        al_destroy_bitmap(background_image);
        background_image = NULL;
    }
}

Scene create_setting_scene(void) {
    Scene scene;
    memset(&scene, 0, sizeof(Scene));

    scene.name = "setting";
    scene.init = &init;
    scene.draw = &draw;
    scene.update = &update;
    scene.destroy = &destroy;

    return scene;
}
