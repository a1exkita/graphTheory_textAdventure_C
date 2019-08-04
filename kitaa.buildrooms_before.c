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
void printAdj(int[][V]);
void addEdge(int[][V], int, int);
void deleteEdge(int[][V], int, int);
int checkEdge(int[][V], int, int);
void randomConnectRooms(int[][V]);
int chooseDest(int*, int, int);
void addEdgesToEachRoom(int, int[][V], int);


int main(){
        struct Room* Rooms = (struct Room*)malloc(sizeof(struct Room)*16);
        pid_t process_id;
        char dirname[32];
        process_id = getpid();
        sprintf(dirname, "kitaa.rooms.%d", process_id);
        mkdir(dirname, 0755);
        /*stat(dirname, &buf);
        printf("st_mtime: %d\n", buf.st_mtime);*/
        srand(time(NULL));

        generateRooms(Rooms);
        printRooms(Rooms, process_id);
        return 0;
}


void generateRooms (struct Room* Rooms){
        int i;
        int adjMatrix[V][V];
        initAdjacencyMatrix(adjMatrix);
        randomConnectRooms(adjMatrix);
        printAdj(adjMatrix);
        initRooms(Rooms, adjMatrix);
}

void initAdjacencyMatrix(int arr[][V]){
        int i,j;
        for(i = 0; i < V; i++){
                for(j = 0; j < V; j++)
                        arr[i][j] = 0;
        }
}

void printAdj(int arr[][V]){
        int i,j;
        for(i = 0; i < V; i++){
                for(j = 0; j < V; j++)
                        printf("%d\t", arr[i][j]);
                printf("\n");
        }
}


void randomConnectRooms(int adjMatrix[][V]){
        int i;
        int num_edges;
        for(i = 0; i < V; i++){
                num_edges = rand() % 4 + 3;
                printf("num_edges: %d\n", num_edges);
                addEdgesToEachRoom(num_edges, adjMatrix, i);
        }
}

/******************************************************
description: it chooses 3-6 rooms to be connected with each node
pre: the array is declared. num_edges is also defined by the rand function (3-6)
post: the randamly selected rooms (the # is 3-6 out of 6) are filled
*******************************************************/
void addEdgesToEachRoom(int num_edges, int adjMatrix[][V], int src_room){
        int i=0, j=0, k, done = 0, count;
        int* rand_node = (int*)malloc(sizeof(int)*16);

        while(j < src_room){
                if (adjMatrix[src_room][j])
                        done++;
                j++;
        }

        printf("num_edges: %d done: %d\n", num_edges, done);
        while(i < (num_edges-done)){
                rand_node[i] = rand()%V;
/*                rand_node[i] = rand()%(V-src_room-1) + (src_room+1);  ex. case 0: candidates 1-6*/
                if(checkEdge(adjMatrix, src_room, rand_node[i]) && src_room != rand_node[i]){
                        printf("add: src: %d-[%d] dest:%d \n", src_room, i, rand_node[i] );
                        addEdge(adjMatrix, src_room, rand_node[i]);
                        addEdge(adjMatrix, rand_node[i], src_room);
                        i++;
                }
        }

        i = 0, j = 0;
        while(i < (done - num_edges)){
                while(j < src_room){
                        if (adjMatrix[src_room][j]){
                                for(k=0; k < V; k++){
                                        if(adjMatrix[j][k])
                                                count++;
                                }
                                if(count > 3){
                                        printf("delete: src: %d dest:%d \n", src_room, j );
                                        deleteEdge(adjMatrix, src_room, j);
                                        deleteEdge(adjMatrix, j, src_room);
                                        break;
                                }
                        }
                        j++;
                }
                i++;
        }

/*        if(num_edges < done){
                deleteEdge(adjMatrix, src_room, rand_node[i]);
                deleteEdge(adjMatrix, rand_node[i], src_room);
        } else if (num_edges > done){
                addEdge(adjMatrix, src_room, rand_node[i]);
                addEdge(adjMatrix, rand_node[i], src_room);
        }*/


        /*int i;
        chooseDest(rand_node, num_edges, src_room);
        for(i=0; i < num_edges; i++){
                printf("1: src: %d, dest: %d, arr[src][dest]: %d\n", src_room, rand_node[i], adjMatrix[src_room][rand_node[i]]);
                if(checkEdge(adjMatrix, src_room, rand_node[i]) && src_room < rand_node[i])
                        printf("2: src: %d, dest: %d, arr[src][dest]: %d\n\n", src_room, rand_node[i], adjMatrix[src_room][rand_node[i]]);
                        addEdge(adjMatrix, src_room, rand_node[i]);
                        addEdge(adjMatrix, rand_node[i], src_room);
        }*/
}

