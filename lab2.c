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

struct Node {
    char ch;
    struct Node *next;
};

// Функция для добавления символа в конец списка
void append_node(struct Node **head, struct Node **tail, char c) {
    struct Node *new_node = (struct Node*)malloc(sizeof(struct Node));
    new_node->ch = c;
    new_node->next = NULL;
    if (*head == NULL) {
        *head = new_node;
        *tail = new_node;
    } else {
        (*tail)->next = new_node;
        *tail = new_node;
    }
}

// Функция для создания списка из строки
struct Node* create_list(const char *str) {
    struct Node *head = NULL;
    struct Node *tail = NULL;
    for (const char *p = str; *p != '\0'; p++) {
        append_node(&head, &tail, *p);
    }
    return head;
}

// Функция для удаления лишних пробелов в списке
void remove_extra_spaces(struct Node **head) {
    while (*head != NULL && (*head)->ch == ' ') {
        struct Node *temp = *head;
        *head = (*head)->next;
        free(temp);
    }
    if (*head == NULL) return;

    struct Node *current = *head;
    // Удаляем повторяющиеся пробелы между словами
    while (current->next != NULL) {
        if (current->ch == ' ' && current->next->ch == ' ') {
            struct Node *temp = current->next;
            current->next = temp->next;
            free(temp);
        } else {
            current = current->next;
        }
    }
    // Удаляем пробел в конце списка, если он есть
    if (current->ch == ' ') {
        if (*head == current) {
            free(current);
            *head = NULL;
        } else {
            struct Node *prev = *head;
            while (prev->next != current) {
                prev = prev->next;
            }
            prev->next = NULL;
            free(current);
        }
    }
}

// Функция для подсчета слов и общего количества символов (без пробелов)
int count_words_and_length(struct Node *head, int *total_chars) {
    int words = 0;
    *total_chars = 0;
    int in_word = 0;
    struct Node *current = head;
    while (current != NULL) {
        if (current->ch != ' ') {
            (*total_chars)++;
            if (!in_word) {
                words++;
                in_word = 1;
            }
        } else {
            in_word = 0;
        }
        current = current->next;
    }
    return words;
}

// Функция для подсчета общей длины списка (включая пробелы)
int list_length(struct Node *head) {
    int len = 0;
    struct Node *current = head;
    while (current != NULL) {
        len++;
        current = current->next;
    }
    return len;
}

// Функция для обрезки списка до указанной длины
void truncate_list(struct Node **head, int max_len) {
    if (*head == NULL) return;
    
    struct Node *current = *head;
    struct Node *prev = NULL;
    int count = 0;
    
    while (current != NULL && count < max_len) {
        prev = current;
        current = current->next;
        count++;
    }
    
    if (prev != NULL) {
        prev->next = NULL;
    }
    
    while (current != NULL) {
        struct Node *temp = current;
        current = current->next;
        free(temp);
    }
}

// Функция для вставки пробелов для выравнивания по ширине
void insert_spaces(struct Node *head, int word_count, int total_chars) {
    if (word_count < 2) {
        if (word_count == 1) {
            struct Node *current = head;
            while (current->next != NULL) {
                current = current->next;
            }
            int spaces_to_add = 80 - total_chars;
            for (int i = 0; i < spaces_to_add; i++) {
                struct Node *new_space = (struct Node*)malloc(sizeof(struct Node));
                new_space->ch = ' ';
                new_space->next = NULL;
                current->next = new_space;
                current = new_space;
            }
        }
        return;
    }

    int total_spaces_needed = 80 - total_chars;
    int gaps = word_count - 1;
    int base_spaces = total_spaces_needed / gaps;
    int extra = total_spaces_needed % gaps;

    // Собираем массив указателей на пробелы между словами
    struct Node **space_nodes = (struct Node**)malloc(gaps * sizeof(struct Node*));
    int space_index = 0;
    struct Node *current = head;
    while (current != NULL) {
        if (current->ch == ' ') {
            space_nodes[space_index++] = current;
        }
        current = current->next;
    }

    for (int i = 0; i < gaps; i++) {
        struct Node *space = space_nodes[i];
        int spaces_to_insert = base_spaces;
        if (i < extra) {
            spaces_to_insert++;
        }
        for (int j = 0; j < spaces_to_insert; j++) {
            struct Node *new_space = (struct Node*)malloc(sizeof(struct Node));
            new_space->ch = ' ';
            new_space->next = space->next;
            space->next = new_space;
            space = new_space;
        }
    }
    free(space_nodes);
}

// Вывод списка на экран
void print_list(struct Node *head) {
    struct Node *current = head;
    while (current != NULL) {
        printf("%c", current->ch);
        current = current->next;
    }
    printf("\n");
}

// Освобождения памяти, занятой списком
void free_list(struct Node *head) {
    struct Node *current = head;
    while (current != NULL) {
        struct Node *temp = current;
        current = current->next;
        free(temp);
    }
}

// Чтение строки произвольной длины из входного потока
struct Node* read_line() {
    struct Node *head = NULL;
    struct Node *tail = NULL;
    char buffer[1024];
    int read_more = 1;
    
    while (read_more) {
        // Читаем часть строки до 1023 символов или до символа новой строки
        if (scanf("%1023[^\n]", buffer) == 1) {
            // Добавляем прочитанные символы в список
            for (char *p = buffer; *p != '\0'; p++) {
                // Заменяем табуляции на пробелы
                char c = (*p == '\t') ? ' ' : *p;
                append_node(&head, &tail, c);
            }
        } else {
            read_more = 0;
        }
        
        // Проверяем следующий символ
        int next_char = getchar();
        if (next_char == '\n' || next_char == EOF) {
            read_more = 0;
        } else {
            // Возвращаем символ обратно в поток, если это не конец строки
            ungetc(next_char, stdin);
        }
    }
    
    return head;
}

int main() {
    while (1) {
        // Чтение строки произвольной длины
        struct Node *list = read_line();
        if (list == NULL) {
            break;
        }
        
        remove_extra_spaces(&list);
        
        int total_length = list_length(list);
        
        if (total_length == 0) {
            // Если слов нет, создаем строку из 80 пробелов
            free_list(list);
            list = create_list("                                                                                ");
        } else if (total_length >= 80) {
            // Если строка уже содержит 80 или более символов, обрезаем её
            truncate_list(&list, 80);
        } else {
            // Если строка короче 80 символов, выравниваем по ширине
            int total_chars;
            int word_count = count_words_and_length(list, &total_chars);
            insert_spaces(list, word_count, total_chars);
        }
        
        print_list(list);
        free_list(list);
    }
    return 0;
}