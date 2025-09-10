//Предметная область:
//Лабиринт.
//Основные свойства графа:
//Орентированный граф. Вершины графа содержат следующую информацию:
﻿﻿//-координаты клетки на плоскости (пара целых беззнаковых чисел).
//﻿﻿-тип клетки (перечисляемый: обычная, вход в лабиринт, выход из лабиринта).
//Ребра графа не хранят информацию.
//Ребром могут быть соединены лишь те вершины, которые хранят информацию о соседних клетках (по вертикали или горизонтали).
//Операции, поддерживаемые графом:
//Обход
//Проверка достижимости хотя бы одного из выходов из указанной точки входа (поиск в ширину).
//Поиск кратчайшего пути между двумя заданными вершинами.
//Поиск кратчайшего пути между указанным входом и указанным выходом (алгоритм Беллмана-Форда).
//Особая операция
//Модификация лабиринта, обеспечивающая наличие лишь одного пути от любого из входов до
//любого из выходов (построение минимального основного дерева).

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

typedef enum { CELL_NORMAL=0, CELL_ENTRANCE=1, CELL_EXIT=2 } CellType;

typedef struct {
    unsigned int x, y;
    CellType type;
} Vertex;

typedef struct {
    int V;              // число вершин
    int E;              // число рёбер
    Vertex *verts;      // массив вершин
    int **adj;          // списки смежности (массив массивов)
    int *deg;           // текущая степень
    int *cap;           // ёмкость списка
} Graph;

// Внутреннее представление сетки
typedef struct {
    int width, height;
    int **id;           // id[y][x] -> индекс вершины или -1
} GridIndex;

// Утилиты
static void* xmalloc(size_t n) {
    void *p = malloc(n);
    if (!p) { fprintf(stderr, "Out of memory\n"); exit(1); }
    return p;
}

static void push_edge(Graph *g, int u, int v) {
    if (u < 0 || v < 0) return;
    if (g->deg[u] == g->cap[u]) {
        g->cap[u] = g->cap[u] ? g->cap[u]*2 : 4;
        g->adj[u] = (int*)realloc(g->adj[u], g->cap[u]*sizeof(int));
        if (!g->adj[u]) { fprintf(stderr, "OOM\n"); exit(1); }
    }
    g->adj[u][g->deg[u]++] = v;
    g->E++;
}

static Graph make_graph(int V) {
    Graph g;
    g.V = V; g.E = 0;
    g.verts = (Vertex*)xmalloc(sizeof(Vertex)*V);
    g.adj = (int**)xmalloc(sizeof(int*)*V);
    g.deg = (int*)calloc(V, sizeof(int));
    g.cap = (int*)calloc(V, sizeof(int));
    for (int i=0;i<V;i++) g.adj[i] = NULL;
    return g;
}

static void free_graph(Graph *g) {
    if (!g) return;
    for (int i=0;i<g->V;i++) free(g->adj[i]);
    free(g->adj); free(g->deg); free(g->cap); free(g->verts);
    g->adj = NULL; g->deg = g->cap = NULL; g->verts = NULL;
    g->V = g->E = 0;
}

static GridIndex make_grid_index(int w, int h) {
    GridIndex gi;
    gi.width = w; gi.height = h;
    gi.id = (int**)xmalloc(h*sizeof(int*));
    for (int y=0;y<h;y++) {
        gi.id[y] = (int*)xmalloc(w*sizeof(int));
        for (int x=0;x<w;x++) gi.id[y][x] = -1;
    }
    return gi;
}

static void free_grid_index(GridIndex *gi) {
    for (int y=0;y<gi->height;y++) free(gi->id[y]);
    free(gi->id);
    gi->id = NULL;
}

