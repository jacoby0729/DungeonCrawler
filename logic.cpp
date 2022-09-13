#include <iostream>
#include <fstream>
#include <string>
#include "logic.h"

using namespace std;

/**
 * TODO: Student implement this function
 * Load representation of the dungeon level from file into the 2D map.
 * Calls createMap to allocate the 2D array.
 * @param   fileName    File name of dungeon level.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object by reference to set starting position.
 * @return  pointer to 2D dynamic array representation of dungeon map with player's location., or nullptr if loading fails for any reason
 * @updates  maxRow, maxCol, player
 */
char** loadLevel(const string& fileName, int& maxRow, int& maxCol, Player& player) {
    ifstream file(fileName);

    if(!file.is_open()) {
        return nullptr;
    }

    file >> maxRow;
    file >> maxCol;
    file >> player.row;
    file >> player.col;
    if(file.fail()) {return nullptr;}
    if(player.row >= maxRow || player.col >= maxCol || player.row < 0 || player.col < 0) {return nullptr;}

    if(maxRow < 1 || maxCol < 1)
        return nullptr;
    if(maxRow >= INT32_MAX / maxCol) 
        return nullptr;

    char** map = createMap(maxRow, maxCol);
    string row;
    char i;
    //int y = 0;
    bool exit = false;

    try {
    for(int k = 0; k < maxRow; k++)
    {
        for(int j = 0; j < maxCol; j++)
        {
            file >> map[k][j];
            if(!file.fail()) {
                if(map[k][j] == TILE_DOOR || map[k][j] == TILE_EXIT) {exit = true;}
                else if(map[k][j] != TILE_OPEN && map[k][j] != TILE_AMULET && map[k][j] != TILE_PILLAR && map[k][j] != TILE_TREASURE 
                 && map[k][j] != TILE_MONSTER) {
                    deleteMap(map, maxRow);
                    return nullptr;
                }
            }
            else {
                deleteMap(map, maxRow);
                return nullptr;
            }
        }
    }
    } catch (...) {
        deleteMap(map, maxRow);
        return nullptr;
    }


    if(file >> i){
        deleteMap(map, maxRow);
        return nullptr;
    }
    if(!exit) {
        deleteMap(map, maxRow);
        return nullptr;
    }

    map[player.row][player.col] = TILE_PLAYER;
    file.close();
    return map;
}

/**
 * TODO: Student implement this function
 * Translate the character direction input by the user into row or column change.
 * That is, updates the nextRow or nextCol according to the player's movement direction.
 * @param   input       Character input by the user which translates to a direction.
 * @param   nextRow     Player's next row on the dungeon map (up/down).
 * @param   nextCol     Player's next column on dungeon map (left/right).
 * @updates  nextRow, nextCol
 */
void getDirection(char input, int& nextRow, int& nextCol) {
    if(input == MOVE_UP)
        nextRow -= 1;
    else if(input == MOVE_DOWN)
        nextRow += 1;
    else if(input == MOVE_LEFT)
        nextCol -= 1;
    else if(input == MOVE_RIGHT)
        nextCol += 1;
   
}

/**
 * TODO: [suggested] Student implement this function
 * Allocate the 2D map array.
 * Initialize each cell to TILE_OPEN.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @return  2D map array for the dungeon level, holds char type.
 */
char** createMap(int maxRow, int maxCol) {
    if(maxRow <= 0 || maxCol <= 0) {return nullptr;}
    char** map = new char*[maxRow];
    for(int x = 0; x < maxRow; x++) {
        map[x] = new char[maxCol];
    }
    for(int a = 0; a < maxRow; a++) {
        for(int b = 0; b < maxCol; b++)
            map[a][b] = TILE_OPEN;
    }
    return map;
}

/**
 * TODO: Student implement this function
 * Deallocates the 2D map array.
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @return None
 * @update map, maxRow
 */
void deleteMap(char**& map, int& maxRow) {
    if(!(maxRow <= 0)) {
        if(map) {
            for (int row = 0; row < maxRow; row++)
                delete[] map[row];
            delete[] map;
        }
    }
    map = nullptr;
    maxRow = 0;
}

/**
 * TODO: Student implement this function
 * Resize the 2D map by doubling both dimensions.
 * Copy the current map contents to the right, diagonal down, and below.
 * Do not duplicate the player, and remember to avoid memory leaks!
 * You can use the STATUS constants defined in logic.h to help!
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height), to be doubled.
 * @param   maxCol      Number of columns in the dungeon table (aka width), to be doubled.
 * @return  pointer to a dynamically-allocated 2D array (map) that has twice as many columns and rows in size.
 * @update maxRow, maxCol
 */
char** resizeMap(char** map, int& maxRow, int& maxCol) {
    if(!map) {return nullptr;}
    int newRow = 2 * maxRow;
    int newCol = 2 * maxCol;
    if(newRow <= 0 || newCol <= 0 || newRow * newCol > INT32_MAX) {return nullptr;}
    char** remap = createMap(newRow, newCol);

    for(int x = 0; x < maxRow; x++) {
        for(int y = 0; y < maxCol; y++) {
            remap[x][y] = map[x][y];
            if(map[x][y] == TILE_PLAYER) {
                remap[x + maxRow][y] = TILE_OPEN;
                remap[x][y + maxCol] = TILE_OPEN;
                remap[x + maxRow][y + maxCol] = TILE_OPEN;
            }
            else {
                remap[x + maxRow][y] = map[x][y];
                remap[x][y + maxCol] = map[x][y];
                remap[x + maxRow][y + maxCol] = map[x][y];
            }
        }
    }
    
    deleteMap(map, maxRow);
    maxRow = newRow;
    maxCol = newCol;
    return remap;
}

