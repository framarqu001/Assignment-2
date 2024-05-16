#include <stdlib.h>
#include <string.h>
#include <stdio.h>



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
    Node* previous = adjListOut->head;
    Node* current = previous->next;

    while (current != NULL){
        previous = current;
        current = current->next;
    }

    current = allocate_node(vertexID,distance);
    previous->next = current;

    adjListOut->edgeCount += 1;
    adjListOut->outDegree += 1;
    adjListIn->inDegree += 1;
}

// Converts linkedlist into an array to be sorted.
char** listToArr(LinkedList* adjList) {
    char** tempArr = malloc((adjList->edgeCount) * sizeof(char*)); // creates an array of strings with size of adjList.
    Node* current = adjList->head;
    for (int i = 0; i < adjList->edgeCount; i++) {
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
        addEdgeToList(adjListVertexOut, adjListVertexIn, vertexIn, distance); //adds the vertex to both adjlist
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
        if (command == 'o') {
            LinkedList* adjList = findVertexAdjList(directedGraph, vertexID); // Finds adjList in array of linkedlist pointers
            printf("%d\n", adjList->outDegree); //Count of adjList = # of edges
        }
        if (command == 'i') {
            LinkedList* adjList = findVertexAdjList(directedGraph, vertexID);
            printf("%d\n", adjList->inDegree);
        }
        if (command == 'a') { // Instead of adding to list in order. I'm creating an array and sorting the values.
            LinkedList* adjList = findVertexAdjList(directedGraph, vertexID);
            if (adjList->head == NULL){
                printf("\n");
                continue;
            }
            char** arrVertexID = listToArr(adjList);
            bubbleSort(arrVertexID, adjList->edgeCount);
            printArray(arrVertexID, adjList->edgeCount);
            free(arrVertexID);
        }
    }
    fclose(fp2);
    freeGraphContents(directedGraph);
    free(directedGraph);

    return EXIT_SUCCESS;

}