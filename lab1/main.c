/*
  Лабораторна робота №1
  Тема: Базові типи даних, уведення-виведення, бітові операції, операції зсуву
  Мета роботи: Здобути практичні навички роботи з типами даних мови
  програмування С, бітовими операціями, використання функцій стандартного
  уведення-виведення. Виконав студент Латко Артем, група КМ-32, варіант 10

  Умова завдання: 1) 1.Обчислити об’єм прямої призми, в основі якої лежить
  прямокутний трикутник з катетами a і b. Висота призми — h.
  2. Обчислити f(x) = x^2 + b, x = 1, 2, ..., 5.
  2) Елемент профілю користувача в системі подається у вигляді:

  № розряду | 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
  -----------------------------------------------------------
   значення |  G  G  G  G  G  G  G  G  S  S  S  S  P  0  0  0

   де,
   G..G — ідентифікатор групи, до якої належить користувач;
   S..S — код системи, яка завантажується для користувача;
   P — ознака привілейованого користувача.
*/
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  //задаємо змінні
  char input[100];
  float task;
  float a, b, h;

  printf("Виконав студент Латко Артем, група КМ-32, варіант 10\n");

  while (true) {
    char *endprint;
    printf("Введіть номер завдання 1 або 2, а також щоб вийти введіть 0: ");
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
    if (sscanf(input, "%f", &task) != 1 ||
        (task != 0 && task != 1 && task != 2)) {
      printf("Неправильний ввід даних\n");
      continue;
    } else {
      if (task == 1) {
        printf("\n             1 частина завдання\n");
        char *endprint_1t;
        while (true) {
          printf("Введіть значення катета a: ");
          fgets(input, sizeof(input), stdin);
          strtof(input, &endprint_1t); // перевірка на введення числа

          if (*endprint_1t != '\0' && *endprint_1t != '\n') {
            printf("Неправильний ввід даних\n");
            continue;
          }
          if (sscanf(input, "%f", &a) != 1 || a <= 0) {
            printf("Неправильний ввід даних\n");
            continue;
          } else {
            break;
          }
        }

        while (true) {
          printf("Введіть значення катета b: ");
          fgets(input, sizeof(input), stdin);
          strtof(input, &endprint_1t); // перевірка на введення числа

          if (*endprint_1t != '\0' && *endprint_1t != '\n') {
            printf("Неправильний ввід даних\n");
            continue;
          }
          if (sscanf(input, "%f", &b) != 1 || b <= 0) {
            printf("Неправильний ввід даних\n");
            continue;
          } else {
            break;
          }
        }

        while (true) {
          printf("Введіть значення висоти призми h: ");
          fgets(input, sizeof(input), stdin);
          strtof(input, &endprint_1t); // перевірка на введення числа

          if (*endprint_1t != '\0' && *endprint_1t != '\n') {
            printf("Неправильний ввід даних\n");
            continue;
          }
          if (sscanf(input, "%f", &h) != 1 || h <= 0) {
            printf("Неправильний ввід даних\n");
            continue;
          } else {
            break;
          }
        }

        float prism_volume = a * b * h / 2;
        if (prism_volume > (int)prism_volume) {
          printf("Об'єм призми = %.3f\n",
                 prism_volume); // округлення до 3 знаків після коми
        } else {
          printf("Об'єм призми = %d\n", (int)prism_volume);
        }

        printf("             2 частина завдання\n");
        char *endptr;
        int b2;
        while (true) {
          printf("Введіть значення b: ");
          fgets(input, sizeof(input), stdin);
          if (strchr(input, '.') != NULL) {
            printf("Неправильний ввід даних\n");
            continue;
          }
          strtof(input, &endptr); // перевірка на введення числа

          if (*endptr != '\0' && *endptr != '\n') {
            printf("Неправильний ввід даних\n");
            continue;
          }
          if (sscanf(input, "%d", &b2) != 1) {
            printf("Неправильний ввід даних\n");
            continue;
          } else {
            break;
          }
        }
        for (int x = 1; x <= 5; x++) {
          float f_x;
          f_x = x * x + b2;
          if (f_x > (int)f_x) {
            printf("f(%d) = %.3f\n", x,
                   f_x); // округлення до 3 знаків після коми
          } else {
            printf("f(%d) = %d\n", x, (int)f_x);
          }
        }
        printf("\n");
      }

      else if (task == 2) {
        unsigned char g; // ідентифікатор групи, до якої належить користувач
                         // (від 0 до 255)
        unsigned char
            s; // код системи, яка завантажується для користувача (від 0 до 15)
        unsigned char p; // ознака привілейованого користувача (від 0 до 1)
        unsigned int UnitStateWord; // eлемент профілю користувача

        printf("Уведіть ідентифікатор групи, до якої належить "
               "користувач(0-255): ");
        while (true) {
          if (scanf("%hhd", &g) == 1 && g >= 0 && g <= 255) {
            while (getchar() != '\n')
              ; // Очистити буфер вводу
            break;
          } else {
            printf("Неправильний ввід даних\n");
            while (getchar() != '\n')
              ; // Очистити буфер вводу
            printf("Уведіть ідентифікатор групи, до якої належить "
                   "користувач(0-255): ");
          }
        }

        printf(
            "Уведіть код системи, яка завантажується для користувача(0-15): ");
        while (true) {
          if (scanf("%hhd", &s) == 1 && s >= 0 && s <= 15) {
            while (getchar() != '\n')
              ; // Очистити буфер вводу
            break;
          } else {
            printf("Неправильний ввід даних\n");
            while (getchar() != '\n')
              ; // Очистити буфер вводу
            printf("Уведіть код системи, яка завантажується для "
                   "користувача(0-15): ");
          }
        }

        printf("Уведіть ознаку привілейованого користувача(0/1): ");
        while (true) {
          if (scanf("%hhd", &p) == 1 && p >= 0 && p <= 1) {
            while (getchar() != '\n')
              ; // Очистити буфер вводу
            break;
          } else {
            printf("Неправильний ввід даних\n");
            while (getchar() != '\n')
              ; // Очистити буфер вводу
            printf("Уведіть ознаку привілейованого користувача(0/1): ");
          }
        }

        UnitStateWord = ((unsigned char)g & 0xFF) << 8;
        UnitStateWord |= ((unsigned char)s & 0xF) << 4;
        UnitStateWord |= p & 1;

        printf("Слово стану пристрою = %04x\n\n", UnitStateWord);

        printf("Уведіть елемент профілю користувача\n");
        printf("(16-кове число від 0 до 0xFFFF): ");
        while (true) {
          if (scanf("%x", &UnitStateWord) == 1) {
            while (getchar() != '\n')
              ; // Очистити буфер вводу
            break;
          } else {
            printf("Неправильний ввід даних\n");
            while (getchar() != '\n')
              ; // Очистити буфер вводу
            printf("Уведіть елемент профілю користувача\n");
            printf("(16-кове число від 0 до 0xFFFF): ");
          }
        }
        g = (UnitStateWord >> 8) & 0xFF;
        s = (UnitStateWord >> 4) & 0xF;
        p = UnitStateWord & 1;

        /*виведення результатів у зручному форматі*/
        printf("Ідентифікатор групи = %d\n", g);
        printf("Код системи = %d\n", s);
        printf("Ознака привілейованого користувача = %d\n\n", p);
      }

      else if (task == 0) {
        printf("ВИХІД З ПРОГРАМИ!!!");
        exit(0);
      }

      else {
        printf("Неправильний ввід даних\n");
        while (getchar() != '\n')
          ;
      }
    }
  }
  return 0;
}