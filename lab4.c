//Написать программу для работы по запросам оператора с упорядоченной таблицей, реализованной в виде двоичного дерева поиска.
//Ключ - строка произвольной длины. Информация - строка произвольной длины. Узел дерева содержит указатель на ключ, указатели на правое и левое поддеревья и указатель на информационное поле.
//В таблице не могут храниться записи с одинаковыми ключами.
//Предусмотреть следующие операции:
//-включение нового элемента в таблицу без нарушения свойств упорядоченности; если
//информация с заданным ключом уже есть, то выводится сообщение об ошибке;
//-удаление из таблицы элемента, заданного своим ключом, без нарушения свойств
//упорядоченности таблицы;
//-поиск информации по заданному ключу;
//-вывод всего содержимого таблицы в прямом порядке следования ключей, превышающих
//заданное значение ключа; если ключ не указан, то всей таблицы;
//-поиск элемента, соответствующего значению ключа, совпадающего с заданным по первым N
//символам (выводятся все элементы, удовлетворяющие условию).
//Примечания:
//1.
//Программа должна содержать несколько функций; функция main () должна выполнять: вывод
//меню, ввод и анализ ответа, вызов на исполнение требуемой функции.
//2. В программе нужно предусмотреть проверку правильности ввода данных.
//3.Оценить сложность реализованных алгоритмов.
//4. (*) Для целей отладки реализовать форматированный вывод таблицы в виде дерева.
//5. (*) Для целей отладки реализовать загрузку таблицы из файла в формате
﻿﻿//Ключ1
﻿﻿//Инфрмация 1
//﻿﻿Ключ2 
//6.
//(*) Провести таймирование (или профилирование) программы.
// bst_table.c
// Лабораторная: Упорядоченная таблица на базе ДДП (BST) с ключом-строкой и информацией-строкой.
// Функции: вставка, удаление, поиск, вывод > ключа (или всего), поиск по префиксу первых N символов,
// печать дерева, загрузка из файла, таймирование, безопасный ввод, контроль дубликатов.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

typedef struct Node {
    char *key;
    char *info;
    struct Node *left, *right;
} Node;

// Создание нового узла с проверкой ошибок
Node* createNode(const char *key, const char *info) {
    Node *newNode = (Node*)malloc(sizeof(Node));
    if (!newNode) return NULL;

    newNode->key = strdup(key);
    newNode->info = strdup(info);
    
    if (!newNode->key || !newNode->info) {
        if (newNode->key) free(newNode->key);
        if (newNode->info) free(newNode->info);
        free(newNode);
        return NULL;
    }

    newNode->left = newNode->right = NULL;
    return newNode;
}

// Итеративный поиск по ключу
Node* search(Node *root, const char *key) {
    Node *current = root;
    while (current != NULL) {
        int cmp = strcmp(key, current->key);
        if (cmp == 0)
            return current;
        current = (cmp < 0) ? current->left : current->right;
    }
    return NULL;
}

// Итеративная вставка элемента
Node* insert(Node* root, const char* key, const char* info) {
    if (!root) return createNode(key, info);

    Node* current = root;
    Node* parent = NULL;

    // Поиск места для вставки
    while (current != NULL) {
        parent = current;
        int cmp = strcmp(key, current->key);
        
        if (cmp == 0) {
            printf("Ошибка: ключ '%s' уже существует!\n", key);
            return root;
        }
        
        current = (cmp < 0) ? current->left : current->right;
    }

    // Создание нового узла
    Node* newNode = createNode(key, info);
    if (!newNode) {
        printf("Ошибка выделения памяти!\n");
        return root;
    }

    // Определение позиции для вставки
    int cmp = strcmp(key, parent->key);
    if (cmp < 0)
        parent->left = newNode;
    else
        parent->right = newNode;

    return root;
}

// Поиск минимального узла в поддереве
Node* findMin(Node *root) {
    if (!root) return NULL;
    while (root->left) root = root->left;
    return root;
}

// Удаление узла по ключу
Node* delete(Node *root, const char *key) {
    Node* current = root;
    Node* parent = NULL;

    // Поиск удаляемого узла
    while (current != NULL) {
        int cmp = strcmp(key, current->key);
        if (cmp == 0) break;
        
        parent = current;
        current = (cmp < 0) ? current->left : current->right;
    }

    if (!current) {
        printf("Ключ '%s' не найден!\n", key);
        return root;
    }

    // Случай 1: у узла нет потомков или только один потомок
    if (!current->left || !current->right) {
        Node* temp = current->left ? current->left : current->right;
        
        // Если удаляется корень
        if (!parent) {
            free(current->key);
            free(current->info);
            free(current);
            return temp;
        }

        // Определение, каким потомком является current у parent
        if (parent->left == current)
            parent->left = temp;
        else
            parent->right = temp;

        free(current->key);
        free(current->info);
        free(current);
    } 
    // Случай 2: у узла два потомка
    else {
        Node* successor = findMin(current->right);
        
        // Сохранение данных преемника
        char* successor_key = strdup(successor->key);
        char* successor_info = strdup(successor->info);
        
        // Рекурсивное удаление преемника
        root = delete(root, successor->key);
        
        // Замена данных текущего узла
        free(current->key);
        free(current->info);
        current->key = successor_key;
        current->info = successor_info;
    }
    
    return root;
}

