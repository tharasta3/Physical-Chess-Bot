/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include <string.h>
#include <stdarg.h>
#include "ili9341.h"
#include "fonts.h"
#include <stdlib.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3

#define UP (start[1] + 1)
#define UP2 (start[1] + 2)
#define DOWN (start[1] - 1)
#define DOWN2 (start[1] - 2)
#define RIGHT (start[0] + 1)
#define RIGHT2 (start[0] + 2)
#define LEFT (start[0] - 1)
#define LEFT2 (start[0] - 2)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef hlpuart1;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim4;

/* USER CODE BEGIN PV */
int poll = 0;

int hmove = 0;
int vmove = 0;
int count = 0;
int countmax = 0;
int playermove = 1;
int currstate[8][8] = {
    {1, 1, 1, 1, 1, 1, 1, 1}, // Black pieces
    {1, 1, 1, 1, 1, 1, 1, 1}, // Black pawns
    {0, 0, 0, 0, 0, 0, 0, 0}, // Empty squares
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {1, 1, 1, 1, 1, 1, 1, 1}, // White pawns
    {1, 1, 1, 1, 1, 1, 1, 1}  // White pieces
};
int tempstate[8][8] = {
	{1, 1, 1, 1, 1, 1, 1, 1}, // Black pieces
	{1, 1, 1, 1, 1, 1, 1, 1}, // Black pawns
	{0, 0, 0, 0, 0, 0, 0, 0}, // Empty squares
	{0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0},
	{1, 1, 1, 1, 1, 1, 1, 1}, // White pawns
	{1, 1, 1, 1, 1, 1, 1, 1}  // White pieces
};
//int start[2] = {2,2}; // COL, ROW
//int end[2] = {2,3};
int snextmove[2] = {0,0};
int enextmove[2] = {0,0};
int currpos[2] = {8,8};
uint8_t txbuff[5];
uint8_t rxbuff[5] = {56,56,56,56,0};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM4_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_LPUART1_UART_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/**
  * @brief  Spin the motor in the "up" direction.
  * @retval None
  */
void Up()
{
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, GPIO_PIN_SET);
    // Set PA3 (A0) HIGH
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8, GPIO_PIN_SET);
    // Set PC0 (A1) LOW
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_15, GPIO_PIN_RESET);
    HAL_Delay(4000);
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, GPIO_PIN_RESET);
}

/**
  * @brief  Spin the motor in the "down" direction.
  * @retval None
  */
void Down()
{
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, GPIO_PIN_SET);

	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_15, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8, GPIO_PIN_RESET);


    HAL_Delay(4000);
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, GPIO_PIN_RESET);
}

void hormove(int dir) {
	if (dir == 0) {
		HAL_GPIO_WritePin(HDIR_GPIO_Port, HDIR_Pin, GPIO_PIN_RESET); //move to the right (8->1)
	}
	else {
		HAL_GPIO_WritePin(HDIR_GPIO_Port, HDIR_Pin, GPIO_PIN_SET); //move to the left (1->8)
	}
   hmove = 1;
   countmax = 2225; // change to 1/2 square size
   while (hmove == 1) {}
}

void vertmove(int dir) {
	if (dir == 0) {
		HAL_GPIO_WritePin(VDIR_GPIO_Port, VDIR_Pin, GPIO_PIN_SET); //move up (8->1)
	}
	else {
		HAL_GPIO_WritePin(VDIR_GPIO_Port, VDIR_Pin, GPIO_PIN_RESET); // move down (1->8)
	}
   vmove = 1;
   countmax = 2225; // change to 1/2 square size
   while (vmove == 1) {}
}

void forcemove(int st[2], int e[2]) {
	int hdist = e[0] - st[0]; // start col - end col = horizontal dist
	int vdist = e[1] - st[1]; // start row - end row = vertical dist
	int hdir = 1;
	int vdir = 1;
	if (hdist < 0) {
		hdist *= -1;
		hdir = 0;
	}
	if (vdist < 0) {
		vdist *= -1;
		vdir = 0;
	}
	for (int i = 0; i < hdist; i++) {
		hormove(hdir); //move 1 full inch
		hormove(hdir);
	}
	for (int i = 0; i < vdist; i++) {
		vertmove(vdir); //move 1 full inch
		vertmove(vdir);
	}
}


