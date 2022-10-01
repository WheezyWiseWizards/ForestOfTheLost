//
// Created by Winter on 01/10/2022.
//

#include "TopDownScreen.h"
#include "GameAssets.h"
#include "world/HidingSpot.h"

TopDownScreen::TopDownScreen(wiz::Game& game)
		: Screen(game), world(game.getAssets()), terrain_textures() {
}

void TopDownScreen::tick(float delta) {
    timeAccumulator += delta;

    processInput();

    world.tick(delta);
}

void TopDownScreen::processInput() {
    bool eastPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)
                        || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)
                        || sf::Joystick::isButtonPressed(0, 0);

    bool northPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)
                        || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)
                        || sf::Joystick::isButtonPressed(0, 1);

    bool westPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)
                        || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)
                        || sf::Joystick::isButtonPressed(0, 2);

    bool southPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)
                        || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)
                        || sf::Joystick::isButtonPressed(0, 2);

    if (eastPressed && !westPressed) {
        world.getPlayer().move(EAST);
    } else if (northPressed && !southPressed) {
        world.getPlayer().move(NORTH);
    } else if (westPressed && !eastPressed) {
        world.getPlayer().move(WEST);
    } else if (southPressed && !northPressed) {
        world.getPlayer().move(SOUTH);
    }
}

void TopDownScreen::render(sf::RenderTarget& target) {
	sf::Vector2f viewSize = {16.0f, 9.0f};

	frameBuffer.create(1280, 720);
	frameBuffer.clear();
	frameBuffer.setView(sf::View({ world.getPlayer().getRenderPosition().x + 0.5f, world.getPlayer().getRenderPosition().y + 0.5f }, viewSize));
	target.clear();

	sf::Vector2i start = world.getPlayer().getPosition() - sf::Vector2i(static_cast<int>(ceil(viewSize.x / 2.0f)),
																		 static_cast<int>(ceil(viewSize.y / 2.0f))) - sf::Vector2i{1,1};
	sf::Vector2i end = world.getPlayer().getPosition() + sf::Vector2i(static_cast<int>(floor(viewSize.x / 2.0f)),
																	  static_cast<int>(floor(viewSize.y / 2.0f))) + sf::Vector2i{1,1};
	for(int i = start.x; i <= end.x; i++) {
		for(int j = start.y; j <= end.y; j++) {
            terrain_sprite.setTexture(*terrain_textures[world.getTerrainType({i, j})]);
			terrain_sprite.setPosition({static_cast<float>(i), static_cast<float>(j)});
			terrain_sprite.setScale({1.0f / terrain_sprite.getTexture()->getSize().x, 1.0f / terrain_sprite.getTexture()->getSize().y});
            frameBuffer.draw(terrain_sprite);
		}
	}

	for(Entity* entity : world.getEntities())
		frameBuffer.draw(*entity);

	spookyShader->setUniform("timeAccumulator", timeAccumulator);
	frameBuffer.display(); // done drawing fbo
	sf::Sprite fbo(frameBuffer.getTexture());
	target.clear();
	target.draw(fbo, spookyShader);
}

void TopDownScreen::show() {
    sf::Vector2f viewSize = {16.0f, 9.0f};
	getGame().addWindowListener(this);
	getGame().addInputListener(this);

    spookyShader = getAssets().get(GameAssets::SPOOKY_SHADER);
	terrain_textures[TerrainType::GRASS] = getGame().getAssets().get(GameAssets::GRASS_TERRAIN);
	terrain_textures[TerrainType::WATER] = getGame().getAssets().get(GameAssets::WATER_TERRAIN);
	terrain_textures[TerrainType::SAND] = getGame().getAssets().get(GameAssets::SAND_TERRAIN);

    sf::Sprite* hiding_spot_sprite = new sf::Sprite(*getGame().getAssets().get(GameAssets::HIDING_SPOT));
    hiding_spot_sprite->setScale({1.0f / hiding_spot_sprite->getTexture()->getSize().x, 1.0f / hiding_spot_sprite->getTexture()->getSize().y});
    Entity* hiding_spot = new HidingSpot(world, *new sf::Vector2i(3, 3), *hiding_spot_sprite);

    world.getEntities().push_back(hiding_spot);
}

void TopDownScreen::hide() {
	getGame().removeWindowListener(this);
	getGame().removeWindowListener(this);
}

const std::string& TopDownScreen::getName() const {
	return name;
}

void TopDownScreen::windowClosed() {
	getGame().getWindow().close();
}