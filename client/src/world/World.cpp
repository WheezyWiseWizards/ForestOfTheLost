//
// Created by Winter on 01/10/2022.
//

#include <world/TeddyBear.h>
#include <random>
#include <world/Bat.h>
#include <world/Wraith.h>
#include <world/GroundHog.h>
#include <world/Ghoul.h>
#include <iostream>
#include <world/Beholder.h>
#include <world/Skull.h>
#include <world/monster_kid/MonsterKidHealed.h>
#include "world/World.h"
#include "SFML/System/Vector2.hpp"
#include "util/SimplexNoise.h"
#include "GameAssets.h"
#include "world/Bush.h"
#include "world/Monster.h"
#include "world/Solid.h"
#include "world/Tree.h"
#include "world/EndGoal.h"
#include "world/NPC.h"
#include "world/TeddyKid.h"
#include "world/Snake.h"
#include "world/HotGhostMom.h"
#include "world/Ball.h"
#include "world/monster_kid/MonsterKidMonster.h"
#include "world/Orb.h"

World::World(wiz::AssetLoader& assets, DialogBox& dialogBox)
		: assets(assets),
		  player(*this),
		  terrainMap(),
          entityMap(),
		  terrain_textures(),
		  dialogBox(dialogBox),
		  monsters(),
		  gameOverSound() {
	terrain_textures[TerrainType::SHORT_GRASS] = assets.get(GameAssets::SHORT_GRASS_TERRAIN);
	terrain_textures[TerrainType::GRASS] = assets.get(GameAssets::GRASS_TERRAIN);
	terrain_textures[TerrainType::WATER] = assets.get(GameAssets::WATER_TERRAIN);
	terrain_textures[TerrainType::SAND] = assets.get(GameAssets::SAND_TERRAIN);

	AHHH_SOUND.setBuffer(*assets.get(GameAssets::AHHH_SOUND));
    AHHH_SOUND.setVolume(50);
	gameOverSound.setBuffer(*assets.get(GameAssets::GAMEOVER));

	// srand(20201002);
	generatePhase(GamePhase::INITIAL);
}

void World::spawnEnemy(GamePhase phase, sf::Vector2i position) {

	double val = rand() * 1.0 / RAND_MAX;

	Monster* monster;

	if(phase == INITIAL) {
		if(val < 0.5)
			monster = new Snake(*this, position);
		else
			monster = new GroundHog(*this, position);
	} else if(phase == FIRST_ENCOUNTER) {
		if (val < 0.2)
			monster = new Wraith(*this, position);
		else
			monster = new Bat(*this, position);
	} else if(phase == GHOST) {
		bool hasGhostMom = false;

		for(Monster* current : monsters)
			if(dynamic_cast<HotGhostMom*>(current))
				hasGhostMom = true;

        sf::Vector2i diff = position - cryingGirlPos;

        int distance = (int) sqrt(pow(diff.x, 2) + pow(diff.y, 2));

        if (MIN_HOT_GIRL_GHOST_SPAWN_RADIUS > distance)
            hasGhostMom = true; // Don't spawn ghost mom within this radius

		if(!hasGhostMom && hotGhostMomsCanSpawn) {
			monster = new HotGhostMom(*this, position);hasGhostMom = true;
		} else {
			if(val < 0.5)
				monster = new Wraith(*this, position);
			else
				monster = new Ghoul(*this, position);
		}

	} else if(phase == MONSTER) {
		bool hasMonsterKid = false;

		for(Monster* current : monsters)
			if(dynamic_cast<MonsterKidMonster*>(current))
				hasMonsterKid = true;

		if(!hasMonsterKid && monsterKidCanSpawn) {
			monster = new MonsterKidMonster(*this, position);
		} else {
			if (val < 0.5)
				monster = new Skull(*this, position);
			else
				monster = new Beholder(*this, position);
		}
	} else if(phase == FINAL) {
        monster = new Orb(*this, position);
    } else {
		return;
	}

	addEntity(monster);
	monsters.push_back(monster);
}