// Построение графа из сетки символов
// '.' обычная, 'S' вход, 'E' выход, '#' стена
static void build_from_grid(Graph *g, GridIndex *gi, char **grid) {
    // 1) присвоение id вершинам
    int id = 0;
    for (int y=0;y<gi->height;y++) {
        for (int x=0;x<gi->width;x++) {
            char c = grid[y][x];
            if (c!='#') {
                gi->id[y][x] = id;
                id++;
            }
        }
    }
    *g = make_graph(id);
    // 2) заполнение вершин
    for (int y=0;y<gi->height;y++) {
        for (int x=0;x<gi->width;x++) {
            int vid = gi->id[y][x];
            if (vid<0) continue;
            g->verts[vid].x = (unsigned int)x;
            g->verts[vid].y = (unsigned int)y;
            char c = grid[y][x];
            if (c=='S') g->verts[vid].type = CELL_ENTRANCE;
            else if (c=='E') g->verts[vid].type = CELL_EXIT;
            else g->verts[vid].type = CELL_NORMAL;
        }
    }
    // 3) ориентированные рёбра между ортогональными соседями (в обе стороны)
    int dx[4] = {1,-1,0,0};
    int dy[4] = {0,0,1,-1};
    for (int y=0;y<gi->height;y++) {
        for (int x=0;x<gi->width;x++) {
            int u = gi->id[y][x];
            if (u<0) continue;
            for (int k=0;k<4;k++) {
                int nx = x+dx[k], ny = y+dy[k];
                if (nx<0 || ny<0 || nx>=gi->width || ny>=gi->height) continue;
                int v = gi->id[ny][nx];
                if (v<0) continue;
                // добавляем ориентированное ребро u->v
                push_edge(g, u, v);
            }
        }
    }
}

// Поиск вершины по координатам
static int find_vertex_by_xy(const Graph *g, unsigned int x, unsigned int y) {
    for (int i=0;i<g->V;i++) {
        if (g->verts[i].x==x && g->verts[i].y==y) return i;
    }
    return -1;
}

// BFS: посетить все достижимые из s, опционально восстанавливая путь до t
static int bfs(const Graph *g, int s, int t, int *parent) {
int *q = (int*)xmalloc(sizeof(int)*g->V);
    int *vis = (int*)calloc(g->V, sizeof(int));
    int head=0, tail=0;
    for (int i=0;i<g->V;i++) parent[i] = -1;

    vis[s]=1; parent[s]=-2; q[tail++]=s;

    while (head<tail) {
        int u = q[head++];
        if (u==t) break;
        for (int i=0;i<g->deg[u];i++) {
            int v = g->adj[u][i];
            if (!vis[v]) {
                vis[v]=1; parent[v]=u; q[tail++]=v;
            }
        }
    }
    int found = (t<0) ? 1 : (parent[t]!=-1);
    free(q); free(vis);
    return found;
}

static void print_path(const Graph *g, int s, int t, int *parent) {
    // восстановление пути t->s
    int *stack = (int*)xmalloc(sizeof(int)*g->V);
    int sz=0;
    for (int v=t; v!=-2 && v!=-1; v=parent[v]) stack[sz++]=v;
    if (sz==0 || stack[sz-1]!=s) {
        printf("Путь не найден.\n");
        free(stack);
        return;
    }
    printf("Длина (в рёбрах): %d\n", sz-1);
    printf("Путь:\n");
    for (int i=sz-1;i>=0;i--) {
        unsigned int x = g->verts[stack[i]].x;
        unsigned int y = g->verts[stack[i]].y;
        CellType ttype = g->verts[stack[i]].type;
        const char *label = (ttype==CELL_ENTRANCE?"S":(ttype==CELL_EXIT?"E":"."));
        printf("(%u,%u) %s\n", x, y, label);
    }
    free(stack);
}