//GABE CODE
void moveRelative(int dir, int quarter_inches) {
	for (int i = 0; i < quarter_inches; i++) {
		if (dir == NORTH) { //increasing row, vert = +
			HAL_GPIO_WritePin(VDIR_GPIO_Port, VDIR_Pin, GPIO_PIN_RESET); //move to the right (8->1)
			vmove = 1;
		}
		else if (dir == EAST) { //increasing row, hor = +
			HAL_GPIO_WritePin(HDIR_GPIO_Port, HDIR_Pin, GPIO_PIN_SET); //move to the left (1->8)
			hmove = 1;
		}
		else if (dir == WEST) { //increasing row, hor = -
			HAL_GPIO_WritePin(HDIR_GPIO_Port, HDIR_Pin, GPIO_PIN_RESET); //move to the left (1->8)
			hmove = 1;
		}
		else if (dir == SOUTH) { //increasing row, vert = -
			HAL_GPIO_WritePin(VDIR_GPIO_Port, VDIR_Pin, GPIO_PIN_SET); //move to the left (1->8)
			vmove = 1;
		}
	   countmax = 556; // 1/4 inch
	   while (hmove == 1 || vmove == 1) {}
	}
}

void shiftPiece(int row, int col, int dir) {
	if((row == 1 && dir == SOUTH) || (row == 8 && dir == NORTH) || (col == 1 && dir == WEST) || (col == 8 && dir == EAST))
	    return;
    if(currstate[row-1][col-1]) {
    	int next[2] = {col, row};
        forcemove(currpos, next);
        currpos[0] = next[0];
        currpos[1] = next[1];
        Up();
        moveRelative(dir, 2);
        Down();
        moveRelative((dir+2)%4, 2);
    }
}

void undoShift(int row, int col, int dir) {
	if((row == 1 && dir == SOUTH) || (row == 8 && dir == NORTH) || (col == 1 && dir == WEST) || (col == 8 && dir == EAST))
	    return;
    if(currstate[row-1][col-1]) {
    	int next[2] = {col, row};
        forcemove(currpos, next);
        currpos[0] = next[0];
        currpos[1] = next[1];
        moveRelative(dir, 2);
        Up();
        moveRelative((dir+2)%4, 2);
        Down();
    }
}

void shiftAll(int start[2], int end[2]) {
    int col_dif = end[0] - start[0];
    int row_dif = end[1] - start[1];

    if (abs(col_dif) > abs(row_dif)) { // horizontal
        if (col_dif > 0) { // right
            if (row_dif > 0) { // right up
                shiftPiece(start[1], RIGHT, SOUTH);
                shiftPiece(start[1], RIGHT2, SOUTH);
                shiftPiece(UP, start[0], NORTH);
                shiftPiece(UP, RIGHT, NORTH);
            } else { // right down
                shiftPiece(start[1], RIGHT, NORTH);
                shiftPiece(start[1], RIGHT2, NORTH);
                shiftPiece(UP, start[0], SOUTH);
                shiftPiece(DOWN, RIGHT, SOUTH);
            }
        } else { // left
            if (row_dif > 0) { // left up
                shiftPiece(start[1], LEFT, SOUTH);
                shiftPiece(start[1], LEFT2, SOUTH);
                shiftPiece(UP, start[0], NORTH);
                shiftPiece(UP, LEFT, NORTH);
            } else { // left down
                shiftPiece(start[1], LEFT, NORTH);
                shiftPiece(start[1], LEFT2, NORTH);
                shiftPiece(DOWN, start[0], SOUTH);
                shiftPiece(DOWN, LEFT, SOUTH);
            }
        }
    } else { // vertical
        if (row_dif > 0) { // up
            if (col_dif > 0) { // up right
                shiftPiece(UP, start[0], WEST);
                shiftPiece(UP2, start[0], WEST);
                shiftPiece(start[1], RIGHT, EAST);
                shiftPiece(UP, RIGHT, EAST);
            } else { // up left
                shiftPiece(UP, start[0], EAST);
                shiftPiece(UP2, start[0], EAST);
                shiftPiece(start[1], LEFT, WEST);
                shiftPiece(UP, LEFT, WEST);
            }
        } else { // down
            if (col_dif > 0) { // down right
                shiftPiece(DOWN, start[0], WEST);
                shiftPiece(DOWN2, start[0], WEST);
                shiftPiece(start[1], RIGHT, EAST);
                shiftPiece(DOWN, RIGHT, EAST);
            }
            else { // down left
                shiftPiece(DOWN, start[0], EAST);
                shiftPiece(DOWN2, start[0], EAST);
                shiftPiece(start[1], LEFT, WEST);
                shiftPiece(DOWN, LEFT, WEST);
            }
        }
    }
}

