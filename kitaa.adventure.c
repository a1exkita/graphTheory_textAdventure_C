#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h> /* Compile with -lpthread option in gcc*/
#include <time.h>

#define V 7

struct Room {
        char* name;
        char* type;
        int connection_count;
        char** connections;
};

void findNewestDir(char*);
void readFiles(char*, struct Room*);
void readInfoEachFile(char*, char*, struct Room*, int*);
void countConnections(FILE*, char*, char*, size_t, ssize_t, int*);
void* gameInterface(struct Room*);
void askDestination(struct Room*, int, char*);
int findStartRoom(struct Room*);
int findEndRoom(struct Room*);
void* write_time();


/******************************************************
function: main()
description: read room info files and invoke the game.
pre: exists the files that have rooms' information, compile with -lpthread
post: $? should be 0
*******************************************************/
void main(){
        int i, j, startRoom_id;
        char newestDirName[256]; // Holds the name of the newest dir that contains prefix
        struct Room* Rooms = (struct Room*)malloc(sizeof(struct Room)*V);
        for(i=0; i< V; i++){
                Rooms[i].name = (char*)malloc(sizeof(char)*32);
                Rooms[i].type = (char*)malloc(sizeof(char)*32);
                Rooms[i].connections = (char**)malloc(sizeof(char*)*V);
                for(j=0; j < V; j++){
                        Rooms[i].connections[j] = (char*)malloc(sizeof(char)*32);
                }
        }
        findNewestDir(newestDirName);
        readFiles(newestDirName, Rooms);
        gameInterface(Rooms);
        exit(0);
}


/******************************************************
function: findNewestDir(char newestDirName[256])
description: find the newest directory which has the files of rooms
pre: exists the directory
post: the most recent directory should be chosen to read.
*******************************************************/
void findNewestDir(char newestDirName[256]){
        int newestDirTime = -1; // Modified timestamp of newest subdir examined
        char targetDirPrefix[32] = "kitaa.rooms."; // Prefix we're looking for
        memset(newestDirName, '\0', sizeof(newestDirName));

        DIR* dirToCheck; // Holds the directory we're starting in
        struct dirent *fileInDir; // Holds the current subdir of the starting dir
        struct stat dirAttributes; // Holds information we've gained about subdir

        dirToCheck = opendir("."); // Open up the directory this program was run in

        if (dirToCheck > 0) // Make sure the current directory could be opened
        {
          while ((fileInDir = readdir(dirToCheck)) != NULL) // Check each entry in dir
          {
            if (strstr(fileInDir->d_name, targetDirPrefix) != NULL) // If entry has prefix
            {
              stat(fileInDir->d_name, &dirAttributes); // Get attributes of the entry

              if ((int)dirAttributes.st_mtime > newestDirTime) // If this time is bigger
              {
                newestDirTime = (int)dirAttributes.st_mtime;
                memset(newestDirName, '\0', sizeof(newestDirName));
                strcpy(newestDirName, fileInDir->d_name);
              }
            }
          }
        }

        closedir(dirToCheck); // Close the directory we opened
}


/******************************************************
function: readFiles(char newestDirName[256], struct Room* Rooms)
description: read the file information and store them into the Room structure.
pre: built Room struct, built the array of Room, selected directory
post: the array of Room structure be successfully stored.
*******************************************************/
void readFiles(char newestDirName[256], struct Room* Rooms){
        char targetDirPrefix[32] = "_room"; // Prefix we're looking for
        int room_count;
        DIR* dirToCheck; // Holds the directory we're starting in
        struct dirent *fileInDir; // Holds the current subdir of the starting dir
        struct stat dirAttributes; // Holds information we've gained about subdir

        dirToCheck = opendir(newestDirName); // Open up the directory this program was run in

        if (dirToCheck > 0) // Make sure the current directory could be opened
        {
          while ((fileInDir = readdir(dirToCheck)) != NULL) // Check each entry in dir
          {
            if (strstr(fileInDir->d_name, targetDirPrefix) != NULL) // If entry has prefix
            {
              readInfoEachFile(newestDirName, fileInDir->d_name, Rooms, &room_count);
              room_count++;
            }
          }
        }
        closedir(dirToCheck); // Close the directory we opened
}


/******************************************************
function: readInfoEachFile(char* newestDirName, char* filename, struct Room* Rooms, int* room_count)
description: read each file information and store them into the Room structure.
pre: built Room struct, built the array of Room, selected directory
post: each element in the array of Room structure be successfully stored.
*******************************************************/
void readInfoEachFile(char* newestDirName, char* filename, struct Room* Rooms, int* room_count){
        int i, j=0, lineNum =0; FILE *stream; char *line = NULL; size_t len = 0; ssize_t read; char temp[64];
        char* information = (char*)malloc(sizeof(char)*64);
        char** connection_names = (char**)malloc(sizeof(char*)*V);
        for(i=0; i < V; i++)
                connection_names[i] = (char*)malloc(sizeof(char)*32);
        snprintf(temp, sizeof(temp), "./%s/%s", newestDirName, filename);

        countConnections(stream, temp, line, len, read, &lineNum);

        /* read information line by line from each files*/
        stream = fopen(temp, "r");
        while ((read = getline(&line, &len, stream)) != -1) {
                if(j == 0)
                        sscanf(line, "%*s %*s %s", Rooms[*room_count].name);
                else if (j == lineNum-1)
                        sscanf(line, "%*s %*s %s", Rooms[*room_count].type);
                else
                        sscanf(line, "%*s %*s %s", Rooms[*room_count].connections[j-1]);
                j++;
        }
        Rooms[*room_count].connection_count = lineNum -2;

        free(line);
        fclose(stream);
}

