#pragma once
#include <QObject>
#include <QVector>
#include <QTimer>
#include <QGraphicsScene>
#include "caritem.h"

class GameEngine : public QObject {
    Q_OBJECT
public:
    explicit GameEngine(QObject *parent = nullptr);
    ~GameEngine();

    void startGame(int numLanes, bool nightMode, bool isTwoWay, CarItem* player);
    void stopGame();
    void gameTick();
    void reset();

    int getScore() const { return score; }
    int getDistance() const { return distance; }
    bool isGameRunning() const { return gameTimer->isActive(); }
    QVector<CarItem*>& getBotCars() { return botCars; }
    qreal getRoadOffset() const { return roadOffset; }

    void setScene(QGraphicsScene* scene);

    qreal getLaneX(int laneIndex) const;
    bool isIncomingLane(int laneIndex) const;

signals:
    void scoreUpdated(int score);
    void gameOver();

private:
    void spawnBot();
    void updateBots(qreal roadSpeed);
    void cleanupBots();
    void checkCollisions();

    QTimer *gameTimer;
    CarItem *playerCar = nullptr;
    QVector<CarItem*> botCars;
    QGraphicsScene* scene;

    int lanesCount;
    bool isNight;
    bool twoWayMode;
    int score;
    int distance;
    int spawnTimerCounter;
    qreal roadOffset;
    int lastPlayerLane;
    int laneStayTicks;
    bool gameActive;

    static constexpr qreal ROAD_WIDTH = 300.0;
    static constexpr qreal SCENE_WIDTH = 400.0;
    static constexpr qreal SCENE_HEIGHT = 600.0;
};
