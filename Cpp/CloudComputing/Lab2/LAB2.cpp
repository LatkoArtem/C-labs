#include <windows.h> // Потрібно для SetConsoleOutputCP
#include <stdio.h>   // Для printf
#include <stdlib.h>  // Для new/delete[] (у C++) або malloc/free (у C)
#include <omp.h>     // Головний файл OpenMP

// --- Налаштування для Завдань 2 та 3 ---
// Використовуємо менший розмір для швидкої демонстрації
// Для N=1000+ обчислення можуть зайняти деякий час
const int N = 1000;

/**
 * Завдання 1: Основи OpenMP
 * 1.1: Перевірка підтримки та версії
 * 1.2: Вимірювання точності таймера
 * 1.3: Паралельний "Hello World"
 */
void task1() {
    printf("--- Завдання 1: Основи OpenMP ---\n\n");

    // 1.1: Перевірка версії
    #ifdef _OPENMP
        // Макрос _OPENMP визначається компілятором,
        // якщо він підтримує OpenMP.
        // Його значення - це YYYYMM (наприклад, 201811 для OpenMP 4.5)
        printf("1.1: OpenMP підтримується! Версія: %d\n", _OPENMP);
    #else
        printf("1.1: OpenMP НЕ підтримується.\n");
    #endif

    // 1.2: Точність таймера
    double start_time, end_time, tick;

    start_time = omp_get_wtime();
    // Вимірюємо час, потрібний для виклику самої функції вимірювання
    end_time = omp_get_wtime();

    // omp_get_wtick() повертає кількість секунд між "тіками" таймера
    tick = omp_get_wtick();

    printf("1.2: Час на вимірювання часу: %e секунд\n", end_time - start_time);
    printf("1.2: Точність системного таймера (wtick): %e секунд\n", tick);

    // 1.3: "Hello World" у паралельній області
    printf("\n1.3: Паралельний 'Hello World':\n");

    // Початок паралельного регіону
    #pragma omp parallel
    {
        // Кожен потік виконає цей блок коду
        int thread_id = omp_get_thread_num(); // Отримати ID потоку
        int num_threads = omp_get_num_threads(); // Отримати загальну к-сть потоків

        printf("   Hello World! (Я потік %d з %d)\n", thread_id, num_threads);
    } // Кінець паралельного регіону

    printf("\n");
}


/**
 * Допоміжна функція для ініціалізації матриць
 */
void initialize_matrices(double* a, double* b, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            // Використовуємо flat-array індексацію: A[i][j] -> a[i*n + j]
            a[i * n + j] = (double)(i + j) / 2.0;
            b[i * n + j] = (double)(i - j) / 2.0;
        }
    }
}

/**
 * Завдання 2: Множення матриць (Комбінована версія)
 * 1. Генерує таблицю даних для звіту.
 * 2. Виконує детальний аналіз для N=1000 з розрахунком прискорення.
 */
