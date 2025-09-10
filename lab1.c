#include <stdio.h>
#include <stdlib.h>

// Функция для вычисления суммы цифр числа
int sum_digits(int x) {
    if (x < 0) x = -x; // работаем с положительным значением
    int sum = 0;
    while (x > 0) {
        sum += x % 10;
        x /= 10;
    }
    return sum;
}

int main() {
    int m;
    printf("Введите количество строк m: ");
    scanf("%d", &m);

    // Динамическое хранение матрицы
    int **matrix = (int **)malloc(m * sizeof(int *));
    int *cols = (int *)malloc(m * sizeof(int)); // количество элементов в каждой строке

    // Ввод матрицы
    for (int i = 0; i < m; i++) {
        printf("Введите количество элементов в %d-й строке: ", i + 1);
        scanf("%d", &cols[i]);
        matrix[i] = (int *)malloc(cols[i] * sizeof(int));
        printf("Введите элементы %d-й строки: ", i + 1);
        for (int j = 0; j < cols[i]; j++) {
            scanf("%d", &matrix[i][j]);
        }
    }

    // Вывод исходной матрицы
    printf("\nИсходная матрица:\n");
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < cols[i]; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }

    // Проверка, что есть хотя бы 2 строки
    if (m < 2) {
        printf("\nОшибка: во входной матрице меньше двух строк.\n");
        return 0;
    }

    // Сумма цифр последнего числа второй строки
    int last_num = matrix[1][cols[1] - 1];
    int target_sum = sum_digits(last_num);

    // Формируем новую матрицу (пусть g = 1, т.е. первая строка новой матрицы)
    int *new_row = (int *)malloc(cols[1] * sizeof(int));
    int new_count = 0;

    for (int j = 0; j < cols[1]; j++) {
        if (sum_digits(matrix[1][j]) == target_sum) {
            new_row[new_count++] = matrix[1][j];
        }
    }

    // Вывод новой матрицы
    printf("\nНовая матрица (в 1-й строке числа из 2-й строки исходной, сумма цифр которых равна %d):\n", target_sum);
    for (int j = 0; j < new_count; j++) {
        printf("%d ", new_row[j]);
    }
    printf("\n");

    // Освобождение памяти
    for (int i = 0; i < m; i++) {
        free(matrix[i]);
    }
    free(matrix);
    free(cols);
    free(new_row);

    return 0;
}
