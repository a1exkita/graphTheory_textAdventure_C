/* readDirectory.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h> /* Compile with -lpthread option in gcc*/

#define V 7

struct Room {
        char* name;
        char* type;
        int connection_count;
        char** connections;
};

/*void allocateRooms(struct Room*);*/
void findNewestDir(char*);
void readFiles(char*, struct Room*);
void readInfoEachFile(char*, char*, struct Room*, int*);
/*void* gameInterface(struct Room*, pthread_t*, int*, int*);*/
void* gameInterface(struct Room*);
int findStartRoom(struct Room*);
int findEndRoom(struct Room*);
void* perform_work();

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

/*void allocateRooms(struct Room* Rooms){
        int i;
        Rooms = (struct Room*)malloc(sizeof(struct Room)*V);
        for(i=0; i< V; i++){
                Rooms[i].name = (char*)malloc(sizeof(char)*32);
                Rooms[i].type = (char*)malloc(sizeof(char)*32);
                Rooms[i].connections = (struct Room*)malloc(sizeof(struct Room)*V);
        }
}*/

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
              printf("Found the prefex: %s\n", fileInDir->d_name);
              stat(fileInDir->d_name, &dirAttributes); // Get attributes of the entry

              if ((int)dirAttributes.st_mtime > newestDirTime) // If this time is bigger
              {
                newestDirTime = (int)dirAttributes.st_mtime;
                memset(newestDirName, '\0', sizeof(newestDirName));
                strcpy(newestDirName, fileInDir->d_name);
                printf("Newer subdir: %s, new time: %d\n",
                       fileInDir->d_name, newestDirTime);
              }
            }
          }
        }

        closedir(dirToCheck); // Close the directory we opened
}

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
              printf("Found the prefex: %s\n", fileInDir->d_name);
              readInfoEachFile(newestDirName, fileInDir->d_name, Rooms, &room_count);
              room_count++;
            }
          }
        }
        closedir(dirToCheck); // Close the directory we opened
}


void readInfoEachFile(char* newestDirName, char* filename, struct Room* Rooms, int* room_count){
        int i, j=0, lineNum =0;
        FILE *stream;
        char *line = NULL;
        size_t len = 0;
        ssize_t read;
        char temp[64];
        char* information = (char*)malloc(sizeof(char)*64);
        snprintf(temp, sizeof(temp), "./%s/%s", newestDirName, filename);

        /*count the # of connections manually*/
        stream = fopen(temp, "r");
        if (stream == NULL)
        	exit(EXIT_FAILURE);
        while ((read = getline(&line, &len, stream)) != -1) {
                lineNum++;
        }

        fclose(stream);

        char** connection_names = (char**)malloc(sizeof(char*)*V);
        for(i=0; i < V; i++){
                connection_names[i] = (char*)malloc(sizeof(char)*32);
        }

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

        printf("Rooms[%d].name: %s\n",*room_count, Rooms[*room_count].name);
        for(i = 0; i< Rooms[*room_count].connection_count; i++)
                printf("CONNECT %d: %s\n", i+1, Rooms[*room_count].connections[i]);
        printf("ROOM TYPE: %s\n", Rooms[*room_count].type);
        free(line);
        fclose(stream);
}

void* perform_work(){
        pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER;
        /*int passed_in_value;
        passed_in_value = *((int *) argument);
        printf("Hello World! It's me, thread with argument %d!\n", passed_in_value);*/
        time_t t ;
        struct tm *tmp ;
        char MY_TIME[64];
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

void* gameInterface(struct Room* Rooms){
        int i, cur, end, num_steps = 0;
        int resultInt;
        pthread_t myThreadID;
        char** path = (char**)malloc(sizeof(char*)*32);
        for(i=0; i < 32; i++){
                path[i] = (char*)malloc(sizeof(char)*32);
        }
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
                int flag=0;
                char destination[256];
                FILE* file;
                char c[1000];
                while(!flag){
                        printf("WHERE TO? > ");
                        scanf("%s", destination);
                        if(!strcmp(destination, "time")){
                                /*printf(" 1:03pm, Tuesday, September 13, 2016\n");*/
                                resultInt = pthread_create(&myThreadID, NULL, perform_work, NULL);
                                assert(0 == resultInt);
                                file = fopen("./currentTime.txt", "r");
                                fscanf(file, "%[^\n]", c);
                                printf("%s\n", c);
                                fclose(file);
                                printf("In main: thread %d has completed\n", 1);
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
                strcpy(path[num_steps++], Rooms[cur].name);
                for(i=0; i< V; i++){
                        if(!strcmp(destination, Rooms[i].name)){
                                cur = i;
                                break;
                        }
                }
        } while(cur != end);
        strcpy(path[num_steps++], Rooms[end].name);
        printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
        printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", num_steps);
        for(i=0; i< num_steps; i++){
                printf("%s\n", path[i]);
        }

}


int findStartRoom(struct Room* Rooms){
        int i;
        for (i= 0; i< V; i++){
                if(!strcmp(Rooms[i].type,"START_ROOM"))
                        return i;
        }
}

int findEndRoom(struct Room* Rooms){
        int i;
        for (i= 0; i< V; i++){
                if(!strcmp(Rooms[i].type,"END_ROOM"))
                        return i;
        }
}
