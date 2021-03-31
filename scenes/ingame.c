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

#include <assert.h>
#include <stdlib.h>
#include "raylib.h"
#include "scenes.h"

/* In-game states. */
#define S_DEFAULT          0
#define S_REMOVING_PIECE   1
#define S_PIECE_SHIFTING   2
#define S_CONFIRM_REMOVE   4
#define S_FADE_END         8
#define S_FADE_PLAY_AGAIN 16
static int state;

/* Frame-management + animations. */
static int frame_counter;
static float alpha = 0.0f;
static float alpha_again = 0.0f;
static float alpha_inc = 0.0f;
static int move_start_x;
static int move_step;

/* In-game global vars. */
static Texture2D crystal;
static Texture2D accept;
static Texture2D deny;
static int crystal_idx = -1;

/* Texture sizes. */
#define CRYSTAL_WIDTH    (70)
#define CRYSTAL_HEIGHT  (110)
#define CB_ACCEPT_WIDTH  (50)
#define CB_ACCEPT_HEIGHT (50)
#define CB_DENY_WIDTH    (50)
#define CB_DENY_HEIGHT   (50)

/*
 * Crystal click structure.
 *
 * Contains the coordinates a single crystal and the coordinates
 * inside the sticks list.
 */
static struct crystal_click
{
	Rectangle rect;
	int row;
	int col;
} crystal_click[MAX_STICKS];

/* Recalculate crystal_clicks. */
static int recalculate_crystal_clicks = 1;

/* Selected crystal. */
static int crystal_row = -1;
static int crystal_col = -1;

/* Options. */
static Rectangle accept_rect;
static Rectangle deny_rect;
static Rectangle play_again_rect;

/* Status bar. */
#define SB_WIDTH   330
#define SB_SPACE   10
#define SB_X       (SCREEN_WIDTH - SB_WIDTH - SB_SPACE)
#define SB_Y       (SB_SPACE)

#define SB_TITLE_X (SCREEN_WIDTH - SB_WIDTH)
#define SB_TITLE_Y (SB_Y + SB_SPACE)

#define CB_SPACING 30
#define CB_START_X (SB_TITLE_X - SB_SPACE)
#define CB_START_Y (SB_TITLE_Y + 210)

/* Common phrases. */
#define TXT_YWIN   "YOU WIN!!!"
#define TXT_YLOSE  "YOU LOSE!!!"
#define TXT_PA     "Play again? (click here to play again)"
#define YWL_Y      80
#define YWL_SIZE   90
#define PA_Y       170
#define PA_SIZE    30

/* ---------------------------------------------------------------------- */
/* Internal routines - game logic                                         */
/* ---------------------------------------------------------------------- */

/**
 * Computer "AI", i.e: algorithm that chooses the best row and
 * amount to remove.
 */
