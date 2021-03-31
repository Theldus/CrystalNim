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

#include <stdlib.h>
#include "scenes.h"

#if defined(WEB)
    #include <emscripten/emscripten.h>
#endif

/* Game state. */
int global_state = STATE_TUTORIAL;

/* Inter-state variables. */
Vector2 mouse;

/* Game vars. */
int sticks[MAX_ROWS] = {1, 3, 5, 7};
int sticks_count     = MAX_STICKS;

/* Turn. */
int turn;

/* Images. */
Texture2D back;

/* Program icon. */
Image icon;

/**
 * Draw game title.
 */
static inline void draw_title(void)
{
	DrawText(TITLE, START_X, 0, TITLE_SIZE, BLACK);
	DrawText("by Theldus", START_X + MeasureText(TITLE, TITLE_SIZE) + 10, 30,
		TITLE_BY_SIZE, BLACK);
}

/**
 * Update logic and drawing for each frame
 */
static INLINE void update_frame(void)
{
	/* ----------------------------------------------------------------- */
	/* Update logic                                                      */
	/* ----------------------------------------------------------------- */
	mouse = GetMousePosition();

	switch (global_state)
	{
		case STATE_TUTORIAL:
			update_tutorial_logic();
			break;

		case STATE_INGAME:
			update_ingame_logic();
			break;

		default:
			break;
	}

	/* ----------------------------------------------------------------- */
	/* Update drawing                                                    */
	/* ----------------------------------------------------------------- */
	BeginDrawing();

		ClearBackground(BLACK);
		DrawTexture(back, 0, 0, WHITE);

		/* Draw title. */
		draw_title();

		switch (global_state)
		{
			case STATE_TUTORIAL:
				update_tutorial_drawing();
				break;

			case STATE_INGAME:
				update_ingame_drawing();
				break;

			default:
				break;
		}

	EndDrawing();
}

/**
 * Main game loop.
 */
int main(void)
{
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, TITLE);

#if !defined(WEB)
	SetTargetFPS(FPS);
#endif

	back = LoadTexture("resources/crystals.jpg");
	icon = LoadImage("resources/crystal.png");
	SetWindowIcon(icon);

	init_gear();
	init_tutorial();
	init_ingame();

#if !defined(WEB)
	while (!WindowShouldClose())
		update_frame();
#else
	emscripten_set_main_loop(update_frame, 0, 1);
#endif

	finish_ingame();
	finish_tutorial();
	finish_gear();
	UnloadTexture(back);
	UnloadImage(icon);
	CloseWindow();
}
