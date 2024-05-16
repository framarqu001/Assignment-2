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

// Dynamically resizes by 2. Doesnt shrink
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

//Sorting the array of Linked List
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

int cycleFound = 0;
void dfsRecursive(Graph* graph, Stack* visited, Stack* topoSort, Stack* cycle, char* sourceVertex) {
    char* currentVertex = sourceVertex;
    
    push(visited,currentVertex);
    LinkedList* adjList = findVertexAdjList(graph, currentVertex);
    Node* currentNode = adjList->head;
    while (currentNode != NULL) {
        currentVertex = currentNode->vertex;
        if (contains(cycle, currentVertex)){
            cycleFound = 1;
            return;
        }
        if (!contains(visited, currentVertex)) {
            push(cycle, currentVertex);
            dfsRecursive(graph, visited, topoSort, cycle, currentVertex);
        }
        currentNode = currentNode->next;
    }
    push(topoSort, sourceVertex);
    pop(cycle);
}

// changed back nodes to be sorted lexicographically
int topologicalSort(Graph* graph, char** sortedArr) {
    int graphLength = graph->vertices;
    LinkedList** adjListArr = graph->pArrLinkedList;
    Stack* visited = allocate_stack();
    Stack* topoSort = allocate_stack();
    Stack* cycle = allocate_stack();
    
    // bubbleSort(adjListArr, graphLength); // Sort adj list

    for (int i = 0; i < graphLength; i++) { // Iterares each adj list incase of disconnected graphs.
        char* currentVertex = adjListArr[i]->identifier;
        if (contains(visited, currentVertex)) continue;
        push(cycle, currentVertex);
        dfsRecursive(graph, visited, topoSort, cycle, currentVertex); 
        if (cycleFound) return 1; 
    }
    
    for(int i = 0; i < graphLength; i++) {
        sortedArr[i] = pop(topoSort);
    }
    
    free(cycle->stackArr);
    free(cycle);
    free(visited->stackArr);
    free(visited);
    free(topoSort->stackArr);
    free(topoSort);
    return 0;  
}

//Finds the index of a vertex in topoArr
int indexMap(char** topoArr, char* sourceVertex, int n) {
    int index = 0;
    for (int i =0; i < n; i++) {
        if (strcmp(topoArr[i], sourceVertex) == 0) {
            index = i;
        }
    }
    return index;
}
    
void algo1(Graph* graph, char* sourceVertex, char** topoArr, int* distanceArr) { 
    int n = graph->vertices;
    for (int i = 0; i < n; i++) { // set distance to inf
        distanceArr[i] = INT_MAX;
    }
    int index = indexMap(topoArr, sourceVertex,n); // Index map will be used to map Distance arr and Topo Arr
    distanceArr[index] = 0; //source index = 0;

    for (int i = 0; i < n; i++) { // for each vertex u in topologically sorted order
        LinkedList* adjList = findVertexAdjList(graph, topoArr[i]); // U

        char* vertexU = adjList->identifier;
        int index = indexMap(topoArr, vertexU, n);
        int distanceU = distanceArr[index];

        Node* currentNode = adjList->head; // V
        for (int j = 0; j < adjList->edgeCount; j++) {

            char* vertexV = currentNode->vertex;
            int weight = currentNode->distance;
            int index = indexMap(topoArr, vertexV, n);
            int distanceV = distanceArr[index];

            if  ((distanceU != INT_MAX ) && distanceV > distanceU + weight) {
                distanceArr[index] = distanceU + weight;
            }
            currentNode = currentNode->next;
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

    //start DFS FROM 1st vertex
    fclose(fp);

    FILE* fp2 = fopen(argv[2], "r"); // change back to input from command line
    if (fp2 == NULL) {
        printf("unable to open input file \n");
        exit(EXIT_SUCCESS);
    }

    int n = directedGraph->vertices;
    int* distanceArr = malloc(n * sizeof(int));
    char** topoArr = malloc(n * sizeof(char*));
    topologicalSort(directedGraph, topoArr);
    char** sortedArr = malloc(n * sizeof(char*));
    for (int i = 0 ; i < n; i++) {
        sortedArr[i] = topoArr[i];
    }
    bubbleSort(sortedArr, n);


    char vertexID[STRING_BUFFER];
    printf("\n"); // results are formated like this?
    while (fscanf(fp2,"%s\n", vertexID) != EOF) {
        if (cycleFound){
            printf("CYCLE\n");
            continue;
        }
    
        algo1(directedGraph, vertexID, topoArr, distanceArr);
        for (int i = 0; i < n; i++) {
            char* vertex = sortedArr[i];
            int index = indexMap(topoArr, vertex, n);
            int distance = distanceArr[index];
            //mapindex with new lexo sorted array with topoarray then the distance = distance arr
            if (distance == INT_MAX) {
                printf("%s INF\n", sortedArr[i]);
            } else {
                printf("%s %d\n", vertex, distance);
            }
            if (i == n-1){
                printf("\n");
            }
            
            
        }
    }

    free(distanceArr);
    free(topoArr);
    free(sortedArr);
    fclose(fp2);
    freeGraphContents(directedGraph);
    free(directedGraph);


    return EXIT_SUCCESS;
}