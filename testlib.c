#include "primlib.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#define NUMBER_OF_DISCS 5
#define NUMBER_OF_PEGS 5
#define PEG_MIN_WIDTH 10
#define TABLE_HEIGHT 30
#define SPEED_OF_DISC 8
#define TEXT_POSITION_PEG_SELECTION 50
#define DISC_BREAK_POINT 20
#define SIZE_OF_DISC_WHEN_LOWER_THEN_20 20
#define DISC_BREAK_POINT 20
#define MAX_HEIGHT_OF_ALL_DISCS 400
#define BASIC_VALUE_NO_KEY -1
#define GAP_BETWEEN_PEG_AND_DISC 70
#define ASCII_VALUE_OF_ZERO 48
#define WIDTH_OF_PEG 5
#define CURRENT_DELAY 10

bool is_move_possible(int board[NUMBER_OF_PEGS][NUMBER_OF_DISCS], int block_number, int move_to)
{
	for (int num_disc = NUMBER_OF_DISCS - 1; num_disc > -1; num_disc--)
	{
		if (board[move_to][num_disc] != BASIC_VALUE_NO_KEY)
		{
			if (board[move_to][num_disc] <= block_number)
			{
				return false;
			}
		}
	}
	return true;
}

int get_disc_number(int board[NUMBER_OF_PEGS][NUMBER_OF_DISCS], int block_number)
{
	for (int num_disc = 0; num_disc < NUMBER_OF_DISCS; num_disc++)
	{
		if (board[block_number][num_disc] != BASIC_VALUE_NO_KEY)
		{
			return board[block_number][num_disc];
		}
	}
	return -1;
}

void drawing_board(int pegs_mid_x[NUMBER_OF_PEGS], int discs_mid[NUMBER_OF_DISCS][2], float discs_width_f[NUMBER_OF_DISCS],
				   int height_of_peg, int height_of_disc, int press[2])
{
	gfx_filledRect(0, 0, gfx_screenWidth() - 1, gfx_screenHeight() - 1, BLACK);
	for (int num_peg = 0; num_peg < NUMBER_OF_PEGS; num_peg++)
	{
		gfx_filledRect(pegs_mid_x[num_peg] - WIDTH_OF_PEG, gfx_screenHeight(), pegs_mid_x[num_peg] + WIDTH_OF_PEG, gfx_screenHeight() - height_of_peg, MAGENTA);
	}

	for (int num_disc = 0; num_disc < NUMBER_OF_DISCS; num_disc++)
	{
		enum color c = GREEN;
			// every second disc has a different collor thus num_disc%2
			if (num_disc % 2 == 0)
		{
			c = YELLOW;
		}
		gfx_filledRect(discs_mid[num_disc][0] - discs_width_f[num_disc],
					   discs_mid[num_disc][1], discs_mid[num_disc][0] + discs_width_f[num_disc], discs_mid[num_disc][1] - height_of_disc, c);
		gfx_rect(discs_mid[num_disc][0] - discs_width_f[num_disc],
				 discs_mid[num_disc][1], discs_mid[num_disc][0] + discs_width_f[num_disc], discs_mid[num_disc][1] - height_of_disc, WHITE);
	}
	if (press[0] != BASIC_VALUE_NO_KEY)
	{
		char str[12];
		sprintf(str, "%d", press[0]);
		gfx_textout(TEXT_POSITION_PEG_SELECTION, TEXT_POSITION_PEG_SELECTION, str, WHITE);
	}
	if (press[1] != BASIC_VALUE_NO_KEY)
	{
		char str[12];
		sprintf(str, "%d", press[1]);
		gfx_textout(gfx_screenWidth() - TEXT_POSITION_PEG_SELECTION, TEXT_POSITION_PEG_SELECTION, str, WHITE);
	}
	gfx_filledRect(gfx_screenWidth(), gfx_screenHeight(), 0, gfx_screenHeight() - TABLE_HEIGHT, CYAN);
	gfx_updateScreen();
	SDL_Delay(10);
}

