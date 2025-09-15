#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 100

// Структура для элемента таблицы
typedef struct {
    int key;
    long offset;
    int length;
} Item;

// Структура для самой таблицы
typedef struct {
    Item items[SIZE];
    int count;
} Table;

Table table;
FILE *data_file = NULL;

// Прототипы функций
void save_table_to_file();
int read_int();
void add_item();
void delete_item();
void search_items();
void display_table();
void load_table_from_file();

int read_int() {
    int value;
    while (scanf("%d", &value) != 1) {
        printf("Ошибка ввода. Введите целое число: ");
        while (getchar() != '\n');
    }
    return value;
}

void add_item() {
    if (table.count >= SIZE) {
        printf("Таблица полна. Невозможно добавить элемент.\n");
        return;
    }

    printf("Введите ключ: ");
    int key = read_int();

    // Проверка на уникальность ключа
    for (int i = 0; i < table.count; i++) {
        if (table.items[i].key == key) {
            printf("Ошибка: элемент с ключом %d уже существует.\n", key);
            return;
        }
    }

    printf("Введите информацию: ");
    char info[256];
    scanf(" %255[^\n]", info);
    int length = strlen(info) + 1;

    // Запись информации в файл
    fseek(data_file, 0, SEEK_END);
    long offset = ftell(data_file);

    if (fwrite(info, sizeof(char), length, data_file) != length) {
        printf("Ошибка записи в файл.\n");
        return;
    }
    fflush(data_file);

    // Добавление элемента в таблицу
    table.items[table.count].key = key;
    table.items[table.count].offset = offset;
    table.items[table.count].length = length;
    table.count++;

    printf("Элемент добавлен успешно.\n");
}

void delete_item() {
    printf("Введите ключ для удаления: ");
    int key = read_int();

    for (int i = 0; i < table.count; i++) {
        if (table.items[i].key == key) {
            table.items[i] = table.items[table.count - 1];
            table.count--;
            
            printf("Элемент удален успешно.\n");
            return;
        }
    }
    printf("Элемент с ключом %d не найден.\n", key);
}

void search_items() {
    printf("Введите начальный ключ: ");
    int start = read_int();
    printf("Введите конечный ключ: ");
    int end = read_int();

    Table result = {0};

    for (int i = 0; i < table.count; i++) {
        if (table.items[i].key >= start && table.items[i].key <= end) {
            result.items[result.count] = table.items[i];
            result.count++;
        }
    }

    if (result.count == 0) {
        printf("Элементы в диапазоне не найдены.\n");
        return;
    }

    printf("Найденные элементы:\n");
    for (int i = 0; i < result.count; i++) {
        char *info = (char*)malloc(result.items[i].length);
        fseek(data_file, result.items[i].offset, SEEK_SET);
        fread(info, sizeof(char), result.items[i].length, data_file);
        printf("Ключ: %d, Информация: %s\n", result.items[i].key, info);
        free(info);
    }
}

void display_table() {
    if (table.count == 0) {
        printf("Таблица пуста.\n");
        return;
    }

    printf("Содержимое таблицы:\n");
    for (int i = 0; i < table.count; i++) {
        char *info = (char*)malloc(table.items[i].length);
        fseek(data_file, table.items[i].offset, SEEK_SET);
        fread(info, sizeof(char), table.items[i].length, data_file);
        printf("Ключ: %d, Информация: %s\n", table.items[i].key, info);
        free(info);
    }
}

void load_table_from_file() {
    if (fread(&table.count, sizeof(int), 1, data_file) != 1) {
        table.count = 0;
        return;
    }
    
    if (table.count > SIZE) table.count = SIZE;
    
    fread(table.items, sizeof(Item), table.count, data_file);
}

void save_table_to_file() {
    rewind(data_file);
    fwrite(&table.count, sizeof(int), 1, data_file);
    fwrite(table.items, sizeof(Item), table.count, data_file);
}

int main() {
    char filename[256];
    printf("Введите имя файла: ");
    scanf("%255s", filename);

    data_file = fopen(filename, "r+b");
    if (!data_file) {
        data_file = fopen(filename, "w+b");
        if (!data_file) {
            printf("Ошибка открытия файла.\n");
            return 1;
        }
    }

    load_table_from_file();

    int choice;
    do {
        printf("\nМеню:\n");
        printf("1. Добавить элемент\n");
        printf("2. Удалить элемент\n");
        printf("3. Поиск по диапазону\n");
        printf("4. Вывод таблицы\n");
        printf("5. Выход\n");
        printf("Выберите операцию: ");
        choice = read_int();

        switch (choice) {
            case 1: add_item(); break;
            case 2: delete_item(); break;
            case 3: search_items(); break;
            case 4: display_table(); break;
            case 5: save_table_to_file(); break;
            default: printf("Неверный выбор.\n");
        }
    } while (choice != 5);

    fclose(data_file);
    return 0;
}