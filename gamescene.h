#pragma once
#include <QGraphicsScene>
#include <QTimer>
#include <QVector>
#include "gameengine.h"

class CarItem;

class GameScene : public QGraphicsScene {
    Q_OBJECT
public:
    GameScene(QObject *parent = nullptr);
    ~GameScene();

    void startNewGame(int numLanes, bool nightMode, bool isTwoWay);
    void stopGame();
    CarItem* getPlayer() const { return playerCar; }

signals:
    void scoreUpdated(int score);
    void gameOver();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void drawBackground(QPainter *painter, const QRectF &rect) override;

private slots:
    void onScoreUpdated(int score);
    void onGameOver();

private:
    qreal getLaneX(int laneIndex);
    void setupPlayer(int lanesCount, bool isNight);

    GameEngine *engine;
    CarItem *playerCar = nullptr;
    int lanesCount;
    bool isNight;
    bool twoWayMode;
};
