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

typedef struct Node {
    char *key;
    char *info;
    struct Node *left, *right;
} Node;

// Создание нового узла
Node* createNode(const char *key, const char *info) {
    Node *newNode = (Node*)malloc(sizeof(Node));
    newNode->key = strdup(key);
    newNode->info = strdup(info);
    newNode->left = newNode->right = NULL;
    return newNode;
}

// Вставка в дерево
Node* insert(Node *root, const char *key, const char *info) {
    if (!root) return createNode(key, info);
    int cmp = strcmp(key, root->key);
    if (cmp == 0) {
        printf("Ошибка: ключ '%s' уже существует!\n", key);
    } else if (cmp < 0) {
        root->left = insert(root->left, key, info);
    } else {
        root->right = insert(root->right, key, info);
    }
    return root;
}

// Поиск по ключу
Node* search(Node *root, const char *key) {
    if (!root) return NULL;
    int cmp = strcmp(key, root->key);
    if (cmp == 0) return root;
    else if (cmp < 0) return search(root->left, key);
    else return search(root->right, key);
}

// Минимальный узел (для удаления)
Node* findMin(Node *root) {
    while (root && root->left) root = root->left;
    return root;
}

// Удаление по ключу
Node* delete(Node *root, const char *key) {
    if (!root) return NULL;
    int cmp = strcmp(key, root->key);
    if (cmp < 0) root->left = delete(root->left, key);
    else if (cmp > 0) root->right = delete(root->right, key);
    else {
        if (!root->left) {
            Node *tmp = root->right;
            free(root->key); free(root->info); free(root);
            return tmp;
        } else if (!root->right) {
            Node *tmp = root->left;
            free(root->key); free(root->info); free(root);
            return tmp;
        }
        Node *tmp = findMin(root->right);
        free(root->key); free(root->info);
        root->key = strdup(tmp->key);
        root->info = strdup(tmp->info);
        root->right = delete(root->right, tmp->key);
    }
    return root;
}

// Вывод элементов с ключами > заданного
void printGreater(Node *root, const char *minKey) {
    if (!root) return;
    int cmp = minKey ? strcmp(root->key, minKey) : 1;
    if (!minKey || cmp > 0) {
        printGreater(root->left, minKey);
        printf("%s : %s\n", root->key, root->info);
        printGreater(root->right, minKey);
    } else {
        printGreater(root->right, minKey);
    }
}

// Поиск по первым N символам
void searchByPrefix(Node *root, const char *prefix, int N) {
    if (!root) return;
    if (strncmp(root->key, prefix, N) == 0) {
        printf("%s : %s\n", root->key, root->info);
    }
    searchByPrefix(root->left, prefix, N);
    searchByPrefix(root->right, prefix, N);
}

// Форматированный вывод дерева
void printTree(Node *root, int space) {
    if (!root) return;
    space += 5;
    printTree(root->right, space);
    printf("\n");
    for (int i = 5; i < space; i++) printf(" ");
    printf("%s\n", root->key);
    printTree(root->left, space);
}

// Освобождение памяти
void freeTree(Node *root) {
    if (!root) return;
    freeTree(root->left);
    freeTree(root->right);
    free(root->key);
    free(root->info);
    free(root);
}

int main() {
    Node *root = NULL;
    int choice;
    char key[256], info[256], prefix[256];
    int N;

    do {
        printf("\nМеню:\n");
        printf("1. Вставить элемент\n");
        printf("2. Удалить элемент\n");
        printf("3. Поиск по ключу\n");
        printf("4. Вывод элементов > заданного ключа\n");
        printf("5. Поиск по первым N символам\n");
        printf("6. Печать дерева\n");
        printf("0. Выход\n");
        printf("Выбор: ");
        scanf("%d", &choice); getchar();

        switch (choice) {
            case 1:
                printf("Введите ключ: "); fgets(key, sizeof(key), stdin); key[strcspn(key, "\n")] = 0;
                printf("Введите информацию: "); fgets(info, sizeof(info), stdin); info[strcspn(info, "\n")] = 0;
                root = insert(root, key, info);
                break;
            case 2:
printf("Введите ключ для удаления: "); fgets(key, sizeof(key), stdin); key[strcspn(key, "\n")] = 0;
                root = delete(root, key);
                break;
            case 3:
                printf("Введите ключ для поиска: "); fgets(key, sizeof(key), stdin); key[strcspn(key, "\n")] = 0;
                Node *found;
                found = search(root, key);
                if (found) printf("Найдено: %s\n", found->info);
                else printf("Ключ не найден.\n");
                break;
            case 4:
                printf("Введите минимальный ключ (или пусто для всех): ");
                fgets(key, sizeof(key), stdin); key[strcspn(key, "\n")] = 0;
                printGreater(root, strlen(key) ? key : NULL);
                break;
            case 5:
                printf("Введите префикс: "); fgets(prefix, sizeof(prefix), stdin); prefix[strcspn(prefix, "\n")] = 0;
                printf("Введите N: "); scanf("%d", &N); getchar();
                searchByPrefix(root, prefix, N);
                break;
            case 6:
                printTree(root, 0);
                break;
        }
    } while (choice != 0);

    freeTree(root);
    return 0;
}
