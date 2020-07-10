//
// Created by Jason McElhenney (NET-ID: JPM190001) on 9/11/19.
//

#ifndef ANTWARS_ANT_H
#define ANTWARS_ANT_H


#include "Creature.h"

class Ant : public Creature {
public:
    Ant(char displayCharacter, bool modified);
    virtual ~Ant() = default;

    Direction move(std::map<Direction, std::pair<int, int>> cardinals) override;
    Direction breed(std::map<Direction, bool> cardinals) override;

};


#endif //ANTWARS_ANT_H
