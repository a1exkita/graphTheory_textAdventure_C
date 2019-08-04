#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define V 7
#define M 3
#define N 6

struct Room {
        int id;
        char* name;
        char* type;
        int* connections;
};

int stat(const char *, struct stat *);
void generateRooms (struct Room*);
void printRooms(struct Room*, pid_t);
void initRooms(struct Room*, int[][V]);
void hardCodeNames(struct Room*);
void initAdjacencyMatrix(int[][V]);
void addEdge(int[][V], int, int);
void deleteEdge(int[][V], int, int);
int checkEdge(int[][V], int, int);
void randomConnectRooms(int[][V]);
int chooseDest(int*, int, int);
void addEdgesToEachRoom(int, int[][V], int);


/******************************************************
function: main()
description: builds rooms data and stores it into a new directory
pre: none
post: generated Rooms data and stored it into a directory
*******************************************************/
int main(){
        struct Room* Rooms = (struct Room*)malloc(sizeof(struct Room)*16);
        pid_t process_id;
        char dirname[32];
        process_id = getpid();
        sprintf(dirname, "kitaa.rooms.%d", process_id);
        mkdir(dirname, 0755);
        srand(time(NULL));
        generateRooms(Rooms);
        printRooms(Rooms, process_id);
        return 0;
}

/******************************************************
function: generateRooms()
description: creates a graph structre of rooms and its adjacencyMatrix,
and based on this matrix, generates the array of room structure with random connections among them
pre: none
post: generated Rooms data based on the adjacencyMatrix
*******************************************************/
void generateRooms (struct Room* Rooms){
        int i;
        int adjMatrix[V][V];
        initAdjacencyMatrix(adjMatrix);
        randomConnectRooms(adjMatrix);
        initRooms(Rooms, adjMatrix);
}

/******************************************************
function: initAdjacencyMatrix(int arr[][V])
description: initilizes an adjacencyMatrix for managing connections among the rooms
pre: two dimentional int is generated
post: the matrix is filled with 0
*******************************************************/
void initAdjacencyMatrix(int arr[][V]){
        int i,j;
        for(i = 0; i < V; i++){
                for(j = 0; j < V; j++)
                        arr[i][j] = 0;
        }
}

/******************************************************
function: randomConnectRooms(int adjMatrix[][V])
description: connects rooms randomly by adding edges among the nodes in the graph on the adjacencyMatrix
pre: an adjacencyMatrix is initilized
post: the matrix is filled with the data of connections among rooms
*******************************************************/
void randomConnectRooms(int adjMatrix[][V]){
        int i;
        int num_edges;
        for(i = 0; i < V; i++){
                num_edges = rand() % 4 + 3;
                addEdgesToEachRoom(num_edges, adjMatrix, i);
        }
}

/******************************************************
function: addEdgesToEachRoom(int num_edges, int adjMatrix[][V], int src_room)
description: it chooses 3-6 rooms to be connected with each node.
        Based on the randomly generated number from 3-6, the number of edges will be assigned to each nodes.
        Since starting from #0 node of the graph, gradually the edges are filled before adding operation.
        Thus, it counts the # of edges that each nodes already have, and add or delete edge(s) based on each case.
        This enables it to generate completely random connections with the requirement that each node should have 3 to 6 connections.
        pre: the array is declared. num_edges is also defined by the rand function (3-6)
pre: adjacencyMatrix is initilized
post: connects the rooms on adjacencyMatrix by adding edges (change the value 0 -> 1) among them
*******************************************************/
void addEdgesToEachRoom(int num_edges, int adjMatrix[][V], int src_room){
        int i=0, j=0, k, done = 0, count;
        int* rand_node = (int*)malloc(sizeof(int)*16);

        while(j < src_room){
                if (adjMatrix[src_room][j])
                        done++;
                j++;
        }

        /*add operation if the node should have get more edges*/
        while(i < (num_edges-done)){
                rand_node[i] = rand()%V;
                if(checkEdge(adjMatrix, src_room, rand_node[i]) && src_room != rand_node[i]){
                        addEdge(adjMatrix, src_room, rand_node[i]);
                        addEdge(adjMatrix, rand_node[i], src_room);
                        i++;
                }
        }
        /*delete operation if the node should have get less edges*/
        i = 0, j = 0;
        while(i < (done - num_edges)){
                while(j < src_room){
                        if (adjMatrix[src_room][j]){
                                for(k=0; k < V; k++){
                                        if(adjMatrix[j][k])
                                                count++;
                                }
                                /*only if the selected node have more than 3 edges, it can delete one edge*/
                                if(count > 3){
                                        deleteEdge(adjMatrix, src_room, j);
                                        deleteEdge(adjMatrix, j, src_room);
                                        break;
                                }
                        }
                        j++;
                }
                i++;
        }
}

/******************************************************
function: chooseDest(int* rand_node, int num_edges, int src)
description: it chooses 3-6 rooms to be connected with each node
pre: the array is declared. num_edges is also defined by the rand function (3-6)
post: the randamly selected rooms (the # is 3-6 out of 6) are filled
*******************************************************/
int chooseDest(int* rand_node, int num_edges, int src){
        int i = 0, j, ch;
        while(i < num_edges){
                ch = rand()%(V-i) + (i+1);
                rand_node[i] = ch;
                if(rand_node[i] == src)
                        i--;
                else if (i > 0){
                        for(j = 0; j < i; j++){
                                if ( rand_node[i] == rand_node[j] ){
                                        i--;
                                        break;
                                }
                        }
                }
                i++;
        }
}