void move(int from, int to, int board[NUMBER_OF_PEGS][NUMBER_OF_DISCS], int discs_m[NUMBER_OF_DISCS][2],
		  int height_of_p, int disc_number, float discs_width_f[NUMBER_OF_DISCS], int pegs_mid_x[NUMBER_OF_PEGS], int height_of_disc, int press[2])
{
	int which_y = 0;
	while (gfx_screenHeight() - discs_m[disc_number][1] < height_of_p + GAP_BETWEEN_PEG_AND_DISC)
	{
		discs_m[disc_number][1] -= SPEED_OF_DISC;
		drawing_board(pegs_mid_x, discs_m, discs_width_f, height_of_p, height_of_disc, press);
	}
	if (to > from)
	{
		while (discs_m[disc_number][0] < pegs_mid_x[to])
		{
			discs_m[disc_number][0] += SPEED_OF_DISC;
			drawing_board(pegs_mid_x, discs_m, discs_width_f, height_of_p, height_of_disc, press);
		}
	}
	else if (from > to)
	{
		while (discs_m[disc_number][0] > pegs_mid_x[to])
		{
			discs_m[disc_number][0] -= SPEED_OF_DISC;
			drawing_board(pegs_mid_x, discs_m, discs_width_f, height_of_p, height_of_disc, press);
		}
	}
	for (int num_peg = 0; num_peg < NUMBER_OF_DISCS; num_peg++)
	{
		if (board[to][num_peg] > BASIC_VALUE_NO_KEY)
		{
			which_y = NUMBER_OF_DISCS - num_peg;
			break;
		}
	}
	while (discs_m[disc_number][1] < gfx_screenHeight() - height_of_disc * which_y - TABLE_HEIGHT)
	{
		discs_m[disc_number][1] += SPEED_OF_DISC;
		drawing_board(pegs_mid_x, discs_m, discs_width_f, height_of_p, height_of_disc, press);
	}
	if (discs_m[disc_number][1] > gfx_screenHeight() - height_of_disc * which_y - TABLE_HEIGHT)
	{
		discs_m[disc_number][1] = gfx_screenHeight() - height_of_disc * which_y - TABLE_HEIGHT;
		drawing_board(pegs_mid_x, discs_m, discs_width_f, height_of_p, height_of_disc, press);
	}
}

void change_board_status(int from, int to, int board[NUMBER_OF_PEGS][NUMBER_OF_DISCS])
{
	int x_1, x_2, temp;
	x_2 = NUMBER_OF_DISCS - 1;
	for (int num_disc = 0; num_disc < NUMBER_OF_DISCS; num_disc++)
	{
		if (board[from][num_disc] != BASIC_VALUE_NO_KEY)
		{
			x_1 = num_disc;
			break;
		}
	}
	for (int num_disc = 0; num_disc < NUMBER_OF_DISCS; num_disc++)
	{
		if (board[to][num_disc] != BASIC_VALUE_NO_KEY)
		{
			x_2 = num_disc - 1;
			break;
		}
	}
	temp = board[from][x_1];
	board[from][x_1] = board[to][x_2];
	board[to][x_2] = temp;
}

int check_if_win(int board[NUMBER_OF_PEGS][NUMBER_OF_DISCS])
{
	int temp_sum;
	for (int num_peg = 1; num_peg < NUMBER_OF_PEGS; num_peg++)
	{
		temp_sum = 0;
		for (int num_disc = 0; num_disc < NUMBER_OF_DISCS; num_disc++)
		{
			if (board[num_peg][num_disc] != BASIC_VALUE_NO_KEY)
			{
				temp_sum += 1;
			}
		}
		if (temp_sum == NUMBER_OF_DISCS)
		{
			return true;
		}
	}
	return false;
}

int get_key()
{
	int key_input = BASIC_VALUE_NO_KEY;
	key_input = gfx_pollkey();
	if (key_input == SDLK_ESCAPE)
	{
		exit(3);
	}
	if (key_input == SDLK_0)
	{
		return 10;
	}
	else if (key_input >= SDLK_1 && key_input <= SDLK_9)
	{
		return key_input % ASCII_VALUE_OF_ZERO;
	}
	else
	{
		return BASIC_VALUE_NO_KEY;
	}
}

void input_handler(int presses_f[2], int board_state_f[NUMBER_OF_DISCS][NUMBER_OF_PEGS], int *chosen_disc_f)
{
	if (presses_f[0] == BASIC_VALUE_NO_KEY)
	{
		presses_f[0] = get_key();
	}
	if (presses_f[0] != BASIC_VALUE_NO_KEY && presses_f[1] == BASIC_VALUE_NO_KEY)
	{
		presses_f[1] = get_key();
	}

	if (presses_f[0] > NUMBER_OF_PEGS)
	{
		presses_f[0] = BASIC_VALUE_NO_KEY;
	}

	if (presses_f[1] > NUMBER_OF_PEGS)
	{
		presses_f[1] = BASIC_VALUE_NO_KEY;
	}

	if (presses_f[0] != BASIC_VALUE_NO_KEY && presses_f[1] != BASIC_VALUE_NO_KEY)
	{
		*chosen_disc_f = get_disc_number(board_state_f, presses_f[0] - 1);
		if ((*chosen_disc_f) == BASIC_VALUE_NO_KEY)
		{
			presses_f[0] = BASIC_VALUE_NO_KEY;
			presses_f[1] = BASIC_VALUE_NO_KEY;
		}
	}
}

