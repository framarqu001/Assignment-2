#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>


typedef struct node {
    char* vertex;
    int distance;
    struct node* next;
} Node;

// Linked list will keep track of a vertex's edges
typedef struct linkedList {
    char* identifier;
    struct node* head;
    int inDegree;
    int outDegree;
    int edgeCount;
} LinkedList;

// A graph will be made up of an array of linked list. 
typedef struct graph {
    int vertices;
    struct linkedList** pArrLinkedList;
} Graph;

typedef struct stack {
    char** stackArr;
    int index;
    int size;
} Stack;

typedef struct pair {
    int distance;
    char* vertex;
} Pair;

typedef struct prioqueue {
    Pair** pairArr;
    int index;
    int size;
} PrioQueue;


Pair* allocate_pair(int distance, char* vertex) {
    Pair* pair = malloc(sizeof(Pair));
    pair->distance = distance;
    pair->vertex = vertex; 
    return pair;
}

PrioQueue* allocate_PriorityQueue() {
    int INITIAL_CAPAICTY = 10;
    PrioQueue* temp = malloc(sizeof(PrioQueue));
    temp->pairArr = malloc(INITIAL_CAPAICTY * sizeof(Pair*));
    temp->index = 0;
    temp->size = INITIAL_CAPAICTY;
    return temp;
}   

Stack* allocate_stack(){
    int INITIAL_CAPAICTY = 10;
    Stack* temp = malloc(sizeof(struct stack));
    char** stack = malloc(INITIAL_CAPAICTY * sizeof(char*));
    temp->stackArr = stack;
    temp->index = 0;
    temp->size = INITIAL_CAPAICTY;
    return temp;  
}

Node* allocate_node(char* vertexID, int distance) {
    Node* temp = malloc(sizeof(struct node));
    temp->vertex = strdup(vertexID); // Was copying/allocating manually until I saw this func existd
    temp->distance = distance;
    temp->next = NULL;
    return temp;
};

LinkedList* allocate_linkedlist(char* vertexID) {
    LinkedList* temp = malloc(sizeof(LinkedList));
    temp->identifier = strdup(vertexID); //String is dynamically allocated
    temp->head = NULL;
    temp->edgeCount = 0;
    temp->inDegree = 0;
    temp->outDegree = 0;
    return temp;
}

// Creates a graph with a count of the Vertices.
// Creates a an array of linked list pointers 
// does not initialize the linked list
Graph* allocate_graph(int vertices){
    Graph* temp = malloc(sizeof(Graph));
    temp->vertices = vertices;
    temp->pArrLinkedList = malloc(vertices * sizeof(LinkedList*));
    for (int i = 0; i < vertices; i ++) {
        temp->pArrLinkedList[i] = NULL;  //setting points to null
    }
    return temp;
}



void enqueue(PrioQueue* pq, Pair* pair) {

    if (pq->index == pq->size) {
        Pair** temp = malloc((pq->size * 2) * sizeof(Pair*));
        for (int i = 0; i < pq->size; i++) {
            temp[i] = pq->pairArr[i];
        }
        free(pq->pairArr);
        pq->pairArr = temp;
        pq->size *= 2;
    }

    
    pq->pairArr[pq->index] = pair;
    pq->index = pq->index +1;
}

Pair* dequeue(PrioQueue* pq) {
    if (pq->index == 0) {
        return NULL;
    }   

    Pair** pairArr = pq->pairArr;
    Pair* minPair = pairArr[0];
    int min = pairArr[0]->distance;
    int indexFound = 0;
    for (int i = 0; i < pq->index; i++) {
        if (pairArr[i]->distance < min ) {
            min = pairArr[i]->distance;
            minPair = pairArr[i];
            indexFound = i;
        }
    }
    
    for (int i = indexFound; i < pq->index - 1; i++){
        pairArr[i] = pairArr[i+1];
    }
    pq->index--;
    pairArr[pq->index] = NULL;
    return minPair;
}

void decrease_priority(PrioQueue*  pq, char* vertex, int distance) {
    for (int i = 0; i < pq->index; i++) {
       char* pqVertex = pq->pairArr[i]->vertex; 
       if (strcmp(pqVertex, vertex) == 0) {
            pq->pairArr[i]->distance = distance;
       }
    }
}