static void computer_think(void)
{
	int greater_than_one;
	int nim_sum;
	int amnt;
	int i;

	/* Ensure that we're unrolling the right amount. */
	COMPILE_TIME_ASSERT(MAX_ROWS == 4);

	/* Count amount of heaps with more than one crystal. */
	greater_than_one = 0;
	for (i = 0; i < MAX_ROWS; i++)
		if (sticks[i] > 1)
			greater_than_one++;

	/* Nim sum. */
	nim_sum = sticks[0] ^ sticks[1] ^ sticks[2] ^ sticks[3];

	/*
	 * Two options here:
	 *
	 * a) If nim_sum != 0, the computer _will_ win =).
	 *
	 * b) If nim_sum is 0, the computer have no way to create another
	 *    sequence that has 0 as the nim_sum value, in other words:
	 *    computer will lose if the player keeps playing like this =).
	 */
	if (nim_sum)
	{
		/* Recalculate nim-sum. */
		for (i = 0; i < MAX_ROWS; i++)
			if ((amnt = sticks[i] ^ nim_sum) < sticks[i])
				break;

		/*
		 * We have 2 options here: (again)
		 * a) More than one heap with more than one stick
		 * b) Exactly one heap with more than one stick
		 *
		 * The option b) do not conforms with the maths as
		 * expected and we fall in a case were we remove
		 * n or n-1 sticks from that column. So is necessary
		 * to bifurcate these two scenarios here.
		 */

		/* Scenario a). */
		if (greater_than_one > 1)
		{
			/* Update crystal_* vars. */
			crystal_row = i;
			crystal_col = (sticks[i] - amnt - 1);
		}

		/* Scenario b). */
		else
		{
			/* Odd number of crystals, remove all row. */
			if ((sticks_count - sticks[i]) & 1)
			{
				crystal_row = i;
				crystal_col = (sticks[i] - 1);
			}

			/* Even number, remove n-1 crystals. */
			else
			{
				crystal_row = i;
				crystal_col = (sticks[i] - 2);
			}
		}
	}

	/*
	 * There is no way to get nin_sum == 0 here, so let us remove
	 * a single piece and hope the player makes a wrong move.
	 */
	else
	{
		for (i = 0; i < MAX_ROWS; i++)
			if (sticks[i] > 0)
				break;

		crystal_row = i;
		crystal_col = 0;
	}
}

/* ---------------------------------------------------------------------- */
/* Internal routines - drawing                                            */
/* ---------------------------------------------------------------------- */

/**
 * Draw status bar, that contains the current turn, selected row,
 * column and the confirm/deny buttons.
 */
static void draw_status_bar(void)
{
	Rectangle rec;
	int row;
	int amt;
	int posX;
	int posY;

	row = amt = 0;
	if (crystal_idx != -1)
	{
		row = crystal_click[crystal_idx].row + 1;
		amt = crystal_click[crystal_idx].col + 1;
	}

	/* Background. */
	rec.x = SB_X;
	rec.y = SB_Y;
	rec.width = SB_WIDTH;
	rec.height = SCREEN_HEIGHT - SB_SPACE - SB_SPACE;
	DrawRectangleRounded(rec, 0.10f, 0, ColorAlpha(BLUE, 0.2f));
	DrawRectangleRoundedLines(rec, 0.10f, 0, 1, BLACK);

	/* Texts. */
	DrawText("Status: ", SB_TITLE_X, SB_TITLE_Y, 30, BLACK);
	DrawText(TextFormat(
		" > Turn: %s\n"
		" > Selected row:     %d\n"
		" > Amount to remove: %d\n",
		(turn == PLAYER_TURN ? "Player" : "Computer"),
		row, amt),
		SB_TITLE_X, SB_TITLE_Y + 50, 20, BLACK);

	/* Check if there is a row selected. */
	if (crystal_row > -1 && (state == S_DEFAULT || state == S_CONFIRM_REMOVE))
	{
		if (turn == PLAYER_TURN)
			DrawText(TextFormat("Do you really want to remove\n%d crystals "
				"from row %d ?\n", amt, row), SB_TITLE_X, SB_TITLE_Y + 150,
				20, BLACK);
		else if (turn == COMPUTER_TURN)
			DrawText("Thats my turn, can I play?", SB_TITLE_X, SB_TITLE_Y + 150,
				20, BLACK);


		posX = CB_START_X + ((SB_WIDTH - ((CB_ACCEPT_WIDTH << 1) +
			CB_SPACING)) >> 1);
		posY = CB_START_Y;

	
		DrawTexture(accept, posX, posY, WHITE);
		DrawTexture(deny,   posX + CB_ACCEPT_WIDTH + CB_SPACING, posY, WHITE);
	}
}

/**
 * Draw all the remaining crystals on the screen, also does the fade
 * and shifting effect.
 */
