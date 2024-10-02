/*
Лабораторна робота №5
Тема: Типи даних, визначені користувачем
Мета роботи: Вивчити синтаксис і правила роботи з типами даних, визначеними користувачем.
Виконав студент Латко Артем, група КМ-32, варіант 10

Умова завдання: 
1) 1. Описати структуру з іменем ROUTE, що містить такі поля:
  - назва початкового пункту маршруту;
  - назва кінцевого пункту маршруту;
  - номер маршруту.
2. Написати програму, що виконує такі дії:
  - уведення з клавіатури даних у масив із 5 структур типу ROUTE;
  - упорядкування записів за номерами маршрутів;
  - виведення на екран інформації про маршрут, номер якого уведено з клавіатури; якщо такого маршруту немає, вивести на екран відповідне повідомлення.
*/
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

// Функція для перевірки на введення int
int check_for_int(char *text, int *variable) {
  char input[100];
  while (true) {
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


// Функція для перевірки на введення char
int check_for_char(char *text, char *variable) {
  char input[100];
  while (true) {
    printf("%s", text);
    fgets(input, sizeof(input), stdin);
    if (sscanf(input, "%s", variable) != 1) {
      printf("Неправильний ввід даних\n");
      continue;
    }
    break;
  }
  return 0;
}


// Оголошення структури ROUTE
struct ROUTE {
  char start[50];   // - назва початкового пункту маршруту;
  char end[50];     // - назва кінцевого пункту маршруту;
  int route_number; // - номер маршруту
};


// Функція для сортування за номером маршруту
void swap(struct ROUTE *a, struct ROUTE *b) {
  struct ROUTE temp = *a;
  *a = *b;
  *b = temp;
}


int main() {
  printf("Виконав студент Латко Артем, група КМ-32, варіант 10\n\n");
  printf("1) 1 завдання;\n");
  printf("0) Вихід з програми\n\n");
  // задаємо текст для функції перевірки на введення, щоб коли вводиш
  // неправильно, то щоб ще раз писало що потрібно ввести
  char text_1[] = "Введіть номер завдання 1, а також щоб вийти введіть 0: ";
  // задаємо змінну номеру завдання
  int task;

  while (true) {
    // Вводимо дані та перевіряємо на тип даних
    check_for_int(text_1, &task);

    if (task == 1){
      // 1 Завдання
      struct ROUTE routes[5];
      int search_route;
      // Уведення даних про маршрути
      for (int i = 0; i < 5; i++){
        printf("Маршрут №%d\n", i+1);
        check_for_char("Введіть назву початкового пункту маршруту: ", routes[i].start);
        check_for_char("Введіть назву кінцевого пункту маршруту: ", routes[i].end);
        while(true){
          check_for_int("Введіть номер маршруту: ", &routes[i].route_number);
          if (routes[i].route_number < 1){
            printf("Номер маршруту не може бути менше 1\n");
            continue;
          }
          break;
        }
      }
      // Сортування масиву за номерами маршрутів за допомогою сортування бульбашкою
      for (int i = 0; i < 5 - 1; i++) {
        for (int j = 0; j < 5 - i - 1; j++) {
          if (routes[j].route_number > routes[j + 1].route_number) {
            swap(&routes[j], &routes[j + 1]);
          }
        }
      }
      // Виведення відсортованого масиву за номерами маршрутів у виляді таблички
      printf("\n%-30s|  %s\n", "Номер марш.", "Початок - Кінець");
      printf("------------------------------------------------------\n");
      for (int i = 0; i < 5; i++) {
        printf("%-21d|  %s - %s\n", routes[i].route_number, routes[i].start, routes[i].end);
      }
      printf("\n");
      
      // Пошук і виведення інформації про маршрут за номером
      while(true){
        check_for_int("Введіть номер маршруту, який хочете знайти, або введіть 0 щоб закінчити пошук: ", &search_route);
        if (search_route == 0){
          break;
        }
        int found = 0;
        for (int i = 0; i < 5; i++) {
          if (routes[i].route_number == search_route) {
            printf("Знайдений маршрут №%d: %s - %s\n", search_route, routes[i].start, routes[i].end);
            found = 1;
            break;
          }
        }
        if (!found) {
          printf("Такий маршрут не знайдено.\n");
        }
      }
    }
    else if (task == 0){
      // ВИХІД ІЗ ПРОГРАМИ
      system("clear");
      printf("\nВИХІД З ПРОГРАМИ!!!");
      exit(0);
    }
    else {
      printf("Неправильний ввід даних\n");
    }
  }
  return 0;
}