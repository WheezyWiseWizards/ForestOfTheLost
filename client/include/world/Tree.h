//
// Created by Winter on 01/10/2022.
//

#ifndef LD51_CLIENT_BUSH_H
#define LD51_CLIENT_BUSH_H


#include "HidingSpot.h"
#include "Solid.h"

class Tree : public HidingSpot, public Solid {
public:
    Tree(World& world, const sf::Vector2i& position);

    bool isBlocking(sf::Vector2i vec);
};


#endif //LD51_CLIENT_BUSH_H