void World::generatePhase(GamePhase phase) {
    currentPhase = phase;

    double offsetX = rand() * 10.0 / RAND_MAX;
    double offsetY = rand() * 10.0 / RAND_MAX;

    if (phase == GHOST) {
        for (TeddyKid *teddyKid : teddyKids) {
            removeEntity(teddyKid);
        }

        for (int i = -200; i <= 200; i++) {
            for (int j = -200; j <= 200; j++) {
                if (i == 0 && j == 0)
                    continue;

                double nx = i / 400.0 - 0.5 + offsetX;
                double ny = j / 400.0 - 0.5 + offsetY;

                nx *= 5.0;
                ny *= 5.0;

                double noise = SimplexNoise::noise(nx, ny);

                if (noise > -0.75 && (player.getPosition() - sf::Vector2i{i, j}).lengthSq() > 10.0 * 10.0 &&
                    sf::Vector2i(i, j).lengthSq() > 10.0 * 10.0) {
                    double nx2 = i / 400.0 - 0.5 + offsetX * 9.0;
                    double ny2 = j / 400.0 - 0.5 + offsetY * 9.0;

                    nx2 *= 5000.0;
                    ny2 *= 5000.0;

                    double noise2 = SimplexNoise::noise(nx2, ny2);

                    int chunkX = i / 20;
                    int chunkY = j / 20;

                    int x = (int) abs(SimplexNoise::noise(chunkX / 100.0 - 400.0, chunkY / 100.0 - 400.0)) % 20;
                    int y = (int) abs(SimplexNoise::noise(chunkX / 100.0 - 400.0, chunkY / 100.0 - 400.0)) % 20;

                    if (i % 20 == x && y == j % 20) {

                        CryingGirl *sir_dick = new CryingGirl(*this, sf::Vector2i(i, j));
                        addEntity(sir_dick);
                        cryingGirls.push_back(sir_dick);
                    }
                }
            }
        }

        return;
    }

#ifdef ENTITY_DEBUG
    for(int i = 0; i < entities.size(); i++) {
        for(int j = 0; j < entities.size(); j++) {
            if(i != j && entities[i] == entities[j])
                std::cout << "FUCK!!!" << std::endl;
        }
    }
#endif

    std::vector<Entity *> copy = entities;

    entities.clear();
    entityMap.clear();
    monsters.clear();
    teddyKids.clear();
    cryingGirls.clear();
	balls.clear();

    for (Entity *entity : copy)
        if (entity != &player)
            delete entity;

    addEntity(&player);
    player.teleport({0, 0});
    terrainMap.clear();

    for (int i = -210; i <= 210; i++) {
        for (int j = -210; j <= 210; j++) {
            double nx = i / 400.0 - 0.5 + offsetX;
            double ny = j / 400.0 - 0.5 + offsetY;

            nx *= 5.0;
            ny *= 5.0;

            double noise = SimplexNoise::noise(nx, ny);

            if (sf::Vector2i{i, j}.lengthSq() <= 10.0 * 10.0)
                noise += (100 - sf::Vector2i{i, j}.lengthSq()) / 50.0;

			if((i <= -200 || i >= 200) || (j <= -200 || j >= 200)) {
				terrainMap[sf::Vector2i(i, j)] = TerrainType::WATER;
				continue;
			} else if((i <= -195 || i >= 195) || (j <= -195 || j >= 195)) {
				terrainMap[sf::Vector2i(i, j)] = TerrainType::SAND;
				continue;
			}

            if (noise < -0.75)
                terrainMap[sf::Vector2i(i, j)] = TerrainType::WATER;
            else if (noise < -0.7)
                terrainMap[sf::Vector2i(i, j)] = TerrainType::SAND;
            else if (noise > 1.5f)
                terrainMap[sf::Vector2i(i, j)] = TerrainType::GRASS;

            if (i == 0 && j == 0)
                continue;

            if (noise > -0.75) {
                double nx2 = i / 400.0 - 0.5 + offsetX * 9.0;
                double ny2 = j / 400.0 - 0.5 + offsetY * 9.0;

                nx2 *= 5000.0;
                ny2 *= 5000.0;

                double noise2 = SimplexNoise::noise(nx2, ny2);

                double prob = rand() * 1.0 / RAND_MAX;

                if (noise2 > 0.9) {
                    TreeType type;
                    if (phase == GamePhase::INITIAL) {
                        if (prob < 0.95)
                            type = TreeType::ALIVE;
                        else
                            type = TreeType::SQUIRREL;
                    } else if (phase == GamePhase::FIRST_ENCOUNTER) {
                        if (prob < 0.4) {
                            type = TreeType::ALIVE;
                        } else if (prob < 0.9) {
                            type = TreeType::DEAD;
                        } else {
                            type = TreeType::THICK_DEAD;
                        }
                    } else {
                        if (prob < 0.9) {
                            type = TreeType::DEAD;
                        } else {
                            type = TreeType::THICK_DEAD;
                        }
                    }

                    for (int o = -1; o <= 1; o++)
                        for (int p = -1; p <= 1; p++)
                            for (Entity *entity : getEntitiesAt({i + o, j + p}))
                                if (dynamic_cast<Tree *>(entity))
                                    goto notree;

                    addEntity(new Tree(*this, {i, j}, type));
                    for (int o = -1; o <= 1; o++)
                        for (int p = -1; p <= 1; p++)
                            if (!terrainMap.contains(sf::Vector2i(i + o, j + p)))
                                terrainMap[sf::Vector2i(i + o, j + p)] = TerrainType::GRASS;
                    notree:;
                } else if (noise2 > 0.7) {

                    BushType type;
                    if (phase == GamePhase::INITIAL) {
                        if (prob < 0.5)
                            type = BushType::BUSH;
                        else
                            type = BushType::BUSH2;
                    } else if (phase == GamePhase::FIRST_ENCOUNTER) {
                        if (prob < 0.25) {
                            type = BushType::BUSH;
                        } else if (prob < 0.5) {
                            type = BushType::BUSH2;
                        } else if (prob < 0.75) {
                            type = BushType::WITHERED_BUSH;
                        } else {
                            type = BushType::WITHERED_BUSH2;
                        }
                    } else {
                        if (prob < 0.5)
                            type = BushType::WITHERED_BUSH;
                        else
                            type = BushType::WITHERED_BUSH2;
                    }


                    for (int o = -1; o <= 1; o++)
                        for (int p = -1; p <= 1; p++)
                            for (Entity *entity : getEntitiesAt({i + o, j + p}))
                                if (dynamic_cast<Bush *>(entity))
                                    goto nobush;

                    addEntity(new Bush(*this, {i, j}, type));
                    for (int o = -1; o <= 1; o++)
                        for (int p = -1; p <= 1; p++)
                            if (!terrainMap.contains(sf::Vector2i(i + o, j + p)))
                                terrainMap[sf::Vector2i(i + o, j + p)] = TerrainType::GRASS;
                    nobush:;
                } else {
                    if (phase == GamePhase::INITIAL) {
                        if ((player.getPosition() - sf::Vector2i{i, j}).lengthSq() > 10.0 * 10.0 &&
                            sf::Vector2i(i, j).lengthSq() > 10.0 * 10.0) {
                            int chunkX = i / 20;
                            int chunkY = j / 20;

                            int x = (int) abs(SimplexNoise::noise(chunkX / 100.0, chunkY / 100.0)) % 20;
                            int y = (int) abs(SimplexNoise::noise(chunkX / 100.0, chunkY / 100.0)) % 20;

                            if (i % 20 == x && y == j % 20) {

                                Entity *teddy_bear = new TeddyBear(*this, sf::Vector2i(i, j));
                                addEntity(teddy_bear);
                            }
                        }
                    } else if (phase == GamePhase::FIRST_ENCOUNTER) {
                        if ((player.getPosition() - sf::Vector2i{i, j}).lengthSq() > 10.0 * 10.0 &&
                            sf::Vector2i(i, j).lengthSq() > 10.0 * 10.0) {
                            int chunkX = i / 20;
                            int chunkY = j / 20;

                            int x = (int) abs(SimplexNoise::noise(chunkX / 100.0 - 200.0, chunkY / 100.0 - 200.0)) % 20;
                            int y = (int) abs(SimplexNoise::noise(chunkX / 100.0 - 200.0, chunkY / 100.0 - 200.0)) % 20;

                            if (i % 20 == x && y == j % 20) {

                                TeddyKid *sir_dick = new TeddyKid(*this, sf::Vector2i(i, j));
                                addEntity(sir_dick);
                                teddyKids.push_back(sir_dick);
                            }
                        }
                    } else if (phase == GamePhase::MONSTER) {
						if ((player.getPosition() - sf::Vector2i{i, j}).lengthSq() > 10.0 * 10.0 &&
							sf::Vector2i(i, j).lengthSq() > 10.0 * 10.0) {
							int chunkX = i / 20;
							int chunkY = j / 20;

							int x = (int) abs(SimplexNoise::noise(chunkX / 100.0 - 200.0, chunkY / 100.0 - 200.0)) % 20;
							int y = (int) abs(SimplexNoise::noise(chunkX / 100.0 - 200.0, chunkY / 100.0 - 200.0)) % 20;

							if (i % 20 == x && y == j % 20) {

								Ball *ball = new Ball(*this, sf::Vector2i(i, j));
								addEntity(ball);
								balls.push_back(ball);
							}
						}

                        setCheckPoint = true;
					}
                }
            }

        }
    }

    if (phase == GamePhase::INITIAL) {
		grayscaleness = 0.0;
		scan_effect = 0.0;
	} else if(phase == GamePhase::FIRST_ENCOUNTER) {
		grayscaleness = 0.5;
		scan_effect = 0.5;
	} else {
		grayscaleness = 1.0;
		scan_effect = 1.0;
	}
}