// Рекурсивный вывод элементов с ключами > заданного
void printGreater(Node *root, const char *minKey) {
    if (!root) return;
    
    // Если minKey не задан, выводим все элементы
    int cmp = minKey ? strcmp(root->key, minKey) : 1;
    
    if (cmp > 0) {
        printGreater(root->left, minKey);
        printf("Ключ: %s, Информация: %s\n", root->key, root->info);
    }
    printGreater(root->right, minKey);
}

// Рекурсивный поиск по первым N символам
void searchByPrefix(Node *root, const char *prefix, int N) {
    if (!root) return;
    
    if (strncmp(root->key, prefix, N) == 0) {
        printf("Найдено: %s : %s\n", root->key, root->info);
    }
    
    searchByPrefix(root->left, prefix, N);
    searchByPrefix(root->right, prefix, N);
}

// Рекурсивный вывод дерева
void printTree(Node *root, int level) {
    if (!root) return;
    
    printTree(root->right, level + 1);
    
    for (int i = 0; i < level; i++) printf("    ");
    printf("%s\n", root->key);
    
    printTree(root->left, level + 1);
}

// Освобождение памяти дерева
void freeTree(Node *root) {
    if (!root) return;
    
    freeTree(root->left);
    freeTree(root->right);
    free(root->key);
    free(root->info);
    free(root);
}

// Загрузка данных из файла
Node* loadFromFile(Node *root, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Ошибка открытия файла %s!\n", filename);
        return root;
    }
    
    char key[256], info[256];
    int count = 0;
    
    while (fgets(key, sizeof(key), file)) {
        key[strcspn(key, "\n")] = 0;
        
        if (!fgets(info, sizeof(info), file)) {
            printf("Ошибка: файл имеет нечетное количество строк!\n");
            break;
        }
        info[strcspn(info, "\n")] = 0;
        
        root = insert(root, key, info);
        count++;
    }
    
    fclose(file);
    printf("Загружено %d записей из файла %s\n", count, filename);
    return root;
}

// Функция для таймирования операции
void timeOperation(Node *root, int operationType, const char *key, const char *info) {
    clock_t start = clock();
    
    switch (operationType) {
        case 1:
            insert(root, key, info);
            break;
        case 2:
            search(root, key);
            break;
        case 3:
            delete(root, key);
            break;
    }
    
    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Операция выполнена за %f секунд\n", time_spent);
}

void safeInput(char *buffer, int size, const char *prompt) {
    printf("%s", prompt);
    if (!fgets(buffer, size, stdin)) {
        buffer[0] = '\0';
        return;
    }
    buffer[strcspn(buffer, "\n")] = 0;
}

int main() {
    Node *root = NULL;
    int choice;
    char key[256], info[256], prefix[256], filename[256];
    int N;
    
    do {
        printf("\nМеню:\n");
        printf("1. Вставить элемент\n");
        printf("2. Удалить элемент\n");
        printf("3. Поиск по ключу\n");
        printf("4. Вывод элементов с ключами больше заданного\n");
        printf("5. Поиск по первым N символам ключа\n");
        printf("6. Печать дерева\n");
        printf("7. Загрузка данных из файла\n");
        printf("8. Таймирование операций\n");
        printf("0. Выход\n");
        printf("Выбор: ");
        
        if (scanf("%d", &choice) != 1) {
            printf("Ошибка ввода! Пожалуйста, введите число.\n");
            while (getchar() != '\n'); // Очистка буфера ввода
            continue;
        }
        getchar(); // Удаление символа новой строки
        
        switch (choice) {
            case 1:
                safeInput(key, sizeof(key), "Введите ключ: ");
                safeInput(info, sizeof(info), "Введите информацию: ");
                root = insert(root, key, info);
                break;
            case 2:
                safeInput(key, sizeof(key), "Введите ключ для удаления: ");
                root = delete(root, key);
                break;
            case 3:
                safeInput(key, sizeof(key), "Введите ключ для поиска: ");
                Node *found = search(root, key);
                if (found) 
                    printf("Найдено: ключ=%s, информация=%s\n", found->key, found->info);
                else 
                    printf("Ключ не найден.\n");
                break;
            case 4:
                safeInput(key, sizeof(key), "Введите минимальный ключ (или Enter для вывода всех): ");
                printf("Элементы с ключами больше '%s':\n", key[0] ? key : "все");
                printGreater(root, key[0] ? key : NULL);
                break;
            case 5:
                safeInput(prefix, sizeof(prefix), "Введите префикс: ");
                printf("Введите N: ");
                scanf("%d", &N);
                getchar();
                searchByPrefix(root, prefix, N);
                break;
            case 6:
                printf("Дерево:\n");
                printTree(root, 0);
                break;
            case 7:
                safeInput(filename, sizeof(filename), "Введите имя файла: ");
                root = loadFromFile(root, filename);
                break;
            case 8:
                printf("Таймирование операций:\n");
                printf("1. Вставка\n2. Поиск\n3. Удаление\n");
                printf("Выберите операцию: ");
                int opType;
                scanf("%d", &opType);
                getchar();
                
                safeInput(key, sizeof(key), "Введите ключ: ");
                if (opType == 1) {
                    safeInput(info, sizeof(info), "Введите информацию: ");
                }
                
                timeOperation(root, opType, key, info);
                break;
            case 0:
                printf("Выход из программы.\n");
                break;
            default:
                printf("Неверный выбор! Попробуйте снова.\n");
        }
    } while (choice != 0);
    
    freeTree(root);
    return 0;
}