void initializing_value(int board_state_f[NUMBER_OF_PEGS][NUMBER_OF_DISCS], int PEGS_MIDDLE_X_F[NUMBER_OF_PEGS], int *HEIGHT_OF_PEGS_F, int *HEIGHT_OF_DISCS, int discs_middle_f[NUMBER_OF_DISCS][2], float DISCS_WIDTH_F[NUMBER_OF_DISCS], int PEG_MAX_WIDTH_F)
{
	for (int num_peg = 0; num_peg < NUMBER_OF_PEGS; num_peg++)
	{
		for (int num_disc = 0; num_disc < NUMBER_OF_DISCS; num_disc++)
		{
			board_state_f[num_peg][num_disc] = BASIC_VALUE_NO_KEY;
		}
	}

	for (int num_disc = NUMBER_OF_DISCS - 1; num_disc > -1; num_disc--)
	{
		board_state_f[0][num_disc] = num_disc;
	}

	for (int num_peg = 0; num_peg < NUMBER_OF_PEGS; num_peg++)
	{
		PEGS_MIDDLE_X_F[num_peg] = gfx_screenWidth() / NUMBER_OF_PEGS / 2 + gfx_screenWidth() / NUMBER_OF_PEGS * num_peg;
	}

	if (NUMBER_OF_DISCS > DISC_BREAK_POINT)
	{
		*HEIGHT_OF_PEGS_F = ((DISC_BREAK_POINT + 1) * SIZE_OF_DISC_WHEN_LOWER_THEN_20) + TABLE_HEIGHT;
	}

	else
	{
		*HEIGHT_OF_PEGS_F = NUMBER_OF_DISCS * SIZE_OF_DISC_WHEN_LOWER_THEN_20 + SIZE_OF_DISC_WHEN_LOWER_THEN_20 + TABLE_HEIGHT;
	}

	if (NUMBER_OF_DISCS > DISC_BREAK_POINT)
	{
		*HEIGHT_OF_DISCS = MAX_HEIGHT_OF_ALL_DISCS / NUMBER_OF_DISCS;
	}

	else
	{
		*HEIGHT_OF_DISCS = SIZE_OF_DISC_WHEN_LOWER_THEN_20;
	}

	for (int num_disc = NUMBER_OF_DISCS - 1; num_disc > -1; num_disc--)
	{
		discs_middle_f[num_disc][0] = PEGS_MIDDLE_X_F[0];
		discs_middle_f[num_disc][1] = gfx_screenHeight() - *HEIGHT_OF_DISCS * (NUMBER_OF_DISCS - 1 - num_disc) - TABLE_HEIGHT;
	}

	for (int num_disc = NUMBER_OF_DISCS - 1; num_disc > -1; num_disc--)
	{
		DISCS_WIDTH_F[num_disc] = 1.0 * PEG_MIN_WIDTH + 1.0 * (PEG_MAX_WIDTH_F - PEG_MIN_WIDTH) / NUMBER_OF_DISCS * num_disc;
	}
}

int main(int argc, char *argv[])
{
	if (gfx_init())
	{
		exit(3);
	}
	int is_end = false;
	int presses[2];

	// -1 is the basic value when no key/disc is stored by the variable
	presses[0] = BASIC_VALUE_NO_KEY;
	presses[1] = BASIC_VALUE_NO_KEY;
	int chosen_disc = BASIC_VALUE_NO_KEY;

	int discs_middle[NUMBER_OF_DISCS][2];
	int HEIGHT_OF_PEGS;
	int HEIGHT_OF_DISCS;
	int PEGS_MIDDLE_X[NUMBER_OF_PEGS];
	float PEG_MAX_WIDTH = gfx_screenWidth() / NUMBER_OF_PEGS / 2;
	float DISCS_WIDTH[NUMBER_OF_DISCS];
	int board_state[NUMBER_OF_PEGS][NUMBER_OF_DISCS];

	initializing_value(board_state, PEGS_MIDDLE_X, &HEIGHT_OF_PEGS, &HEIGHT_OF_DISCS, discs_middle, DISCS_WIDTH, PEG_MAX_WIDTH);

	// game loop
	while (!is_end)
	{
		drawing_board(PEGS_MIDDLE_X, discs_middle, DISCS_WIDTH, HEIGHT_OF_PEGS, HEIGHT_OF_DISCS, presses);
		SDL_Delay(CURRENT_DELAY);

		input_handler(presses, board_state, &chosen_disc);

		if (chosen_disc != BASIC_VALUE_NO_KEY && presses[0] != BASIC_VALUE_NO_KEY && presses[1] != BASIC_VALUE_NO_KEY)
		{
			if (is_move_possible(board_state, chosen_disc, presses[1] - 1))
			{
				move(presses[0] - 1, presses[1] - 1, board_state, discs_middle, HEIGHT_OF_PEGS, chosen_disc, DISCS_WIDTH,
					 PEGS_MIDDLE_X, HEIGHT_OF_DISCS, presses);
				change_board_status(presses[0] - 1, presses[1] - 1, board_state);
				is_end = check_if_win(board_state);
				presses[0] = BASIC_VALUE_NO_KEY;
				presses[1] = BASIC_VALUE_NO_KEY;
			}
			else
			{
				presses[0] = BASIC_VALUE_NO_KEY;
				presses[1] = BASIC_VALUE_NO_KEY;
			}
		}
	}
	// winning loop
	while (true)
	{
		gfx_filledRect(0, 0, gfx_screenWidth() - 1, gfx_screenHeight() - 1, GREEN);
		gfx_textout(gfx_screenWidth() / 2, gfx_screenHeight() / 2, "YOU WON", WHITE);
		gfx_updateScreen();
		get_key();
	}
	return 0;
}