void World::hotGhostMomInteraction(CryingGirl* cryingGirl, HotGhostMom* hotGhostMom) {
    changePhaseIn(2);
    resetAccumulator();

    auto pos = std::find(monsters.begin(), monsters.end(), hotGhostMom);
    if (pos == monsters.end()) {
        throw std::runtime_error("Tried to removing an hotGhostMom not in monster vector (get fucked idiot)");
    }
    monsters.erase(pos);
    removeEntity(cryingGirl);
    delete cryingGirl;
    removeEntity(hotGhostMom);
    delete hotGhostMom;
}

TerrainType World::getTerrainType(sf::Vector2i position) const {
	if(terrainMap.contains(position))
		return terrainMap.at(position);
	return TerrainType::SHORT_GRASS;
}

const std::vector<Entity*>& World::getEntities() const {
	return entities;
}

const Player& World::getPlayer() const {
	return player;
}

Player& World::getPlayer() {
	return player;
}

wiz::AssetLoader& World::getAssets() {
	return assets;
}

bool World::tileOccupied(sf::Vector2i tile, Entity* exclude) {

	if(terrainMap[tile] == WATER)
		return true;

	int solid_range = 1;

	for(int i = -solid_range; i <= solid_range; i++) {
		for(int j = -solid_range; j <= solid_range; j++) {
			for(Entity* entity : entityMap[tile + sf::Vector2i{i, j}]) {
                if (entity == exclude)
                    continue;

				Solid* solid = dynamic_cast<Solid*>(entity);

				if(solid && solid->isBlocking(tile)) {
                    EndGoal *endGoal = dynamic_cast<EndGoal *>(solid);
                    if (endGoal)
                        endPointReached = true;

                    return true;
                }
			}
		}
	}

	return false;
}

