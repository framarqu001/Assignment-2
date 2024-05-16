#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node {
    char* vertex;
    struct node* next;
} Node;

// Linked list will keep track of a vertex's edges
typedef struct linkedList {
    char* identifier;
    struct node* head;
    int count;
} LinkedList;

// A graph will be made up of an array of linked list. 
typedef struct graph {
    int vertices;
    struct linkedList** pArrLinkedList;
} Graph;

Node* allocate_node(char* vertexID) {
    Node* temp = malloc(sizeof(struct node));
    temp->vertex = strdup(vertexID); // Was copying/allocating manually until I saw this func existd
    temp->next = NULL;
    return temp;
};

LinkedList* allocate_linkedlist(char* vertexID) {
    LinkedList* temp = malloc(sizeof(LinkedList));
    temp->identifier = strdup(vertexID); //String is dynamically allocated
    temp->head = NULL;
    temp->count = 0;
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

// Finds the Adj List for a vertex
LinkedList* findVertexAdjList(Graph* graph, char* vertexID) {
    LinkedList** arr = graph->pArrLinkedList;
    for (int i = 0; i < graph->vertices; i++) {
        if (strcmp(arr[i]->identifier, vertexID) == 0) return arr[i];
    }
    return NULL;
}

void addEdgeToList(LinkedList* adjList, char* vertexID) {
    if (adjList->head == NULL) {
        adjList->head = allocate_node(vertexID);
        adjList->count += 1;
        return;
    }
    Node* previous = adjList->head;
    Node* current = previous->next;

    while (current != NULL){
        previous = current;
        current = current->next;
    }

    current = allocate_node(vertexID);
    previous->next = current;

    adjList->count += 1;
}

// Converts linkedlist into an array to be sorted.
char** listToArr(LinkedList* adjList) {
    char** tempArr = malloc((adjList->count) * sizeof(char*)); // creates an array of strings with size of adjList.
    Node* current = adjList->head;
    for (int i = 0; i < adjList->count; i++) {
        tempArr[i] = current->vertex;
        current = current->next;
    }
    return tempArr;
}

void bubbleSort(char** arr, int n) {
    for (int i = 0; i < n -1; i++) {
        for (int j = 0; j< n-i-1; j++) {
            if (strcmp(arr[j],arr[j+1]) > 0) {
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

int main (int argc, char** argv){

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
    Graph* undirectedGraph = allocate_graph(verticies); //Creates array of pointers to an Adjacentcy List.
    for (int i = 0; i < verticies; i++) {
        char vertexName[STRING_BUFFER];
        fscanf(fp, "%s", vertexName);
        undirectedGraph->pArrLinkedList[i] = allocate_linkedlist(vertexName); // Creates an Adjacentcy list at every index of graph array  
    }

    char vertex1[STRING_BUFFER];
    char vertex2[STRING_BUFFER];
    while (fscanf(fp,"%s %s", vertex1, vertex2) != EOF) {
        LinkedList* adjListVertex1 = findVertexAdjList(undirectedGraph, vertex1); // finds matching adjList
        LinkedList* adjListVertex2 = findVertexAdjList(undirectedGraph, vertex2);
        addEdgeToList(adjListVertex1, vertex2); //adds the vertex to both adjlist
        addEdgeToList(adjListVertex2, vertex1);
    }
    fclose(fp);

    FILE* fp2 = fopen(argv[2], "r"); // change back to input from command line

    if (fp2 == NULL) {
        printf("unable to open input file \n");
        exit(EXIT_SUCCESS);
    }

    char command;
    char vertexID[STRING_BUFFER];

    while (fscanf(fp2,"%c %s\n", &command, vertexID) != EOF) {
        if (command == 'd') {
            LinkedList* adjList = findVertexAdjList(undirectedGraph, vertexID); // Finds adjList in array of linkedlist pointers
            printf("%d\n", adjList->count); //Count of adjList = # of edges
        }
        if (command == 'a') { // Instead of adding to list in order. I'm creating an array and sorting the values.
            LinkedList* adjList = findVertexAdjList(undirectedGraph, vertexID);
            if (adjList->head == NULL){
                printf("\n");
                continue;
            }
            char** arrVertexID = listToArr(adjList);
            bubbleSort(arrVertexID, adjList->count);
            printArray(arrVertexID, adjList->count);
            free(arrVertexID);
        }
    }
    fclose(fp2);
    freeGraphContents(undirectedGraph);
    free(undirectedGraph);

    return EXIT_SUCCESS;

}