void task2() {
    // --- ЧАСТИНА 1: Генерація таблиці для звіту ---

    printf("--- Завдання 2, Частина 1: Таблиця для звіту (Множення матриць) ---\n\n");

    // Значення 'n' (розмірність) з вашої таблиці
    int n_values[] = {10, 20, 50, 100, 200, 500, 1000};
    // Значення 'p' (кількість потоків) з вашої таблиці
    int p_values[] = {2, 4, 6, 8, 10, 12};

    int n_count = sizeof(n_values) / sizeof(n_values[0]);
    int p_count = sizeof(p_values) / sizeof(p_values[0]);

    // Друк заголовка таблиці для зручності
    printf("| %-8s | %-12s |", "n", "Однопоточна ");
    for (int p_idx = 0; p_idx < p_count; p_idx++) {
        char header[16];
        sprintf(header, "p = %d", p_values[p_idx]);
        printf(" %-12s |", header);
    }
    printf("\n");
    // Друк розділювача
    printf("|----------|--------------|");
    for (int p_idx = 0; p_idx < p_count; p_idx++) {
        printf("--------------|");
    }
    printf("\n");

    // Головний цикл по всім 'n' (рядки таблиці)
    for (int n_idx = 0; n_idx < n_count; n_idx++) {
        int n_loop = n_values[n_idx]; // Використовуємо n_loop, щоб не конфліктувати з N
        printf("| %-8d | ", n_loop);

        // Виділення пам'яті для поточного 'n'
        double* A_loop = new double[n_loop * n_loop];
        double* B_loop = new double[n_loop * n_loop];
        double* C_serial_loop = new double[n_loop * n_loop];
        double* C_parallel_loop = new double[n_loop * n_loop];

        initialize_matrices(A_loop, B_loop, n_loop);

        double t1_loop, t2_loop;
        int i, j, k;

        // --- 1. Послідовна версія (Колонка "Однопоточна") ---
        t1_loop = omp_get_wtime();
        for (i = 0; i < n_loop; i++) {
            for (j = 0; j < n_loop; j++) {
                double sum = 0.0;
                for (k = 0; k < n_loop; k++) {
                    sum += A_loop[i * n_loop + k] * B_loop[k * n_loop + j];
                }
                C_serial_loop[i * n_loop + j] = sum;
            }
        }
        t2_loop = omp_get_wtime();
        double serial_time_loop = t2_loop - t1_loop;
        printf("%-12.6f | ", serial_time_loop);

        // --- 2. Паралельні версії (Колонки p=2, p=4, ...) ---
        for (int p_idx = 0; p_idx < p_count; p_idx++) {
            int p = p_values[p_idx];
            omp_set_num_threads(p);

            t1_loop = omp_get_wtime();

            #pragma omp parallel for shared(A_loop, B_loop, C_parallel_loop, n_loop) private(i, j, k)
            for (i = 0; i < n_loop; i++) {
                for (j = 0; j < n_loop; j++) {
                    double sum = 0.0;
                    for (k = 0; k < n_loop; k++) {
                        sum += A_loop[i * n_loop + k] * B_loop[k * n_loop + j];
                    }
                    C_parallel_loop[i * n_loop + j] = sum;
                }
            }

            t2_loop = omp_get_wtime();
            double parallel_time_loop = t2_loop - t1_loop;
            printf("%-12.6f | ", parallel_time_loop);
        }

        printf("\n"); // Новий рядок для наступного 'n'

        // Звільнення пам'яті
        delete[] A_loop;
        delete[] B_loop;
        delete[] C_serial_loop;
        delete[] C_parallel_loop;
    }

    printf("\n\n---\n\n"); // Розділювач між частинами

    // --- ЧАСТИНА 2: Детальний аналіз (Ваш початковий код) ---
    // Використовуємо глобальну константу N

    printf("--- Завдання 2, Частина 2: Детальний аналіз для N = %d ---\n\n", N);

    // Виділення пам'яті в "купі" (heap)
    double* A = new double[N * N];
    double* B = new double[N * N];
    double* C_serial = new double[N * N];
    double* C_parallel = new double[N * N];

    initialize_matrices(A, B, N);

    double t1, t2;
    int i, j, k; // Змінні для циклів

    // --- 1. Послідовна версія ---
    printf("Виконується послідовне множення...\n");
    t1 = omp_get_wtime();

    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            double sum = 0.0; // Локальна сума для C[i][j]
            for (k = 0; k < N; k++) {
                // C[i][j] += A[i][k] * B[k][j]
                sum += A[i * N + k] * B[k * N + j];
            }
            C_serial[i * N + j] = sum;
        }
    }

    t2 = omp_get_wtime();
    double serial_time = t2 - t1;
    printf("Час (послідовно): %lf секунд\n", serial_time);

    // --- 2. Паралельна версія ---
    printf("Виконується паралельне множення...\n");

    // Встановлюємо максимальну кількість потоків для "фінального" тесту
    // (Цикл вище залишив 12, але так надійніше)
    omp_set_num_threads(omp_get_max_threads());

    t1 = omp_get_wtime();

    #pragma omp parallel for shared(A, B, C_parallel, N) private(i, j, k)
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            double sum = 0.0;
            for (k = 0; k < N; k++) {
                sum += A[i * N + k] * B[k * N + j];
            }
            C_parallel[i * N + j] = sum;
        }
    }

    t2 = omp_get_wtime();
    double parallel_time = t2 - t1;
    printf("Час (паралельно, %d потоків): %lf секунд\n", omp_get_max_threads(), parallel_time);

    // --- 3. Порівняння ---
    if (parallel_time > 0) { // Уникнення ділення на нуль
        printf("Прискорення (Speedup): %.2fx\n", serial_time / parallel_time);
    }

    // (Додатково) Перевірка коректності:
    bool correct = true;
    for (i = 0; i < N * N; i++) {
        // Порівнюємо з невеликою похибкою (epsilon)
        if (abs(C_serial[i] - C_parallel[i]) > 1e-9) {
            correct = false;
            break;
        }
    }
    printf("Перевірка коректності: %s\n", correct ? "УСПІХ" : "ПОМИЛКА");

    // Звільнення пам'яті
    delete[] A;
    delete[] B;
    delete[] C_serial;
    delete[] C_parallel;

    printf("\n");
}


