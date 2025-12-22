#define NOMINMAX
#include <windows.h>
#include <iostream>
#include <vector>
#include <omp.h>
#include <random>
#include <string>

// --- Налаштування симуляції ---
const int N = 60;
const int CELL_SIZE = 12;
const int WINDOW_SIZE = N * CELL_SIZE;

// --- Стани клітин (ті самі) ---
const int EMPTY = 0;
const int TREE = 1;
const int BUSH = 2;
const int BURNING_BUSH = 5;
const int BURNING_TREE_GEN1 = 6;
const int BURNING_TREE_GEN2 = 7;

using Grid = std::vector<std::vector<int>>;

// --- Глобальні змінні ---
Grid grid_a(N, std::vector<int>(N));
Grid grid_b(N, std::vector<int>(N));
Grid* current_grid = &grid_a;
Grid* next_grid = &grid_b;

bool fire_is_burning = false;
bool simulation_started = false; // <-- НОВИЙ ПРАПОР
int generation = 0;

int di_4[] = {-1, 1, 0, 0};
int dj_4[] = {0, 0, -1, 1};
int di_diag[] = {-1, -1, 1, 1};
int dj_diag[] = {-1, 1, -1, 1};

HBRUSH hBrushEmpty, hBrushTree, hBrushBush, hBrushFire1, hBrushFire2, hBrushFire3;

/**
 * @brief Ініціалізує поле ТІЛЬКИ деревами та кущами
 */
void initialize_grid(Grid& grid) {
    // --- НАЛАШТУЙТЕ ВІДСОТКИ ТУТ ---
    // (Сума не повинна перевищувати 100)
    const int PERCENT_EMPTY = 30; // <-- 15% шанс на порожню клітинку
    const int PERCENT_BUSH  = 30; // <-- 30% шанс на кущ
    // Решта (100 - 15 - 30 = 55%) буде ДЕРЕВОМ
    // ------------------------------------

    std::random_device rd;
    std::mt19937 gen(rd());
    // Генератор чисел від 1 до 100
    std::uniform_int_distribution<> dis(1, 100);

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            // "Кидаємо" 100-гранний кубик
            int roll = dis(gen);

            // Перевіряємо пороги
            if (roll <= PERCENT_EMPTY) {
                grid[i][j] = EMPTY;
            }
            else if (roll <= (PERCENT_EMPTY + PERCENT_BUSH)) {
                grid[i][j] = BUSH;
            }
            else {
                grid[i][j] = TREE;
            }
        }
    }
}

/**
 * @brief Логіка ОДНОГО покоління симуляції (без змін)
 */
void run_simulation_step() {
    if (!fire_is_burning) return;

    fire_is_burning = false;
    generation++;

    #pragma omp parallel for collapse(2) reduction(||:fire_is_burning)
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            int current_state = (*current_grid)[i][j];
            int new_state = current_state;

            switch (current_state) { /* ... (логіка горіння, без змін) ... */
                case BURNING_BUSH: new_state = EMPTY; break;
                case BURNING_TREE_GEN1: new_state = BURNING_TREE_GEN2; break;
                case BURNING_TREE_GEN2: new_state = EMPTY; break;
            }

            if (current_state == TREE || current_state == BUSH) {
                bool adjacent_fire = false;
                int diagonal_fire_count = 0;
                for (int k = 0; k < 4; ++k) {
                    int ni = i + di_4[k]; int nj = j + dj_4[k];
                    if (ni >= 0 && ni < N && nj >= 0 && nj < N && (*current_grid)[ni][nj] >= 5) {
                        adjacent_fire = true; break;
                    }
                }
                if (!adjacent_fire) {
                    for (int k = 0; k < 4; ++k) {
                        int ni = i + di_diag[k]; int nj = j + dj_diag[k];
                        if (ni >= 0 && ni < N && nj >= 0 && nj < N && (*current_grid)[ni][nj] >= 5) {
                            diagonal_fire_count++;
                        }
                    }
                }
                if (adjacent_fire || diagonal_fire_count >= 3) {
                    new_state = (current_state == TREE) ? BURNING_TREE_GEN1 : BURNING_BUSH;
                }
            }
            if (new_state >= 5) fire_is_burning = true;
            (*next_grid)[i][j] = new_state;
        }
    }
    std::swap(current_grid, next_grid);
}

/**
 * @brief Функція малювання (без змін)
 */