void undoshiftAll(int start[2], int end[2]) {
    int col_dif = end[0] - start[0];
    int row_dif = end[1] - start[1];


    if (abs(col_dif) > abs(row_dif)) { // horizontal
        if (col_dif > 0) { // right
            if (row_dif > 0) { // right up
                undoShift(start[1], RIGHT, SOUTH);
                undoShift(start[1], RIGHT2, SOUTH);
                undoShift(UP, start[0], NORTH);
                undoShift(UP, RIGHT, NORTH);
            } else { // right down
                undoShift(start[1], RIGHT, NORTH);
                undoShift(start[1], RIGHT2, NORTH);
                undoShift(UP, start[0], SOUTH);
                undoShift(DOWN, RIGHT, SOUTH);
            }
        } else { // left
            if (row_dif > 0) { // left up
                undoShift(start[1], LEFT, SOUTH);
                undoShift(start[1], LEFT2, SOUTH);
                undoShift(UP, start[0], NORTH);
                undoShift(UP, LEFT, NORTH);
            } else { // left down
                undoShift(start[1], LEFT, NORTH);
                undoShift(start[1], LEFT2, NORTH);
                undoShift(DOWN, start[0], SOUTH);
                undoShift(DOWN, LEFT, SOUTH);
            }
        }
    } else { // vertical
        if (row_dif > 0) { // up
            if (col_dif > 0) { // up right
                undoShift(UP, start[0], WEST);
                undoShift(UP2, start[0], WEST);
                undoShift(start[1], RIGHT, EAST);
                undoShift(UP, RIGHT, EAST);
            } else { // up left
                undoShift(UP, start[0], EAST);
                undoShift(UP2, start[0], EAST);
                undoShift(start[1], LEFT, WEST);
                undoShift(UP, LEFT, WEST);
            }
        } else { // down
            if (col_dif > 0) { // down right
                undoShift(DOWN, start[0], WEST);
                undoShift(DOWN2, start[0], WEST);
                undoShift(start[1], RIGHT, EAST);
                undoShift(DOWN, RIGHT, EAST);
            }
            else { // down left
                undoShift(DOWN, start[0], EAST);
                undoShift(DOWN2, start[0], EAST);
                undoShift(start[1], LEFT, WEST);
                undoShift(DOWN, LEFT, WEST);
            }
        }
    }
}

void shiftKnight(int start[2], int end[2]) {
    int col_dif = end[0] - start[0];
    int row_dif = end[1] - start[1];

    Up();
    if (abs(col_dif) > abs(row_dif)) { // horizontal
        if (col_dif > 0) { // right
            if (row_dif > 0) { // right up
                moveRelative(NORTH, 4);
                moveRelative(EAST, 16);
                moveRelative(NORTH, 4);
            } else { // right down
                moveRelative(SOUTH, 4);
                moveRelative(EAST, 16);
                moveRelative(SOUTH, 4);
            }
        } else { // left
            if (row_dif > 0) { // left up
                moveRelative(NORTH, 4);
                moveRelative(WEST, 16);
                moveRelative(NORTH, 4);
            } else { // left down
                moveRelative(SOUTH, 4);
                moveRelative(WEST, 16);
                moveRelative(SOUTH, 4);
            }
        }
    } else { // vertical
        if (row_dif > 0) { // up
            if (col_dif > 0) { // up right
                moveRelative(EAST, 4);
                moveRelative(NORTH, 16);
                moveRelative(EAST, 4);
            } else { // up left
                moveRelative(WEST, 4);
                moveRelative(NORTH, 16);
                moveRelative(WEST, 4);
            }
        } else { // down
            if (col_dif > 0) { // down right
                moveRelative(EAST, 4);
                moveRelative(SOUTH, 16);
                moveRelative(EAST, 4);
            }
            else { // down left
                moveRelative(WEST, 4);
                moveRelative(SOUTH, 16);
                moveRelative(WEST, 4);
            }
        }
    }
    Down();
    currpos[0] = end[0];
	currpos[1] = end[1];
}

void moveKnight(int start[2], int end[2]) {
    shiftAll(start, end);
    forcemove(currpos, start);
    shiftKnight(start, end);
    undoshiftAll(start, end);
    forcemove(currpos, end);
    currpos[0] = end[0];
    currpos[1] = end[1];
}

//END GABE CODE

void play_audio() {
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, 0); // Set GPIO low
    HAL_Delay(1000);
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, 1);
}

void init() {
    ILI9341_Unselect();
    ILI9341_Init();
}

void diagonalmove(int hdir, int vdir) {
	if (hdir == 0) {
		HAL_GPIO_WritePin(HDIR_GPIO_Port, HDIR_Pin, GPIO_PIN_RESET); //move to the right (8->1)
	}
	else {
		HAL_GPIO_WritePin(HDIR_GPIO_Port, HDIR_Pin, GPIO_PIN_SET); //move to the left (1->8)
	}

	if (vdir == 0) {
		HAL_GPIO_WritePin(VDIR_GPIO_Port, VDIR_Pin, GPIO_PIN_SET); //move up (8->1)
	}
	else {
		HAL_GPIO_WritePin(VDIR_GPIO_Port, VDIR_Pin, GPIO_PIN_RESET); // move down (1->8)
	}
	hmove = 1;
	vmove = 1;
	countmax = 2225;
	while (vmove == 1 || hmove == 1) {}
}

