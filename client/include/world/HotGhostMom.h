//
// Created by william on 02/10/22.
//

#ifndef LD51_CLIENT_HOTGHOSTMOM_H
#define LD51_CLIENT_HOTGHOSTMOM_H

#include "Monster.h"

class HotGhostMom : public Monster {
    const int MIN_RANGE_TO_PLAYER = 3;
    const int MAX_RANGE_TO_PLAYER = 10;

    float searchGirlRadius = 7;

    bool ghostMomReachCryingGirl = false;

    bool playerTargeted = false;

    CryingGirl* cryingGirl = nullptr;

public:
    HotGhostMom(World &world, sf::Vector2i position);

    void tick(float delta) override;

    void targetPlayerInRange() override;

    void moveTowardsPlayer() override;

    void findNewSpot() override;

    void draw(sf::RenderTarget& target, const sf::RenderStates& states) const override;

    void drawDarkness(sf::RenderTarget& target) const override;

    void checkForCryingGirl();
};

#endif //LD51_CLIENT_HOTGHOSTMOM_H
