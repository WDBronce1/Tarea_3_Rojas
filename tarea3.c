#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "tdas/list.h"
#include "tdas/heap.h"
#include "tdas/extra.h"
#include "tdas/stack.h"
#include "tdas/queue.h"
#include <string.h>

// Definición de la estructura para el estado del puzzle
typedef struct {
    int square[3][3]; // Matriz 3x3 que representa el tablero
    int x;    // Posición x del espacio vacío
    int y;    // Posición y del espacio vacío
    List* actions; //Secuencia de movimientos para llegar al estado
    bool visitado; //Se utiliza para verificar si un estado ya ha sido visitado
} State;

int distancia_L1(State* state) 
{
    int ev = 0;
    int k = 1;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++) 
        {
            int val = state->square[i][j];
            if (val == 0) continue;
            int ii = (val) / 3;
            int jj = (val) % 3;
            ev += abs(ii - i) + abs(jj - j);
        }
    return ev;
}

// Función para imprimir el estado del puzzle
void imprimirEstado(const State *estado) 
{
    for (int i = 0; i < 3; i++) 
    {
        for (int j = 0; j < 3; j++) 
        {
            if (estado->square[i][j] == 0)
                printf("x "); // Imprime un espacio en blanco para el espacio vacío
            else
                printf("%d ", estado->square[i][j]);
        }
        printf("\n");
    }
}

State* copy(State* n) 
{
    State* new = (State*) malloc(sizeof(State));
    *new = *n;
    new->actions = list_create(); // Inicializa la lista de acciones en la copia
    return new;
}

//      FUNCIONES //////////////////////////////////////////////////////////////////
int is_final_state(const State *actual) 
{ //Regresa 1 si el estado es el estado final
    int k = 0;
    for (int i = 0; i < 3; i++) 
    {
        for (int j = 0; j < 3; j++) 
        {
            if (actual->square[i][j] != k) 
            {
                return 0;
            }
            k++;
        }
    }
    return 1;
}

//****************************************************************************
// arriba=1, abajo=2, izquierda=3, derecha=4
int transition(State *actual, int move) 
{ //Regresa 1 si el movimiento es valido
    int x = actual->x, y = actual->y;
    int new_x, new_y, aux;
    switch (move) 
    {
        case 1: // Mover arriba
            if (x == 0) 
            {
                return 0;
            } 
            else 
            {
                new_x = x - 1;
                new_y = y;
            }
            break;
        case 2: // Mover abajo
            if (x == 2) 
            {
                return 0;
            } 
            else 
            {
                new_x = x + 1;
                new_y = y;
            }
            break;
        case 3: // Mover izquierda
            if (y == 0) 
            {
                return 0;
            } 
            else 
            {
                new_x = x;
                new_y = y - 1;
            }
            break;
        case 4: // Mover derecha
            if (y == 2) 
            {
                return 0;
            } 
            else 
            {
                new_x = x;
                new_y = y + 1;
            }
            break;
        default:
            return 0;
    }
    aux = actual->square[new_x][new_y];
    actual->square[new_x][new_y] = actual->square[x][y];
    actual->square[x][y] = aux;
    actual->x = new_x;
    actual->y = new_y;
    actual->actions = list_create();
    list_pushFront(actual->actions, &move);
    actual->visitado = true;
    return 1;
}

//****************************************************************************

List *get_adj_nodes(State *n) 
{ //Regresa una lista con los nodos adyacentes del nodo actual
    List *list = list_create();
    int moves[4] = {1, 2, 3, 4}; // Movimientos posibles: arriba, abajo, izquierda, derecha

    for (int i = 0; i < 4; i++) 
    {
        State *new = copy(n);
        if (transition(new, moves[i])) 
        {
            list_pushBack(list, new);
        } 
        else 
        {
            free(new);
        }
    }
    return list;
}

