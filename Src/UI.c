#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <string.h>
#include "UI.h"
#include "utility.h"
#include "game.h"

static bool mouse_in_rect(Point mouse, RecArea rect);
static bool mouse_clicked = false;
static bool mouse_was_pressed = false;

Button button_create(int x, int y, int w, int h, const char* default_image_path, const char* hovered_image_path) {

	Button button;
	memset(&button, 0, sizeof(Button));

	button.default_img = al_load_bitmap(default_image_path);
	button.hovered_img = al_load_bitmap(hovered_image_path);

	if (!button.default_img) {
		game_log("failed loading button image %s", default_image_path);
	}
	if (!button.hovered_img) {
		game_log("failed loading button image %s", hovered_image_path);
	}
	button.click_sound = al_load_sample("Assets/audio/button_clicked.mp3");
	if (!button.click_sound) {
		game_log("failed loading button sound");
	}

	button.x = x;
	button.y = y;
	button.w = w;
	button.h = h;

	button.hovered = false;

	return button;
}

void draw_button(Button button) {
	ALLEGRO_BITMAP* _img = button.hovered ? button.hovered_img : button.default_img;
	al_draw_scaled_bitmap(
		_img,
		0, 0,
		al_get_bitmap_width(_img), al_get_bitmap_height(_img),
		button.x, button.y,
		button.w, button.h, 0
	);
}

//void update_button(Button* button) {
//	Point mouse = { mouseState.x, mouseState.y };
//	RecArea rect = { button->x, button->y, button->w, button->h };
//	/*
//		[TODO Hackathon 4-2]
//
//		Using function you completed before,
//		determine the button if it's hovered or not (button->hovered)
//	*/
//	button->hovered = mouse_in_rect(mouse, rect);
//
//}
void update_button(Button* button) {
	Point mouse = { mouseState.x, mouseState.y };
	RecArea rect = { button->x, button->y, button->w, button->h };
	button->hovered = mouse_in_rect(mouse, rect);

	if (mouseState.buttons & 1) {
		if (!mouse_clicked) {
			button->clicked = true;
			mouse_clicked = true;
			if (button->click_sound) {
				al_play_sample(button->click_sound, sfx_volume, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
			}
		}
	}
	else {
		mouse_clicked = false;
		button->clicked = false;
	}
}

void destroy_button(Button* button) {
	al_destroy_bitmap(button->default_img);
	al_destroy_bitmap(button->hovered_img);
	al_destroy_sample(button->click_sound);
}

static bool mouse_in_rect(Point mouse, RecArea rect) {
	/*
		[TODO Hackathon 4-1]

		Return true if mouse is inside the rectangle
	*/
	if (mouse.x >= rect.x && mouse.x <= rect.x + rect.w &&
		mouse.y >= rect.y && mouse.y <= rect.y + rect.h) {
		return true;
	}
	return false;
}