static void draw_crystals(void)
{
	int crystal_x;
	int crystal_y;
	int stick_idx;

	stick_idx = 0;
	crystal_x = CRYSTAL_X;
	crystal_y = CRYSTAL_Y;

	if (state != S_PIECE_SHIFTING)
	{
		for (int i = 0; i < MAX_ROWS; i++)
		{
			for (int j = 0; j < sticks[i]; j++)
			{
				/* Draw. */
				if (state != S_REMOVING_PIECE || crystal_row != i || j > crystal_col)
					DrawTexture(crystal, crystal_x, crystal_y, WHITE);
				else
					DrawTexture(crystal, crystal_x, crystal_y, ColorAlpha(WHITE,
						alpha));

				/* Configure rectangle to track the click. */
				if (recalculate_crystal_clicks)
				{
					crystal_click[stick_idx].row = i;
					crystal_click[stick_idx].col = j;
					crystal_click[stick_idx].rect.x = crystal_x;
					crystal_click[stick_idx].rect.y = crystal_y;
					crystal_click[stick_idx].rect.width = CRYSTAL_WIDTH;
					crystal_click[stick_idx].rect.height = CRYSTAL_HEIGHT;
					stick_idx++;
				}

				/* Increase X axis. */
				crystal_x = (crystal_x + CRYSTAL_WIDTH);
			}
			crystal_y += CRYSTAL_HEIGHT;
			crystal_x  = CRYSTAL_X;
		}
	}

	/* Left-shifting effect. */
	else
	{
		for (int i = 0; i < MAX_ROWS; i++)
		{
			int j = 0;
			if (i == crystal_row)
			{
				j = crystal_col + 1;
				crystal_x = move_start_x;
			}

			for (; j < sticks[i]; j++)
			{
				/* Draw. */
				DrawTexture(crystal, crystal_x, crystal_y, WHITE);

				/* Increase X axis. */
				crystal_x += CRYSTAL_WIDTH;
			}
			crystal_y += CRYSTAL_HEIGHT;
			crystal_x  = CRYSTAL_X;
		}
	}
}

/**
 * Draws the rectangle around the current selected crystals; change
 * the color when they are selected.
 */
static void draw_crystal_selection(void)
{
	/* If there is a click, draw the selection. */
	if (turn == PLAYER_TURN && (crystal_idx != -1 || crystal_row > -1))
	{
		int posX = crystal_click[crystal_idx -
						crystal_click[crystal_idx].col].rect.x;
		int posY = crystal_click[crystal_idx -
						crystal_click[crystal_idx].col].rect.y;

		int width  = crystal_click[crystal_idx].rect.x + CRYSTAL_WIDTH -
			posX + 1;
		int height = CRYSTAL_HEIGHT;

		if (crystal_row == -1)
			DrawRectangleRoundedLines(((Rectangle){.x=posX, .y=posY,
				.width=width, .height=height}), 0.2f, 0, 3, BLUE);
		else if (state == S_DEFAULT)
			DrawRectangleRoundedLines(((Rectangle){.x=posX, .y=posY,
				.width=width, .height=height}), 0.2f, 0, 3, DARKGREEN);
		else if (state == S_REMOVING_PIECE)
			DrawRectangleRoundedLines(((Rectangle){.x=posX, .y=posY,
				.width=width, .height=height}), 0.2f, 0, 3,
				ColorAlpha(DARKGREEN, alpha));
	}
}

/**
 * Main game logic is here:
 * - Check for mouse clicks on the sticks
 * - Manage the states and alpha channels
 * - Manage crystals movement while removing them
 * - Manage crystals selection
 */
