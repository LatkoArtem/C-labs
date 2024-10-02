/*
Лабораторна робота №3
Тема: -- подання рядків у мові програмування С;
      -- функції, описані в <string.h>.
Мета роботи: Вивчити опис символьних даних та операцій над ними.
Виконав студент Латко Артем, група КМ-32, варіант 10

Умова завдання: 1) Дано N речень, слова в яких розділено пробілами. Вивести їх на екран у порядку зростання кількості слів у реченні.
*/
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_N 10 
// Я вирішив що максимальна кількість речень - 10
#define MAX_SENTENCE_LENGTH 256
// Я вирішив що максимальна довжина речення - 256

bool isLettersOrSpacesOnly(const char *str) {
  while (*str) {
    if (!isalpha((unsigned char)*str) && !isspace((unsigned char)*str)) {
      return false;
    }
    str++;
  }
  return true;
}

int countWords(const char *sentence) {
  int wordCount = 0;
  const char *ptr = sentence;
  while (*ptr != '\0') {
    // Пропускаємо пробіли
    while (*ptr == ' ') {
      ptr++;
    }
    if (*ptr == '\0') {
      break;
    }
    while (*ptr != ' ' && *ptr != '\0') {
      ptr++;
    }
    wordCount++;
  }
  return wordCount;
}

int main() {
  printf("Виконав студент Латко Артем, група КМ-32, варіант 10\n\n");
  //задаємо змінні
  char input[100];
  int task;

  printf("1) 1 завдання способом заповнення масивів із клавіатури;\n");
  printf("0) Вихід з програми\n\n");

  while (true) {
    char *endprint;
    printf("Введіть номер завдання 1, а також щоб вийти введіть 0: ");
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
    if (sscanf(input, "%d", &task) != 1 || (task != 0 && task != 1)) {
      printf("Неправильний ввід даних\n");
      continue;
    } else {
      if (task == 1) {
        int N;
        char input[100];
        char sentences[MAX_N][MAX_SENTENCE_LENGTH];
        char sentence[MAX_N][MAX_SENTENCE_LENGTH];

        while (true) {
          printf("Введіть кількість речень N (максимальне значення для N - 10): ");
          fgets(input, sizeof(input), stdin);
          if (strchr(input, '.') != NULL) {
            printf("Неправильний ввід даних\n");
            continue;
          }

          char *endprint;
          N = strtol(input, &endprint, MAX_N); // перевірка на введення числа
          if (*endprint != '\0' && *endprint != '\n') {
            printf("Неправильний ввід даних\n");
            continue;
          }

          if (N <= 0 || N > MAX_N) {
            printf("Неправильний ввід даних\n");
            continue;
          }

          for (int i = 0; i < N; i++) {
            while (true) {
              printf("Введіть %d речення (можна використовувати лише ЛАТИНСЬКІ літери та пробіли, Max довжина - 256): ", i + 1);
              fgets(sentence[i], MAX_SENTENCE_LENGTH, stdin); // Зчитування рядка

              // Перевірка довжини рядка
              if (strlen(sentence[i]) >= MAX_SENTENCE_LENGTH - 1) {
                printf("Перевищено максимальну довжину речення.\n");
                // Очищення залишків буфера введення
                int c;
                while ((c = getchar()) != '\n' && c != EOF);
                continue;
              }

              // Перевірка наявності хоча б однієї літери
              bool containsLetter = false;
              for (int j = 0; j < strlen(sentence[i]); j++) {
                if (isalpha((unsigned char)sentence[i][j])) {
                  containsLetter = true;
                  break;
                }
              }
              if (!containsLetter) {
                printf("Не міститься жодної ЛАТИНСЬКОЇ літери.\n");
                continue;
              }
              
              // Перевірка рядка
              if (isLettersOrSpacesOnly(sentence[i])) {
                strcpy(sentences[i], sentence[i]);
                break;
              } else {
                printf("Неправильний ввід даних\n");
                continue;
              }
            }
          }
          printf("\nВиведення речень:\n");
          for (int i = 0; i < N; i++) {
            printf("%s", sentences[i]);
          }
          
          // Сортування речень за кількістю слів у них у порядку зростання(використано сортування бульбашкою, як в попередній лабі)
          for (int i = 0; i < N - 1; i++) {
            for (int j = 0; j < N - i - 1; j++) {
              if (countWords(sentences[j]) > countWords(sentences[j + 1])) {
                char temp[MAX_SENTENCE_LENGTH];
                strcpy(temp, sentences[j]);
                strcpy(sentences[j], sentences[j + 1]);
                strcpy(sentences[j + 1], temp);
              }
            }
          }

          printf("\nВиведення речень у порядку зростання кількості слів у реченні:\n");
          for (int i = 0; i < N; i++) {
            printf("%s", sentences[i]);
          }
          break;
        } 
      }
      else if (task == 0) {
        printf("ВИХІД З ПРОГРАМИ!!!");
        exit(0);
      }
    }
  }
  return 0;
}