// Беллман — Форд: веса рёбер = 1, возвращает 1 если найден путь, parent[] для восстановления
static int bellman_ford(const Graph *g, int s, int t, int *parent) {
    int V = g->V;
    int *dist = (int*)xmalloc(sizeof(int)*V);
    for (int i=0;i<V;i++) { dist[i]=INT_MAX/4; parent[i]=-1; }
    dist[s]=0; parent[s]=-2;

    // Сформировать список рёбер
    int m = 0;
    for (int u=0;u<V;u++) m += g->deg[u];
    int *eu = (int*)xmalloc(sizeof(int)*m);
    int *ev = (int*)xmalloc(sizeof(int)*m);
    int pos=0;
    for (int u=0;u<V;u++) {
        for (int i=0;i<g->deg[u];i++) { eu[pos]=u; ev[pos]=g->adj[u][i]; pos++; }
    }

    // Основной цикл
    for (int it=0; it<V-1; it++) {
        int changed = 0;
        for (int i=0;i<m;i++) {
            int u = eu[i], v = ev[i];
            if (dist[u] + 1 < dist[v]) {
                dist[v] = dist[u] + 1;
                parent[v] = u;
                changed = 1;
            }
        }
        if (!changed) break;
    }

    int ok = (dist[t] < INT_MAX/4);
    free(dist); free(eu); free(ev);
    return ok;
}

// Модификация графа: оставить только рёбра остовного леса (BFS-остов, в обе стороны)
// После операции между любыми двумя вершинами компоненты ровно один простой путь.
static void make_tree(Graph *g) {
    int V = g->V;
    int *vis = (int*)calloc(V, sizeof(int));
    int *parent = (int*)malloc(sizeof(int)*V);
    for (int i=0;i<V;i++) parent[i]=-1;

    // Подготовить новые списки (пустые)
    int **nadj = (int**)xmalloc(sizeof(int*)*V);
    int *ndeg = (int*)calloc(V, sizeof(int));
    int *ncap = (int*)calloc(V, sizeof(int));
    for (int i=0;i<V;i++) nadj[i]=NULL;

    // Для каждой компоненты — BFS остов, добавить двунаправленные рёбра parent<->child
    int *q = (int*)xmalloc(sizeof(int)*V);
    for (int s=0;s<V;s++) if (!vis[s]) {
        int head=0, tail=0;
        vis[s]=1; parent[s]=-1; q[tail++]=s;
        while (head<tail) {
            int u=q[head++];
            for (int i=0;i<g->deg[u];i++) {
                int v = g->adj[u][i];
                if (!vis[v]) {
                    vis[v]=1; parent[v]=u; q[tail++]=v;
                    // добавить ребро u->v и v->u в новые списки
                    // локальный push в nadj
                    // u->v
                    if (ndeg[u]==ncap[u]) {
                        ncap[u] = ncap[u]? ncap[u]*2 : 4;
                        nadj[u] = (int*)realloc(nadj[u], ncap[u]*sizeof(int));
                    }
                    nadj[u][ndeg[u]++] = v;
                    // v->u
                    if (ndeg[v]==ncap[v]) {
                        ncap[v] = ncap[v]? ncap[v]*2 : 4;
                        nadj[v] = (int*)realloc(nadj[v], ncap[v]*sizeof(int));
                    }
                    nadj[v][ndeg[v]++] = u;
                }
}
        }
    }
    free(q); free(vis); free(parent);

    // Заменить старые списки на новые
    for (int i=0;i<V;i++) free(g->adj[i]);
    free(g->adj); free(g->deg); free(g->cap);
    g->adj = nadj; g->deg = ndeg; g->cap = ncap;

    // Пересчитать число рёбер
    int E=0; for (int i=0;i<V;i++) E += g->deg[i];
    g->E = E;
    printf("Построен остовный лес. Рёбер осталось: %d (учитывая направление).\n", g->E);
}

// Вывод информации о вершине
static void print_vertex(const Vertex *v) {
    const char *t = (v->type==CELL_ENTRANCE?"ENTRANCE":(v->type==CELL_EXIT?"EXIT":"NORMAL"));
    printf("(%u,%u) [%s]\n", v->x, v->y, t);
}

