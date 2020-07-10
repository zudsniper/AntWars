//
// Created by Jason McElhenney (NET-ID: JPM190001) on 9/11/19.
//

#include <vector>
#include <algorithm>
#include "Ant.h"

Ant::Ant(char displayCharacter, bool modified):
        Creature::Creature(displayCharacter, modified, 3) {
}

Direction Ant::move(std::map<Direction, std::pair<int, int>> cardinals) {
    Direction forwards[4] = {NORTH, EAST, SOUTH, WEST};
    Direction backwards[4] = {WEST, SOUTH, EAST, NORTH};
    int closest = 10000;
    std::vector<Direction> awayFromClosestDirs;
    //find closest displacements of beetles and add the opposite of their directions to awayFromClosestDirs
    for(auto dir : backwards) {
        int current = cardinals[dir].first;
        if(current < closest && current > 0) {
            closest = current;
            awayFromClosestDirs.clear();
            awayFromClosestDirs.push_back(Creature::getOpposite(dir));
        } else if(current == closest) {
            awayFromClosestDirs.push_back(Creature::getOpposite(dir));
        }
    }

    //no orthogonal beetles
    if(awayFromClosestDirs.empty()) {
        return NONE;
    }

    if(awayFromClosestDirs.size() == 1) { //only one closest beetle, return away from it
        if(cardinals[awayFromClosestDirs[0]].first >= 0) { //make sure not out of bounds
            return awayFromClosestDirs[0];
        }
        return NONE;
    } else { //multiple
        int farthest = -10000;
        Direction furthestBeetleDirection = NONE;
        for(auto dir : forwards) { //in priority direction
            if(find(awayFromClosestDirs.begin(), awayFromClosestDirs.end(), dir) != awayFromClosestDirs.end()) { //awayFromClosestDirs contains this direction
                int current = cardinals[dir].first;
                if(current == 0) { //no beetle in this direction, also not facing an edge (-1)
                    return dir;
                } else if(current > 0) { //beetle this way
                    if(current > farthest) {
                        farthest = current;
                        furthestBeetleDirection = dir;
                    }
                }
            }
        }
        return furthestBeetleDirection;
    }
}

Direction Ant::breed(std::map<Direction, bool> cardinals) {
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