int enemyCountForPhase(GamePhase phase) {
	switch (phase) {
		case INITIAL:
			return 4;
		case FIRST_ENCOUNTER:
			return 4;
		case GHOST:
			return 6;
		case MONSTER:
			return 8;
		case FINAL:
			return 10;
	}
	throw std::runtime_error("UNKNOWN PHASE");
}

void World::tick(float delta) {
#ifdef ENTITY_DEBUG
	for(Entity* entity : entities) {
		if(!entityMap.contains(entity->getPosition()))
			throw std::runtime_error("CANCER entity found not to be correct in map (NO VECTOR) (moved without moveEntity)?");
		if(std::find(entityMap[entity->getPosition()].begin(), entityMap[entity->getPosition()].end(), entity) == entityMap[entity->getPosition()].end())
			throw std::runtime_error("CANCER entity found not to be correct in map (moved without moveEntity)?");
	}
#endif

    if (healedKid) {
        removeEntity(healedKid);
        delete healedKid;
        healedKid = nullptr;
        changePhaseIn(2);
        timePaused = false;
    }

    getPlayer().setLockMovement(isTimePaused());

	if(!isTimePaused()) {
		timeAccumulator += delta;
		tenSecAccumulator += delta;

		if(tenSecAccumulator > 9250.0 || tenSecAccumulator < 500.0)
			getPlayer().setLockMovement(true);
		else
			getPlayer().setLockMovement(false);

		if(tenSecAccumulator > 9250.0 && !entitiesStartMoving) {
			for(int i = 0; i < getEntities().size(); i++) {
				if(Monster* monster = dynamic_cast<Monster*>(getEntities().at(i)))
					// Don't want to go to the same bush
					monster->findNewSpot();
			}
			entitiesStartMoving = true;
		}

		if(tenSecAccumulator > 10000.0) {
			if(countBlinkBeforePhaseChange != -1) {
				countBlinkBeforePhaseChange--;
				if(countBlinkBeforePhaseChange == -1 && currentPhase < FINAL) {
					generatePhase(static_cast<GamePhase>(currentPhase + 1));
					if(currentPhase != FIRST_ENCOUNTER)
                    	player.setHeartBeatDelay(player.getHeartBeatDelay() / 2);
                    switch (currentPhase) {
                        case FIRST_ENCOUNTER:
                            dialogBox.startDialog({
                                 "What?...",
                                 "Where am I?...",
                             });
                            break;
                        case MONSTER:
                            dialogBox.startDialog({
                                  "Not Again! No!",
                                  "God almighty...",
                                  "What are these things?...",
                              });
                            AHHH_SOUND.play();
                            break;
                        case FINAL:
                            dialogBox.startDialog({
                                  "The illusion is broken...",
                                  "Now I see the true face of these monsters...",
                                  "These entities...",
                                  "Am I stuck here forever?",
                                  "[The mystery is solved but you are trapped in this ethereal plane for eternity]",
                                  "[Thank you for playing!]",
                                  "[Game by: Adrien, Alexander, Cedric and William]"
                            });
                            break;
                    };
				}
			}

			tenSecAccumulator = fmod(tenSecAccumulator, 10000.0f);
			entitiesStartMoving = false;
		}
	}

    if (setCheckPoint) {
        playerCheckpointPos = player.getPosition();
        setCheckPoint = false;
    }

    if (loadCheckPoint) {
        sf::Vector2i oldPos = player.getPosition();
        player.setPosition(playerCheckpointPos);
        player.setDestination(playerCheckpointPos);
        moveEntity(oldPos, &player);
        loadCheckPoint = false;
    }


	int len = monsters.size();
	for(int i = 0; i < len; i++) {
		Monster* monster = monsters[i];
		if((monster->getPosition() - getPlayer().getPosition()).lengthSq() > 30.0 * 30.0) {
			removeEntity(monster);
			std::cout << "Despawning monster" << std::endl;
			monsters[i] = monsters[len - 1];
			monsters.erase(monsters.end() - 1);
			delete monster;
			len--;
			i--;
		}
	}

	int monstersVisible = 0;

	for(Monster* monster : monsters) {
		sf::Vector2i diff = monster->getPosition() - getPlayer().getPosition();
		if(diff.x > -VIEW_SIZE.x / 2.0
		&& diff.x < VIEW_SIZE.x / 2.0
		&& diff.y > -VIEW_SIZE.y / 2.0
		&& diff.y < VIEW_SIZE.y / 2.0) {
			monstersVisible++;
		}
	}


	while(monstersVisible < enemyCountForPhase(currentPhase) && (tenSecAccumulator > 9250.0 || tenSecAccumulator < 500.0)) {
		sf::Vector2i position = { 0, 0 };

		do {
			position = getPlayer().getPosition() + sf::Vector2i {rand() % 20 - 10, rand() % 20 - 10 };
		} while(tileOccupied(position, nullptr) || (position - getPlayer().getPosition()).lengthSq() < 6.0 * 6.0);

		spawnEnemy(getPhase(), position);
		last_monster_spawn = std::chrono::system_clock::now();
		monstersVisible++;
	}


	for (Entity *entity : entities)
        entity->tick(delta);
}

