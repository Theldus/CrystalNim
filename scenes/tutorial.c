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
#include "raylib.h"
#include "scenes.h"

/* Tutorial global vars. */
static Texture2D pc;
static Texture2D user;
static Rectangle  rec_pc;
static Rectangle  rec_user;
static Rectangle *rec_sel;

/**
 * Load tutorial resources.
 */
void init_tutorial(void)
{
	pc   = LoadTexture("resources/monitor.png");
	user = LoadTexture("resources/user.png");

	rec_pc.x      = TUTORIAL_PC_X;
	rec_pc.y      = TUTORIAL_PC_Y;
	rec_pc.width  = pc.width;
	rec_pc.height = pc.height;

	rec_user.x      = TUTORIAL_USER_X;
	rec_user.y      = TUTORIAL_USER_Y;
	rec_user.width  = user.width;
	rec_user.height = user.height;
}

/**
 * Free tutorial resources.
 */
void finish_tutorial(void)
{
	UnloadTexture(pc);
	UnloadTexture(user);
}

/**
 * Update game logic.
 */
void update_tutorial_logic(void)
{
	/* Which one starts. */
	if (CheckCollisionPointRec(mouse, rec_pc))
		rec_sel = &rec_pc;
	else if (CheckCollisionPointRec(mouse, rec_user))
		rec_sel = &rec_user;
	else
		rec_sel = NULL;

	/* If mouse click, starts game play */
	if (IsClick() && rec_sel)
	{
		global_state = STATE_INGAME;
		if (rec_sel == &rec_user)
			turn = PLAYER_TURN;
		else
			turn = COMPUTER_TURN;
	}
}

/**
 * Update game drawing, i.e: draws a single frame.
 */
void update_tutorial_drawing(void)
{
	/* Draw rules. */
	DrawText("The NIM game consists of removing the sticks from the "
		"table, the amount\nyou want, a single row per time. The last "
		"to remove the sticks, LOSES!!\nGood Luck!!!", START_X,
		TUTORIAL_START_Y, TUTORIAL_SIZE, BLACK);


	DrawText("Who starts?:", START_X, TUTORIAL_START_Y + 100, TUTORIAL_SIZE,
		BLACK);
	DrawTexture(pc,   TUTORIAL_PC_X, TUTORIAL_PC_Y, WHITE);
	DrawTexture(user, TUTORIAL_USER_X, TUTORIAL_USER_Y, WHITE);

	if (rec_sel)
	{
		DrawRectangleRec(*rec_sel, ColorAlpha(LIGHT_BLUE, 0.6f));
		DrawLineEx(
			((Vector2){.x=rec_sel->x, .y=rec_sel->y + rec_sel->height}),
			((Vector2){.x=rec_sel->x + rec_sel->width, .y=rec_sel->y +
				rec_sel->height}), 2, BLUE);
	}
}
