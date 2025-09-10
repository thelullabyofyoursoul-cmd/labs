#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 100

struct Item {
    int key;
    long offset; // смещение в файле
    int length;  // длина информации
};

struct Item table[SIZE];
int count = 0;
char filename[256];
FILE *dataFile;

int findByKey(int key) {
    for (int i = 0; i < count; i++) {
        if (table[i].key == key) return i;
    }
    return -1;
}

void addItem() {
    if (count >= SIZE) {
        printf("Таблица переполнена!\n");
        return;
    }
    int key;
    char buffer[256];
    printf("Введите ключ: ");
    scanf("%d", &key);
    if (findByKey(key) != -1) {
        printf("Ошибка: элемент с таким ключом уже существует!\n");
        return;
    }
    printf("Введите информацию: ");
    scanf(" %[^\n]", buffer);

    fseek(dataFile, 0, SEEK_END);
    long pos = ftell(dataFile);
    int len = strlen(buffer) + 1;
    fwrite(buffer, sizeof(char), len, dataFile);

    table[count].key = key;
    table[count].offset = pos;
    table[count].length = len;
    count++;
    printf("Элемент добавлен.\n");
}

void deleteItem() {
    int key;
    printf("Введите ключ для удаления: ");
    scanf("%d", &key);
    int idx = findByKey(key);
    if (idx == -1) {
        printf("Элемент не найден.\n");
        return;
    }
    for (int i = idx; i < count - 1; i++) {
        table[i] = table[i + 1];
    }
    count--;
    printf("Элемент удален.\n");
}

void searchRange() {
    int minKey, maxKey;
    printf("Введите минимальный ключ: ");
    scanf("%d", &minKey);
    printf("Введите максимальный ключ: ");
    scanf("%d", &maxKey);

    char *buffer = NULL;
    for (int i = 0; i < count; i++) {
        if (table[i].key >= minKey && table[i].key <= maxKey) {
            buffer = malloc(table[i].length);
            fseek(dataFile, table[i].offset, SEEK_SET);
            fread(buffer, sizeof(char), table[i].length, dataFile);
            printf("Ключ: %d, Инфо: %s\n", table[i].key, buffer);
            free(buffer);
        }
    }
}

void printTable() {
    char *buffer = NULL;
    for (int i = 0; i < count; i++) {
        buffer = malloc(table[i].length);
        fseek(dataFile, table[i].offset, SEEK_SET);
        fread(buffer, sizeof(char), table[i].length, dataFile);
        printf("%d: %s\n", table[i].key, buffer);
        free(buffer);
    }
}

void saveIndex() {
    FILE *indexFile = fopen("index.bin", "wb");
    fwrite(&count, sizeof(int), 1, indexFile);
    fwrite(table, sizeof(struct Item), count, indexFile);
    fclose(indexFile);
}

void loadIndex() {
    FILE *indexFile = fopen("index.bin", "rb");
    if (!indexFile) return;
    fread(&count, sizeof(int), 1, indexFile);
    fread(table, sizeof(struct Item), count, indexFile);
    fclose(indexFile);
}

void menu() {
    int choice;
    do {
        printf("\n--- МЕНЮ ---\n");
        printf("1. Добавить элемент\n");
        printf("2. Удалить элемент\n");
        printf("3. Поиск по диапазону\n");
        printf("4. Показать таблицу\n");
        printf("0. Выход\n");
        printf("Ваш выбор: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: addItem(); break;
            case 2: deleteItem(); break;
            case 3: searchRange(); break;
            case 4: printTable(); break;
            case 0: break;
            default: printf("Неверный выбор!\n");
        }
    } while (choice != 0);
}

int main() {
    printf("Введите имя файла данных: ");
    scanf("%s", filename);
    dataFile = fopen(filename, "a+b");
    if (!dataFile) {
        printf("Ошибка открытия файла.\n");
        return 1;
    }
    loadIndex();
    menu();
    saveIndex();
    fclose(dataFile);
    return 0;
}