const std::vector<Entity *> &World::getEntitiesAt(sf::Vector2i position) const {
    return entityMap.contains(position) ? entityMap.at(position) : empty;
}

void World::addEntity(Entity* entity) {
#ifdef ENTITY_DEBUG
	for(auto it = entities.begin(); it != entities.end(); it++)
		if(*it == entity)
			throw std::invalid_argument("Attempt to add an entity that is already there FUCKTARD");
#endif

    entities.push_back(entity);
    if (entityMap.contains(entity->getPosition()))
        entityMap[entity->getPosition()].push_back(entity);
    else
        entityMap[entity->getPosition()] = {entity};
}

void World::moveEntity(sf::Vector2i oldPosition, Entity *entity) {
    auto removePos = std::find(entityMap[oldPosition].begin(), entityMap[oldPosition].end(), entity);

	if(removePos == entityMap[oldPosition].end())
		throw std::runtime_error("Tried to move an entity not in entity map (may pain permeates into your suffering)");

	entityMap[oldPosition].erase(removePos);

	if (entityMap.contains(entity->getPosition()))
		entityMap[entity->getPosition()].push_back(entity);
	else
		entityMap[entity->getPosition()] = {entity};
}

void World::removeEntity(Entity* entity) {
	if(!entityMap.contains(entity->getPosition()))
		throw std::runtime_error("Entity map doesn't contain position to remove at");

	auto removePos = std::find(entityMap[entity->getPosition()].begin(),
							   entityMap[entity->getPosition()].end(), entity);

	if(removePos == entityMap[entity->getPosition()].end())
	    throw std::runtime_error("Tried to removing an entity not in entity map (skill issue)");

	entityMap[entity->getPosition()].erase(removePos);

#ifdef ENTITY_DEBUG
	if(std::find(entityMap[entity->getPosition()].begin(),
				 entityMap[entity->getPosition()].end(), entity) != entityMap[entity->getPosition()].end())
		throw std::runtime_error("Found entity to be inside entity map after successful removal");
#endif

	removePos = std::find(entities.begin(), entities.end(), entity);

	if(removePos == entities.end())
	    throw std::runtime_error("Tried to remove an entity not in entities vector");

	entities.erase(removePos);
}

