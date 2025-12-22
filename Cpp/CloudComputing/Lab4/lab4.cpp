// Додано windows.h для SetConsoleOutputCP
#include <windows.h>
#include <iostream>
#include <vector>     // Використовуємо вектори для зручності
#include <cmath>      // Для fabs()
#include <omp.h>      // Головний файл OpenMP
#include <chrono>     // Для вимірювання часу
#include <iomanip>    // Для std::setw

// Використовуємо std::vector для матриці та вектора розв'язків
using Matrix = std::vector<std::vector<double>>;
using Vector = std::vector<double>;

/**
 * @brief Ініціалізує розширену матрицю A|b.
 * Створює діагонально домінантну матрицю для гарантованої збіжності.
 * @param matrix Розширена матриця (n x n+1)
 * @param n Розмірність системи
 */
void initialize_system(Matrix& matrix, int n) {
    matrix.assign(n, std::vector<double>(n + 1));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            matrix[i][j] = (i == j) ? n : 1.0; // Діагонально домінантна
        }
        // Заповнення вектора b (останній стовпець)
        matrix[i][n] = n + i;
    }
}

/**
 * @brief Виводить перші 10 елементів вектора розв'язків
 * @param xx Вектор розв'язків
 */
void print_solution(const Vector& xx) {
    std::cout << "Розв'язок (перші 10 елементів):\n";
    int n_to_print = std::min((int)xx.size(), 10);
    for (int i = 0; i < n_to_print; ++i) {
        std::cout << "x[" << i << "] = " << xx[i] << "\n";
    }
}

/**
 * @brief Розв'язує СЛАР методом Гаусса з паралельним прямим ходом
 * @param matrix Розширена матриця (буде модифікована)
 * @param xx Вектор для збереження розв'язку
 * @param n Розмірність системи
 * @param num_threads Кількість потоків OpenMP
 */
void parallel_gauss_solve(Matrix& matrix, Vector& xx, int n, int num_threads) {
    // Встановлюємо задану кількість потоків
    omp_set_num_threads(num_threads);

    double tmp;
    int i, j, k; // Оголошуємо заздалегідь для C++ стилю

    // --- ПРЯМИЙ ХІД (з розпаралеленням) ---
    // Цей цикл (за 'i') має бути послідовним
    for (i = 0; i < n; i++) {

        // --- Часткове обертання (Pivoting) ---
        // Це важливо для стабільності та уникнення ділення на нуль
        int max_row = i;
        for (k = i + 1; k < n; k++) {
            if (std::fabs(matrix[k][i]) > std::fabs(matrix[max_row][i])) {
                max_row = k;
            }
        }
        // Обмін рядками
        if (i != max_row) {
            std::swap(matrix[i], matrix[max_row]);
        }
        // ------------------------------------

        // Нормалізація i-го рядка (як у вашому прикладі)
        tmp = matrix[i][i];
        if (std::fabs(tmp) < 1e-9) {
            std::cerr << "Помилка: Матриця вироджена!\n";
            return; // Система не має єдиного розв'язку
        }

        for (j = n; j >= i; j--) {
            matrix[i][j] /= tmp;
        }

        // --- Паралельна частина ---
        // Оновлюємо всі рядки нижче i-го паралельно
        #pragma omp parallel for private(j, k, tmp)
        for (j = i + 1; j < n; j++) {
            tmp = matrix[j][i];
            for (k = n; k >= i; k--) {
                matrix[j][k] -= tmp * matrix[i][k];
            }
        }
    }

    // --- ЗВОРОТНИЙ ХІД (послідовний) ---
    // Як зазначено у "Примітці", цей O(n^2) етап
    // зазвичай не розпаралелюють.
    xx[n - 1] = matrix[n - 1][n];
    for (i = n - 2; i >= 0; i--) {
        xx[i] = matrix[i][n];
        for (j = i + 1; j < n; j++) {
            xx[i] -= matrix[i][j] * xx[j];
        }
    }
}


/**
 * @brief Головна функція для тестування (Завдання 2)
 */
int main() {
    // === ВИРІШЕННЯ ПРОБЛЕМИ КОДУВАННЯ ===
    SetConsoleOutputCP(CP_UTF8);
    // ======================================

    std::vector<int> sizes = {250, 500, 1000, 1500, 2000}; // Розмірності систем
    std::vector<int> threads = {1, 2, 4, 8, 12};          // Кількість потоків

    std::cout << "--- Тестування швидкодії паралельного методу Гаусса ---\n\n";

    // Встановлюємо налаштування для рядків з ДАНИМИ
    std::cout << std::left << std::fixed << std::setprecision(3);

    // --- НОВИЙ ЗАГОЛОВОК (Ручне вирівнювання за вашими пробілами) ---
    // Повністю прибираємо setw() звідси і додаємо пробіли вручну.
    std::cout << "Розмір (N)" << "  "         // 2 пробіли, як ви просили (загалом 12 символів)
              << "Потоки"      << "    "         // 4 пробіли (загалом 10 символів)
              << "Час (мс)"  << "       "         // 7 пробілів (загалом 15 символів)
              << "Прискорення"  << "       "    // 7 пробілів (загалом 18 символів)
              << "Ефективність (%)"         // Остання колонка, їй пробіли не потрібні
              << "\n";

    std::cout << "-------------------------------------------------------------------------\n";

    Matrix matrix;
    Vector xx;

    for (int n : sizes) {

        double sequential_time = 0.0; // Базовий час для N

        for (int num_threads : threads) {

            // Ініціалізуємо дані для кожного тесту
            initialize_system(matrix, n);
            xx.assign(n, 0.0);

            // Вимірюємо час
            auto start_time = std::chrono::high_resolution_clock::now();
            parallel_gauss_solve(matrix, xx, n, num_threads);
            auto end_time = std::chrono::high_resolution_clock::now();

            std::chrono::duration<double, std::milli> elapsed_ms = end_time - start_time;
            double current_time_ms = elapsed_ms.count();

            // --- ДАНІ (тут setw() ЗАЛИШАЄТЬСЯ) ---
            // Ширина цих setw() тепер точно відповідає довжині рядків заголовка
            std::cout << std::setw(12) << n
                      << std::setw(10) << num_threads
                      << std::setw(15) << current_time_ms;

            // Розрахунок та вивід детальних метрик
            if (num_threads == 1) {
                sequential_time = current_time_ms;
                std::cout << std::setw(18) << "N/A"
                          << std::setw(20) << "N/A" << "\n";
            } else {
                double speedup = sequential_time / current_time_ms;
                double efficiency = (speedup / num_threads) * 100.0;

                // Конвертуємо speedup в рядок, щоб додати "x"
                std::string speedup_str = std::to_string(speedup) + "x";

                std::cout << std::setw(18) << speedup_str
                          << std::setw(20) << efficiency << "\n";
            }
        }
        std::cout << "-------------------------------------------------------------------------\n";
    }

    return 0;
}