/******************************************************
function: countConnections(FILE* stream, char* temp, char* line, size_t len, ssize_t read, int* lineNum)
description: count the line numbers for each files to find the number of the connection for each room.
pre: built Room struct, built the array of Room, selected directory, variables to getline is set.
post: count the number of the line successfully
*******************************************************/
void countConnections(FILE* stream, char* temp, char* line, size_t len, ssize_t read, int* lineNum){
        stream = fopen(temp, "r");
        if (stream == NULL)
        	exit(EXIT_FAILURE);
        while ((read = getline(&line, &len, stream)) != -1) {
                (*lineNum)++;
        }
        fclose(stream);
}

/******************************************************
function: write_time()
description: Create a text file to write the time of the input with mutex function
pre: created a new thread for writing time
post: create a text file that has the time when the user input "time"
*******************************************************/
void* write_time(){
        pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER;
        time_t t ;
        struct tm *tmp ;
        char MY_TIME[32];
        time( &t );
        tmp = localtime( &t );
        strftime(MY_TIME, sizeof(MY_TIME), "%I:%M%p, %A, %B %d, %Y", tmp);
        pthread_mutex_lock(&myMutex);
        FILE* file;
        file = fopen("./currentTime.txt", "w");
                fprintf(file, MY_TIME);
        fclose(file);
        pthread_mutex_unlock(&myMutex);
        pthread_mutex_destroy(&myMutex);
        return NULL;
}

/******************************************************
function: gameInterface(struct Room* Rooms)
description: this is a game interface that hadnles user inputs and reactions for them
pre: the Room structure is successfully stored.
post: process the game successfully
*******************************************************/
void* gameInterface(struct Room* Rooms){
        printf("\n=== WELCOME TO STRANGER THINGS TEXT GAME!! ===\n\n");
        int i, cur, end, num_steps = 0;
        char destination[256];
        char** path = (char**)malloc(sizeof(char*)*32);
        for(i=0; i < 32; i++)
                path[i] = (char*)malloc(sizeof(char)*32);
        cur = findStartRoom(Rooms);
        end = findEndRoom(Rooms);
        do {
                printf("CURRENT LOCATION: %s\n", Rooms[cur].name);
                printf("POSSIBLE CONNECTIONS: ");
                for(i=0; i< Rooms[cur].connection_count; i++){
                        if(i != Rooms[cur].connection_count-1)
                                printf("%s, ", Rooms[cur].connections[i]);
                        else
                                printf("%s.\n", Rooms[cur].connections[i]);
                }
                askDestination(Rooms, cur, destination);
                for(i=0; i< V; i++){
                        if(!strcmp(destination, Rooms[i].name)){
                                cur = i;
                                break;
                        }
                }
                strcpy(path[num_steps++], Rooms[cur].name);
        } while(cur != end);
        printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
        printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", num_steps);
        for(i=0; i< num_steps; i++){
                printf("%s\n", path[i]);
        }

}

/******************************************************
function: askDestination(struct Room* Rooms, int cur, char* destination)
description: loops for asking "WHERE TO? > "
pre: Current location and possible connection are showed to the user
post: time -> read a time text file. loop until user inputs a connected room with the current room.
*******************************************************/
void askDestination(struct Room* Rooms, int cur, char* destination){
        int i, resultInt;
        pthread_t myThreadID;
        int flag=0;
        FILE* file;
        char c[1000];
        while(!flag){
                printf("WHERE TO? >");
                scanf("%s", destination);
                if(!strcmp(destination, "time")){
                        resultInt = pthread_create(&myThreadID, NULL, write_time, NULL);
                        pthread_join(myThreadID, NULL);
                        assert(0 == resultInt);
                        file = fopen("./currentTime.txt", "r");
                        fscanf(file, "%[^\n]", c);
                        printf("%s\n", c);
                        fclose(file);
                } else {
                        for(i=0; i< Rooms[cur].connection_count; i++){
                                if(!strcmp(destination, Rooms[cur].connections[i])){
                                        flag = 1;
                                        break;
                                }
                        }
                        if(!flag)
                                printf("HUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
                }
        }
}

/******************************************************
function: findStartRoom(struct Room* Rooms)
description: find the type of "START_ROOM"
pre: types of Rooms are successfully stored
post: return the id of the start room.
*******************************************************/
int findStartRoom(struct Room* Rooms){
        int i;
        for (i= 0; i< V; i++){
                if(!strcmp(Rooms[i].type,"START_ROOM"))
                        return i;
        }
}

/******************************************************
function: findEndRoom(struct Room* Rooms)
description: find the type of "END_ROOM"
pre: types of Rooms are successfully stored
post: return the id of the end room.
*******************************************************/
int findEndRoom(struct Room* Rooms){
        int i;
        for (i= 0; i< V; i++){
                if(!strcmp(Rooms[i].type,"END_ROOM"))
                        return i;
        }
}
