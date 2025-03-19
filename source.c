#include "helper.h"
#include "source.h"

static Node* head = NULL;
static int food[2];
static int key_pressed = 0;
static bool is_game_over = FALSE;
static bool is_game_paused = FALSE;
static int score = 0;

void handleTimeTick()
{
    handleKeyPress();

    if (key_pressed != 0)
    {
        if (head->x == food[0] && head->y == food[1]) {     // snake eats food
            score++;
            do {
                food[0] = generate_random_int(2, GRID_SIZE - 2);
                food[1] = generate_random_int(2, GRID_SIZE - 2);
            } while (linked_list_contains(head, food[0], food[1]));
        }
        else linked_list_remove_back(head);

        if (isSnakeDead(head)) {
            key_pressed = 0;
            is_game_over = TRUE;
        }
    }
}

void handleKeyPress()
{
    switch (key_pressed)
    {
        case VK_RIGHT:  head = linked_list_append(head, head->x + 1, head->y); break;
        case VK_LEFT:   head = linked_list_append(head, head->x - 1, head->y); break;
        case VK_UP:     head = linked_list_append(head, head->x, head->y - 1); break;
        case VK_DOWN:   head = linked_list_append(head, head->x, head->y + 1); break;
        default: break;
    }
}

bool isSnakeDead(Node* head)
{
    if (linked_list_contains(head->next, head->x, head->y)) return TRUE;                                        // snake eats itself
    if (head->x <= 1 || head->x >= GRID_SIZE - 1 || head->y <= 1 || head->y >= GRID_SIZE - 1) return TRUE;      // snake hits wall
    return FALSE;
}

void verifyAndSetKeyPressed(unsigned long long wParam)
{
    // exit for invalid arrow key press
    switch (wParam)
    {
        case VK_RIGHT:  if (head->x + 1 == head->next->x && head->y == head->next->y) return; break;
        case VK_LEFT:   if (head->x - 1 == head->next->x && head->y == head->next->y) return; break;
        case VK_UP:     if (head->x == head->next->x && head->y - 1 == head->next->y) return; break;
        case VK_DOWN:   if (head->x == head->next->x && head->y + 1 == head->next->y) return; break;
    }
    key_pressed = (int)wParam;
}

void displayAgent(HDC hdc)
{
    Node* curr = head;
    while (curr != NULL) {
        drawAtLocation(hdc, curr->x, curr->y);
        curr = curr->next;
    }
}

void displayScoreText(HDC hdc)
{
    char str_score[5] = "", text[12] = "";
    sprintf_s(str_score, sizeof(str_score), "%d", score);
    multi_concat(text, sizeof(text), 2, "SCORE: ", str_score);

    SetTextColor(hdc, TEXT_COLOR);
    SetBkMode(hdc, TRANSPARENT);

    SIZE text_size = { 0 };
    GetTextExtentPoint32A(hdc, text, lstrlenA(text), &text_size);
    TextOutA(hdc, WIN_CENTER * CELL_SIZE - text_size.cx / 2, 0, text, lstrlenA(text));
}

void drawAtLocation(HDC hdc, int x, int y)
{
    Rectangle(hdc, x * CELL_SIZE, y * CELL_SIZE, (x + 1) * CELL_SIZE, (y + 1) * CELL_SIZE);
}

void drawBounds(HDC hdc)
{
    Rectangle(hdc, 0, 0, CELL_SIZE, WIN_HEIGHT);
    Rectangle(hdc, 0, 0, WIN_WIDTH, CELL_SIZE);
    Rectangle(hdc, WIN_WIDTH - CELL_SIZE, 0, WIN_WIDTH, WIN_HEIGHT);
    Rectangle(hdc, 0, WIN_HEIGHT - CELL_SIZE, WIN_WIDTH, WIN_HEIGHT);
}

void drawGameOverScreen(HDC hdc)
{
    Rectangle(hdc, GAME_OVER_SCREEN_COORDS.left, GAME_OVER_SCREEN_COORDS.top, GAME_OVER_SCREEN_COORDS.right, GAME_OVER_SCREEN_COORDS.bottom);

    const char* text = "GAME OVER";

    HFONT hFont = CreateFontA(40, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_SWISS, TEXT_FONT);
    HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

    SetTextColor(hdc, TEXT_COLOR);
    SetBkMode(hdc, TRANSPARENT);

    SIZE text_size = { 0 };
    GetTextExtentPoint32A(hdc, text, lstrlenA(text), &text_size);
    TextOutA(hdc, WIN_CENTER * CELL_SIZE - text_size.cx / 2, WIN_CENTER * CELL_SIZE - text_size.cy / 2, text, lstrlenA(text));
    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);
}

