#include <stdio.h>
#include <stdlib.h>

int sumDigits(int x) {
    int sum = 0;
    x = abs(x);
    while (x > 0) {
        sum += x % 10;
        x /= 10;
    }
    return sum;
}

int main() {
    int m, n;
    
    printf("Введите количество строк m и столбцов n: ");
    if (scanf("%d %d", &m, &n) != 2 || m <= 0 || n <= 0) {
        printf("Ошибка ввода размеров матрицы.\n");
        return 1;
    }

    // Динамически выделяем память под исходную матрицу размером m x n
    int **matrix = malloc(m * sizeof(int *));
    for (int i = 0; i < m; i++) {
        matrix[i] = malloc(n * sizeof(int));
    }
    printf("Введите элементы матрицы (%d строк по %d чисел):\n", m, n);
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            scanf("%d", &matrix[i][j]);
        }
    }

    // Подготовка структуры для новой матрицы:для каждой строки храним массив из n (максимум) элементов
    int **newMatrix = malloc(m * sizeof(int *));
    int *newSizes  = malloc(m * sizeof(int));  // фактические размеры строк в новой матрице
    for (int i = 0; i < m; i++) {
        newMatrix[i] = malloc(n * sizeof(int));
        newSizes[i] = 0;
    }

    // Формируем новую матрицу по заданному правилу
    for (int i = 0; i < m; i++) {
        int targetSum = sumDigits(matrix[i][n - 1]);

        // Проверяем все числа в этой строке
        for (int j = 0; j < n; j++) {
            if (sumDigits(matrix[i][j]) == targetSum) {
                newMatrix[i][ newSizes[i]++ ] = matrix[i][j];
            }
        }
    }

    // Вывод исходной матрицы
    printf("\nИсходная матрица (%d x %d):\n", m, n);
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            printf("%6d", matrix[i][j]);
        }
        printf("\n");
    }

    // Вывод новой матрицы
    printf("\nНовая матрица (элементы с нужной суммой цифр):\n");
    for (int i = 0; i < m; i++) {
        printf("Строка %d:", i + 1);
        if (newSizes[i] == 0) {
            printf(" (нет элементов)\n");
        } else {
            //  отобранные числа
            for (int k = 0; k < newSizes[i]; k++) {
                printf(" %d", newMatrix[i][k]);
            }
            printf("\n");
        }
    }

    // Освобождение динамической памяти
    for (int i = 0; i < m; i++) {
        free(matrix[i]);
        free(newMatrix[i]);
    }
    free(matrix);
    free(newMatrix);
    free(newSizes);

    return 0;
}
