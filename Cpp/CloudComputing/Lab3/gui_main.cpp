#include <windows.h>
#include <string>
#include <vector>

#include <stringapiset.h> // Для MultiByteToWideChar

// ID для наших елементів управління
#define IDC_EDIT_N       101
#define IDC_EDIT_P       102
#define IDC_BUTTON_CALC  103
#define IDC_STATIC_RESULT 104

// Глобальні змінні для дескрипторів (handles)
HWND hEditN;
HWND hEditP;
HWND hStaticResult;

/**
 * @brief Функція для запуску нашого 'integral_calc.exe' та отримання результату
 * @param n_steps Кількість кроків
 * @param p_threads Кількість потоків
 * @return std::wstring - результат або повідомлення про помилку
 */
std::wstring RunCalculatorProcess(int n_steps, int p_threads)
{
    // 1. Формуємо команду для запуску
    std::wstring cmd = L"integral_calc.exe " + std::to_wstring(n_steps) + L" " + std::to_wstring(p_threads);

    // 2. Налаштування для перехоплення виводу (stdout)
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    HANDLE hReadPipe, hWritePipe;
    if (!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0)) {
        return L"Помилка: CreatePipe збій";
    }
    SetHandleInformation(hReadPipe, HANDLE_FLAG_INHERIT, 0);

    // 3. Налаштування запуску процесу
    STARTUPINFOW si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.hStdError = hWritePipe;
    si.hStdOutput = hWritePipe;
    si.dwFlags |= STARTF_USESTDHANDLES;
    ZeroMemory(&pi, sizeof(pi));

    // 4. Запуск процесу!
    if (!CreateProcessW(NULL, &cmd[0], NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
    {
        CloseHandle(hReadPipe);
        CloseHandle(hWritePipe);
        return L"Помилка: CreateProcess збій";
    }

    CloseHandle(hWritePipe);

    // 5. Читаємо результат з каналу
    CHAR buffer[256];
    DWORD bytesRead;
    std::string output = ""; // <- Це std::string (char)
    while (ReadFile(hReadPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0)
    {
        buffer[bytesRead] = '\0';
        output += buffer;
    }

    // 6. Очищення
    CloseHandle(hReadPipe);
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    // 7. Форматуємо результат
    if (output.empty()) {
        return L"Помилка: Процес не повернув результат.";
    }

    //
    // --- ПОЧАТОК ВИПРАВЛЕННЯ ---
    //
    // Конвертуємо std::string (UTF-8 з консолі) в std::wstring (UTF-16 для WinAPI)

    // 1. Отримуємо необхідний розмір буфера для std::wstring
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &output[0], (int)output.size(), NULL, 0);

    // 2. Створюємо wstring потрібного розміру
    std::wstring wide_output(size_needed, 0);

    // 3. Виконуємо саму конвертацію
    MultiByteToWideChar(CP_UTF8, 0, &output[0], (int)output.size(), &wide_output[0], size_needed);
    //
    // --- КІНЕЦЬ ВИПРАВЛЕННЯ ---
    //

    // Парсимо "value,time"
    size_t comma_pos = wide_output.find(L",");
    if (comma_pos == std::wstring::npos) {
        return L"Помилка: Неправильний формат виводу: " + wide_output;
    }
    std::wstring value = wide_output.substr(0, comma_pos);
    std::wstring time = wide_output.substr(comma_pos + 1);

    // .substr() тут потрібен, щоб прибрати зайві \r\n в кінці
    return L"Значення: " + value + L"\nЧас: " + time.substr(0, time.find(L"\r")) + L" мс";
}


/**
 * @brief Головна "Віконна процедура", обробляє всі події
 */
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE: // Викликається при створенні вікна
    {
        // Створюємо всі елементи GUI
        CreateWindowW(L"STATIC", L"Кількість інтервалів (n):", WS_VISIBLE | WS_CHILD,
            20, 20, 200, 20, hwnd, NULL, NULL, NULL);

        hEditN = CreateWindowW(L"EDIT", L"10000000", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
            20, 45, 200, 25, hwnd, (HMENU)IDC_EDIT_N, NULL, NULL);

        CreateWindowW(L"STATIC", L"Кількість потоків (p):", WS_VISIBLE | WS_CHILD,
            20, 80, 200, 20, hwnd, NULL, NULL, NULL);

        hEditP = CreateWindowW(L"EDIT", L"8", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
            20, 105, 200, 25, hwnd, (HMENU)IDC_EDIT_P, NULL, NULL);

        CreateWindowW(L"BUTTON", L"Обчислити", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            250, 65, 120, 50, hwnd, (HMENU)IDC_BUTTON_CALC, NULL, NULL);

        hStaticResult = CreateWindowW(L"STATIC", L"Натисніть 'Обчислити'", WS_VISIBLE | WS_CHILD | SS_CENTER,
            20, 160, 350, 50, hwnd, (HMENU)IDC_STATIC_RESULT, NULL, NULL);

        break;
    }

    case WM_COMMAND: // Викликається при натисканні кнопок
    {
        if (LOWORD(wParam) == IDC_BUTTON_CALC) // Якщо натиснуто нашу кнопку
        {
            wchar_t bufferN[100];
            wchar_t bufferP[100];
            GetWindowTextW(hEditN, bufferN, 100);
            GetWindowTextW(hEditP, bufferP, 100);

            try {
                int n = std::stoi(bufferN);
                int p = std::stoi(bufferP);

                SetWindowTextW(hStaticResult, L"Обчислення... будь ласка, зачекайте.");

                // Запускаємо наш "рушій" і отримуємо результат
                std::wstring result = RunCalculatorProcess(n, p);

                SetWindowTextW(hStaticResult, result.c_str());
            }
            catch (const std::exception&) {
                SetWindowTextW(hStaticResult, L"Помилка: 'n' та 'p' мають бути числами.");
            }
        }
        break;
    }

    case WM_DESTROY: // Викликається при закритті вікна
        PostQuitMessage(0);
        return 0;

    case WM_PAINT: // Викликається при перемальовуванні
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
        EndPaint(hwnd, &ps);
        break;
    }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

/**
 * @brief Головна функція (вхідна точка) для WinAPI програми
 */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    // 1. Реєстрація класу вікна
    const wchar_t CLASS_NAME[] = L"IntegralCalculatorClass";
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassW(&wc);

    // 2. Створення вікна
    HWND hwnd = CreateWindowExW(
        0, CLASS_NAME, L"Обчислювач інтегралів (OpenMP)", WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 250,
        NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL) {
        return 0;
    }

    // 3. Показ вікна
    ShowWindow(hwnd, nCmdShow);

    // 4. Головний цикл повідомлень
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}