void handleDrawing(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    // clear background
    HBRUSH bgBrush = CreateSolidBrush(BG_COLOR);
    FillRect(hdc, &ps.rcPaint, bgBrush);
    DeleteObject(bgBrush);

    // draw boundaries
    HBRUSH boundsBrush = CreateSolidBrush(BOUNDS_COLOR);
    HBRUSH oldBrush = SelectObject(hdc, boundsBrush);
    drawBounds(hdc);
    SelectObject(hdc, oldBrush);
    DeleteObject(boundsBrush);

    // display agent
    HBRUSH agentBrush = CreateSolidBrush(AGENT_COLOR);
    oldBrush = SelectObject(hdc, agentBrush);
    displayAgent(hdc);
    SelectObject(hdc, oldBrush);
    DeleteObject(agentBrush);

    // display food
    HBRUSH foodBrush = CreateSolidBrush(FOOD_COLOR);
    oldBrush = SelectObject(hdc, foodBrush);
    drawAtLocation(hdc, food[0], food[1]);
    SelectObject(hdc, oldBrush);
    DeleteObject(foodBrush);

    // display score
    displayScoreText(hdc);

    // display game over screen
    if (is_game_over) {
        HBRUSH gameoverBrush = CreateSolidBrush(BG_GAME_OVER_COLOR);
        oldBrush = SelectObject(hdc, gameoverBrush);
        drawGameOverScreen(hdc);
        SelectObject(hdc, oldBrush);
        DeleteObject(gameoverBrush);
    }
    EndPaint(hwnd, &ps);
}

void initializeObjects()
{
    if (head != NULL) free_linked_list(head);
    head = (Node*)malloc(sizeof(Node));
    head->x = WIN_CENTER;
    head->y = WIN_CENTER + 1;
    head->next = NULL;

    head = linked_list_append(head, WIN_CENTER, WIN_CENTER);
    head = linked_list_append(head, WIN_CENTER, WIN_CENTER - 1);

    food[0] = FOOD_START_POS[0];
    food[1] = FOOD_START_POS[1];
}

// Window procedure function to handle events
long long __stdcall WindowProc(HWND hwnd, UINT uMsg, unsigned long long wParam, long long lParam) {
    switch (uMsg) {
    case WM_PAINT:
        handleDrawing(hwnd);
        break;
    case WM_KEYDOWN:
        if (!is_game_over && !is_game_paused && is_arrow_key(wParam)) verifyAndSetKeyPressed(wParam);
        else if (!is_game_over && key_pressed != 0 && wParam == VK_SPACE) is_game_paused = !is_game_paused;
        else if (is_game_over && wParam == VK_SPACE) {
            initializeObjects();
            is_game_over = FALSE;
            is_game_paused = FALSE;
            score = 0;
        }
        break;
    case WM_TIMER:
        if (!is_game_over && !is_game_paused) {
            handleTimeTick();
            InvalidateRect(hwnd, NULL, TRUE);   // Request complete window redraw
        }
        break;
    case WM_DESTROY:
        KillTimer(hwnd, 1);
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcA(hwnd, uMsg, wParam, lParam);
}

// Entry point for Windows applications
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char* lpCmdLine, int nCmdShow) {
    // initialize objects
    initializeObjects();

    // setup the window
    WNDCLASSA wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "BasicWindowClass";

    RegisterClassA(&wc);

    RECT adjustedRect = { 0, 0, WIN_WIDTH, WIN_HEIGHT };
    AdjustWindowRect(&adjustedRect, WS_OVERLAPPEDWINDOW, FALSE);

    int actualWidth = adjustedRect.right - adjustedRect.left;
    int actualHeight = adjustedRect.bottom - adjustedRect.top;

    // create a window (having no maximize/minimize, and being non-resizeable)
    HWND hwnd = CreateWindowExA(
        0, "BasicWindowClass", "Snake",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        actualWidth,
        actualHeight,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    ShowWindow(hwnd, nCmdShow);
    SetTimer(hwnd, 1, 100, NULL);    // Timer event every 100 ms

    MSG msg = { 0 };
    while (GetMessageA(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
    return 0;
}