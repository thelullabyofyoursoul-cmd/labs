//Написать программу для работы с просматриваемой таблицей по запросам оператора.
//Просматриваемая таблица организована вектором; каждый элемент таблицы имеет следующую структуру:
//struct Item
//int key;
/// * ключ элемента
//char *info; / * указатель на информацию
//* /
//* /
//Максимальный размер таблицы ограничен (для задания максимального размера таблицы использовать константу -
//Hanpuep, const int SIZE=…..；）.
//Предусмотреть следующие операции:
//включение нового элемента в таблицу при условии, что в таблице не может быть двух элементов с
//одинаковыми ключами; если при включении нового элемента возникает такая ситуация, на экран должно быть выведено сообщение об ошибке;
//удаление из таблицы элемента, заданного своим ключом, с реорганизацией таблицы; поиск элементов, заданных диапазоном ключей. В таблице могут отсутствовать элементы с ключами,
//задающими диапазон. Результатом поиска должна быть новая таблица, содержащая найденные элементы;
//вывод содержимого таблицы на экран.
//Разработать два варианта программы:
//a)
//b)
//и сама таблица, и информация, относящаяся к элементу таблицы, хранятся в основной памяти; и сама таблица, и информация, относящаяся к элементу таблицы, хранятся во внешней памяти
//(используется двоичный файл произвольного доступа). Все операции выполняются с таблицей, размещенной в основной памяти. Таблица считывается из файла (или создается в первый раз) в начале сеанса работы и записывается в файл в конце сеанса работы. Информация, относящаяся к элементу таблицы, записывается в файл сразу же при выполнении операции включения в таблицу. Имя файла вводится по запросу из программы.
//Примечания:
//1.
//Программа должна содержать несколько функций; функция та in должна выполнять: вывод меню, ввод и
//анализ ответа, вызов на исполнение требуемой функции;
//В программе нужно предусмотреть проверку правильности ввода данных;
//3.
//Для варианта b) следует модифицировать структуру, определяющую элемент таблицы, включив в нее
//длину информации и ее смещение в файле;
//4.
//В варианте b) для работы с файлом использовать функции пакета stdio.h; чтение и запись выполнять с
//помощью fread ( ) и fwrite (), в которых должна быть указана реальная длина информации.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 100

// Структура для элемента таблицы
typedef struct {
    int key;
    char* info;
} Item;

// Структура для самой таблицы
typedef struct {
    Item items[SIZE];
    int count;
} Table;

Table table = {0};

// Прототипы функций
int read_int();
void add_item();
void delete_item();
void search_items();
void display_table();
void free_table();

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

    // Выделяем память для строки и копируем информацию
    char* info_copy = (char*)malloc(strlen(info) + 1);
    if (info_copy == NULL) {
        printf("Ошибка выделения памяти.\n");
        return;
    }
    strcpy(info_copy, info);

    // Добавление элемента в таблицу
    table.items[table.count].key = key;
    table.items[table.count].info = info_copy;
    table.count++;

    printf("Элемент добавлен успешно.\n");
}

void delete_item() {
    printf("Введите ключ для удаления: ");
    int key = read_int();

    for (int i = 0; i < table.count; i++) {
        if (table.items[i].key == key) {
            // Освобождаем память, занятую информацией
            free(table.items[i].info);
            
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
            // Создаем копию элемента для результата
            result.items[result.count].key = table.items[i].key;
            
            // Выделяем память и копируем информацию
            result.items[result.count].info = (char*)malloc(strlen(table.items[i].info) + 1);
            if (result.items[result.count].info == NULL) {
                printf("Ошибка выделения памяти.\n");
                // Освобождаем уже выделенную память в случае ошибки
                for (int j = 0; j < result.count; j++) {
                    free(result.items[j].info);
                }
                return;
            }
            strcpy(result.items[result.count].info, table.items[i].info);
            
            result.count++;
        }
    }

    if (result.count == 0) {
        printf("Элементы в диапазоне не найдены.\n");
        return;
    }

    printf("Найденные элементы:\n");
    for (int i = 0; i < result.count; i++) {
        printf("Ключ: %d, Информация: %s\n", result.items[i].key, result.items[i].info);
    }

    // Освобождаем память, выделенную для результатов поиска
    for (int i = 0; i < result.count; i++) {
        free(result.items[i].info);
    }
}

void display_table() {
    if (table.count == 0) {
        printf("Таблица пуста.\n");
        return;
    }

    printf("Содержимое таблицы:\n");
    for (int i = 0; i < table.count; i++) {
        printf("Ключ: %d, Информация: %s\n", table.items[i].key, table.items[i].info);
    }
}

void free_table() {
    for (int i = 0; i < table.count; i++) {
        free(table.items[i].info);
    }
    table.count = 0;
}

int main() {
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
            case 5: break;
            default: printf("Неверный выбор.\n");
        }
    } while (choice != 5);

    free_table();
    
    return 0;
}