/******************************************************
description: it chooses 3-6 rooms to be connected with each node
pre: the array is declared. num_edges is also defined by the rand function (3-6)
post: the randamly selected rooms (the # is 3-6 out of 6) are filled
*******************************************************/
int chooseDest(int* rand_node, int num_edges, int src){
        int i = 0, j, ch;
        while(i < num_edges){
                ch = rand()%(V-i) + (i+1);
/*                printf("i: %d ch: %d\n",i, ch);*/
                rand_node[i] = ch;
/*                printf("rand_node[i]: %d src: %d \n", rand_node[i], src);*/
                if(rand_node[i] == src)
                        i--;
                else if (i > 0){
                        for(j = 0; j < i; j++){
/*                                printf("rand_node[%d]: %d rand_node[%d]: %d \n", i, rand_node[i], j, rand_node[j]);*/
                                if ( rand_node[i] == rand_node[j] ){
                                        i--;
                                        break;
                                }
                        }
                }
                i++;
/*                printf("i: %d\n\n", i);*/
        }
/*
        int in, im, rn, rm;
        im = 0;
        for (in = 0; in < N && im < num_edges; ++in) {
                int rn = N - in;  how many numbers we still need to iterate through
                int rm = num_edges - im;  how many numbers we still need to find
                if (rand() % rn < rm){
                        if(in == src){
                                printf("in: %d src: %d\n", in, src);
                                in--;
                        }
                        else
                                rand_node[im++] = in;  the chosen id would be stored into rand_node arr
                }
        }*/
/*        assert(im == num_edges);*/
}

/******************************************************
description: if the chosen set of src and dest by Knuth is not filled, return 1.
pre:
post: the randamly selected rooms (the # is 3-6 out of 6) are filled
*******************************************************/
int checkEdge(int arr[][V], int src, int dest){
        if(arr[src][dest])
                return 0;
        return 1;
}

void addEdge(int arr[][V], int src, int dest){
        arr[src][dest] = 1;
}

void deleteEdge(int arr[][V], int src, int dest){
        arr[src][dest] = 0;
}

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
                /*printf("Rooms[i].connections[0]: %d\n", Rooms[i].connections[0]);
                printf("Rooms[0].connections[i]: %d\n", Rooms[0].connections[i]);*/
        }
        hardCodeNames(Rooms);
}

/*        strcpy(Rooms[0].name, "Byers");
        strcpy(Rooms[1].name, "School");
        strcpy(Rooms[2].name, "Police");
        strcpy(Rooms[3].name, "Wheeler");
        strcpy(Rooms[4].name, "Mirkwood");
        strcpy(Rooms[5].name, "Library");
        strcpy(Rooms[6].name, "Labo");*/

/******************************************************
description: it chooses 3-6 rooms to be connected with each node
pre: the array is declared. num_edges is also defined by the rand function (3-6)
post: the randamly selected rooms (the # is 3-6 out of 6) are filled
*******************************************************/
void hardCodeNames(struct Room* Rooms){
/*        int in, im, rn, rm;
        im = 0;
        int mess = rand()%10; Knuth is randamly choosing 7 out of 10, but it doesn't guarantee the random order. So we
        for (in = 0; in < candidates && im < num_edges; ++in) {
                int rn = candidates - in;  how many numbers we still need to iterate through
                int rm = num_edges - im;  how many numbers we still need to find
                if (rand() % rn < rm){
                        printf("in: %d mess: %d in+mess%10: %d\n", in, mess,(in+mess) % 10);*/
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
                        /*switch ((in+mess) % 10) {
                                case 0:
                                        Rooms[im++].name = "Byers";
                                        break;
                                case 1:
                                        Rooms[im++].name = "School";
                                        break;
                                case 2:
                                        Rooms[im++].name = "Police";
                                        break;
                                case 3:
                                        Rooms[im++].name = "Wheeler";
                                        break;
                                case 4:
                                        Rooms[im++].name = "Mirkwood";
                                        break;
                                case 5:
                                        Rooms[im++].name = "Library";
                                        break;
                                case 6:
                                        Rooms[im++].name = "Labo";
                                        break;
                                case 7:
                                        Rooms[im++].name = "MindFlyr";
                                        break;
                                case 8:
                                        Rooms[im++].name = "ScpsAhoy";
                                        break;
                                default:
                                        Rooms[im++].name = "StrCourt";
                        }*/


        /*        assert(im == num_edges);*/
}


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
/*
        for(i = 0; i < V; i++){
                for(j = 0; j < V; j++)
                        printf("%d\t", Rooms[i].connections[j]);
                printf("\n");
        }
*/
}