void OnPaint(HWND hWnd) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);

    HDC hdcMem = CreateCompatibleDC(hdc);
    HBITMAP hbmMem = CreateCompatibleBitmap(hdc, WINDOW_SIZE, WINDOW_SIZE);
    HANDLE hOld = SelectObject(hdcMem, hbmMem);

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            RECT r = { j * CELL_SIZE, i * CELL_SIZE, (j + 1) * CELL_SIZE, (i + 1) * CELL_SIZE };
            HBRUSH hCurrentBrush;
            switch ((*current_grid)[i][j]) {
                case EMPTY: hCurrentBrush = hBrushEmpty; break;
                case TREE: hCurrentBrush = hBrushTree; break;
                case BUSH: hCurrentBrush = hBrushBush; break;
                case BURNING_BUSH: hCurrentBrush = hBrushFire1; break;
                case BURNING_TREE_GEN1: hCurrentBrush = hBrushFire2; break;
                case BURNING_TREE_GEN2: hCurrentBrush = hBrushFire3; break;
                default: hCurrentBrush = hBrushEmpty; break;
            }
            FillRect(hdcMem, &r, hCurrentBrush);
        }
    }

    BitBlt(hdc, 0, 0, WINDOW_SIZE, WINDOW_SIZE, hdcMem, 0, 0, SRCCOPY);
    SelectObject(hdcMem, hOld);
    DeleteObject(hbmMem);
    DeleteDC(hdcMem);

    EndPaint(hWnd, &ps);
}

/**
 * @brief Головна "процедура вікна".
 */
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_CREATE:
            hBrushEmpty = CreateSolidBrush(RGB(30, 30, 30));
            hBrushTree = CreateSolidBrush(RGB(0, 100, 0));
            hBrushBush = CreateSolidBrush(RGB(50, 150, 50));
            hBrushFire1 = CreateSolidBrush(RGB(255, 255, 0)); // Yellow
            hBrushFire2 = CreateSolidBrush(RGB(255, 0, 0));   // Red
            hBrushFire3 = CreateSolidBrush(RGB(255, 140, 0)); // Orange

            // *** ТАЙМЕР БІЛЬШЕ НЕ ЗАПУСКАЄТЬСЯ ТУТ ***
            // SetTimer(hWnd, 1, 100, NULL);
            break;

        // --- НОВИЙ ОБРОБНИК ---
        case WM_LBUTTONDOWN: // Клік лівою кнопкою миші
        {
            // 1. Отримуємо піксельні координати
            int xPos = LOWORD(lParam);
            int yPos = HIWORD(lParam);

            // 2. Конвертуємо у координати сітки
            int j = xPos / CELL_SIZE; // j-колонка
            int i = yPos / CELL_SIZE; // i-рядок

            bool ignited_something = false;

            // 3. Перевіряємо межі та запалюємо
            if (i >= 0 && i < N && j >= 0 && j < N) {
                int state = (*current_grid)[i][j];
                if (state == TREE) {
                    (*current_grid)[i][j] = BURNING_TREE_GEN1;
                    ignited_something = true;
                } else if (state == BUSH) {
                    (*current_grid)[i][j] = BURNING_BUSH;
                    ignited_something = true;
                }
            }

            // 4. Якщо це ПЕРШИЙ вдалий підпал - запускаємо таймер
            if (!simulation_started && ignited_something) {
                simulation_started = true;
                fire_is_burning = true; // Даємо початковий поштовх
                SetTimer(hWnd, 1, 100, NULL);
            } else if (simulation_started && ignited_something) {
                // Якщо симуляція вже йде, просто додаємо нове джерело вогню
                fire_is_burning = true;
            }

            // 5. Негайно перемалювати, щоб показати клік
            InvalidateRect(hWnd, NULL, FALSE);
            break;
        }

        case WM_TIMER:
            // Таймер спрацьовує, лише якщо симуляція запущена
            if (simulation_started) {
                run_simulation_step();

                // Оновлюємо заголовок вікна
                std::wstring title = L"Симуляція пожежі | Покоління: " + std::to_wstring(generation);
                SetWindowText(hWnd, title.c_str());

                // Змушуємо вікно перемалюватися
                InvalidateRect(hWnd, NULL, FALSE);
            }
            break;

        case WM_PAINT:
            OnPaint(hWnd);
            break;

        case WM_ERASEBKGND:
            return 1;

        case WM_DESTROY:
            DeleteObject(hBrushEmpty);
            DeleteObject(hBrushTree);
            DeleteObject(hBrushBush);
            DeleteObject(hBrushFire1);
            DeleteObject(hBrushFire2);
            DeleteObject(hBrushFire3);

            KillTimer(hWnd, 1);
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

/**
 * @brief WinMain (без змін)
 */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    // Ініціалізуємо поле (тепер воно БЕЗ ВОГНЮ)
    initialize_grid(grid_a);

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"FireSimWindowClass";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClass(&wc);

    RECT wr = { 0, 0, WINDOW_SIZE, WINDOW_SIZE };
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

    HWND hWnd = CreateWindow(
        wc.lpszClassName,
        L"Симуляція пожежі (Win32/GDI) | Клікніть, щоб почати",
        WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT,
        wr.right - wr.left,
        wr.bottom - wr.top,
        NULL, NULL, hInstance, NULL
    );

    if (hWnd == NULL) return 0;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}