#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include "Creature.h"
#include "Ant.h"
#include "Beetle.h"
using namespace std;

int const R = 10;
int const C = 10;

char const inputAntCharacter = 'a';
char const inputBeetleCharacter = 'B';

int const START_ON_TURN = 1; // default 1
bool const PROVIDE_INPUT = false;
bool const DEBUG_MODE = false;

//utility methods
void printBoard(Creature* board[R][C]);
void readMatrixFromFile(string& filename, char antChar, char beetleChar, Creature* board[R][C]);
pair<int, int> getCoordsToDirection(Direction direction, int r, int c);

//traversal methods
void moveCreature(Creature* board[R][C], int r, int c, Direction direction);
template <typename T>
map<Direction, pair<int, int>> getDistancesToCreatures(Creature* board[R][C], int r, int c, vector<Direction> const &order, bool skipNeighborAnts = true);
map<Direction, int> getDistancesToEdges(int r, int c);
map<Direction, bool> getEmptyCardinals(Creature* board[R][C], int r, int c);
int getNeighborAnts(Creature* board[R][C], int r, int c);


int main() {
    string filename;
    char antChar;
    char beetleChar;
    int turns = 0;

    vector<Direction> const clockwise = {NORTH, EAST, SOUTH, WEST};
    vector<Direction> const counterclockwise = {WEST, SOUTH, EAST, NORTH};

    Creature* board[R][C] = {{nullptr}};

    if(DEBUG_MODE) {
        Creature* board[R][C];
        string filename = "/Users/jason/CLionProjects/AntWars/world1.txt";
        readMatrixFromFile(filename,'a','B',board);
        cout << "**********DEBUG MODE ON**********" << endl;
        if(PROVIDE_INPUT) {
            cout << "WARNING: PROVIDE_INPUT is enabled." << endl;
        }

        printBoard(board);
    }

    if (!PROVIDE_INPUT) {
        //cout << "Enter Filename: ";
        cin >> filename;

        //cout << "Enter ant character: ";
        cin >> antChar;

        //cout << "Enter beetle character: ";
        cin >> beetleChar;

        //cout << "Enter number of turns: ";
        cin >> turns;
    } else {
        filename = "/Users/jason/CLionProjects/AntWars/world1.txt";
        antChar = 'a';
        beetleChar = 'B';
        turns = 1;
    }
    //parse beginning board setup from file
    readMatrixFromFile(filename, antChar, beetleChar, board);

    //starts at START_ON_TURN so turn index is correct and so turnsSurvived calculations with i will be accurate.
    for (int i = START_ON_TURN; i <= turns; i++) {
        if(DEBUG_MODE) {
            cout << "BEGINNING OF TURN " << i << endl << endl;
        } else {
            cout << "TURN " << i << endl;
        }

        if(DEBUG_MODE) {
            cout << "\033[44;30mBEETLES MOVE\033[0m (TURN " << i << ")" << endl << endl;
        }
        //1: BEETLES MOVE
        //iterate column first (can't be a jagged matrix!!)
        for (int c = 0; c < C; c++) {
            for (int r = 0; r < R; r++) {
                if(board[r][c]!=nullptr && !board[r][c]->Creature::isModified()) { //make sure creatures don't move multiple times
                    if (dynamic_cast<Beetle *>(board[r][c])) {
                        Beetle *currentBeetle = (Beetle *) board[r][c];
                        currentBeetle->Creature::setModified(true);
                        //increase turnsSurvived and decrease food
                        currentBeetle->survived();
                        currentBeetle->setFood(currentBeetle->getFood() - 1);
                        map<Direction, pair<int, int>> tempMap = getDistancesToCreatures<Ant *>(board, r, c, counterclockwise, false);
                        Direction dir = currentBeetle->move(tempMap);
                        if (dir == NONE) {
                            dir = currentBeetle->moveNoAnts(getDistancesToEdges(r, c));
                        }
                        moveCreature(board, r, c, dir);
                        if(DEBUG_MODE) {
                            printBoard(board);
                        }
                    }
                }
            }
        }

        if(DEBUG_MODE) {
            cout << "\033[43;30mANTS MOVE\033[0m (TURN " << i << ")" << endl << endl;
        }
        //2: ANTS MOVE
        //iterate column first (can't be a jagged matrix!!)
        for (int c = 0; c < C; c++) {
            for (int r = 0; r < R; r++) {
                if(board[r][c]!=nullptr && !board[r][c]->Creature::isModified()) { //make sure creatures don't move multiple times
                    if (dynamic_cast<Ant *>(board[r][c])) {
                        Ant *currentAnt = (Ant *) board[r][c];
                        currentAnt->Creature::setModified(true);
                        currentAnt->survived();
                        map<Direction, pair<int,int>> tempMap = getDistancesToCreatures<Beetle *>(board, r, c, counterclockwise);
                        Direction dir = currentAnt->move(tempMap);
                        moveCreature(board, r, c, dir);
                        if(DEBUG_MODE) {
                            printBoard(board);
                        }
                    }
                }
            }
        }

        //reset modified for all for breeding
        for(int r = 0; r<R; r++) {
            for(int c = 0; c<C; c++) {
                if(board[r][c]!=nullptr && dynamic_cast<Creature*>(board[r][c])) {
                    board[r][c]->Creature::setModified(false);
                }
            }
        }

        if(DEBUG_MODE) {
            cout << "\033[41;30mBEETLES STARVE\033[0m (TURN " << i << ")" << endl << endl;
        }
        //3: BEETLES STARVE
        //iterate column first (can't be a jagged matrix!!)
        for (int c = 0; c < C; c++) {
            for (int r = 0; r < R; r++) {
                if (dynamic_cast<Beetle *>(board[r][c])) {
                    Beetle *currentBeetle = (Beetle *) board[r][c];
                    if (currentBeetle->starve()) {
                        board[r][c] = nullptr;
                        delete currentBeetle;
                    }
                    if(DEBUG_MODE) {
                        printBoard(board);
                    }
                }
            }
        }

        if(DEBUG_MODE) {
            cout << "\033[33;40mANTS BREED\033[0m (TURN " << i << ")" << endl << endl;
        }
        //4: ANTS BREED
        //iterate column first (can't be a jagged matrix!!)
        for (int c = 0; c < C; c++) {
            for (int r = 0; r < R; r++) {
                if(board[r][c]!=nullptr && !board[r][c]->Creature::isModified()) { //make sure not null and haven't breed this turn
                    if (dynamic_cast<Ant *>(board[r][c])) {
                        Ant *currentAnt = (Ant *) board[r][c];
                        currentAnt->Creature::setModified(true);
                        Direction dir = currentAnt->breed(getEmptyCardinals(board, r, c));
                        if(DEBUG_MODE) {
                            cout << "\033[7mpos(" << r << ", " << c << ")\033[0m" << endl;
                        }
                        if (dir != NONE) {
                            //get coords for new ant and create dynamic object
                            pair<int, int> myNewCoords = getCoordsToDirection(dir, r, c);
                            int newR = myNewCoords.first;
                            int newC = myNewCoords.second;
                            board[newR][newC] = new Ant(antChar, true);
                            if(DEBUG_MODE) {
                                cout << "\033[7;4mnew pos(" << newR << ", " << newC << ")\033[0m" << endl << endl;
                            }
                        }
                        if (DEBUG_MODE) {
                            printBoard(board);
                        }
                    }
                }
            }
        }

        if(DEBUG_MODE) {
            cout << "\033[34;40mBEETLES BREED\033[0m (TURN " << i << ")" << endl << endl;
        }
        //5: BEETLES BREED
        //iterate column first (can't be a jagged matrix!!)
        for (int c = 0; c < C; c++) {
            for (int r = 0; r < R; r++) {
                if(board[r][c]!=nullptr && !board[r][c]->Creature::isModified()) { //make sure not null and haven't breed this turn
                    if (dynamic_cast<Beetle *>(board[r][c])) {
                        Beetle *currentBeetle = (Beetle *) board[r][c];
                        currentBeetle->Creature::setModified(true);
                        Direction dir = currentBeetle->breed(getEmptyCardinals(board, r, c));
                        if(DEBUG_MODE) {
                            cout << "\033[7mpos(" << r << ", " << c << ")\033[0m" << endl;
                        }
                        if (dir != NONE) {
                            //get coords for new ant and create dynamic object
                            pair<int, int> myNewCoords = getCoordsToDirection(dir, r, c);
                            int newR = myNewCoords.first;
                            int newC = myNewCoords.second;
                            board[newR][newC] = new Beetle(beetleChar, true);
                            if(DEBUG_MODE) {
                                cout << "\033[7;4mnew pos(" << newR << ", " << newC << ")\033[0m" << endl << endl;
                            }
                        }
                        if (DEBUG_MODE) {
                            printBoard(board);
                        }
                    }
                }
            }
        }

        //reset modified for all for breeding
        for(int r = 0; r<R; r++) {
            for(int c = 0; c<C; c++) {
                if(board[r][c]!=nullptr && dynamic_cast<Creature*>(board[r][c])) {
                    board[r][c]->Creature::setModified(false);
                }
            }
        }

        if(DEBUG_MODE) {
            cout << "************TURN " << i << " FINAL START************" << endl;
        }
        printBoard(board);
        if(DEBUG_MODE) {
            cout << "*************TURN " << i << " FINAL END*************" << endl;
        }
    }

    //delete all dynamic objs
    for(int r = 0; r<R; r++) {
        for(int c = 0; c<C; c++) {
            if(board[r][c] != nullptr) {
                delete board[r][c];
            }
        }
    }

    return 0;
}