void homing()
{
	int xgo = 1;
	int ygo = 1;
	while (xgo == 1) {
		HAL_GPIO_WritePin(HDIR_GPIO_Port, HDIR_Pin, GPIO_PIN_SET); //move left
		hmove = 1;
	    countmax = 1; // change to 1/2 square size
	    while (hmove == 1) {}
	    if (HAL_GPIO_ReadPin(xlimit_GPIO_Port, xlimit_Pin) == 0) {
	    	xgo = 0;
	    }
	}
	while (ygo == 1) {
		HAL_GPIO_WritePin(VDIR_GPIO_Port, VDIR_Pin, GPIO_PIN_RESET); //move down
		vmove = 1;
	    countmax = 1; // change to 1/2 square size
	    while (vmove == 1) {}
	    if (HAL_GPIO_ReadPin(ylimit_GPIO_Port, ylimit_Pin) == 0) {
	    	ygo = 0;
	    }
	}
}

void movefrom(int st[2], int e[2]) { //st[col, row] e[col, row] (1-8)
	int hdist = e[0] - st[0]; // start col - end col = horizontal dist
	int vdist = e[1] - st[1]; // start row - end row = vertical dist
	int hdir = 1;
	int vdir = 1;
	if (hdist < 0) {
		hdist *= -1;
		hdir = 0;
	}
	if (vdist < 0) {
		vdist *= -1;
		vdir = 0;
	}
	if (vdist == 0) { // moving only horizontally
		Up();
		for (int i = 0; i < hdist; i++) {
			hormove(hdir); //move 1 full inch
			hormove(hdir);
		}
		Down();
 	}
	else if (hdist == 0) {
		Up();
		for (int i = 0; i < vdist; i++) {
			vertmove(vdir); //move 1 full inch
			vertmove(vdir);
		}
		Down();
	}
	else if (hdist == vdist) {
		Up();
		for (int i = 0; i < vdist; i++) {
			diagonalmove(hdir, vdir);
			diagonalmove(hdir, vdir);
		}
		Down();
	}
	else {
		moveKnight(st, e);//knight move
	}
}



