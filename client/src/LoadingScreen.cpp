//
// Created by Winter on 12/02/2022.
//

#include <GameAssets.h>
#include "LoadingScreen.h"
#include "TitleScreen.h"
#include "TopDownScreen.h"

LoadingScreen::LoadingScreen(wiz::Game& game)
	: Screen(game) {}

void LoadingScreen::tick(float delta) {
	getGame().getAssets().update(16.0f);

	if(getGame().getAssets().isAllLoaded())
		getGame().setScreen(std::shared_ptr<TopDownScreen>(new TopDownScreen(getGame())));
}

void LoadingScreen::render(sf::RenderTarget& target) {

	target.clear(sf::Color(0, 0, 0, 255));

	float width = target.getView().getSize().x * 0.5f;
	float y = target.getView().getSize().y * 3.0f / 4.0f;

	sprite.setColor(sf::Color(128, 128, 128, 255));
	sprite.setPosition(sf::Vector2(target.getView().getCenter().x - width / 2.0f, y));
	sprite.setScale(sf::Vector2(width, 1.0f));
	target.draw(sprite);

	sprite.setColor(sf::Color(255, 255, 255, 255));
	sprite.setPosition(sf::Vector2(target.getView().getCenter().x - width / 2.0f, y));
	sprite.setScale(sf::Vector2(width * getAssets().getProgress(), 1.0f));
	target.draw(sprite);

	sf::FloatRect rect = message.getLocalBounds();

	message.setPosition(sf::Vector2f(target.getView().getCenter().x - rect.getSize().x / 2.0f,
						target.getView().getSize().y / 4.0f));
	target.draw(message);
}

void LoadingScreen::show() {

	getGame().getAssets().loadAll(GameAssets::ALL);
	getGame().getAssets().finishLoading(GameAssets::SANS_TTF);
	getGame().getAssets().finishLoading(GameAssets::WHITE_PIXEL);

	sprite.setTexture(*getGame().getAssets().get(GameAssets::WHITE_PIXEL));

#ifdef OS_WINDOWS
	message.setString("Forest of the Lost");
#endif
#ifdef OS_UNIX
	message.setString("Forest of the Lost");
#endif
#ifdef OS_MAC
	message.setString("Forest of the Lost");
#endif
#ifdef OS_SWITCH
	message.setString("Forest of the Lost");
#endif

	message.setFont(*getGame().getAssets().get(GameAssets::SANS_TTF));

	getGame().addWindowListener(this);
}

void LoadingScreen::hide() {
	getGame().removeWindowListener(this);
}

const std::string& LoadingScreen::getName() const {
	return name;
}

void LoadingScreen::windowClosed() {
	getGame().getWindow().close();
}