void World::draw(sf::RenderTarget& target, const sf::RenderStates& states) const {
	sf::Vector2f viewSize = VIEW_SIZE;
	sf::Vector2i start = getPlayer().getPosition() - sf::Vector2i(static_cast<int>(ceil(viewSize.x / 2.0f)),
																  static_cast<int>(ceil(viewSize.y / 2.0f))) - sf::Vector2i{3, 3};
	sf::Vector2i end = getPlayer().getPosition() + sf::Vector2i(static_cast<int>(floor(viewSize.x / 2.0f)),
																static_cast<int>(floor(viewSize.y / 2.0f))) + sf::Vector2i{3, 3};

	for(int i = start.x; i <= end.x; i++) {
		for(int j = start.y; j <= end.y; j++) {
			terrain_sprite.setTexture(*terrain_textures.at(this->getTerrainType({i, j})), true);
			terrain_sprite.setPosition({static_cast<float>(i), -static_cast<float>(j)});
			terrain_sprite.setScale({1.0f / terrain_sprite.getTexture()->getSize().x,
									 1.0f / terrain_sprite.getTexture()->getSize().y});
			target.draw(terrain_sprite);
		}
	}

	for(Entity* entity : getEntities())
		if(entity->getPosition().x >= start.x
		   && entity->getPosition().y >= start.y
		   && entity->getPosition().x <= end.x
		   && entity->getPosition().y <= end.y)
			entityDrawList.push_back(entity);

	std::sort(entityDrawList.begin(), entityDrawList.end(), [](Entity* a, Entity* b) {
		return a->getRenderPosition().y > b->getRenderPosition().y
			|| a->getRenderPosition().y == b->getRenderPosition().y
			&& (a->getZOrder() < b->getZOrder()
			|| a->getZOrder() == b->getZOrder()
			&& a->getRenderPosition().x > b->getRenderPosition().x);
	});

	for(sf::Drawable* drawable : entityDrawList)
		target.draw(*drawable);
	entityDrawList.clear();
}