void readsensor() {
	//col 1
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2,  1);
	GPIO_PinState pin_c1r1 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_2);
	GPIO_PinState pin_c1r2 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_4);
	GPIO_PinState pin_c1r3 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_5);
	GPIO_PinState pin_c1r4 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_6);
	GPIO_PinState pin_c1r5 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_3);
	GPIO_PinState pin_c1r6 = HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_8);
	GPIO_PinState pin_c1r7 = HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_7);
	GPIO_PinState pin_c1r8 = HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_9);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2,  0);

	//col 2
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6,  1);
	GPIO_PinState pin_c2r1 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_2);
	GPIO_PinState pin_c2r2 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_4);
	GPIO_PinState pin_c2r3 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_5);
	GPIO_PinState pin_c2r4 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_6);
	GPIO_PinState pin_c2r5 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_3);
	GPIO_PinState pin_c2r6 = HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_8);
	GPIO_PinState pin_c2r7 = HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_7);
	GPIO_PinState pin_c2r8 = HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_9);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6,  0);

	//col 3
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_2,  1);
	GPIO_PinState pin_c3r1 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_2);
	GPIO_PinState pin_c3r2 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_4);
	GPIO_PinState pin_c3r3 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_5);
	GPIO_PinState pin_c3r4 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_6);
	GPIO_PinState pin_c3r5 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_3);
	GPIO_PinState pin_c3r6 = HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_8);
	GPIO_PinState pin_c3r7 = HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_7);
	GPIO_PinState pin_c3r8 = HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_9);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_2,  0);

	//col 4
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_1,  1);
	GPIO_PinState pin_c4r1 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_2);
	GPIO_PinState pin_c4r2 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_4);
	GPIO_PinState pin_c4r3 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_5);
	GPIO_PinState pin_c4r4 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_6);
	GPIO_PinState pin_c4r5 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_3);
	GPIO_PinState pin_c4r6 = HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_8);
	GPIO_PinState pin_c4r7 = HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_7);
	GPIO_PinState pin_c4r8 = HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_9);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_1,  0);

	//col 5
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_0,  1);
	GPIO_PinState pin_c5r1 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_2);
	GPIO_PinState pin_c5r2 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_4);
	GPIO_PinState pin_c5r3 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_5);
	GPIO_PinState pin_c5r4 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_6);
	GPIO_PinState pin_c5r5 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_3);
	GPIO_PinState pin_c5r6 = HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_8);
	GPIO_PinState pin_c5r7 = HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_7);
	GPIO_PinState pin_c5r8 = HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_9);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_0,  0);

	//col 6
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0,  1);
	GPIO_PinState pin_c6r1 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_2);
	GPIO_PinState pin_c6r2 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_4);
	GPIO_PinState pin_c6r3 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_5);
	GPIO_PinState pin_c6r4 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_6);
	GPIO_PinState pin_c6r5 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_3);
	GPIO_PinState pin_c6r6 = HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_8);
	GPIO_PinState pin_c6r7 = HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_7);
	GPIO_PinState pin_c6r8 = HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_9);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0,  0);

	//col 7
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1,  1);
	GPIO_PinState pin_c7r1 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_2);
	GPIO_PinState pin_c7r2 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_4);
	GPIO_PinState pin_c7r3 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_5);
	GPIO_PinState pin_c7r4 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_6);
	GPIO_PinState pin_c7r5 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_3);
	GPIO_PinState pin_c7r6 = HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_8);
	GPIO_PinState pin_c7r7 = HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_7);
	GPIO_PinState pin_c7r8 = HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_9);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1,  0);

	//col 8
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_0,  1);
	GPIO_PinState pin_c8r1 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_2);
	GPIO_PinState pin_c8r2 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_4);
	GPIO_PinState pin_c8r3 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_5);
	GPIO_PinState pin_c8r4 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_6);
	GPIO_PinState pin_c8r5 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_3);
	GPIO_PinState pin_c8r6 = HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_8);
	GPIO_PinState pin_c8r7 = HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_7);
	GPIO_PinState pin_c8r8 = HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_9);
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_0,  0);

	printf("%d | %d | %d | %d | %d | %d | %d | %d\n\r", pin_c1r1, pin_c2r1, pin_c3r1, pin_c4r1, pin_c5r1, pin_c6r1, pin_c7r1, pin_c8r1);
	printf("%d | %d | %d | %d | %d | %d | %d | %d\n\r", pin_c1r2, pin_c2r2, pin_c3r2, pin_c4r2, pin_c5r2, pin_c6r2, pin_c7r2, pin_c8r2);
	printf("%d | %d | %d | %d | %d | %d | %d | %d\n\r", pin_c1r3, pin_c2r3, pin_c3r3, pin_c4r3, pin_c5r3, pin_c6r3, pin_c7r3, pin_c8r3);
	printf("%d | %d | %d | %d | %d | %d | %d | %d\n\r", pin_c1r4, pin_c2r4, pin_c3r4, pin_c4r4, pin_c5r4, pin_c6r4, pin_c7r4, pin_c8r4);
	printf("%d | %d | %d | %d | %d | %d | %d | %d\n\r", pin_c1r5, pin_c2r5, pin_c3r5, pin_c4r5, pin_c5r5, pin_c6r5, pin_c7r5, pin_c8r5);
	printf("%d | %d | %d | %d | %d | %d | %d | %d\n\r", pin_c1r6, pin_c2r6, pin_c3r6, pin_c4r6, pin_c5r6, pin_c6r6, pin_c7r6, pin_c8r6);
	printf("%d | %d | %d | %d | %d | %d | %d | %d\n\r", pin_c1r7, pin_c2r7, pin_c3r7, pin_c4r7, pin_c5r7, pin_c6r7, pin_c7r7, pin_c8r7);
	printf("%d | %d | %d | %d | %d | %d | %d | %d\n\r", pin_c1r8, pin_c2r8, pin_c3r8, pin_c4r8, pin_c5r8, pin_c6r8, pin_c7r8, pin_c8r8);

	GPIO_PinState* pins[8][8] = {
	    {&pin_c1r1, &pin_c2r1, &pin_c3r1, &pin_c4r1, &pin_c5r1, &pin_c6r1, &pin_c7r1, &pin_c8r1},
	    {&pin_c1r2, &pin_c2r2, &pin_c3r2, &pin_c4r2, &pin_c5r2, &pin_c6r2, &pin_c7r2, &pin_c8r2},
	    {&pin_c1r3, &pin_c2r3, &pin_c3r3, &pin_c4r3, &pin_c5r3, &pin_c6r3, &pin_c7r3, &pin_c8r3},
	    {&pin_c1r4, &pin_c2r4, &pin_c3r4, &pin_c4r4, &pin_c5r4, &pin_c6r4, &pin_c7r4, &pin_c8r4},
	    {&pin_c1r5, &pin_c2r5, &pin_c3r5, &pin_c4r5, &pin_c5r5, &pin_c6r5, &pin_c7r5, &pin_c8r5},
	    {&pin_c1r6, &pin_c2r6, &pin_c3r6, &pin_c4r6, &pin_c5r6, &pin_c6r6, &pin_c7r6, &pin_c8r6},
	    {&pin_c1r7, &pin_c2r7, &pin_c3r7, &pin_c4r7, &pin_c5r7, &pin_c6r7, &pin_c7r7, &pin_c8r7},
	    {&pin_c1r8, &pin_c2r8, &pin_c3r8, &pin_c4r8, &pin_c5r8, &pin_c6r8, &pin_c7r8, &pin_c8r8}
	};

	// Update tempstate with values from pin variables
	for (int row = 0; row < 8; row++) {
	    for (int col = 0; col < 8; col++) {
	        tempstate[row][col] = *pins[row][col];
	    }
	}

}


