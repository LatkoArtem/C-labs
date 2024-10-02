/*
Лабораторна робота №4
Тема: Указівники, функції, рекурсія
Мета роботи: Ознайомитися з новим типом організації даних С (указівники) і
набути практичних навичок його використання для написання програм; оволодіти
синтаксисом написання функцій і їхніх прототипів, методикою складання і
відлагодження програм, що містять функції,специфікою передачі параметрів у
функцію та повернення одержаних результатів; вивчити методи використання
алгоритмів і програм із рекурсією мовою С. Виконав студент Латко Артем, група
КМ-32, варіант 10

Умова завдання: 1) Підрахувати число точок, розташованих усередині кола радіусом
r із центром у початку координат. Координати задано масивами X[10] та Y[10].
Відстань точки від початку координат обчислювати у функції. 2) Дано цілі числа m
і n, де 0 <= m <= n. Написати рекурсивну функцію C(m, n) для обчислення
біноміальних коефіцієнтів, тобто числа сполучень за формулою: C(0, n) = C(n, n)
= 1;  C(m, n) = C(m, n-1) + C(m-1, n-1).
*/
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_COORDINATE 20
// Я вирішив, що нехай максимальна координата буде 20
#define MIN_COORDINATE -20
// Я вирішив, що нехай мінімальна координата буде -20

// Функція для підрахунку відстані від початку координат до точки
float distance(float *x, float *y) { return sqrt((*x * *x) + (*y * *y)); }

// Функція підрахунку кількості точок
int points_inside_circle(float *X, float *Y, float radius) {
  int count = 0;
  for (int i = 0; i < 10; i++) {
    if (distance(&X[i], &Y[i]) <= radius) {
      count++;
    }
  }
  return count;
}

// Функція для перевірки на введення int
int check_for_int(char *text, int *variable) {
  char input[100];
  while (true) {
    int i = 0;
    printf("%s", text);
    fgets(input, sizeof(input), stdin);
    // Перевірити, чи є крапка в рядку введення
    if (strchr(input, '.') != NULL) {
      printf("Неправильний ввід даних\n");
      continue;
    }
    if (sscanf(input, "%d", variable) != 1) {
      printf("Неправильний ввід даних\n");
      continue;
    }
    // Перевірити, чи введене значення має непотрібні символи
    char *endptr;
    strtol(input, &endptr, 10);
    if (*endptr != '\0' && *endptr != '\n') {
      printf("Неправильний ввід даних\n");
      continue;
    }
    break;
  }
  return 0;
}

// Функція для перевірки на введення float
int check_for_float(char *text, float *variable) {
  char input[100];
  while (true) {
    int i = 0;
    printf("%s", text);
    fgets(input, sizeof(input), stdin);
    if (sscanf(input, "%f", variable) != 1) {
      printf("Неправильний ввід даних\n");
      continue;
    }
    // Перевірити, чи введене значення має непотрібні символи
    char *endptr;
    strtof(input, &endptr);
    if (*endptr != '\0' && *endptr != '\n') {
      printf("Неправильний ввід даних\n");
      continue;
    }
    break;
  }
  return 0;
}

// 1 Завдання
int task_1(float X[], float Y[], float radius) {
  srand(time(NULL)); // Ініціалізація генератора випадкових чисел
  // Виводимо координати точок
  printf("Координати точок: { ");
  for (int i = 0; i < 10; i++) {
    printf("(%.2f, %.2f) ", X[i], Y[i]);
  }
  printf("}\n");

  int points_count = points_inside_circle(X, Y, radius);
  printf("\nКількість точок всередині кола: %d\n\n", points_count);

  return 0;
}

// 2 Завдання (функція до другого завдання)
int binomial_coefficient(int m, int n) {
  if (m == 0 || m == n) {
    return 1;
  } else {
    return binomial_coefficient(m, n - 1) + binomial_coefficient(m - 1, n - 1);
  }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                          ГОЛОВНИЙ КОД
//                                           | | | | |
//                                           V V V V V
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main() {
  printf("Виконав студент Латко Артем, група КМ-32, варіант 10\n\n");
  printf("1) 1 завдання;\n");
  printf("0) Вихід з програми\n\n");
  // задаємо текст для функції перевірки на введення, щоб коли вводиш
  // неправильно, то щоб ще раз писало що потрібно ввести
  char text_1[] =
      "Введіть номер завдання 1 або 2, а також щоб вийти введіть 0: ";
  char text_radius[] = "Введіть радіус кола: ";
  char text_m[] = "Введіть число m(повинно бути не від'ємним): ";
  char text_n[] = "Введіть число n(повинно бути не меншим за m): ";
  // задаємо змінні
  int task;

  while (true) {
    // Вводимо дані та перевіряємо на тип даних
    check_for_int(text_1, &task);

    if (task == 1) {
      // 1 Завдання
      printf("\n          Task 1\n");
      float X[10];
      float Y[10];
      float radius;
      char input[100];

      // Генеруємо координати точок
      for (int i = 0; i < 10; i++) {
        X[i] = MIN_COORDINATE + ((float)rand() / (float)RAND_MAX) *
                                    (MAX_COORDINATE - MIN_COORDINATE);
        Y[i] = MIN_COORDINATE + ((float)rand() / (float)RAND_MAX) *
                                    (MAX_COORDINATE - MIN_COORDINATE);
      }
      // Вводимо дані та перевіряємо на тип даних
      check_for_float(text_radius, &radius);
      task_1(X, Y, radius);
    } else if (task == 2) {
      // 2 Завдання
      printf("\n          Task 2\n");
      int m, n;
      char input[100];

      // Вводимо дані та перевіряємо на тип даних
      while (true) {
        check_for_int(text_m, &m);
        // перевіряємо, щоб m не було менше нуля
        if (m < 0) {
          printf("Неправильний ввід даних\n");
          continue;
        }
        break;
      }

      while (true) {
        check_for_int(text_n, &n);
        // перевіряємо, щоб n не було менше ніж m
        if (n < m) {
          printf("Неправильний ввід даних\n");
          continue;
        }
        break;
      }
      printf("\n");
      printf("m = %d\nn = %d\n", m, n);
      printf("C(m, n) = C(%d, %d) = %d\n\n", m, n,
             binomial_coefficient(m, n)); // Виводимо результат
    } else if (task == 0) {
      // ВИХІД ІЗ ПРОГРАМИ
      system("clear");
      printf("\nВИХІД З ПРОГРАМИ!!!");
      exit(0);
    } else {
      printf("Неправильний ввід даних\n");
    }
  }
  return 0;
}