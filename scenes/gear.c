/*
 * MIT License
 *
 * Copyright (c) 2021 Davidson Francis <davidsondfgl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "raylib.h"
#include "scenes.h"

/* Textures. */
static Texture2D gear;
static Rectangle rec_gear;
static Rectangle rec_gear_window;
static Rectangle rec_cb_click;
static Vector2   gear_settings_vec;

/* Window and Checkbox current values. */
static bool gear_window = false;
bool cb_rnd_amt_selected = false;

/* Gear settings values. */
#define GEAR_SETTINGS_TXT  "Settings:"
#define GEAR_SETTINGS_SIZE 20
#define GEAR_RND_AMT_TXT   "Random amount of crystals"
#define GEAR_RND_AMT_SIZE  20

/* Gear window values. */
#define GEAR_WINDOW_WIDTH  310
#define GEAR_WINDOW_HEIGHT 80
#define GEAR_WINDOW_X ((SCREEN_WIDTH/2) - (GEAR_WINDOW_WIDTH/2))
#define GEAR_WINDOW_Y ((SCREEN_HEIGHT/2) - (GEAR_WINDOW_HEIGHT/2))
#define GEAR_WINDOW_PADDING_X (GEAR_WINDOW_X + 5)
#define GEAR_WINDOW_PADDING_Y (GEAR_WINDOW_Y + 5)

/* Checkbox button settings. */
#define GEAR_CB_BUTTON_OUT_SIZE 20 /* Checkbox outer size. */
#define GEAR_CB_BUTTON_INN_SIZE 16 /* Checkbox inner size. */

/**
 * Init gear window.
 */
void init_gear(void)
{
	Vector2 rnd_amt_size;

	gear = LoadTexture("resources/gear.png");
	rec_gear.x      = GEAR_X;
	rec_gear.y      = GEAR_Y;
	rec_gear.width  = gear.width;
	rec_gear.height = gear.height;

	rec_gear_window.x      = GEAR_WINDOW_X;
	rec_gear_window.y      = GEAR_WINDOW_Y;
	rec_gear_window.width  = GEAR_WINDOW_WIDTH;
	rec_gear_window.height = GEAR_WINDOW_HEIGHT;

	gear_settings_vec = MeasureTextEx(GetFontDefault(), GEAR_SETTINGS_TXT,
		(float)GEAR_SETTINGS_SIZE, (float)GEAR_SETTINGS_SIZE/10);

	rnd_amt_size = MeasureTextEx(GetFontDefault(), GEAR_RND_AMT_TXT,
		(float)GEAR_RND_AMT_SIZE, (float)GEAR_RND_AMT_SIZE/10);

	rec_cb_click.x      = GEAR_WINDOW_PADDING_X;
	rec_cb_click.y      = GEAR_WINDOW_PADDING_Y + gear_settings_vec.y;
	rec_cb_click.width  = GEAR_CB_BUTTON_OUT_SIZE + 5 + rnd_amt_size.x;
	rec_cb_click.height = GEAR_CB_BUTTON_OUT_SIZE;
}

/**
 * Finish gear window.
 */
void finish_gear(void)
{
	UnloadTexture(gear);
}

/**
 * Gear button logic.
 */
void update_gear_logic(void)
{
	if (IsClick())
	{
		mouse = GetMousePosition();

		if (CheckCollisionPointRec(mouse, rec_gear))
			gear_window = !gear_window;

		else if (gear_window && CheckCollisionPointRec(mouse, rec_cb_click))
			cb_rnd_amt_selected = !cb_rnd_amt_selected;
	}
}

/**
 * Update gear button drawing.
 */
void update_gear_drawing(void)
{
	DrawTexture(gear, GEAR_X, GEAR_Y, WHITE);

	if (!gear_window)
		return;

	DrawRectangleRounded(rec_gear_window, 0.10f, 0, ColorAlpha(BLUE, 0.2f));
	DrawRectangleRoundedLines(rec_gear_window, 0.10f, 0, 1, BLACK);
	DrawText(GEAR_SETTINGS_TXT, GEAR_WINDOW_PADDING_X, GEAR_WINDOW_Y,
		GEAR_SETTINGS_SIZE, BLACK);

	DrawLine(GEAR_WINDOW_X, GEAR_WINDOW_Y + gear_settings_vec.y,
		GEAR_WINDOW_X + GEAR_WINDOW_WIDTH,
		GEAR_WINDOW_Y + gear_settings_vec.y, BLACK);

	DrawRectangleLines(GEAR_WINDOW_PADDING_X, GEAR_WINDOW_PADDING_Y +
		gear_settings_vec.y, GEAR_CB_BUTTON_OUT_SIZE,
		GEAR_CB_BUTTON_OUT_SIZE, BLACK);

	if (cb_rnd_amt_selected)
	{
		DrawRectangle(GEAR_WINDOW_PADDING_X +
			(GEAR_CB_BUTTON_OUT_SIZE-GEAR_CB_BUTTON_INN_SIZE)/2,
			GEAR_WINDOW_PADDING_Y + gear_settings_vec.y +
			(GEAR_CB_BUTTON_OUT_SIZE-GEAR_CB_BUTTON_INN_SIZE)/2,
			GEAR_CB_BUTTON_INN_SIZE, GEAR_CB_BUTTON_INN_SIZE, BLACK);
	}

	DrawText(GEAR_RND_AMT_TXT,
		GEAR_WINDOW_PADDING_X + GEAR_CB_BUTTON_OUT_SIZE + 5,
		GEAR_WINDOW_PADDING_Y + gear_settings_vec.y,
		GEAR_RND_AMT_SIZE, BLACK);
}
