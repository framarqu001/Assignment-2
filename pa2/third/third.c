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
    int edgeCount;
} LinkedList;

// A graph will be made up of an array of linked list. 
typedef struct graph {
    int vertices;
    struct linkedList** pArrLinkedList;
} Graph;

typedef struct queue {
    struct node* head;
    struct node* tail;
} Queue;

Node* allocate_node(char* vertexID) {
    Node* temp = malloc(sizeof(struct node));
    temp->vertex = strdup(vertexID); // Was copying/allocating manually until I saw this func existd
    temp->next = NULL;
    return temp;
};

Queue* allocate_queue(){
    Queue* temp = malloc(sizeof(struct queue));
    temp->head = NULL;
    temp->tail = NULL;
    return temp;
}

LinkedList* allocate_linkedlist(char* vertexID) {
    LinkedList* temp = malloc(sizeof(LinkedList));
    temp->identifier = strdup(vertexID); //String is dynamically allocated
    temp->head = NULL;
    temp->edgeCount = 0;
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
        adjList->edgeCount += 1;
        return;
    }

    Node* current = adjList->head;
    
    while (current != NULL) {
        if ((strcmp(current->vertex, vertexID) > 0) && current == adjList->head) { // Adds node before head
            Node* newNode = allocate_node(vertexID);
            newNode->next = current;
            adjList->head = newNode;
            adjList->edgeCount += 1;
            return;
        } else if (current->next == NULL) { // Adds node to end of list.
            Node* newNode = allocate_node(vertexID);
            current->next = newNode;
            adjList->edgeCount += 1;
            return;
        } else if (strcmp(current->vertex, vertexID) < 0 && strcmp(vertexID, current->next->vertex) < 0) { //adds node between two nodes
            struct node* newNode = allocate_node(vertexID);
            struct node* temp = current->next;
            current->next = newNode;
            newNode->next = temp;
            adjList->edgeCount += 1;
            return;
        }
        current = current->next;
    }
}

void enqueue(Queue* queue, char* vertexID) {
    if (queue->head == NULL && queue->tail == NULL) {
        queue->head = allocate_node(vertexID);
        queue->tail = queue->head;
        return;
    }

    Node* temp = allocate_node(vertexID);

    queue->tail->next = temp;
    queue->tail = temp;
}

char* dequeue(Queue* queue) {
    if (queue->head == NULL && queue->tail == NULL) {
        char* temp = "Queue is empty";
        return temp;
    }

    Node* temp = queue->head->next;
    char* vertex = queue->head->vertex;
    free(queue->head);
    queue->head = temp;
    if (queue->head == NULL) {
        queue->tail = NULL;
    }

    return vertex;
}

int contains(char** arr, char* string, int n) {
    for (int i = 0; i < n; i++){
        if (strcmp(arr[i], string) == 0) {
            return 1;
        }
    }
    return 0;
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


char** bfs(Graph* graph, LinkedList* startVertex) {
    int arrLength = graph->vertices;
    char** visitedArr = malloc(arrLength * sizeof(char*)); //Create visitArray with length of verticies.
    visitedArr[0] = startVertex->identifier; //source vertex always visited.
    int index = 1;

    Queue* queue = allocate_queue(); // Queue will be a node based implementation.
    enqueue(queue, startVertex->identifier);

    while (index < arrLength) { //while visted array is not full
        char* currentVertex = dequeue(queue);
        LinkedList* adjList = findVertexAdjList(graph, currentVertex); //Find adj of vertex were exploring
        Node* currentNode = adjList->head; //gets list
        for (int i = 0; i < adjList->edgeCount; i++) { //Iterates through list
            if (!contains(visitedArr, currentNode->vertex, index)) { //if adjlist contains vertex not in Visited. We add to queue.
                enqueue(queue, currentNode->vertex);
                visitedArr[index] = currentNode->vertex;
                index++;
            }
            currentNode = currentNode->next;
            
        }
    }
    freeNodes(queue->head);
    free(queue);
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


    int MAX_SIZE = undirectedGraph->vertices;
    char vertexID[STRING_BUFFER];
    
    while (fscanf(fp2,"%s\n", vertexID) != EOF) {
        LinkedList* adjList = findVertexAdjList(undirectedGraph, vertexID); // Finds adjList in array of linkedlist pointers
        char** visitedArr = bfs(undirectedGraph, adjList);
        printArray(visitedArr, MAX_SIZE);
        free(visitedArr);
    }
    fclose(fp2);


    freeGraphContents(undirectedGraph);
    free(undirectedGraph);


    return EXIT_SUCCESS;
}