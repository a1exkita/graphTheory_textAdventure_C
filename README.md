# cs344_as2
text adventure with graph structure

## 📝 Contents
It is a primitive text adventure on my Operating System class.
The contents are:
* kitaa.buildroom
    This execution file is for building rooms literally. This generates a directory on the same level, which has 7 rooms that are picked up randomly from 10 candidate rooms.
    Each room has connected with 3-6 rooms, and the program connects them randomly. In order to that, an adjacency matrix is used for managing the connections among them.
* kitaa.adventure
    This file is an interface that a user can play the game via text commands. The user will type a room name, if it is connected with the current room,
    they can move forward to the next room. Quite simple.

## 🚀 Compile files
Compile kitaa.buildrooms.c and kitaa.adventure.c, and then run kitaa.buildrooms and kitaa.adventure in this order as follows.
```bash
$ gcc -o kitaa.buildrooms -c kitaa.buildrooms.c
$ gcc -o kitaa.adventure -c kitaa.adventure.c
$ kitaa.buildrooms
$ kitaa.adventure
```