void push(Stack* stack, char* vertex) {
    if (stack->index == stack->size) {
        char** temp = malloc((stack->size * 2) * sizeof(char*));
        for (int i = 0; i < stack->size; i++) {
            temp[i] = stack->stackArr[i];
        }
        free(stack->stackArr);
        stack->stackArr = temp;  
        stack->size = stack->size * 2;
    }
    int index = stack->index;
    stack->stackArr[index] = vertex;
    stack->index += 1;
}

char* pop(Stack* stack) {
    if (stack->index ==0) {
        return NULL;
    }
    stack->index -= 1;
    char* temp = stack->stackArr[stack->index];
    stack->stackArr[stack->index] = NULL;    
    return temp;
}

// Finds the Adj List for a vertex
LinkedList* findVertexAdjList(Graph* graph, char* vertexID) {
    LinkedList** arr = graph->pArrLinkedList;
    for (int i = 0; i < graph->vertices; i++) {
        if (strcmp(arr[i]->identifier, vertexID) == 0) return arr[i];
    }
    return NULL;
}

void addEdgeToList(LinkedList* adjListOut, LinkedList* adjListIn, char* vertexID, int distance) { // need to find BOTH LIST BEFORE
    
    if (adjListOut->head == NULL) {
        adjListOut->head = allocate_node(vertexID, distance);
        adjListOut->edgeCount += 1;
        adjListOut->outDegree += 1;
        adjListIn->inDegree += 1;
        return;
    }
    Node* current = adjListOut->head;

    while (current != NULL) {
        if ((strcmp(current->vertex, vertexID) > 0) && current == adjListOut->head) { // Adds node before head
            Node* newNode = allocate_node(vertexID, distance);
            newNode->next = current;
            adjListOut->head = newNode;
            adjListOut->edgeCount += 1;
            adjListOut->outDegree += 1;
            adjListIn->inDegree += 1;
            return;
        } else if (current->next == NULL) { // Adds node to end of list.
            Node* newNode = allocate_node(vertexID, distance);
            current->next = newNode;
            adjListOut->edgeCount += 1;
            adjListOut->outDegree += 1;
            adjListIn->inDegree += 1;
            return;
        } else if (strcmp(current->vertex, vertexID) < 0 && strcmp(vertexID, current->next->vertex) < 0) { //adds node between two nodes
            struct node* newNode = allocate_node(vertexID, distance);
            struct node* temp = current->next;
            current->next = newNode;
            newNode->next = temp;
            adjListOut->edgeCount += 1;
            adjListOut->outDegree += 1;
            adjListIn->inDegree += 1;
            return;
        }
        current = current->next;
    }
}

int contains(Stack* stack, char* string) {
    char** arr = stack->stackArr;
    int n = stack->index;
    for (int i = 0; i < n; i++){
        if (strcmp(arr[i], string) == 0) {
            return 1;
        }
    }
    return 0;
}

int indexMap(char* sourceVertex, char** vertexArr, int n) {
    for(int i = 0; i < n; i++) {
        if (strcmp(vertexArr[i], sourceVertex) == 0){
            return i;
        }
    }
    return -1;
}

int* dAlgo(Graph* graph, char* sourceVertex, char** vertexArr) {
    int n = graph->vertices;
    int index;
    int* distanceArr = malloc(n * sizeof(int));
    index = indexMap(sourceVertex, vertexArr, n);
    distanceArr[index] = 0;


    PrioQueue* pq = allocate_PriorityQueue();

    for (int i = 0; i < n; i++) {
        if (i != index) { // v != source
            distanceArr[i] = INT_MAX;
        }
        Pair* pair = allocate_pair(INT_MAX, vertexArr[i]); 
        enqueue(pq, pair); //smallest pair should be the source
    }

    while (pq->index != 0) { // while q is not empty
        Pair* minPair = dequeue(pq); //extract min
        index = indexMap(minPair->vertex, vertexArr, n);
        int distanceU = distanceArr[index];
        if (distanceU == INT_MAX) {
            free(minPair);
            continue;
        }
        LinkedList* adjList = findVertexAdjList(graph, minPair->vertex);
        Node* currentNode = adjList->head;
        free(minPair);

        while (currentNode != NULL) {
            index = indexMap(currentNode->vertex, vertexArr, n);
            int distanceV = distanceArr[index];
            int alt = distanceU + currentNode->distance;
            if (distanceV == INT_MAX || (alt < distanceV)){
                distanceArr[index] = alt;
                decrease_priority(pq, currentNode->vertex, alt);
            } 
            currentNode = currentNode->next;
        } 

    }

    
    for (int i = 0; i < pq->index; i++) {
        free(pq->pairArr[i]);
    }
    free(pq->pairArr);
    free(pq);
    return distanceArr;
}


