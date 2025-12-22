#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ROUTES 5
#define MAX_NAME_LEN 50

// Оголошення структури ROUTE
struct ROUTE
{
    char start[MAX_NAME_LEN];
    char end[MAX_NAME_LEN];
    int route_number;
};

void swap(struct ROUTE *a, struct ROUTE *b)
{
    struct ROUTE temp = *a;
    *a = *b;
    *b = temp;
}

// Функція для сортування за номером маршруту
void sort_routes(struct ROUTE *routes, int n)
{
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n - i - 1; j++)
        {
            if (routes[j].route_number > routes[j + 1].route_number)
            {
                swap(&routes[j], &routes[j + 1]);
            }
        }
    }
}

// Функція для пошуку маршруту за номером
int find_route(struct ROUTE *routes, int n, int route_number, struct ROUTE *result)
{
    for (int i = 0; i < n; i++)
    {
        if (routes[i].route_number == route_number)
        {
            *result = routes[i];
            return 0; // Знайдено
        }
    }
    return -1; // Не знайдено
}