/**
 * Moves a Creature. If the creature is a beetle, it will eat ants it lands on. (WARNING: Does not validate movement direction!)
 * @param board
 * @param r
 * @param c
 * @param direction
 */
void moveCreature(Creature* board[R][C], int const r, int const c, Direction direction) {

    if(direction == NONE) {
        return;
    }

    pair<int, int> myNewCoords = getCoordsToDirection(direction, r, c);
    int newR = myNewCoords.first;
    int newC = myNewCoords.second;

    //if a beetle moves onto an ant, feed the beetle and remove the ant
    if(dynamic_cast<Beetle*>(board[r][c]) && dynamic_cast<Ant*>(board[newR][newC])) {
        Beetle* beetle = (Beetle*)board[r][c];
        Ant* ant = (Ant*)board[newR][newC];
        delete ant;
        beetle->setFood(5);
    } else { //if any other creature tries to move onto another, no movement
        if(board[newR][newC]) {
            return;
        }
    }
    // move creature
    if((newR != r) || (newC != c)) {
        board[newR][newC] = board[r][c];
        board[r][c] = nullptr;
    }
}

/**
 * get the number of adjacent and orthogonal ant neighbors
 * @param board
 * @param r
 * @param c
 * @return
 */
int getNeighborAnts(Creature* board[R][C], int const r, int const c) {
    int ants = 0;
    //iterate the square from r-1, c-1 to r+1, c+1
    for(int row = -1; row <= 1; row++) {
        for(int col = -1; col <= 1; col++) {
            int newRow = r+row;
            int newCol = c+col;
            if(newCol != c || newRow != r) { //don't count actual position as neighbor
                if ((newRow >= 0 && newRow < R) && (newCol >= 0 && newCol < C)) { //don't go out of bounds
                    if (dynamic_cast<Ant *>(board[newRow][newCol])) {
                        ants++;
                    }
                }
            }
        }
    }
    return ants;
}

