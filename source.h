#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <windows.h>
#include <stdio.h>

#define WIN_HEIGHT		400
#define WIN_WIDTH		400
#define AGENT_HEIGHT	20
#define AGENT_WIDTH		20
#define CELL_SIZE		15

const int GRID_SIZE					= (WIN_WIDTH / CELL_SIZE);
const int WIN_CENTER				= (WIN_WIDTH / CELL_SIZE) / 2;
const int FOOD_START_POS[2]			= { (WIN_WIDTH / CELL_SIZE) / 2, (WIN_WIDTH / CELL_SIZE) / 4 };
const COLORREF BG_COLOR				= RGB(31, 39, 17);		// Very Dark Olive Green
const COLORREF BOUNDS_COLOR			= RGB(53, 66, 29);		// Dark Olive Green
const COLORREF BG_GAME_OVER_COLOR	= RGB(54, 69, 79);		// Dark Grey
const COLORREF AGENT_COLOR			= RGB(139, 175, 77);	// Light Olive Green
const COLORREF FOOD_COLOR			= RGB(220, 20, 60);		// Crimson
const COLORREF TEXT_COLOR			= RGB(192, 192, 192);	// Light Grey
const RECT GAME_OVER_SCREEN_COORDS	= { WIN_WIDTH / 8, WIN_HEIGHT / 3, 7 * WIN_WIDTH / 8, 2 * WIN_HEIGHT / 3 };
const char* TEXT_FONT				= "Old English Text MT";

void initializeObjects();
void drawAtLocation(HDC hdc, int x, int y);
void drawBounds(HDC hdc);
void displayScoreText(HDC hdc);
void drawGameOverScreen(HDC hdc);
void handleDrawing(HWND hwnd);
void handleTimeTick();
void handleKeyPress();
void displayAgent(HDC hdc);
void verifyAndSetKeyPressed(unsigned long long wParam);
bool isSnakeDead(Node* head);

#endif