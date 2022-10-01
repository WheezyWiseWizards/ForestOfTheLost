//
// Created by adrien on 01/10/22.
//

#ifndef LD51_CLIENT_HIDINGSPOT_H
#define LD51_CLIENT_HIDINGSPOT_H


#include "Entity.h"
#include "SFML/Graphics/Sprite.hpp"
#include "Direction.h"

class HidingSpot : public Entity {
    sf::Vector2i position;
    sf::Sprite sprite;
public:
    sf::Vector2i getPosition() const ;
    sf::Sprite getSprite() const;
    void tick(float delta) override;
    void draw(sf::RenderTarget& target, const sf::RenderStates& states) const override;
    HidingSpot(World &world, sf::Vector2i position, sf::Sprite sprite);
};

#endif //LD51_CLIENT_HIDINGSPOT_H
