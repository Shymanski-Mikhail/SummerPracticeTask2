#include "gameengine.h"
#include <cmath>
#include <cstdlib>
#include <QList>

GameEngine::GameEngine(QObject *parent)
    : QObject(parent)
    , gameTimer(new QTimer(this))
    , playerCar(nullptr)
    , lanesCount(2)
    , isNight(false)
    , twoWayMode(false)
    , score(0)
    , distance(0)
    , spawnTimerCounter(0)
    , roadOffset(0)
    , lastPlayerLane(-1)
    , laneStayTicks(0)
    , gameActive(false)
    , scene(nullptr) {
    connect(gameTimer, &QTimer::timeout, this, &GameEngine::gameTick);
}

GameEngine::~GameEngine() {
    stopGame();
    for (CarItem* bot : botCars) {
        delete bot;
    }
    botCars.clear();
}

void GameEngine::startGame(int numLanes, bool nightMode, bool isTwoWay, CarItem* player) {
    stopGame();
    reset();

    lanesCount = numLanes;
    isNight = nightMode;
    twoWayMode = isTwoWay;
    playerCar = player;
    gameActive = true;

    if (playerCar) {
        lastPlayerLane = playerCar->currentLane;
    }

    gameTimer->start(16);
}

void GameEngine::stopGame() {
    gameTimer->stop();
    gameActive = false;
}

void GameEngine::reset() {
    for (CarItem* bot : botCars) {
        delete bot;
    }
    botCars.clear();

    score = 0;
    distance = 0;
    spawnTimerCounter = 0;
    roadOffset = 0;
    lastPlayerLane = -1;
    laneStayTicks = 0;
    playerCar = nullptr;
}

void GameEngine::setScene(QGraphicsScene* scene) {
    this->scene = scene;
}

qreal GameEngine::getLaneX(int laneIndex) const {
    qreal startX = (SCENE_WIDTH - ROAD_WIDTH) / 2;
    qreal laneWidth = ROAD_WIDTH / lanesCount;
    return startX + (laneIndex * laneWidth) + (laneWidth / 2);
}

bool GameEngine::isIncomingLane(int laneIndex) const {
    if (!twoWayMode) return false;
    if (lanesCount == 2) return (laneIndex == 0);
    if (lanesCount == 3) return (laneIndex == 0);
    if (lanesCount == 4) return (laneIndex < 2);
    return false;
}

void GameEngine::spawnBot() {
    if (!gameActive || !playerCar) return;

    QList<QColor> carColors = {Qt::red, Qt::blue, Qt::green, Qt::yellow, Qt::cyan, Qt::magenta};
    int randomLane = rand() % lanesCount;

    int occupiedLanes = 0;
    for (int l = 0; l < lanesCount; ++l) {
        for (CarItem* bot : botCars) {
            if (bot->currentLane == l && bot->y() > -100 && bot->y() < 300) {
                occupiedLanes++;
                break;
            }
        }
    }

    bool canSpawn = (occupiedLanes < lanesCount - 1);
    int playerLane = playerCar->currentLane;
    bool laneLeftFree = (playerLane > 0);
    bool laneRightFree = (playerLane < lanesCount - 1);

    for (CarItem* bot : botCars) {
        if (bot->y() > -100 && bot->y() < 300) {
            if (bot->currentLane == playerLane - 1) laneLeftFree = false;
            if (bot->currentLane == playerLane + 1) laneRightFree = false;
        }
    }

    bool isSpawnLaneBusy = false;
    for (CarItem* bot : botCars) {
        if (bot->currentLane == randomLane && bot->y() > -100 && bot->y() < 300) {
            isSpawnLaneBusy = true;
        }
    }

    if (isSpawnLaneBusy) {
        if (laneLeftFree) randomLane = playerLane - 1;
        else if (laneRightFree) randomLane = playerLane + 1;
        else canSpawn = false;
    }

    if (canSpawn) {
        bool isIncoming = (twoWayMode && isIncomingLane(randomLane));
        CarItem *bot = new CarItem(false, isIncoming);
        bot->currentLane = randomLane;
        bot->setNightMode(isNight);
        bot->setBodyColor(carColors[rand() % carColors.size()]);

        qreal difficultySpeedMultiplier = std::fmin((score / 500) * 0.5, 7);

        if (isIncoming) {
            bot->speedY = 3.0 + (rand() % 2) + difficultySpeedMultiplier;
        } else {
            bot->speedY = 1.0 + (rand() % 2) + difficultySpeedMultiplier;
        }

        bot->setPos(getLaneX(randomLane), -150);

        if (scene) {
            scene->addItem(bot);
        }

        botCars.append(bot);
    }
}

void GameEngine::updateBots(qreal roadSpeed) {
    for (CarItem* bot : botCars) {
        qreal speedOnScreen = 0;

        if (twoWayMode && isIncomingLane(bot->currentLane)) {
            speedOnScreen = roadSpeed + bot->speedY;
        } else {
            speedOnScreen = roadSpeed - bot->speedY;
            if (speedOnScreen < 0) speedOnScreen = 0;
        }

        bot->setY(bot->y() + speedOnScreen);
        bot->updatePhysics();
    }
}

void GameEngine::cleanupBots() {
    for (int i = botCars.size() - 1; i >= 0; --i) {
        if (botCars[i]->y() > 600 || botCars[i]->y() < -300) {
            CarItem *toDelete = botCars[i];
            botCars.removeAt(i);

            if (scene) {
                scene->removeItem(toDelete);
            }

            delete toDelete;
        }
    }
}

void GameEngine::checkCollisions() {
    if (!playerCar) return;

    for (CarItem* bot : botCars) {
        if (playerCar->collidesWithItem(bot)) {
            gameActive = false;
            gameTimer->stop();
            emit gameOver();
            return;
        }
    }
}

void GameEngine::gameTick() {
    if (!gameActive || !playerCar) return;

    distance++;
    if (distance % 10 == 0) score += 10;

    qreal currentRoadSpeed = 6.0 + (score / 400.0);
    roadOffset += currentRoadSpeed;

    if (roadOffset > 1000000.0) {
        roadOffset = fmod(roadOffset, 240.0);
    }

    playerCar->updatePhysics();

    spawnTimerCounter++;
    if (spawnTimerCounter > 45) {
        spawnTimerCounter = 0;
        spawnBot();
    }

    updateBots(currentRoadSpeed);
    checkCollisions();
    cleanupBots();

    if (scene) {
        scene->update();
    }

    emit scoreUpdated(score);
}