// Обход (BFS) от координаты
static void traverse_bfs(const Graph *g, int s) {
    if (s<0) { printf("Стартовая вершина не найдена.\n"); return; }
    int *parent = (int*)malloc(sizeof(int)*g->V);
    // используем bfs с t=-1 (просто обойдём всё достижимое)
    int *q = (int*)xmalloc(sizeof(int)*g->V);
    int *vis = (int*)calloc(g->V, sizeof(int));
    int head=0, tail=0;
    for (int i=0;i<g->V;i++) parent[i]=-1;

    vis[s]=1; parent[s]=-2; q[tail++]=s;
    printf("Обход BFS от (%u,%u):\n", g->verts[s].x, g->verts[s].y);
    while (head<tail) {
        int u = q[head++];
        print_vertex(&g->verts[u]);
        for (int i=0;i<g->deg[u];i++) {
            int v = g->adj[u][i];
            if (!vis[v]) { vis[v]=1; parent[v]=u; q[tail++]=v; }
        }
    }
    free(parent); free(q); free(vis);
}

// Ввод целого с проверкой
static int read_int(const char *prompt, int *out) {
    printf("%s", prompt);
    if (scanf("%d", out)!=1) return 0;
    return 1;
}

int main(void) {
    Graph g = {0};
    GridIndex gi = {0};
    char **grid = NULL;
    int built = 0;

    while (1) {
        printf("\nМеню:\n");
        printf("1. Загрузить лабиринт (сетку)\n");
        printf("2. Обход графа (BFS от координаты)\n");
        printf("3. Достижимость выхода из входа (BFS)\n");
        printf("4. Кратчайший путь между двумя координатами (BFS)\n");
        printf("5. Кратчайший путь вход -> выход (Беллман — Форд)\n");
        printf("6. Преобразовать лабиринт к единственному пути (остов)\n");
        printf("7. Информация о графе\n");
        printf("0. Выход\n");
        printf("Выбор: ");
        int choice;
        if (scanf("%d", &choice)!=1) { printf("Некорректный ввод.\n"); return 0; }

        if (choice==0) break;

        if (choice==1) {
            // Очистка прошлого
            if (built) {
                for (int y=0;y<gi.height;y++) free(grid[y]);
                free(grid);
                free_grid_index(&gi);
                free_graph(&g);
                built = 0;
            }
            int w,h;
            if (!read_int("Ширина: ", &w) || !read_int("Высота: ", &h) || w<=0 || h<=0 || w>2000 || h>2000) {
                printf("Некорректные размеры.\n");
                // очистить остатки ввода
                return 0;
            }
            gi = make_grid_index(w,h);
            grid = (char**)xmalloc(h*sizeof(char*));
            printf("Введите %d строк(и) карты (# . S E):\n", h);
            // съесть перевод строки
            int ch; do { ch=getchar(); } while (ch!='\n' && ch!=EOF);
            for (int y=0;y<h;y++) {
                grid[y] = (char*)xmalloc(w+5);
                if (!fgets(grid[y], w+5, stdin)) { printf("Ошибка ввода.\n"); return 0; }
                // обрезать перевод строки
                if ((int)strlen(grid[y])>0 && (grid[y][strlen(grid[y])-1]=='\n' || grid[y][strlen(grid[y])-1]=='\r'))
                    grid[y][strlen(grid[y])-1] = '\0';
                if ((int)strlen(grid[y])<w) { printf("Слишком короткая строка.\n"); return 0; }
                // гарантировать длину w
                grid[y][w] = '\0';
                for (int x=0;x<w;x++) {
                    char c = grid[y][x];
                    if (c!='#' && c!='.' && c!='S' && c!='E') {
                        printf("Недопустимый символ '%c' в (%d,%d)\n", c, x, y);
                        return 0;
                    }
                }
            }
build_from_grid(&g, &gi, grid);
            built = 1;
            printf("Граф загружен: V=%d, E=%d\n", g.V, g.E);
        }
        else {
            if (!built) { printf("Сначала загрузите лабиринт (п.1).\n"); continue; }
            if (choice==2) {
                int x,y;
                if (!read_int("X: ", &x) || !read_int("Y: ", &y)) { printf("Некорректный ввод.\n"); continue; }
                int s = find_vertex_by_xy(&g, (unsigned)x, (unsigned)y);
                traverse_bfs(&g, s);
            } else if (choice==3) {
                int x,y;
                if (!read_int("Координаты входа X: ", &x) || !read_int("Y: ", &y)) { printf("Некорректный ввод.\n"); continue; }
                int s = find_vertex_by_xy(&g, (unsigned)x, (unsigned)y);
                if (s<0 || g.verts[s].type!=CELL_ENTRANCE) {
                    printf("Заданная клетка не является входом.\n"); continue;
                }
                // BFS до любого выхода
                int *parent = (int*)malloc(sizeof(int)*g.V);
                int *q = (int*)xmalloc(sizeof(int)*g.V);
                int *vis = (int*)calloc(g.V, sizeof(int));
                int head=0, tail=0;
                for (int i=0;i<g->V;i++) parent[i]=-1;
                vis[s]=1; parent[s]=-2; q[tail++]=s;
                int exit_found = -1;
                while (head<tail) {
                    int u=q[head++];
                    if (g.verts[u].type==CELL_EXIT) { exit_found = u; break; }
                    for (int i=0;i<g.deg[u];i++) {
                        int v = g.adj[u][i];
                        if (!vis[v]) { vis[v]=1; parent[v]=u; q[tail++]=v; }
                    }
                }
                if (exit_found>=0) {
                    printf("Выход достижим. Пример пути:\n");
                    print_path(&g, s, exit_found, parent);
                } else {
                    printf("Ни один выход не достижим из указанного входа.\n");
                }
                free(parent); free(q); free(vis);
            } else if (choice==4) {
                int x1,y1,x2,y2;
                if (!read_int("X1: ", &x1) || !read_int("Y1: ", &y1) ||
                    !read_int("X2: ", &x2) || !read_int("Y2: ", &y2)) { printf("Некорректный ввод.\n"); continue; }
                int s = find_vertex_by_xy(&g, (unsigned)x1, (unsigned)y1);
                int t = find_vertex_by_xy(&g, (unsigned)x2, (unsigned)y2);
                if (s<0 || t<0) { printf("Одна из координат отсутствует (возможно, стена '#').\n"); continue; }
                int *parent = (int*)malloc(sizeof(int)*g.V);
                if (bfs(&g, s, t, parent)) print_path(&g, s, t, parent);
                else printf("Путь не найден.\n");
                free(parent);
            } else if (choice==5) {
                int xs,ys, xe,ye;
                if (!read_int("Вход X: ", &xs) || !read_int("Вход Y: ", &ys) ||
                    !read_int("Выход X: ", &xe) || !read_int("Выход Y: ", &ye)) { printf("Некорректный ввод.\n"); continue; }
                int s = find_vertex_by_xy(&g, (unsigned)xs, (unsigned)ys);
                int t = find_vertex_by_xy(&g, (unsigned)xe, (unsigned)ye);
                if (s<0 || t<0) { printf("Координаты не найдены.\n"); continue; }
                if (g.verts[s].type!=CELL_ENTRANCE) { printf("Начальная точка не является входом.\n"); continue; }
                if (g.verts[t].type!=CELL_EXIT) { printf("Конечная точка не является выходом.\n"); continue; }
                int *parent = (int*)malloc(sizeof(int)*g.V);
                if (bellman_ford(&g, s, t, parent)) print_path(&g, s, t, parent);
                else printf("Путь не найден (Беллман — Форд).\n");
                free(parent);
            } else if (choice==6) {
                make_tree(&g);
                printf("Граф преобразован к остову (уникальные пути).\n");
            } else if (choice==7) {
                int entrances=0, exits=0;
                for (int i=0;i<g.V;i++) {
                    if (g.verts[i].type==CELL_ENTRANCE) entrances++;
else if (g.verts[i].type==CELL_EXIT) exits++;
                }
                printf("Вершин: %d, рёбер: %d\n", g.V, g.E);
                printf("Входов: %d, выходов: %d\n", entrances, exits);
            } else {
                printf("Неизвестный пункт.\n");
            }
        }
    }

    // Очистка
    if (grid) {
        for (int y=0;y<gi.height;y++) free(grid[y]);
        free(grid);
    }
    if (gi.id) free_grid_index(&gi);
    free_graph(&g);
    return 0;
}
