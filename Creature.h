//
// Created by Jason McElhenney (NET-ID: JPM190001) on 9/11/19.
//

#ifndef ANTWARS_CREATURE_H
#define ANTWARS_CREATURE_H
#include <map>
#include <utility>

enum Direction {NORTH, SOUTH, EAST, WEST, NONE};

class Creature {
private:
    char displayCharacter;
protected:
    int turnsSurvived;
    bool modified;
    int turnsToBreed;
public:
    Creature() {
        turnsSurvived = 0;
        turnsToBreed = 0;
        displayCharacter = ' ';
        modified = false;
    }
    Creature(char displayCharacter, bool modified, int turnsToBreed) {
        Creature::turnsSurvived = 0;
        Creature::turnsToBreed = turnsToBreed;
        Creature::displayCharacter = displayCharacter;
        Creature::modified = modified;
    }
    virtual ~Creature() = default;

    //pair first is distance to Creature in direction, second is number of adjacent ants
    virtual Direction move(std::map<Direction, std::pair<int, int>> cardinals) = 0;
    virtual Direction breed(std::map<Direction, bool> cardinals) = 0;

    static Direction getOpposite(Direction direction) {
        switch(direction) {
            case NORTH: return SOUTH;
            case SOUTH: return NORTH;
            case EAST: return WEST;
            case WEST: return EAST;
            default: return NONE;
        }
    }

//    Direction getDistancesToCreatures(int r, int c, vector<Direction> order, Direction (*f)(int, int, Creature));
//    Direction getDistancesToCreatures(int r, int c, Direction (*f)(int, int, Creature));

    void survived() { turnsSurvived++; }

    char getDisplayCharacter() const {
        return displayCharacter;
    }

    void setDisplayCharacter(char displayCharacter) {
        Creature::displayCharacter = displayCharacter;
    }

    int getTurnsToBreed() const {
        return turnsToBreed;
    }

    void setTurnsToBreed(int turnsToBreed) {
        Creature::turnsToBreed = turnsToBreed;
    }

    int getTurnsSurvived() const {
        return turnsSurvived;
    }

    void setTurnsSurvived(int turnsSurvived) {
        Creature::turnsSurvived = turnsSurvived;
    }

    bool isModified() const {
        return modified;
    }

    void setModified(bool modified) {
        Creature::modified = modified;
    }

};


#endif //ANTWARS_CREATURE_H