static void logic_think(void)
{
	int i;

	if (turn == PLAYER_TURN)
	{
		if (state == S_DEFAULT)
		{
			/* Only check if there is no crystal selected. */
			if (crystal_row == -1)
			{
				/* Check for clicks in the sticks/crystals. */
				for (i = 0; i < sticks_count; i++)
				{
					if (CheckCollisionPointRec(mouse, crystal_click[i].rect))
					{
						crystal_idx = i;
						break;
					}
				}

				/* Reset selection if mouse is not over them anymore. */
				if (i == sticks_count)
					crystal_idx = -1;

				/* If there is a mouse click and a valid crystal selection. */
				if (IsClick() && crystal_idx > -1)
				{
					crystal_row = crystal_click[crystal_idx].row;
					crystal_col = crystal_click[crystal_idx].col;
				}
			}
		}
	}

	/* If Computer turn and default state. */
	else if (state == S_DEFAULT)
	{
		computer_think();
		alpha = 1.0f;
		state = S_CONFIRM_REMOVE;
		frame_counter = 0;
	}

	/* Define rectangles for accept/deny buttons. */
	if (crystal_row != -1)
	{
		if (IsClick())
		{
			mouse = GetMousePosition();

			if (CheckCollisionPointRec(mouse, accept_rect))
			{
				/* Confirmed sticks deletion. */
				alpha = 1.0f;
				state = S_REMOVING_PIECE;
				frame_counter = 0;
			}
		
			else if (CheckCollisionPointRec(mouse, deny_rect))
			{
				if (IsClick())
				{
					/* Reset selection. */
					crystal_idx = -1;
					crystal_row = -1;
					crystal_col = -1;
				}
			}
		}
	}

	/* Common between turns. */
	if (state == S_REMOVING_PIECE)
	{
		frame_counter++;
		if (frame_counter < (FPS))
			alpha -= 1.0f/(float)(FPS);
		else
		{
			state = S_PIECE_SHIFTING;
			alpha = 1.0f;

			/* If removing entire row, do not waste time shifting. */
			if (sticks[crystal_row] != crystal_col + 1)
				frame_counter = 0;
			else
				frame_counter = FPS;

			/* Set our variables. */
			move_start_x = CRYSTAL_X + ((crystal_col+1)*CRYSTAL_WIDTH);
			move_step    = ((crystal_col+1)*CRYSTAL_WIDTH)/FPS;
		}
	}

	else if (state == S_PIECE_SHIFTING)
	{
		frame_counter++;
		if (frame_counter <= FPS)
			move_start_x -= move_step;
		else
		{
			/*
			 * Remove properly the pieces.
			 */
			sticks[crystal_row] -= crystal_col + 1;
			sticks_count -= crystal_col + 1;
			recalculate_crystal_clicks = 1;

			/* Reset selection and state. */
			state = S_DEFAULT;
			crystal_idx = -1;
			crystal_row = -1;
			crystal_col = -1;

			/* Invert turn. */
			turn = !turn;

			/* Reset alpha if game is over. */
			if (!sticks_count)
			{
				alpha = 0.0f;
				alpha_inc = 1.0f/(float)FPS*2;
			}
		}
	}
}

/* ---------------------------------------------------------------------- */
/* Public routines.                                                       */
/* ---------------------------------------------------------------------- */

/**
 * Configure the crystals amount accordingly with the current
 * state of the 'random amount'-checkbox.
 */
void setup_crystals_amount(void)
{
	int i;

	/* Random selected. */
	if (cb_rnd_amt_selected)
	{
		sticks_count = 0;
		for (i = 0; i < MAX_ROWS; i++)
		{
			sticks[i] = GetRandomValue(1, 7);
			sticks_count += sticks[i];
		}
	}

	/* Default config. */
	else
	{
		sticks_count  = DEFAULT_STICKS;
		sticks[0]     = 1;
		sticks[1]     = 3;
		sticks[2]     = 5;
		sticks[3]     = 7;
	}
}

/**
 * Load in-game resources.
 */