//*******************************************************************
void dfs(State initial) 
{
    Stack *stack = stack_create(stack);
    stack_push(stack, &initial);
    State *current = (State *) list_first(stack);

    while (list_first(stack) != NULL) 
    {
        current = (State *) list_first(stack);
        List* adj_nodes = get_adj_nodes(current);
        State* adj_node = list_first(adj_nodes);
        while (adj_node != NULL) 
        {
            if (is_final_state(adj_node) == 1) 
                {
                    stack_clean(stack);
                    imprimirEstado(adj_node);
                    return;
                }
            if(adj_node->visitado == false)
            {
                stack_push(stack, adj_node);
            }
            adj_node = list_next(adj_nodes);
        }
        list_clean(adj_nodes);
        printf("-------------\n");
        printf("Depth: %d\n", list_size(stack));
        imprimirEstado(current);
        
    }
    stack_clean(stack);
}



void bfs(State I) 
{
    Queue* Q = queue_create(Q);
    queue_insert(Q, &I);
    while (list_size(Q) != 0) 
    {
        State* n = queue_remove(Q);
        if (n->visitado == true) 
        {
            continue;
        }
        if (is_final_state(n)) 
        {
            queue_clean(Q);
            imprimirEstado(n);
            return;
        }

        n->visitado = true;
        List* adj = get_adj_nodes(n);
        State* aux = list_first(adj);
        while (aux) 
        {
            queue_insert(Q, aux);
            printf("---------\n");
            imprimirEstado(aux);
            aux = list_next(adj);
        }
        
    }
    
}

////////////////////////////////////////////////////////////////////////////////////

int main() 
{
    // Estado inicial del puzzle
    State estado_inicial = 
    {
        {
            {0, 2, 8}, // Primera fila (0 representa espacio vacío)
            {1, 3, 4}, // Segunda fila
            {6, 5, 7}, // Tercera fila
        },
        0, 0   // Posición del espacio vacío (fila 0, columna 0)
    };
    estado_inicial.actions = list_create();

    // Imprime el estado inicial
    printf("Estado inicial del puzzle:\n");
    imprimirEstado(&estado_inicial);

    printf("Distancia L1:%d\n", distancia_L1(&estado_inicial));

    //Ejemplo de heap (cola con prioridad)
    printf("\n***** EJEMPLO USO DE HEAP ******\nCreamos un Heap e insertamos 3 elementos con distinta prioridad\n");
    Heap* heap = heap_create();
    char* data = strdup("Cinco");
    printf("Insertamos el elemento %s con prioridad -5\n", data);
    heap_push(heap, data, -5);
    data = strdup("Seis");
    printf("Insertamos el elemento %s con prioridad -6\n", data);
    heap_push(heap, data, -6);
    data = strdup("Siete");
    printf("Insertamos el elemento %s con prioridad -7\n", data);
    heap_push(heap, data, -7);

    printf("\nLos elementos salen del Heap ordenados de mayor a menor prioridad\n");
    while (heap_top(heap) != NULL) {
        printf("Top: %s\n", (char*) heap_top(heap));
        heap_pop(heap);
    }
    printf("No hay más elementos en el Heap\n");

    char opcion;
    do {
        puts("========================================");
        puts("     Escoge método de búsqueda");
        puts("========================================");

        puts("1) Búsqueda en Profundidad");
        puts("2) Buscar en Anchura");
        puts("3) Buscar Mejor Primero");
        puts("4) Salir");

        printf("Ingrese su opción: ");
        scanf(" %c", &opcion);

        switch (opcion) {
        case '1':
            dfs(estado_inicial);
            break;
        case '2':
            bfs(estado_inicial);
            break;
        case '3':
            // best_first(estado_inicial);
            break;
        }
        printf("Presione Enter para continuar...");
        getchar(); // Para pausar la ejecución
        getchar(); // Para limpiar el buffer
    } while (opcion != '4');

    return 0;
}