/**
 * return a map of the cardinal directions and a boolean indicating if that direction is empty. True is empty.
 * @param board
 * @param r
 * @param c
 * @return
 */
map<Direction, bool> getEmptyCardinals(Creature* board[R][C], int r, int c) {
    vector<Direction> order = {NORTH, EAST, SOUTH, WEST};
    map<Direction, bool> myMap;
    if(r==0) {
        myMap[NORTH] = false;
    } else {
        myMap[NORTH] = board[r-1][c]==nullptr;
    }
    if(c==(C-1)) {
        myMap[EAST] = false;
    } else {
        myMap[EAST] = board[r][c+1]==nullptr;
    }
    if(r==(R-1)) {
        myMap[SOUTH] = false;
    } else {
        myMap[SOUTH] = board[r+1][c]==nullptr;
    }
    if(c==0) {
        myMap[WEST] = false;
    } else {
        myMap[WEST] = board[r][c-1]==nullptr;
    }
    return myMap;
}

/**
 * Gets the distance to edges (WARNING: only called when no ants found in path, so just uses position to build map fast.)
 * @param board
 * @param r
 * @param c
 * @param order
 * @return
 */
map<Direction, int> getDistancesToEdges(int r, int c) {
    map<Direction, int> myMap;
    myMap[NORTH] = r;
    myMap[EAST] = C-c;
    myMap[SOUTH] = R-r;
    myMap[WEST] = c;
    return myMap;
}

/**
 * returns a map with the 4 cardinal directions as keys and values of pairs consisting of the distance from the closest of type T
 * in board in the key direction and (if told to calculate it) the number of neighbor ants around this closest T in direction
 * @tparam T type of creature to check for
 * @param board
 * @param r
 * @param c
 * @param order
 * @param skipNeighborAnts
 * @return
 */