void init_ingame(void)
{
	Vector2 pa_vec;

	crystal = LoadTexture("resources/crystal.png");
	accept  = LoadTexture("resources/accept.png");
	deny    = LoadTexture("resources/deny.png");

	assert(accept.width   == CB_ACCEPT_WIDTH);
	assert(accept.height  == CB_ACCEPT_HEIGHT);
	assert(deny.width     == CB_DENY_WIDTH);
	assert(deny.height    == CB_DENY_HEIGHT);
	assert(crystal.width  == CRYSTAL_WIDTH);
	assert(crystal.height == CRYSTAL_HEIGHT);

	/* Too earlier to do that, but saves some processing later. */
	accept_rect.x = CB_START_X + ((SB_WIDTH - ((CB_ACCEPT_WIDTH << 1) +
		CB_SPACING)) >> 1);
	accept_rect.y = CB_START_Y;
	accept_rect.width = CB_ACCEPT_WIDTH;
	accept_rect.height = CB_ACCEPT_HEIGHT;

	deny_rect.x = accept_rect.x + CB_ACCEPT_WIDTH + CB_SPACING;
	deny_rect.y = CB_START_Y;
	deny_rect.width = CB_DENY_WIDTH;
	deny_rect.height = CB_DENY_HEIGHT;

	pa_vec = MeasureTextEx(GetFontDefault(), TXT_PA, (float)PA_SIZE,
		(float)PA_SIZE/10);
	play_again_rect.x = ((SCREEN_WIDTH >> 1) - (MeasureText(TXT_PA,
		PA_SIZE) >> 1));
	play_again_rect.y = PA_Y;
	play_again_rect.width = pa_vec.x;
	play_again_rect.height = pa_vec.y;
}

/**
 * Free in-game resources.
 */
void finish_ingame(void)
{
	UnloadTexture(crystal);
	UnloadTexture(accept);
	UnloadTexture(deny);
}

/**
 * Update in-game logic, i.e: manages the game and when the
 * game is over or not.
 */
void update_ingame_logic(void)
{
	if (sticks_count > 0)
	{
		logic_think();
		return;
	}

	switch (state)
	{
		case S_FADE_END:
		{
			frame_counter++;
			alpha += alpha_inc;
			if (frame_counter >= FPS/2)
			{
				frame_counter = 0;
				alpha = 1.0f;
				state = S_FADE_PLAY_AGAIN;
			}
			break;
		}

		case S_FADE_PLAY_AGAIN:
		{
			frame_counter++;
			alpha_again += alpha_inc;
		
			if (frame_counter >= FPS/2)
			{
				frame_counter = 0;
				alpha_inc *= -1.0f;
			}

			/* Check if mouse click. */
			if (CheckCollisionPointRec(mouse, play_again_rect))
			{
				if (IsClick())
				{
					/* Reset all the game state. */
					frame_counter = 0;
					alpha         = 1.0f;
					alpha_again   = 0.0f;
					alpha_inc     = 1.0f/(float)FPS*2;
					crystal_idx   = -1;
					crystal_row   = -1;
					crystal_col   = -1;
					move_start_x  =  0;
					move_step     =  0;
					state         = S_DEFAULT;
					global_state  = STATE_TUTORIAL;
					recalculate_crystal_clicks = -1;
					setup_crystals_amount();
				}
			}
			break;
		}

		default:
			frame_counter = 0;
			state = S_FADE_END;
			break;
	}
}

/**
 * Manages the in-game drawing.
 */
void update_ingame_drawing(void)
{
	if (sticks_count > 0)
	{
		draw_crystals();

		/* Reset recalculate if set. */
		if (recalculate_crystal_clicks)
			recalculate_crystal_clicks = 0;

		draw_crystal_selection();

		/* Status bar. */
		draw_status_bar();
		return;
	}

	if (turn == PLAYER_TURN)
		DrawText(TXT_YWIN, ((SCREEN_WIDTH >> 1) - (MeasureText(TXT_YWIN,
			YWL_SIZE) >> 1)), YWL_Y, YWL_SIZE, ColorAlpha(BLACK, alpha));
	else
		DrawText(TXT_YLOSE, ((SCREEN_WIDTH >> 1) - (MeasureText(TXT_YLOSE,
			YWL_SIZE) >> 1)), YWL_Y, YWL_SIZE, ColorAlpha(BLACK, alpha));

	DrawText(TXT_PA, play_again_rect.x, PA_Y, PA_SIZE, ColorAlpha(BLACK,
		alpha_again));
}
