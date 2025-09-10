//Из входного потока вводится произвольное число строк, состоящих из последователь-ности слов, которые могут быть разделены одним или несколькими пробелами и/или знаками табуляции. Длина каждой строки произвольна. Конец ввода определяется концом файла.
//При обработке каждой строки сформировать новую строку, поместив слова исходной строки «по ширине» строки длиной 80 символов. Слова в образованной строке для выравни-вания «по ширине» следует разделить равномерно группами (одним или несколькими) про-белов. Полученную строку вывести на экран.
//Примечания:
//﻿﻿﻿Каждая строка представлена списком; первое поле элемента списка - символ строки; второе - указатель на следующий элемент списка или NULL в конце списка; список результата формировать, модифицируя исходный список.
//﻿﻿﻿Ввод строк должен быть организован с помощью функции scanf () со спецификациями для ввода строк. Функцию getchar () или спецификацию вс в scanf () не использо-вать!
//﻿﻿﻿Логически законченные части алгоритма решения задачи должны быть оформлены отдельными функциями с параметрами (например, пропуск разделителей, создание списка, вывод списка и др.).
//﻿﻿﻿Целесообразно обработку строк оформить соответствующей функцией, тогда в функции ma in должны быть ввод строки, вызов функции обработки, вывод результата - в цикле, пока не закончится ввод; конец цикла - по достижении конца файла.
//﻿﻿﻿Все функции должны работать со списком; при вводе строки следует сразу создавать список.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LEN 1024
#define WIDTH 80

typedef struct Node {
    char ch;
    struct Node *next;
} Node;

// Создание нового узла
Node* create_node(char ch) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->ch = ch;
    node->next = NULL;
    return node;
}

// Создание списка из строки
Node* build_list_from_string(const char* str) {
    Node* head = NULL;
    Node* tail = NULL;
    while (*str) {
        Node* node = create_node(*str++);
        if (!head) head = node;
        else tail->next = node;
        tail = node;
    }
    return head;
}

// Вывод 
void print_list(Node* head) {
    while (head) {
        putchar(head->ch);
        head = head->next;
    }
    putchar('\n');
}

// Освобождение 
void free_list(Node* head) {
    while (head) {
        Node* temp = head;
        head = head->next;
        free(temp);
    }
}

// Пропуск пробелов и табуляции
Node* skip_delimiters(Node* node) {
    while (node && (node->ch == ' ' || node->ch == '\t')) {
        node = node->next;
    }
    return node;
}

// Извлечение слова из списка
char* extract_word(Node** node) {
    char buffer[MAX_LINE_LEN];
    int i = 0;
    *node = skip_delimiters(*node);
    while (*node && (*node)->ch != ' ' && (*node)->ch != '\t') {
        buffer[i++] = (*node)->ch;
        *node = (*node)->next;
    }
    buffer[i] = '\0';
    return strdup(buffer);
}

// Подсчет слов
int count_words(Node* head) {
    int count = 0;
    while ((head = skip_delimiters(head))) {
        extract_word(&head);
        count++;
    }
    return count;
}

// Формирование строки по ширине
Node* justify_line(Node* head) {
    char* words[MAX_LINE_LEN];
    int word_count = 0;
    Node* temp = head;

    while ((temp = skip_delimiters(temp))) {
        words[word_count++] = extract_word(&temp);
    }

    int total_len = 0;
    for (int i = 0; i < word_count; i++) {
        total_len += strlen(words[i]);
    }

    int spaces = WIDTH - total_len;
    int gaps = word_count > 1 ? word_count - 1 : 1;
    int space_per_gap = spaces / gaps;
    int extra_spaces = spaces % gaps;

    Node* result = NULL;
    Node* tail = NULL;

    for (int i = 0; i < word_count; i++) {
        for (int j = 0; j < strlen(words[i]); j++) {
            Node* node = create_node(words[i][j]);
            if (!result) result = node;
            else tail->next = node;
            tail = node;
        }

        if (i < word_count - 1) {
            int space_count = space_per_gap + (extra_spaces-- > 0 ? 1 : 0);
            for (int s = 0; s < space_count; s++) {
                Node* space_node = create_node(' ');
                tail->next = space_node;
                tail = space_node;
            }
        }
        free(words[i]);
    }

    return result;
}

// Основная функция
int main() {
    char buffer[MAX_LINE_LEN];
    while (scanf("%1023[^\n]%*c", buffer) != EOF) {
        Node* line = build_list_from_string(buffer);
        Node* justified = justify_line(line);
        print_list(justified);
        free_list(line);
        free_list(justified);
    }
    return 0;
}