void loop() {
    if(HAL_SPI_DeInit(&hspi1) != HAL_OK) {
        while (1);
        return;
    }

    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;

    if(HAL_SPI_Init(&hspi1) != HAL_OK) {
    	while (1);
        return;
    }

    // Check border
    ILI9341_FillScreen(ILI9341_BLACK);

    for(int x = 0; x < ILI9341_WIDTH; x++) {
        ILI9341_DrawPixel(x, 0, ILI9341_RED);
        ILI9341_DrawPixel(x, ILI9341_HEIGHT-1, ILI9341_RED);
    }

    for(int y = 0; y < ILI9341_HEIGHT; y++) {
        ILI9341_DrawPixel(0, y, ILI9341_RED);
        ILI9341_DrawPixel(ILI9341_WIDTH-1, y, ILI9341_RED);
    }

    HAL_Delay(3000);

    // Check fonts
    ILI9341_FillScreen(ILI9341_BLACK);
    ILI9341_WriteString(0, 0, "Font_7x10, red on black, lorem ipsum dolor sit amet", Font_7x10, ILI9341_RED, ILI9341_BLACK);
    ILI9341_WriteString(0, 3*10, "Font_11x18, green, lorem ipsum dolor sit amet", Font_11x18, ILI9341_GREEN, ILI9341_BLACK);
    ILI9341_WriteString(0, 3*10+3*18, "Font_16x26, blue, lorem ipsum dolor sit amet", Font_16x26, ILI9341_BLUE, ILI9341_BLACK);

    HAL_Delay(1000);
    ILI9341_InvertColors(true);
    HAL_Delay(1000);
    ILI9341_InvertColors(false);

    HAL_Delay(5000);

    // Check colors
    ILI9341_FillScreen(ILI9341_WHITE);
    ILI9341_WriteString(0, 0, "WHITE", Font_11x18, ILI9341_BLACK, ILI9341_WHITE);
    HAL_Delay(500);

    ILI9341_FillScreen(ILI9341_BLUE);
    ILI9341_WriteString(0, 0, "BLUE", Font_11x18, ILI9341_BLACK, ILI9341_BLUE);
    HAL_Delay(500);

    ILI9341_FillScreen(ILI9341_RED);
    ILI9341_WriteString(0, 0, "RED", Font_11x18, ILI9341_BLACK, ILI9341_RED);
    HAL_Delay(500);

    ILI9341_FillScreen(ILI9341_GREEN);
    ILI9341_WriteString(0, 0, "GREEN", Font_11x18, ILI9341_BLACK, ILI9341_GREEN);
    HAL_Delay(500);

    ILI9341_FillScreen(ILI9341_CYAN);
    ILI9341_WriteString(0, 0, "CYAN", Font_11x18, ILI9341_BLACK, ILI9341_CYAN);
    HAL_Delay(500);

    ILI9341_FillScreen(ILI9341_MAGENTA);
    ILI9341_WriteString(0, 0, "MAGENTA", Font_11x18, ILI9341_BLACK, ILI9341_MAGENTA);
    HAL_Delay(500);

    ILI9341_FillScreen(ILI9341_YELLOW);
    ILI9341_WriteString(0, 0, "YELLOW", Font_11x18, ILI9341_BLACK, ILI9341_YELLOW);
    HAL_Delay(500);

    ILI9341_FillScreen(ILI9341_BLACK);
    ILI9341_WriteString(0, 0, "BLACK", Font_11x18, ILI9341_WHITE, ILI9341_BLACK);
    HAL_Delay(500);

    HAL_Delay(3000);

    ILI9341_FillScreen(ILI9341_BLACK);
    ILI9341_WriteString(0, 0, "Touchpad test.  Draw something!", Font_11x18, ILI9341_WHITE, ILI9341_BLACK);
    HAL_Delay(1000);
    ILI9341_FillScreen(ILI9341_BLACK);

    if(HAL_SPI_DeInit(&hspi1) != HAL_OK) {
    	while (1);
        return;
    }

    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;

    if(HAL_SPI_Init(&hspi1) != HAL_OK) {
    	while (1);
        return;
    }
}

