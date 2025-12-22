#include <iostream>
#include <vector>
#include <cmath>        // Для exp() та fabs()
#include <omp.h>        // Головний файл OpenMP
#include <iomanip>      // Для std::fixed та std::setprecision
#include <cassert>      // Потрібно для модульних тестів (Вимога 3)
#include <string>       // Для std::stoi

/**
 * @brief (Вимога 1, частина 1) Абстрактний базовий клас для функції.
 */
class IFunction {
public:
    virtual ~IFunction() {}
    virtual double calculate(double x) const = 0;
};

/**
 * @brief (Вимога 1, частина 2) Конкретна реалізація функції для Варіанту 6.
 * f(x) = (e^x - 1) / (e^x + 1)
 */
class FunctionV6 : public IFunction {
public:
    double calculate(double x) const override {
        double e_x = exp(x);
        return (e_x - 1.0) / (e_x + 1.0);
    }
};

/**
 * @brief (Вимога 2) Клас "Обчислювач інтегралів".
 * Реалізує паралельний метод Сімпсона.
 */
class IntegralCalculator {
public:
    // (Вимога 5) Структура для повернення результатів
    struct Result {
        double value;    // Обчислене значення
        double time_ms;  // Час виконання в мілісекундах
    };

    /**
     * @brief (Вимога 2) Метод "обчислити" (Метод Сімпсона).
     * @param a - початок інтервалу
     * @param b - кінець інтервалу
     * @param n - кількість кроків
     * @param f - підінтегральна функція
     */
    Result calculate(double a, double b, int n, const IFunction& f) {

        double start_time = omp_get_wtime();

        if (n % 2 != 0) {
            n++; // Метод Сімпсона вимагає парної кількості кроків
        }

        const double h = (b - a) / n;
        double sum = 0.0;

        sum = f.calculate(a) + f.calculate(b);

        // (Вимога 1) Паралельне обчислення з редукцією
        #pragma omp parallel for reduction(+:sum)
        for (int i = 1; i < n; i++) {
            double x_i = a + i * h;
            if (i % 2 == 0) {
                sum += 2.0 * f.calculate(x_i); // Парні
            } else {
                sum += 4.0 * f.calculate(x_i); // Непарні
            }
        }

        double result_value = (h / 3.0) * sum;
        double end_time = omp_get_wtime();

        return {result_value, (end_time - start_time) * 1000.0};
    }
};

/**
 * @brief (Вимога 3) Функція модульного тестування
 */
void runUnitTests() {
    std::cout << "\n--- (Вимога 3) Запуск модульних тестів ---" << std::endl;

    FunctionV6 test_func;
    IntegralCalculator test_calc;

    // (Виправлене) Еталонне значення, обчислене аналітично
    const double EXPECTED_VALUE = 0.62727261;
    const double TOLERANCE = 1e-6; // Допустима похибка

    int n_test = 1000000;
    IntegralCalculator::Result res = test_calc.calculate(1.0, 2.0, n_test, test_func);

    // assert() зупинить програму, якщо перевірка не пройде
    assert(fabs(res.value - EXPECTED_VALUE) < TOLERANCE);

    std::cout << "OK: Тест калькулятора пройшов успішно." << std::endl;
    std::cout << std::fixed << std::setprecision(8);
    std::cout << "   Очікуване: " << EXPECTED_VALUE << std::endl;
    std::cout << "   Отримане:  " << res.value << std::endl;
    std::cout << "--------------------------------------------\n" << std::endl;
}

/**
 * @brief (Вимога 6) Запуск тестів продуктивності
 */
void runPerformanceTests() {
    FunctionV6 my_func;
    IntegralCalculator calculator;
    const double a = 1.0;
    const double b = 2.0;

    std::cout << std::fixed << std::setprecision(8);

    // 1. Мала кількість інтервалів (n < 1000)
    int n_small = 999;
    std::cout << "\n--- (Вимога 6) Тест 1: Мала кількість кроків (n ~ " << n_small << ") ---" << std::endl;
    std::cout << "| Потоки |      Результат | Час (мс) |\n";
    std::cout << "|--------|----------------|----------|\n";

    for (int p = 1; p <= 20; p++) {
        omp_set_num_threads(p);
        IntegralCalculator::Result res = calculator.calculate(a, b, n_small, my_func);
        std::cout << "| " << std::setw(6) << p
                  << " | " << std::setw(14) << res.value
                  << " | " << std::setw(8) << res.time_ms << " |\n";
    }

    // 2. Велика кількість інтервалів (n > 1 000 000)
    int n_large = 10'000'000;
    std::cout << "\n--- (Вимога 6) Тест 2: Велика кількість кроків (n = " << n_large << ") ---" << std::endl;
    std::cout << "| Потоки |      Результат | Час (мс) |\n";
    std::cout << "|--------|----------------|----------|\n";

    for (int p = 1; p <= 20; p++) {
        omp_set_num_threads(p);
        IntegralCalculator::Result res = calculator.calculate(a, b, n_large, my_func);
        std::cout << "| " << std::setw(6) << p
                  << " | " << std::setw(14) << res.value
                  << " | " << std::setw(8) << res.time_ms << " |\n";
    }

    // (Вимога 7) Висновки
    std::cout << "\n--- (Вимога 7) Зробити висновки ---" << std::endl;
    std::cout << "Проаналізуйте дві таблиці вище." << std::endl;
}


int main(int argc, char* argv[]) {

    // argc - кількість аргументів, argv - масив аргументів
    // argv[0] - сама програма
    // argv[1] - буде 'n'
    // argv[2] - буде 'p'

    if (argc == 3) {
        // --- РЕЖИМ GUI ---
        // Якщо програму викликано з 2 аргументами (n та p)
        int n = std::stoi(argv[1]);
        int p = std::stoi(argv[2]);

        const double a = 1.0;
        const double b = 2.0;
        FunctionV6 my_func;
        IntegralCalculator calculator;

        omp_set_num_threads(p);
        IntegralCalculator::Result res = calculator.calculate(a, b, n, my_func);

        // Виводимо ТІЛЬКИ два числа через кому,
        // щоб наша віконна програма (GUI) могла їх прочитати
        std::cout << std::fixed << std::setprecision(8);
        std::cout << res.value << "," << res.time_ms << std::endl;

    } else {
        // --- РЕЖИМ ПОВНОГО ТЕСТУ ---
        // Якщо програму запущено звичайно (без аргументів)

        // (Вимога 3)
        runUnitTests();

        // (Вимога 6 та 7)
        runPerformanceTests();
    }

    return 0;
}