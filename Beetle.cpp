//
// Created by Jason McElhenney (NET-ID: JPM190001) on 9/18/19.
//

#include <vector>
#include <algorithm>
#include "Beetle.h"

Beetle::Beetle(char displayCharacter, bool modified):
        Creature::Creature(displayCharacter, modified, 8) {
    food = 5;
}

Direction Beetle::move(std::map<Direction, std::pair<int, int>> cardinals) {
    Direction backwards[4] = {WEST, SOUTH, EAST, NORTH}; //backwards to make sure that if multiple ants closest, the north one is chosen
    int closest = 10000;
    int antNeighbors = -10000;
    Direction direction = NONE;
    //find closest ant
    for(auto dir : backwards) {
        int current = cardinals[dir].first;
        if(current < closest && current>0) {
            closest = current;
            direction = dir;
            antNeighbors = cardinals[direction].second; //so antNeighbors has current winner
            //if this ant is the same distance, check if it has more neighbors
        } else if(current == closest) {
            int currentAntNeighbors = cardinals[dir].second;
            if(currentAntNeighbors >= antNeighbors) {
                antNeighbors = currentAntNeighbors;
                direction = dir;
            }
        }
    }
    return direction;
}

Direction Beetle::moveNoAnts(std::map<Direction, int> cardinals) {
    Direction order[4] = {WEST, SOUTH, EAST, NORTH}; //backwards to maintain priority order
    int farthest = cardinals[WEST];
    Direction direction = NONE;
    //find largest distance to edge
    for(auto dir : order) {
        int current = cardinals[dir];
        if(current >= farthest) {
            farthest = current;
            direction = dir;
        }
    }
    return direction;
}

Direction Beetle::breed(std::map<Direction, bool> cardinals) {
    if(turnsSurvived % turnsToBreed == 0) {
        Direction order[4] = {NORTH, EAST, SOUTH, WEST};
        //find an empty space in NESW order
        for (auto dir : order) {
            if (cardinals[dir]) {
                return dir;
            }
        }
    }
    return NONE;
}

bool Beetle::starve() {
    return food < 1;
}


int Beetle::getFood() const {
    return food;
}

void Beetle::setFood(int food) {
    Beetle::food = food;
}