/******************************************************
function: checkEdge(int arr[][V], int src, int dest)
description: if the chosen set of src and dest is not filled, return 1.
pre: adjacencyMatrix is initilized
post: return the result of the check if the edge exists or not
*******************************************************/
int checkEdge(int arr[][V], int src, int dest){
        if(arr[src][dest])
                return 0;
        return 1;
}


/******************************************************
function: addEdge(int arr[][V], int src, int dest)
description: add the edge to the designated element in the adjacencyMatrix.
pre: adjacencyMatrix is initilized
post: change the value to 1 on the designated element of the adjacencyMatrix
*******************************************************/
void addEdge(int arr[][V], int src, int dest){
        arr[src][dest] = 1;
}

/******************************************************
function: deleteEdge(int arr[][V], int src, int dest)
description: delete the edge to the designated element in the adjacencyMatrix.
pre: adjacencyMatrix is initilized
post: change the value to 0 on the designated element of the adjacencyMatrix
*******************************************************/
void deleteEdge(int arr[][V], int src, int dest){
        arr[src][dest] = 0;
}

/******************************************************
function: initRooms(struct Room* Rooms, int adjMatrix[][V])
description: based ont eh adjacencyMatrix, fills the structure data of the rooms
pre: adjacencyMatrix is done
post: Rooms data are successfully stored into the struct Room* Rooms.
*******************************************************/
void initRooms(struct Room* Rooms, int adjMatrix[][V]){
        int i, j;
        int* connections;
        for (i = 0; i < V; i++){
                Rooms[i].id = i;
                Rooms[i].name = calloc(8, sizeof(char));
                Rooms[i].type = calloc(16, sizeof(char));
                switch (i){
                        case 0:
                                strcpy(Rooms[i].type, "START_ROOM");
                                break;
                        case 6:
                                strcpy(Rooms[i].type, "END_ROOM");
                                break;
                        default:
                                strcpy(Rooms[i].type, "MID_ROOM");
                }
                Rooms[i].connections = (int*)malloc(sizeof(int)*16);
                for(j=0; j<V; j++)
                        Rooms[i].connections[j] = adjMatrix[i][j];
        }
        hardCodeNames(Rooms);
}

/******************************************************
function: hardCodeNames(struct Room* Rooms)
description: it chooses 7 rooms' names out of 10.
pre: Rooms structure successfully has data.
post: randomly selected names are assigned into the name variables of each rooms.
*******************************************************/
void hardCodeNames(struct Room* Rooms){
        int i, j, id, flag;
        int* arr = (int *)malloc(sizeof(int)*V);
        while (id < V){
                flag = 0;
                while(flag != 1){
                        flag =1;
                        i = rand() % 10;
                        if(id > 0){
                                for(j = 0; j < id; j++){
                                        if(arr[j] == i){
                                                flag = 0;
                                                break;
                                        }
                                }
                        }
                }
                arr[id] = i;

                /* switch for 7 out 10*/
                switch (i) {
                        case 0:
                                Rooms[id++].name = "Byers"; /* the chosen id would be stored into rand_node arr */
                                break;
                        case 1:
                                Rooms[id++].name = "School"; /* the chosen id would be stored into rand_node arr */
                                break;
                        case 2:
                                Rooms[id++].name = "Police"; /* the chosen id would be stored into rand_node arr */
                                break;
                        case 3:
                                Rooms[id++].name = "Wheeler"; /* the chosen id would be stored into rand_node arr */
                                break;
                        case 4:
                                Rooms[id++].name = "Mirkwood"; /* the chosen id would be stored into rand_node arr */
                                break;
                        case 5:
                                Rooms[id++].name = "Library"; /* the chosen id would be stored into rand_node arr */
                                break;
                        case 6:
                                Rooms[id++].name = "Labo"; /* the chosen id would be stored into rand_node arr */
                                break;
                        case 7:
                                Rooms[id++].name = "MindFlyr"; /* the chosen id would be stored into rand_node arr */
                                break;
                        case 8:
                                Rooms[id++].name = "ScpsAhoy"; /* the chosen id would be stored into rand_node arr */
                                break;
                        default:
                                Rooms[id++].name = "StrCourt";
                }
        }
}

/******************************************************
function: printRooms(struct Room* Rooms, pid_t process_id)
description: generate 7 room files in the directory with process_id
pre: the directory is generated with process_id
post: 7 rooms information is stored into the directory
*******************************************************/
void printRooms(struct Room* Rooms, pid_t process_id){
        int i,j,k;
        FILE* file;
        for(i=0; i< V; i++){
                char temp[64];
                snprintf(temp, sizeof(temp), "./kitaa.rooms.%d/%s_room", process_id, Rooms[i].name);
                file = fopen(temp, "w");
                fprintf(file, "ROOM NAME: %s\n", Rooms[i].name);
                k=1;
                for(j=0; j < V; j++){
                        if(Rooms[i].connections[j]){
                                fprintf(file, "CONNECTION %d: %s\n", k, Rooms[j].name);
                                k++;
                        }
                }
                fprintf(file, "ROOM TYPE: %s\n", Rooms[i].type);
                fclose(file);
        }
}
