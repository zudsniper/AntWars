//
// Created by Jason McElhenney (NET-ID: JPM190001) on 9/18/19.
//

#ifndef ANTWARS_BEETLE_H
#define ANTWARS_BEETLE_H


#include "Creature.h"

class Beetle : public Creature {
private:
    int food;
public:
    Beetle(char displayCharacter, bool modified);
    virtual ~Beetle() = default;

    Direction move(std::map<Direction, std::pair<int, int>> cardinals) override;

    Direction breed(std::map<Direction, bool> cardinals) override;

    Direction moveNoAnts(std::map<Direction, int> cardinals);

    bool starve();

    int getFood() const;

    void setFood(int food);
};


#endif //ANTWARS_BEETLE_H