/**
 * Завдання 3: Сума елементів матриці
 * 3.1: Сума елементів кожного рядка
 * 3.2: Загальна сума всіх елементів (з редукцією)
 */
void task3() {
    printf("--- Завдання 3: Сума елементів (N = %d) ---\n\n", N);

    double* A = new double[N * N];

    // Ініціалізація (просто заповнимо одиницями для легкої перевірки)
    for (int i = 0; i < N * N; i++) {
        A[i] = 1.0;
    }

    // Очікувані результати:
    // Сума рядка: N * 1.0 = 1000.0
    // Загальна сума: N * (N * 1.0) = 1000 * 1000 = 1,000,000.0

    int i, j;
    double t1, t2;

    // --- Послідовна версія (для порівняння) ---
    printf("Виконується послідовний розрахунок сум...\n");
    t1 = omp_get_wtime();
    double total_sum_serial = 0.0;

    for (i = 0; i < N; i++) {
        double row_sum_serial = 0.0;
        for (j = 0; j < N; j++) {
            row_sum_serial += A[i * N + j];
        }
        // Виводимо суму лише для перших 5 рядків, щоб не спамити консоль
        if (i < 5) {
             printf("   (Serial) Сума рядка %d: %f\n", i, row_sum_serial);
        }
        total_sum_serial += row_sum_serial;
    }
    t2 = omp_get_wtime();
    printf("Час (послідовно): %lf секунд\n", t2 - t1);
    printf("Загальна сума (послідовно): %f\n\n", total_sum_serial);


    // --- Паралельна версія (Завдання 3.1 та 3.2) ---
    printf("Виконується паралельний розрахунок сум (з редукцією)...\n");
    t1 = omp_get_wtime();

    // Ця змінна буде використовуватись для редукції
    double total_sum_parallel = 0.0;

    // 'parallel for' - розпаралелює цикл 'i'
    // 'private(i, j, row_sum)' - 'row_sum' має бути локальною для кожної ітерації 'i'
    // 'reduction(+:total_sum_parallel)' - OpenMP автоматично:
    // 1. Створить приватну копію 'total_sum_parallel' для кожного потоку (ініціалізує 0).
    // 2. Кожен потік буде додавати свої 'row_sum' до *своєї* приватної копії.
    // 3. В кінці всі приватні копії будуть безпечно підсумовані в одну глобальну.

    // УВАГА: Приклад у вашому завданні 3.2 містив помилку 'j = i',
    // що рахувало б лише верхній трикутник матриці.
    // Тут ми рахуємо всі елементи, як вимагає завдання "суму всіх елементів".

    #pragma omp parallel for private(i, j) reduction(+:total_sum_parallel)
    for (i = 0; i < N; i++) {
        double row_sum = 0.0; // Локальна змінна для суми рядка
        for (j = 0; j < N; j++) {
            row_sum += A[i * N + j];
        }

        // 3.1: Виведення суми рядка
        // Використовуємо 'atomic' або 'critical' для безпечного виводу
        // з багатьох потоків. (Закоментовано, щоб не сповільнювати вимірювання)
        // #pragma omp critical
        // {
        //     if (i < 5) { // Виводимо лише перші 5
        //          printf("   (Parallel) Сума рядка %d: %f\n", i, row_sum);
        //     }
        // }

        // 3.2: Додавання суми рядка до загальної суми
        // Це безпечно, оскільки додається до приватної копії
        total_sum_parallel += row_sum;
    }
    // Тут (після циклу) 'total_sum_parallel' містить фінальну суму

    t2 = omp_get_wtime();
    printf("Час (паралельно): %lf секунд\n", t2 - t1);
    printf("Загальна сума (паралельно, reduction): %f\n", total_sum_parallel);

    delete[] A;
}

int main() {
    SetConsoleOutputCP(65001); // Встановлює кодування UTF-8 для виводу

    task1();

    task2();

    task3();

    return 0;
}