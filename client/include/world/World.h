//
// Created by Winter on 01/10/2022.
//

#ifndef LD51_CLIENT_WORLD_H
#define LD51_CLIENT_WORLD_H

#include <vector>
#include <unordered_map>
#include "Terrain.h"
#include "SFML/System/Vector2.hpp"
#include "Player.h"
#include "WIZ/asset/AssetLoader.h"
#include "DialogBox.h"
#include "TeddyKid.h"
#include "CryingGirl.h"


class Entity;
class Player;
class Monster;
class HotGhostMom;
class Ball;
class MonsterKidHealed;

enum GamePhase {
	INITIAL,
	FIRST_ENCOUNTER,
	GHOST,
	MONSTER,
	FINAL
};

struct VecCompare {
	bool operator() (const sf::Vector2i& lhs, const sf::Vector2i& rhs) const {
		return lhs.x < rhs.x || lhs.x == rhs.x && lhs.y < rhs.y;
	}
};

class World : public Tickable, public sf::Drawable {
	wiz::AssetLoader& assets;
	std::vector<Entity*> entities;
	std::vector<Monster*> monsters;
    std::vector<TeddyKid*> teddyKids;
    std::vector<CryingGirl*> cryingGirls;
	std::vector<Ball*> balls;
	Player player;

	std::unordered_map<TerrainType, sf::Texture*> terrain_textures;
	mutable sf::Sprite terrain_sprite;

	std::map<sf::Vector2i, TerrainType, VecCompare> terrainMap;
    std::map<sf::Vector2i, std::vector<Entity*>, VecCompare> entityMap;
    std::vector<Entity*> empty = {};

	mutable std::vector<Entity*> entityDrawList = {};

    bool endPointReached = false;
    bool timePaused = false;
	bool entitiesStartMoving = false;

	float grayscaleness = 0.0f;
	float scan_effect = 0.0f;

	float timeAccumulator = 0.0f;
	float tenSecAccumulator = 0.0f;

	bool got_balls = false;

	bool monsterKidCanSpawn = true;

    MonsterKidHealed* healedKid = nullptr;
public:
    bool isGotBalls() const;

    void setGotBalls(bool gotBalls);
	
	sf::Sound gameOverSound;

private:

    GamePhase currentPhase = INITIAL;
	int countBlinkBeforePhaseChange = -1;

	DialogBox& dialogBox;

    bool setCheckPoint = false;
    bool loadCheckPoint = false;
	bool hotGhostMomsCanSpawn = false;
    sf::Vector2i playerCheckpointPos = {};

	void spawnEnemy(GamePhase phase, sf::Vector2i position);

	std::chrono::system_clock::time_point last_monster_spawn = std::chrono::system_clock::now();

	sf::Sound AHHH_SOUND;

    const int MIN_HOT_GIRL_GHOST_SPAWN_RADIUS = 20;
    sf::Vector2i cryingGirlPos = {};
public:
	constexpr const static sf::Vector2f VIEW_SIZE = { 24.0f, 13.5f };

	World(wiz::AssetLoader& assets, DialogBox& dialogBox);

	TerrainType getTerrainType(sf::Vector2i position) const;

	const std::vector<Entity*>& getEntities() const;

	const Player& getPlayer() const;

	Player& getPlayer();

	wiz::AssetLoader& getAssets();

    bool tileOccupied(sf::Vector2i tile, Entity *exclude);

    void tick(float delta) override;

    const std::vector<Entity*>& getEntitiesAt(sf::Vector2i position) const;

    void addEntity(Entity* entity);

    void moveEntity(sf::Vector2i oldPosition, Entity* entity);

	void removeEntity(Entity* entity);

    bool isEndPointReached() const;

	void removeBalls();

	inline float getGrayscaleness() const {
		return grayscaleness;
	}

	inline float getTimeAccumulator() const {
		return timeAccumulator;
	}

	inline float getScanEffect() const {
	    return scan_effect;
	}

	void draw(sf::RenderTarget& target, const sf::RenderStates& states) const override;

	inline void changePhaseIn(int countBlinks) {
		countBlinkBeforePhaseChange = countBlinks - 1;
	}

	inline DialogBox& getDialogBox() {
		return dialogBox;
	}

	inline bool isChangingPhase() const {
		return countBlinkBeforePhaseChange != -1;
	}

	inline bool isChangingPhaseNext() const {
		return countBlinkBeforePhaseChange == 0;
	}

	inline GamePhase getPhase() const {
		return currentPhase;
	}

    inline void setCryingGirlPos(sf::Vector2i pos) {
        cryingGirlPos = pos;
    }

    void handleMonsterAttack(Monster& monster);

    bool isTimePaused() const;

    void setTimePaused(bool timePaused);

    bool isSetCheckPoint() const;

    void setSetCheckPoint(bool setCheckPoint);

    void resetAccumulator();

	const std::vector<Monster*>& get_monsters() const;

    const std::vector<TeddyKid *> &getTeddyKids() const;

    const std::vector<CryingGirl *> &getCryingGirls() const;

    void shake(sf::Vector2i vec);

    void generatePhase(GamePhase phase);

    void hotGhostMomInteraction(CryingGirl* cryingGirl, HotGhostMom* hotGhostMom);

	inline void setHotGhostMomsCanSpawn() {
		hotGhostMomsCanSpawn = true;
	}

	inline void setHotGhostMomsCantSpawn() {
	    hotGhostMomsCanSpawn = false;
	}
};


#endif //LD51_CLIENT_WORLD_H