void bubbleSort(char** arr, int n) {
    for (int i = 0; i < n -1; i++) {
        for (int j = 0; j< n-i-1; j++) {
            char* vertex1 = arr[j];
            char* vertex2 = arr[j+1];
            if (strcmp(vertex1,vertex2) > 0) {
                char* temp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = temp;
            }
        }
    }
}

void printArray(char** arr, int n){
    for (int i = 0; i < n; i++) {
        if (i == (n-1)){
            printf("%s\n", arr[i]);
        }else {
            printf("%s ", arr[i]);
        }
    }
}

void freeNodes(Node* current) {
    while (current != NULL) {
        Node* temp = current->next;
        free(current->vertex);
        free(current);
        current = temp;
    }
}

void freeGraphContents(Graph* graph) {
    for (int i = 0; i < graph->vertices; i++) {
        LinkedList* adjList = graph->pArrLinkedList[i];
        freeNodes(adjList->head);
        free(adjList->identifier);
        free(adjList);
    }
    free(graph->pArrLinkedList); 
}

int main (int argc, char** argv) {

    if (argc < 3) {
        printf("enter the input arguments\n");
        exit(EXIT_SUCCESS);
    }

    FILE* fp = fopen(argv[1], "r"); // change back to input from command line
    if (fp == NULL) {
        printf("unable to open input file \n");
        exit(EXIT_SUCCESS);
    }

    int verticies;
    fscanf(fp, "%d", &verticies);
    int STRING_BUFFER = 100;
    Graph* directedGraph = allocate_graph(verticies); //Creates array of pointers to an Adjacentcy List.
    for (int i = 0; i < verticies; i++) {
        char vertexName[STRING_BUFFER];
        fscanf(fp, "%s", vertexName);
        directedGraph->pArrLinkedList[i] = allocate_linkedlist(vertexName); // Creates an Adjacentcy list at every index of graph array  
    }

    char vertexOut[STRING_BUFFER];
    char vertexIn[STRING_BUFFER];
    int distance;
    while (fscanf(fp,"%s %s %d\n", vertexOut, vertexIn, &distance) != EOF) {
        LinkedList* adjListVertexOut = findVertexAdjList(directedGraph, vertexOut); // finds matching adjList
        LinkedList* adjListVertexIn = findVertexAdjList(directedGraph, vertexIn);
        addEdgeToList(adjListVertexOut, adjListVertexIn, vertexIn, distance); 
    }

    fclose(fp);


    FILE* fp2 = fopen(argv[2], "r"); // change back to input from command line
    if (fp2 == NULL) {
        printf("unable to open input file \n");
        exit(EXIT_SUCCESS);
    }

    int n = directedGraph->vertices;
    int* distanceArr;
    char** sortedVertexArr = malloc(n * sizeof(char*));
    for (int i = 0; i < n; i++){
        sortedVertexArr[i] = directedGraph->pArrLinkedList[i]->identifier;
    }
    bubbleSort(sortedVertexArr, n);

    

    char vertexID[STRING_BUFFER];
    while (fscanf(fp2,"%s\n", vertexID) != EOF) {
        distanceArr = dAlgo(directedGraph, vertexID, sortedVertexArr);
        for (int i = 0; i < n; i++) {
           char* vertex = sortedVertexArr[i];
           int distance = distanceArr[i];

            if (distance == INT_MAX) {
                printf("%s INF\n", sortedVertexArr[i]);
            } else {
                printf("%s %d\n", vertex, distance);
            }
            if (i == n-1){
                printf("\n");
            }
        }  
        free(distanceArr);
    }

    fclose(fp2);
    free(sortedVertexArr);
    freeGraphContents(directedGraph);
    free(directedGraph);

    return EXIT_SUCCESS;
}