void playerlcd() {
	ILI9341_WriteString(2, 2, "CHESSBOT TURN", Font_16x26, ILI9341_WHITE, ILI9341_WHITE);
	ILI9341_WriteString(2, 2, "TAKING PIECE", Font_16x26, ILI9341_WHITE, ILI9341_WHITE);
    ILI9341_WriteString(2, 2, "PLAYER TURN", Font_16x26, ILI9341_GREEN, ILI9341_WHITE);
    HAL_Delay(500);
}

void botlcd() {
	ILI9341_WriteString(2, 2, "PLAYER TURN", Font_16x26, ILI9341_WHITE, ILI9341_WHITE);
	ILI9341_WriteString(2, 2, "TAKING PIECE", Font_16x26, ILI9341_WHITE, ILI9341_WHITE);
	ILI9341_WriteString(2, 2, "CHESSBOT TURN", Font_16x26, ILI9341_BLACK, ILI9341_WHITE);
	HAL_Delay(500);
}
void takinglcd() {
	ILI9341_WriteString(2, 2, "PLAYER TURN", Font_16x26, ILI9341_WHITE, ILI9341_WHITE);
	ILI9341_WriteString(2, 2, "CHESSBOT TURN", Font_16x26, ILI9341_WHITE, ILI9341_WHITE);
	ILI9341_WriteString(2, 2, "TAKING PIECE", Font_16x26, ILI9341_RED, ILI9341_WHITE);
	HAL_Delay(500);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM4_Init();
  MX_SPI1_Init();
  MX_USART2_UART_Init();
  MX_LPUART1_UART_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_OC_Start_IT(&htim4, TIM_CHANNEL_2);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);
  init(); // LCD
