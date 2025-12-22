#include <iostream>
#include <iomanip>
#include <omp.h>
#include <chrono>
#include <random>
#include <windows.h>
#include <vector>     // Для векторів з налаштуваннями
#include <string>     // Для std::to_string

// =======================================================
// --- ЗАВДАННЯ 1: МЕТОД ІНТЕГРУВАННЯ ---
// =======================================================

// Функція для інтегрування
double f(double y) {
    return (4.0 / (1.0 + y * y));
}

/**
 * ЗАВДАННЯ 1: Об'єднана функція (Інтеграл)
 * @param n Кількість кроків
 * @param num_threads Кількість потоків для запуску
 */
double calculate_pi_integral(long long n, int num_threads) {
    double sum = 0.0;
    double w = 1.0 / n; // Ширина кроку

    // Встановлюємо кількість потоків
    omp_set_num_threads(num_threads);

    #pragma omp parallel for reduction(+:sum)
    for (long long i = 0; i < n; i++) {
        double x = w * (i + 0.5);
        sum += f(x);
    }
    return w * sum;
}

// =======================================================
// --- ЗАВДАННЯ 2: МЕТОД МОНТЕ-КАРЛО ---
// =======================================================

/**
 * ЗАВДАННЯ 2: Об'єднана функція (Монте-Карло)
 * @param numSamples Кількість точок
 * @param num_threads Кількість потоків для запуску
 */
double calculate_pi_mc(long long numSamples, int num_threads) {
    long long counter = 0;

    // Встановлюємо кількість потоків
    omp_set_num_threads(num_threads);

    #pragma omp parallel reduction(+:counter)
    {
        // Кожен потік створює свій ВЛАСНИЙ, унікальний генератор
        unsigned int my_seed = omp_get_thread_num() + (unsigned int)std::chrono::system_clock::now().time_since_epoch().count();
        std::default_random_engine generator(my_seed);
        std::uniform_real_distribution<double> distribution(-1.0, 1.0);

        #pragma omp for
        for (long long s = 0; s < numSamples; s++) {
            double x = distribution(generator);
            double y = distribution(generator);

            if (x * x + y * y < 1) {
                counter++;
            }
        }
    } // OpenMP автоматично додає всі 'counter' тут

    return 4.0 * counter / numSamples;
}


// =======================================================
// --- ГОЛОВНА ФУНКЦІЯ (ДЕТАЛЬНЕ ТЕСТУВАННЯ) ---
// =======================================================

int main() {
    SetConsoleOutputCP(CP_UTF8);
    // Встановлюємо точність 8 знаків для Pi
    std::cout << std::left << std::fixed << std::setprecision(8);

    // --- Налаштування тестів ---
    std::vector<long long> n_values_integral = {100000, 10000000, 100000000};
    std::vector<long long> n_values_mc =       {1000000, 20000000, 200000000};
    std::vector<int> thread_counts = {1, 2, 4, 8, 12};

    // --- Запуск Завдання 1 ---
    std::cout << "--- Завдання 1: Аналіз (Метод Інтеграла) ---\n\n";

    // --- Оновлений заголовок ---
    std::cout << std::setw(15) << "N (кроки)" << "     "
              << std::setw(10) << "Потоки" << "    "
              << std::setw(17) << "Результат (Pi)" << "   "
              << std::setw(15) << "Час (мс)" << "     "
              << std::setw(18) << "Прискорення" << "       "
              << std::setw(20) << "Ефективність (%)"
              << "\n";
    std::cout << "--------------------------------------------------------------------------------------\n"; // Зробив лінію довшою

    for (long long n : n_values_integral) {
        double sequential_time = 0.0;
        for (int threads : thread_counts) {
            auto start = std::chrono::high_resolution_clock::now();
            // Обчислюємо Pi
            double pi = calculate_pi_integral(n, threads);
            auto end = std::chrono::high_resolution_clock::now();

            std::chrono::duration<double, std::milli> time_ms = end - start;
            double current_time_ms = time_ms.count();

            // --- Оновлений вивід даних ---
            std::cout << std::setw(15) << n
                      << std::setw(10) << threads
                      << std::setw(17) << pi  // <-- ВИВОДИМО PI
                      << std::setw(15) << current_time_ms;

            if (threads == 1) {
                sequential_time = current_time_ms;
                std::cout << std::setw(18) << "N/A" << std::setw(20) << "N/A" << "\n";
            } else {
                double speedup = sequential_time / current_time_ms;
                double efficiency = (speedup / threads) * 100.0;
                // Встановлюємо точність 3 знаки для метрик
                std::cout << std::setprecision(3);
                std::cout << std::setw(18) << (std::to_string(speedup) + "x")
                          << std::setw(20) << efficiency << "\n";
                // Повертаємо точність 8 знаків для наступного Pi
                std::cout << std::setprecision(8);
            }
        }
        std::cout << "--------------------------------------------------------------------------------------\n";
    }

    // --- Роздільник ---
    std::cout << "\n======================================================================================\n\n";

    // --- Запуск Завдання 2 ---
    std::cout << "--- Завдання 2: Аналіз (Метод Монте-Карло) ---\n\n";
    std::cout << std::setw(15) << "N (точки)" << "     "
              << std::setw(10) << "Потоки" << "    "
              << std::setw(17) << "Результат (Pi)" << "   "
              << std::setw(15) << "Час (мс)" << "     "
              << std::setw(18) << "Прискорення" << "       "
              << std::setw(20) << "Ефективність (%)"
              << "\n";
    std::cout << "--------------------------------------------------------------------------------------\n";

    for (long long n : n_values_mc) {
        double sequential_time = 0.0;
        for (int threads : thread_counts) {
            auto start = std::chrono::high_resolution_clock::now();
            // Обчислюємо Pi
            double pi = calculate_pi_mc(n, threads);
            auto end = std::chrono::high_resolution_clock::now();

            std::chrono::duration<double, std::milli> time_ms = end - start;
            double current_time_ms = time_ms.count();

            // --- Оновлений вивід даних ---
            std::cout << std::setw(15) << n
                      << std::setw(10) << threads
                      << std::setw(17) << pi  // <-- ВИВОДИМО PI
                      << std::setw(15) << current_time_ms;

            if (threads == 1) {
                sequential_time = current_time_ms;
                std::cout << std::setw(18) << "N/A" << std::setw(20) << "N/A" << "\n";
            } else {
                double speedup = sequential_time / current_time_ms;
                double efficiency = (speedup / threads) * 100.0;
                // Встановлюємо точність 3 знаки для метрик
                std::cout << std::setprecision(3);
                std::cout << std::setw(18) << (std::to_string(speedup) + "x")
                          << std::setw(20) << efficiency << "\n";
                // Повертаємо точність 8 знаків для наступного Pi
                std::cout << std::setprecision(8);
            }
        }
        std::cout << "--------------------------------------------------------------------------------------\n";
    }

    return 0;
}