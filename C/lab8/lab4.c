#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#define MAX_COORDINATE 20
#define MIN_COORDINATE -20

// Функція для підрахунку відстані від початку координат до точки
float distance(float x, float y)
{
    return sqrt((x * x) + (y * y));
}

// Функція підрахунку кількості точок
int points_inside_circle(float *X, float *Y, float radius)
{
    if (radius < 0)
    {
        errno = 0;
        errno = EINVAL;
        return 0;
    }
    int count = 0;
    for (int i = 0; i < 10; i++)
    {
        if (distance(X[i], Y[i]) <= radius)
        {
            count++;
        }
    }
    return count;
}

void generate_points(float *X, float *Y)
{
    for (int i = 0; i < 10; i++)
    {
        X[i] = MIN_COORDINATE +
               ((float) rand() / (float) RAND_MAX) * (MAX_COORDINATE - MIN_COORDINATE);
        Y[i] = MIN_COORDINATE +
               ((float) rand() / (float) RAND_MAX) * (MAX_COORDINATE - MIN_COORDINATE);
    }
}

// Функція для обчислення біноміального коефіцієнта рекурсивно
int binomial_coefficient(int m, int n)
{
    if (m < 0 || n < 0 || m > n)
    {
        return -1; // Помилка: некоректні вхідні дані
    }
    if (m == 0 || m == n)
    {
        return 1;
    }
    return binomial_coefficient(m, n - 1) + binomial_coefficient(m - 1, n - 1);
}