//  ILI9341_FillScreen(ILI9341_WHITE);
  //SET INITIAL STATE TO BOARD STATE
 play_audio();
  printf("\r\n");
  readsensor();
  printf("\r\n");
  for (int row = 0; row < 8; row++) {
	  for (int col = 0; col < 8; col++) {
		  currstate[row][col] = tempstate[row][col];
	  }
  }
  //SET INITIAL STATE TO BOARD STATE
	playerlcd();
	Down();
	homing();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	//TEST CODE 2
	//TEST END 2

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  // only poll after button is pressed if (poll) then poll else skip
	  if (poll == 1) {
	  	  int start[2] = {0,0};
	  	  int end[2] = {0,0};
		  readsensor(); // updates tempstate to the board state
		  for (int row = 0; row < 8; row++) { // compare tempstate and currstate to find the move
			  for (int col = 0; col < 8; col++) {
				  // Check for the start condition
				  if (currstate[row][col] == 1 && tempstate[row][col] == 0) {
					  start[0] = col+1;
					  start[1] = row+1;
				  }
				  // Check for the end condition
				  else if (currstate[row][col] == 0 && tempstate[row][col] == 1) {
					  end[0] = col+1;
					  end[1] = row+1;
				  }
			  }
		  }
		  if (end[0] == 0) {
			  HAL_Delay(200);
			  poll = 0;
			  int x = 1;
			  end[0] = start[0];
			  end[1] = start[1];
			  takinglcd();
			  while (x == 1) {
				  if (poll == 1) {
					  readsensor();
					  for (int row = 0; row < 8; row++) { // compare tempstate and currstate to find the move
						  for (int col = 0; col < 8; col++) {
							  // Check for the start condition
							  if (currstate[row][col] == 1 && tempstate[row][col] == 0) {
								  start[0] = col+1;
								  start[1] = row+1;
							  }
						  }
					  }
					  x = 0;
				  }
			  }
		  }
		  botlcd();
		  txbuff[0] = start[0];
		  txbuff[1] = start[1];
		  txbuff[2] = end[0];
		  txbuff[3] = end[1];
		  if (txbuff[0] != 0) {
			  HAL_UART_Transmit(&huart2, txbuff, 5, 2000); // Send the move to the python code and return the next move
			  HAL_UART_Receive(&huart2, rxbuff, 5, HAL_MAX_DELAY);
			  printf("%d, %d, %d, %d \r\n", rxbuff[0], rxbuff[1], rxbuff[2], rxbuff[3]);
			  snextmove[0] = rxbuff[0] - 48; // Convert ASCII to integer
			  snextmove[1] = rxbuff[1] - 48;
			  enextmove[0] = rxbuff[2] - 48;
			  enextmove[1] = rxbuff[3] - 48;
		  }
		  Down();
		  forcemove(currpos, snextmove); // Perform a non-piece move
		  currpos[0] = snextmove[0];
		  currpos[1] = snextmove[1];
		  movefrom(snextmove, enextmove); // Move a piece based on its type

		  // update currstate to tempstate, current position to end of last move and reset poll
		  readsensor(); //update tempstate to the board state
		  for (int row = 0; row < 8; row++) {
			  for (int col = 0; col < 8; col++) {
				  currstate[row][col] = tempstate[row][col];
			  }
		  }
//		  currstate[snextmove[1]][snextmove[0]] = 0;
//		  currstate[enextmove[1]][enextmove[0]] = 1;

		  // Update the current position and reset poll
		  memset(rxbuff, 0, sizeof(rxbuff));  // Clear the UART Data Register
		  currpos[0] = enextmove[0];
		  currpos[1] = enextmove[1];

		  txbuff[0] = 0;
		  txbuff[1] = 0;
		  txbuff[2] = 0;
		  txbuff[3] = 0;

		  poll = 0;
		  playerlcd();
	  }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief LPUART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_LPUART1_UART_Init(void)
{

  /* USER CODE BEGIN LPUART1_Init 0 */

  /* USER CODE END LPUART1_Init 0 */

  /* USER CODE BEGIN LPUART1_Init 1 */

  /* USER CODE END LPUART1_Init 1 */
  hlpuart1.Instance = LPUART1;
  hlpuart1.Init.BaudRate = 115200;
  hlpuart1.Init.WordLength = UART_WORDLENGTH_8B;
  hlpuart1.Init.StopBits = UART_STOPBITS_1;
  hlpuart1.Init.Parity = UART_PARITY_NONE;
  hlpuart1.Init.Mode = UART_MODE_TX_RX;
  hlpuart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  hlpuart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  hlpuart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  hlpuart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  hlpuart1.FifoMode = UART_FIFOMODE_DISABLE;
  if (HAL_UART_Init(&hlpuart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&hlpuart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&hlpuart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&hlpuart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LPUART1_Init 2 */

  /* USER CODE END LPUART1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */
  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 1;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 2000;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_OC_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_TOGGLE;
  sConfigOC.Pulse = 1000;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_OC_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  __HAL_TIM_ENABLE_OCxPRELOAD(&htim4, TIM_CHANNEL_2);
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  HAL_PWREx_EnableVddIO2();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|Track1_Pin
                          |GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, VSTEP_Pin|HSTEP_Pin|VDIR_Pin|HDIR_Pin
                          |GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4|GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_2|GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_0, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(Actuator_EN_GPIO_Port, Actuator_EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_0|GPIO_PIN_1, GPIO_PIN_RESET);

  /*Configure GPIO pins : PE2 PE3 PE4 PE5
                           PE6 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PF0 PF1 PF2 Track1_Pin
                           PF15 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|Track1_Pin
                          |GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pins : PF7 PF8 PF9 */
  GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pins : VSTEP_Pin HSTEP_Pin VDIR_Pin HDIR_Pin
                           PC5 */
  GPIO_InitStruct.Pin = VSTEP_Pin|HSTEP_Pin|VDIR_Pin|HDIR_Pin
                          |GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PC2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG_ADC_CONTROL;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PA1 PA3 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG_ADC_CONTROL;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PA4 PA6 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB2 PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_2|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB1 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG_ADC_CONTROL;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PG0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pins : PE7 PE8 PE10 PE11
                           PE12 PE13 */
  GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12|GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : Actuator_EN_Pin */
  GPIO_InitStruct.Pin = Actuator_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(Actuator_EN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PE14 */
  GPIO_InitStruct.Pin = GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF3_TIM1_COMP1;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : xlimit_Pin */
  GPIO_InitStruct.Pin = xlimit_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(xlimit_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : ylimit_Pin */
  GPIO_InitStruct.Pin = ylimit_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(ylimit_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PB12 PB13 PB15 */
  GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF13_SAI2;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB14 */
  GPIO_InitStruct.Pin = GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF14_TIM15;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PD8 PD9 PD0 PD1 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : PC6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF13_SAI2;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PC7 */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PC8 PC9 PC10 PC11
                           PC12 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_SDMMC1;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA8 PA10 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA9 */
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PD2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_SDMMC1;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : PB3 PB4 PB5 */
  GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : Move_Button_Pin Take_Button_Pin */
  GPIO_InitStruct.Pin = Move_Button_Pin|Take_Button_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
PUTCHAR_PROTOTYPE
{
  HAL_UART_Transmit(&hlpuart1, (uint8_t *)&ch, 1, 0xFFFF);
  return ch;
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  /* Prevent unused argument(s) compilation warning */
	if (GPIO_Pin == GPIO_PIN_8) {
		poll = 1;
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