/**
 * TODO: Student implement this function
 * Checks if the player can move in the specified direction and performs the move if so.
 * Cannot move out of bounds or onto TILE_PILLAR or TILE_MONSTER.
 * Cannot move onto TILE_EXIT without at least one treasure. 
 * If TILE_TREASURE, increment treasure by 1.
 * Remember to update the map tile that the player moves onto and return the appropriate status.
 * You can use the STATUS constants defined in logic.h to help!
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object to by reference to see current location.
 * @param   nextRow     Player's next row on the dungeon map (up/down).
 * @param   nextCol     Player's next column on dungeon map (left/right).
 * @return  Player's movement status after updating player's position.
 * @update map contents, player
 */
int doPlayerMove(char** map, int maxRow, int maxCol, Player& player, int nextRow, int nextCol) {

    if(nextRow >= maxRow || nextCol >= maxCol || nextRow < 0 || nextCol < 0 || map[nextRow][nextCol] == TILE_PILLAR || map[nextRow][nextCol] == TILE_MONSTER) {
        nextRow = player.row;
        nextCol = player.col;
        return STATUS_STAY;
    }
    else if(map[nextRow][nextCol] == TILE_OPEN) {
        map[player.row][player.col] = TILE_OPEN;
        map[nextRow][nextCol] = TILE_PLAYER;
        player.row = nextRow;
        player.col = nextCol;
        return STATUS_MOVE;
    }
    else if(map[nextRow][nextCol] == TILE_TREASURE) {
        map[player.row][player.col] = TILE_OPEN;
        map[nextRow][nextCol] = TILE_PLAYER;
        player.row = nextRow;
        player.col = nextCol;
        player.treasure += 1;
        return STATUS_TREASURE;
    }
    else if(map[nextRow][nextCol] == TILE_AMULET) {
        map[player.row][player.col] = TILE_OPEN;
        map[nextRow][nextCol] = TILE_PLAYER;
        player.row = nextRow;
        player.col = nextCol;
        return STATUS_AMULET;
    }
    else if(map[nextRow][nextCol] == TILE_DOOR) {
        map[player.row][player.col] = TILE_OPEN;
        map[nextRow][nextCol] = TILE_PLAYER;
        player.row = nextRow;
        player.col = nextCol;
        return STATUS_LEAVE;
    }
    else if(map[nextRow][nextCol] == TILE_EXIT) {
        if(player.treasure >= 1) {
            map[player.row][player.col] = TILE_OPEN;
            map[nextRow][nextCol] = TILE_PLAYER;
            player.row = nextRow;
            player.col = nextCol;
            return STATUS_ESCAPE;
        }
        else {
            nextRow = player.row;
            nextCol = player.col;
            return STATUS_STAY;
        }
    }
    else
        return 0;
}

/**
 * TODO: Student implement this function
 * Update monster locations:
 * We check up, down, left, right from the current player position.
 * If we see an obstacle, there is no line of sight in that direction, and the monster does not move.
 * If we see a monster before an obstacle, the monster moves one tile toward the player.
 * We should update the map as the monster moves.
 * At the end, we check if a monster has moved onto the player's tile.
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object by reference for current location.
 * @return  Boolean value indicating player status: true if monster reaches the player, false if not.
 * @update map contents
 */
bool doMonsterAttack(char** map, int maxRow, int maxCol, const Player& player) {

    for(int x = player.row - 1; x >= 0; x--) {
        if(map[x][player.col] == TILE_PILLAR)
            break;
        if(map[x][player.col] == TILE_MONSTER) {
            map[x][player.col] = TILE_OPEN;
            map[x + 1][player.col] = TILE_MONSTER;
        }
    }
    for(int x = player.row + 1; x < maxRow; x++) {
        if(map[x][player.col] == TILE_PILLAR)
            break;
        if(map[x][player.col] == TILE_MONSTER) {
            map[x][player.col] = TILE_OPEN;
            map[x - 1][player.col] = TILE_MONSTER;
        }
    }
    for(int x = player.col - 1; x >= 0; x--) {
        if(map[player.row][x] == TILE_PILLAR)
            break;
        if(map[player.row][x] == TILE_MONSTER) {
            map[player.row][x] = TILE_OPEN;
            map[player.row][x + 1] = TILE_MONSTER;
        }
    }
    for(int x = player.col + 1; x < maxCol; x++) {
        if(map[player.row][x] == TILE_PILLAR)
            break;
        if(map[player.row][x] == TILE_MONSTER) {
            map[player.row][x] = TILE_OPEN;
            map[player.row][x - 1] = TILE_MONSTER;
        }
    }
    if(map[player.row][player.col] == TILE_MONSTER)
            return true;
    return false;
}