template <typename T>
map<Direction, std::pair<int, int>> getDistancesToCreatures(Creature* board[R][C], int r, int c, vector<Direction> const &order, bool skipNeighborAnts) {
    map<Direction, std::pair<int, int>> myMap;
    for(auto dir : order) {
        switch(dir) {
            case NORTH: {
                std::pair<int, int> indexPair;
                if(r==0) {
                    indexPair.first = -1;
                } else {
                    for(int i = 0; i<r; i++) { // iterate forwards so we get the closest one from the north (top)
                        if(dynamic_cast<T>(board[i][c])) { // if it's the type that is specified by the template
                            indexPair.first = r-i;
                            if(!skipNeighborAnts) {
                                indexPair.second = getNeighborAnts(board, i, c);
                            }
                        }
                    }
                }
                myMap[NORTH] = indexPair;
            } break;
            case EAST: {
                std::pair<int, int> indexPair;
                if(c==(C-1)) {
                    indexPair.first = -1;
                } else {
                    for(int i = C-1; i>c; i--) { // iterate backwards so we get the closest one from the east (right)
                        if(dynamic_cast<T>(board[r][i])) { // if it's the type that is specified by the template
                            indexPair.first = i-c; //- 1;
                            if(!skipNeighborAnts) {
                                indexPair.second = getNeighborAnts(board, r, i);
                            }
                        }
                    }
                }
                myMap[EAST] = indexPair;
            } break;
            case SOUTH: {
                std::pair<int, int> indexPair;
                if(r==(R-1)) {
                    indexPair.first = -1;
                } else {
                    for(int i = R-1; i>r; i--) { // iterate backwards so we get the closest one from the south (bottom)
                        if(dynamic_cast<T>(board[i][c])) { // if it's the type that is specified by the template
                            indexPair.first = i-r; //- 1;
                            if(!skipNeighborAnts) {
                                indexPair.second = getNeighborAnts(board, i, c);
                            }
                        }
                    }
                }
                myMap[SOUTH] = indexPair;
            } break;
            case WEST: {
                std::pair<int, int> indexPair;
                if(c==0) {
                    indexPair.first = -1;
                } else {
                    for(int i = 0; i<c; i++) { // iterate forwards so we get the closest one from the west (left)
                        if(dynamic_cast<T>(board[r][i])) { // if it's the type that is specified by the template
                            indexPair.first = c-i;
                            if(!skipNeighborAnts) {
                                indexPair.second = getNeighborAnts(board, r, i);
                            }
                        }
                    }
                }
                myMap[WEST] = indexPair;
            } break;
            default: {
                //do nothing
            } break;
        }
    }
    if(DEBUG_MODE) {
        cout << "\033[7mpos(" << r << ", " << c << ")\033[0m" << endl;
        cout << "NORTH: " << myMap[NORTH].first << ", " << myMap[NORTH].second << endl;
        cout << "EAST: " << myMap[EAST].first << ", " << myMap[EAST].second << endl;
        cout << "SOUTH: " << myMap[SOUTH].first << ", " << myMap[SOUTH].second << endl;
        cout << "WEST: " << myMap[WEST].first << ", " << myMap[WEST].second << endl << endl;
    }
    return myMap;
}

/**
 * Return the r,c coordinates one unit in a cardinal direction. WARNING: doesn't validate the position.
 * @param r
 * @param c
 * @return
 */
pair<int, int> getCoordsToDirection(Direction direction, int r, int c) {
    int newR = r;
    int newC = c;
    switch(direction) {
        case NORTH: {
            newR = r-1;
            newC = c;
        } break;
        case EAST: {
            newR = r;
            newC = c+1;
        } break;
        case SOUTH: {
            newR = r+1;
            newC = c;
        } break;
        case WEST: {
            newR = r;
            newC = c-1;
        } break;
        default: {
            //don't move
        } break;
    }
    std::pair<int, int> coordPair;
    coordPair.first = newR;
    coordPair.second = newC;
    return coordPair;
}

/**
 * Reads in the starting condition matrix from a file.
 * @param filename
 * @param antChar
 * @param beetleChar
 * @param board
 */
void readMatrixFromFile(string& filename, char antChar, char beetleChar, Creature* board[R][C]) {
    ifstream in;
    in.open(filename);
    string line;
    int lineIndex = 0;

    while(!in.eof()) {
        getline(in, line);
        for(unsigned int i = 0; i<line.length(); i++) { //i functions as column index
            char current = line.at(i);
            if(current == inputAntCharacter) {
                board[lineIndex][i] = new Ant(antChar, false);
            } else if(current == inputBeetleCharacter) {
                board[lineIndex][i] = new Beetle(beetleChar, false);
            }
        }
        lineIndex++; // functions as row index
    }
    in.close();
}

/**
 * Prints the board to cout with spaces representing nullptr. DEBUG_MODE adds lots of convenience features.
 * @param board
 */
void printBoard(Creature* board[R][C]) {

    for(int r = 0; r<R; r++) {
        if(DEBUG_MODE) {
            cout << r << "  ";
        }
        for(int c = 0; c<C; c++) {
            if(board[r][c]) {
                if(DEBUG_MODE) {
                    char display = board[r][c]->getDisplayCharacter();
                    if(display == 'a') {
                        cout << "\033[33m" << "a" << "\033[0m"; // make ants yellow
                    } else if(display == 'B') {
                        cout << "\033[34m" << "B" << "\033[0m"; // make beetles blue
                    }
                } else {
                    cout << board[r][c]->getDisplayCharacter();
                }
            } else {
                if(DEBUG_MODE) {
                    cout << c;
                } else {
                    cout << " ";
                }
            }
        }
        cout << endl;
    }
    cout << endl;
}