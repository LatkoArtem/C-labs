/*
Лабораторна робота №2
Тема: -- циклічні керуючі структури (оператор циклу з заданими числом повторень, оператори циклу з передумовою і постумовою);
      -- правила задання масивів, їхні властивості, операції, допустимі над цим типом даних.
Мета роботи: Вивчити базові операції для роботи з одновимірними та двовимірними масивами.
Виконав студент Латко Артем, група КМ-32, варіант 10

Умова завдання: 1) Дано одновимірний масив A, що складається з n елементів. Знайти перший та останній додатні елементи масиву, підрахувати кількість елементів, розташованих між ними. Впорядкувати масив методом обмінного сортування по зростанню.
2) Дано матрицю з цілих чисел. У рядках, усі елементи яких парні, розташувати елементи у зворотному порядку.
*/
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <time.h> // це тут використовується лише для генерації випадкових чисел

#define MAX_FOR_2D 999
#define MIN_FOR_2D -999

int main() {
  printf("Виконав студент Латко Артем, група КМ-32, варіант 10\n\n");
  //задаємо змінні
  char input[100];
  int task;

  printf("1) 1 завдання способом заповнення масивів із клавіатури;\n");
  printf("2) 1 завдання способом генерування псевдовипадкових чисел;\n");
  printf("3) 2 завдання способом заповнення масивів із клавіатури;\n");
  printf("4) 2 завдання способом генерування псевдовипадкових чисел;\n");
  printf("0) Вихід з програми\n\n");

  while (true) {
    char *endprint;
    printf("Введіть номер завдання 1, 2, 3 або 4, а також щоб вийти введіть 0: ");
    fgets(input, sizeof(input), stdin);
    if (strchr(input, '.') != NULL) {
      printf("Неправильний ввід даних\n");
      continue;
    }
    strtof(input, &endprint); // перевірка на введення числа

    if (*endprint != '\0' && *endprint != '\n') {
      printf("Неправильний ввід даних\n");
      continue;
    }
    if (sscanf(input, "%d", &task) != 1 || (task != 0 && task != 1 && task != 2 && task != 3 && task != 4)) {
      printf("Неправильний ввід даних\n");
      continue;
    } else {
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
      if (task == 1) {
        printf("             1 завдання (1 спосіб)\n");
        int size;
        
        char *endprint_1t;
        while (true) {
          printf("Введіть кількість елементів масиву n: ");
          fgets(input, sizeof(input), stdin);
          if (strchr(input, '.') != NULL) {
            printf("Неправильний ввід даних\n");
            continue;
          }
          strtof(input, &endprint_1t); // перевірка на введення числа

          if (*endprint_1t != '\0' && *endprint_1t != '\n') {
            printf("Неправильний ввід даних\n");
            continue;
          }
          if (sscanf(input, "%d", &size) != 1 || size <= 0) {
            printf("Неправильний ввід даних\n");
            continue;
          } else {
            break;
          }
        }
        const int n = size;
        float A[n];

        for (int i = 0; i < n; i++){
          while (true) {
            printf("Введіть %d елемент масиву: ", i + 1);
            fgets(input, sizeof(input), stdin);
            strtof(input, &endprint_1t); // перевірка на введення числа
    
            if (*endprint_1t != '\0' && *endprint_1t != '\n') {
              printf("Неправильний ввід даних\n");
              continue;
            }
            if (sscanf(input, "%f", &A[i]) != 1) {
              printf("Неправильний ввід даних\n");
              continue;
            } else {
              break;
            }          
          }
        }
        printf("\nМасив A: { ");
        for (int i = 0; i < n; i++){
          if ((int)A[i] == A[i]) {
            printf("%d ", (int)A[i]);
          } else{
          printf("%.3f ", A[i]);
          }
        }
        printf("}\n");
        int first_pos_i;
        for (int i = 0; i < n; i++){
          if (A[i] > 0 && A[i] != 0){
            if ((int)A[i] == A[i]) {
              int first_positive = A[i];
              printf("Перший додатній елемент масиву: %d\n", first_positive);
              first_pos_i = i;
            } else{
              float first_positive = A[i];
              printf("Перший додатній елемент масиву: %.3f\n", first_positive);
              first_pos_i = i;
            }
            break;
          }
        }
        int last_pos_i;
        for (int i = n-1; i >= 0; i--){
          if (A[i] > 0 && A[i] != 0){
            if ((int)A[i] == A[i]) {
              int last_positive = A[i];
              printf("Останній додатній елемент масиву: %d\n", last_positive);
              last_pos_i = i;
            } else{
              float last_positive = A[i];
              printf("Останній додатній елемент масиву: %.3f\n", last_positive);
              last_pos_i = i;
            }
            break;
          }
        }
        int formula = last_pos_i - first_pos_i;
        if (formula >= 0){
          printf("Кількість елементів між першим додатнім і останнім додатнім: %d\n", formula - 1);
        } else{
          printf("Кількість елементів між першим додатнім і останнім додатнім: %d\n", formula);
        }

        for (int i = 0; i < n - 1; i++) {
          for (int j = 0; j < n - i - 1; j++) {
            // Порівняння пар сусідніх елементів і обмін їх, якщо потрібно
            if (A[j] > A[j + 1]) {
              float t = A[j];
              A[j] = A[j + 1];
              A[j + 1] = t;
            }
          }
        }
        printf("\nМасив A після сортування: { ");
        for (int i = 0; i < n; i++){
          if ((int)A[i] == A[i]) {
            printf("%d ", (int)A[i]);
          } else{
          printf("%.3f ", A[i]);
          }
        }
        printf("}\n");        
        printf("\n");
      }

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
        
      else if (task == 2) {
        printf("             1 завдання (2 спосіб)\n");
        int lower_bound = 1;   // Мінімальна межа
        int upper_bound = 50;  // Максимальна межа
        // Я вирішив що максимальне значення масиву буде 50
        int random_n;
        float lower_d = -5000;   // Мінімальна межа для задання елементу
        float upper_d = 5000;    // Максимальна межа для задання елементу
        // Я вирішив що нехай це буде від -5000 до 5000

        srand(time(NULL)); // задаємо початкову точку генерації псевдовипадкових чисел 
        // Генеруємо випадкове число у заданих межах
        random_n = lower_bound + rand() % (upper_bound - lower_bound + 1);
        const int n = random_n;
        float A[n];

        for (int i = 0; i < n; i++){
          A[i] = lower_d + ((float)rand() / (float)RAND_MAX) * (upper_d - lower_d);
        }
        printf("\nМасив A: { ");
        for (int i = 0; i < n; i++){
          if ((int)A[i] == A[i]) {
            printf("%d ", (int)A[i]);
          } else{
          printf("%.2f ", A[i]);
          }
        }
        printf("}\n");
        int first_pos_i;
        for (int i = 0; i < n; i++){
          if (A[i] > 0 && A[i] != 0){
            if ((int)A[i] == A[i]) {
              int first_positive = A[i];
              printf("Перший додатній елемент масиву: %d\n", first_positive);
              first_pos_i = i;
            } else{
              float first_positive = A[i];
              printf("Перший додатній елемент масиву: %.3f\n", first_positive);
              first_pos_i = i;
            }
            break;
          }
        }
        int last_pos_i;
        for (int i = n-1; i >= 0; i--){
          if (A[i] > 0 && A[i] != 0){
            if ((int)A[i] == A[i]) {
              int last_positive = A[i];
              printf("Останній додатній елемент масиву: %d\n", last_positive);
              last_pos_i = i;
            } else{
              float last_positive = A[i];
              printf("Останній додатній елемент масиву: %.2f\n", last_positive);
              last_pos_i = i;
            }
            break;
          }
        }
        int formula = last_pos_i - first_pos_i;
        if (formula >= 0){
          printf("Кількість елементів між першим додатнім і останнім додатнім: %d\n", formula - 1);
        } else{
          printf("Кількість елементів між першим додатнім і останнім додатнім: %d\n", formula);
        }

        for (int i = 0; i < n - 1; i++) {
          for (int j = 0; j < n - i - 1; j++) {
            // Порівняння пар сусідніх елементів і обмін їх, якщо потрібно
            if (A[j] > A[j + 1]) {
              float t = A[j];
              A[j] = A[j + 1];
              A[j + 1] = t;
            }
          }
        }
        printf("\nМасив A після сортування: { ");
        for (int i = 0; i < n; i++){
          if ((int)A[i] == A[i]) {
            printf("%d ", (int)A[i]);
          } else{
          printf("%.2f ", A[i]);
          }
        }
        printf("}\n");        
        printf("\n"); 
      }

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

      else if(task == 3){
        printf("             2 завдання (1 спосіб)\n");
        int rows, columns;

        char *endprint_2t;
        while (true) {
          printf("Введіть кількість рядків двовимірного масиву rows: ");
          fgets(input, sizeof(input), stdin);
          if (strchr(input, '.') != NULL) {
            printf("Неправильний ввід даних\n");
            continue;
          }
          strtof(input, &endprint_2t); // перевірка на введення числа

          if (*endprint_2t != '\0' && *endprint_2t != '\n') {
            printf("Неправильний ввід даних\n");
            continue;
          }
          if (sscanf(input, "%d", &rows) != 1 || rows <= 0) {
            printf("Неправильний ввід даних\n");
            continue;
          } else {
            break;
          }
        }
        const int i = rows;

        while (true) {
          printf("Введіть кількість стовбців двовимірного масиву columns: ");
          fgets(input, sizeof(input), stdin);
          if (strchr(input, '.') != NULL) {
            printf("Неправильний ввід даних\n");
            continue;
          }
          strtof(input, &endprint_2t); // перевірка на введення числа

          if (*endprint_2t != '\0' && *endprint_2t != '\n') {
            printf("Неправильний ввід даних\n");
            continue;
          }
          if (sscanf(input, "%d", &columns) != 1 || columns <= 0) {
            printf("Неправильний ввід даних\n");
            continue;
          } else {
            break;
          }
        }
        const int j = columns;
        
        int array_2d[i][j];
        for (int k = 0; k < i; k++){
          for (int l = 0; l < j; l++){
            while (true) {
              printf("Введіть елемент масиву(ЦІЛЕ ЧИСЛО(я вирішив, що нехай це буде від -999 до 999), бо за умовою) [%d][%d]: ", k, l);
              fgets(input, sizeof(input), stdin);
              if (strchr(input, '.') != NULL) {
                printf("Неправильний ввід даних\n");
                continue;
              }
              strtof(input, &endprint_2t); // перевірка на введення числа

              if (*endprint_2t != '\0' && *endprint_2t != '\n') {
                printf("Неправильний ввід даних\n");
                continue;
              }
              if (sscanf(input, "%d", &array_2d[k][l]) != 1) {
                printf("Неправильний ввід даних\n");
                continue;
              } else {
                // Я вирішив що межі для елементів будкть від -999 до 999 за допомогою '#define' на початку,
                // бо з великими числами не дуже приємно працювати, але якщо що то можна змінити
                if (array_2d[k][l] <= MAX_FOR_2D && array_2d[k][l] >= MIN_FOR_2D){ 
                  break;
                } else{
                  printf("Неправильний ввід даних\n");
                  continue;
                }
              }          
            }
          }
        }
        // Вивід масиву на екран
        printf("\nДвовимірний масив:\n");
        for (int k = 0; k < i; k++) {
          printf("[ ");
          for (int l = 0; l < j; l++) {
            printf("%4d  ", array_2d[k][l]);
          }
          printf("]\n");
        }

        for (int k = 0; k < i; k++) {
          int even_nums = 0; // Ініціалізація перед використанням
          for (int l = 0; l < j; l++) {
            if (array_2d[k][l] % 2 == 0) {
              even_nums += 1;
            }
          }
          if (even_nums == j) {
            for (int l = 0; l < j / 2; l++) {
              int t = array_2d[k][l];
              array_2d[k][l] = array_2d[k][j - l - 1];
              array_2d[k][j - l - 1] = t;
            }
          }
        }

        // Вивід нового масиву
        printf("\nДвовимірний масив з дотриманням умови завдання:\n");
        for (int k = 0; k < i; k++) {
          printf("[ ");
          for (int l = 0; l < j; l++) {
            printf("%4d  ", array_2d[k][l]);
          }
          printf("]\n");
        }
        printf("\n");
      }

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

      else if(task == 4){
        printf("             2 завдання (2 спосіб)\n");
        srand(time(NULL)); // задаємо початкову точку генерації псевдовипадкових чисел
        const int max_rows = 20; // Максимальна кількість рядків
        const int min_rows = 1; // Мінімальна межа для розміру рядка
        const int max_columns = 20; // Максимальна кількість стовбців
        const int min_columns = 1; // Мінімальна межа для розміру стовбця
        // Я вирішив що нехай це буде до 20 рядків та стовбців
        int rows = min_rows + rand() % (max_rows - min_rows + 1);
        int columns = min_columns + rand() % (max_columns - min_columns + 1);
        int array_2d[rows][columns]; // сам масив
        for (int k = 0; k < rows; k++){
          for (int l = 0; l < columns; l++){
            // Я вирішив що нехай це буде до від -999 до 999 за допомогою '#define' на початку
            array_2d[k][l] = MIN_FOR_2D + rand() % (MAX_FOR_2D - MIN_FOR_2D + 1);
          }
        }

        printf("\nДвовимірний масив:\n");
        for (int k = 0; k < rows; k++) {
          printf("[ ");
          for (int l = 0; l < columns; l++) {
            printf("%4d  ", array_2d[k][l]);
          }
          printf("]\n");
        }

        for (int k = 0; k < rows; k++) {
            int even_nums = 0; // Ініціалізація перед використанням
            for (int l = 0; l < columns; l++) {
              if (array_2d[k][l] % 2 == 0) {
                even_nums += 1;
              }
            }
            if (even_nums == columns) {
              for (int l = 0; l < columns / 2; l++) {
                int t = array_2d[k][l];
                array_2d[k][l] = array_2d[k][columns - l - 1];
                array_2d[k][columns - l - 1] = t;
              }
            }
          }

          // Вивід нового масиву
          printf("\nДвовимірний масив з дотриманням умови завдання:\n");
          for (int k = 0; k < rows; k++) {
            printf("[ ");
            for (int l = 0; l < columns; l++) {
              printf("%4d  ", array_2d[k][l]);
            }
            printf("]\n");
          }
          printf("\n");
      }
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
      else if (task == 0) {
        printf("ВИХІД З ПРОГРАМИ!!!");
        exit(0);
      }
    }
  }
  return 0;
}