void World::handleMonsterAttack(Monster& monster) {

    if(MonsterKidMonster* monsterKid = dynamic_cast<MonsterKidMonster*>(&monster))
    {
        if(isGotBalls())
        {
            monsterKidCanSpawn = false;
            dialogBox.startDialog({"Monster Kid: Oh you have a ball! Let's play!"}, [&, monsterKid]{
                healedKid = new MonsterKidHealed(*this, monsterKid->getPosition());
                addEntity(healedKid);

                auto pos = std::find(monsters.begin(), monsters.end(), monsterKid);
                if (pos == monsters.end()) {
                    throw std::runtime_error("Tried to removing an monster not in monster vector (get fucked idiot)");
                }
                monsters.erase(pos);
                removeEntity(monsterKid);
                delete monsterKid;

                dialogBox.startDialog({
                    "Kid: I'm healed... Thanks for playing ball with me."
                }, [](){}); // Don't use this callback! It doesn't execute
            });
            return;
        }
    }

    // Terrible code pt. 1 (making wraiths visible when they are attacking you)
    if (monster.daySprite.getTexture() == getAssets().get(GameAssets::INVISIBLE))
        monster.daySprite.setTexture(*monster.nightSprite.getTexture());

    getPlayer().animateHit();
    dialogBox.startDialog(monster.getAttackMessage(), [&]{
        if (currentPhase != GamePhase::INITIAL) {
            loadCheckPoint = true;
            timePaused = false;

			for(Monster* monster : monsters) {
				removeEntity(monster);
				delete monster;
			}
			monsters.clear();
		}

        // Terrible code pt. 2
        if (monster.daySprite.getTexture() == monster.nightSprite.getTexture())
            monster.daySprite.setTexture(*getAssets().get(GameAssets::INVISIBLE));
    });
}

bool World::isEndPointReached() const {
    return endPointReached;
}

bool World::isTimePaused() const {
    return timePaused;
}

void World::setTimePaused(bool timePaused) {
    World::timePaused = timePaused;
}

void World::resetAccumulator() {
    tenSecAccumulator = 0;
    timeAccumulator = 0;
}

bool World::isSetCheckPoint() const {
    return setCheckPoint;
}

void World::setSetCheckPoint(bool setCheckPoint) {
    World::setCheckPoint = setCheckPoint;
}

const std::vector<TeddyKid*> &World::getTeddyKids() const {
    return teddyKids;
}

const std::vector<Monster*>& World::get_monsters() const {
	return monsters;
}

const std::vector<CryingGirl *> &World::getCryingGirls() const {
    return cryingGirls;
}

void World::shake(sf::Vector2i vec) {
    for(auto entity : entityMap[vec])
    {
        if(auto* hidingSpot = dynamic_cast<HidingSpot*>(entity))
        {
            hidingSpot->shake();
        }
    }
}

void World::removeBalls() {
	for(Ball* ball : balls) {
		removeEntity(ball);
		delete ball;
	}
	balls.clear();
}

bool World::isGotBalls() const {
    return got_balls;
}

void World::setGotBalls(bool gotBalls) {
    got_balls = gotBalls;
}
