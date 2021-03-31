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

#ifndef SCENES_H
#define SCENES_H

	#include "raylib.h"

	/* ---------------------------------------------------------------------- */
	/* Constants.                                                             */
	/* ---------------------------------------------------------------------- */
	
	/* Screen. */
#if !defined(WEB)
	#define FPS 60
#else
	#define FPS GetFPS()
#endif

#if defined(ANDROID)
	#if defined(AR_19_9)
		#define SCREEN_WIDTH  1052 /* 19:9 aspect ratio. */
	#else
		#define SCREEN_WIDTH  888  /* 16:9 aspect ratio. */
	#endif
#else
	#define SCREEN_WIDTH  888
#endif

	#define SCREEN_HEIGHT 500

	/* General text. */
	#define START_X        5
	#define START_Y        70
	#define FONT_SIZE      20

	/* Title. */
	#define TITLE_SIZE    50
	#define TITLE         "Crystal Nim"
	#define TITLE_BY_SIZE 20

	/* Gear button and menu. */
	#define GEAR_X       450
	#define GEAR_Y        10

	/* Tutorial. */
	#define TUTORIAL_SIZE    (FONT_SIZE)
	#define TUTORIAL_START_Y (START_Y)
	#define TUTORIAL_PC_X    (START_X + 150)
	#define TUTORIAL_PC_Y    (TUTORIAL_START_Y + 85)
	#define TUTORIAL_USER_X  (START_X + 220)
	#define TUTORIAL_USER_Y  (TUTORIAL_PC_Y)

	/* Ingame. */
	#define CRYSTAL_X       (START_X)
	#define CRYSTAL_Y       (START_Y - 18)

	/* Colors. */
	#define LIGHT_BLUE ((Color){176, 222, 255, 255})

	/* Game state. */
	#define STATE_TUTORIAL 0
	#define STATE_INGAME   1
	#define STATE_FINISH   2

	/* Sticks. */
	#define DEFAULT_STICKS    16
	#define MAX_STICKS_PER_ROW 7
	#define MAX_ROWS           4
	#define MAX_STICKS        (MAX_STICKS_PER_ROW * MAX_ROWS)
	

	/* Turns. */
	#define PLAYER_TURN   0
	#define COMPUTER_TURN 1

	/**
	 * Triggers a compile time error if the expression
	 * evaluates to 0.
	 */
	#define COMPILE_TIME_ASSERT(expr)  \
		switch(0){case 0:case expr:;}

	/* Always inline. */
#if defined(__GNUC__) || defined(__GNUG__) || defined(__clang__)
	#define INLINE  __attribute__((always_inline)) inline
#else
	#define INLINE inline
#endif

	/* ---------------------------------------------------------------------- */
	/* External declarations.                                                 */
	/* ---------------------------------------------------------------------- */

	/* Common. */
	extern int sticks[MAX_ROWS];
	extern int sticks_count;
	extern int global_state;
	extern Vector2 mouse;
	extern int turn;
	extern bool cb_rnd_amt_selected;

	/* Gear. */
	extern void init_gear(void);
	extern void finish_gear(void);
	extern void update_gear_logic(void);
	extern void update_gear_drawing(void);

	/* Tutorial. */
	extern void init_tutorial(void);
	extern void finish_tutorial(void);
	extern void update_tutorial_logic(void);
	extern void update_tutorial_drawing(void);

	/* Ingame. */
	extern void setup_crystals_amount(void);
	extern void init_ingame(void);
	extern void finish_ingame(void);
	extern void update_ingame_logic(void);
	extern void update_ingame_drawing(void);

	/* IsClick. */
	static inline bool IsClick(void)
	{
#ifndef ANDROID
		return IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
#else
		return IsGestureDetected(GESTURE_TAP);
#endif
	}

#endif /* SCENES_H. */
