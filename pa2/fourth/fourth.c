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

typedef struct stack {
    char** stackArr;
    int index;
    int size;
} Stack;

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

void push(Stack* stack, char* vertex) {
    if (stack->index == stack->size) {
        char** temp = malloc((stack->size * 2) * sizeof(char*));
        for (int i = 0; i < stack->size; i++) {
            temp[i] = stack->stackArr[i];
        }
        free(stack->stackArr);
        stack->stackArr = temp;  
        stack->size = stack->size*2;
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
        if ((strcmp(current->vertex, vertexID) < 0) && current == adjListOut->head) { // Adds node before head
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
        } else if (strcmp(current->vertex, vertexID) > 0 && strcmp(vertexID, current->next->vertex) > 0) { //adds node between two nodes
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

int contains(char** arr, char* string, int n) {
    for (int i = 0; i < n; i++){
        if (strcmp(arr[i], string) == 0) {
            return 1;
        }
    }
    return 0;
}

void bubbleSort(LinkedList** adjListArr, int n) {
    for (int i = 0; i < n -1; i++) {
        for (int j = 0; j< n-i-1; j++) {
            LinkedList* adjList1 = adjListArr[j];
            char* vertex1 = adjList1->identifier;
            LinkedList* adjList2 = adjListArr[j+1];
            char* vertex2 = adjList2->identifier;
            if (strcmp(vertex1,vertex2) > 0) {
                LinkedList* temp = adjListArr[j];
                adjListArr[j] = adjListArr[j+1];
                adjListArr[j+1] = temp;
            }
        }
    }
}

// I changed the nodes to be inserted from greatet to least in order for my stack to traverse lexicograpically
char** dfs(Graph* graph) {
    int graphLength = graph->vertices;
    char** visitedArr = malloc(graphLength* sizeof(char*)); //Create visitArray with length of verticies.
    int size = 0;
    LinkedList** adjListArr = graph->pArrLinkedList;
    
    bubbleSort(adjListArr, graphLength); // Sort
    Stack* stack = allocate_stack(); // stack dynamically resizes

    for (int i =0; i < graphLength; i++) { // Iterares each adj list incase of disconnected graphs.
        char* currentVertex = adjListArr[i]->identifier;
        if (!contains(visitedArr, currentVertex, size)) {
            push(stack, currentVertex); 
            visitedArr[size] = currentVertex;
            size++;
        }
        while(stack->index > 0){ // Iterates each vertex depth until stack is empty.
            currentVertex = pop(stack);
            if (!contains(visitedArr, currentVertex, size)) {
                    visitedArr[size] = currentVertex;
                    size++;
                }
            LinkedList* adjList = findVertexAdjList(graph, currentVertex);
            Node* currentNode = adjList->head;


            for (int j = 0; j < adjList->edgeCount; j++) { //Iterates the nodes. Pushing nodes not in stack.
                currentVertex = currentNode->vertex;
                if (!contains(visitedArr, currentVertex, size)) {
                    push(stack, currentVertex);
                }
                currentNode = currentNode->next;
            }
        }   
    }

    free(stack->stackArr);
    free(stack);
    return visitedArr;
    
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

    if (argc < 2) {
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


    //start DFS FROM 1st vertex
    char** visited= dfs(directedGraph);
    printArray(visited, directedGraph->vertices);
    
    
    fclose(fp);
    free(visited);
    freeGraphContents(directedGraph);
    free(directedGraph);


    return EXIT_SUCCESS;
}