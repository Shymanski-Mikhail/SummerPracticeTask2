#include "gamescene.h"
#include "caritem.h"
#include <QKeyEvent>
#include <QPainter>
#include <cstdlib>
#include <cmath>

GameScene::GameScene(QObject *parent)
    : QGraphicsScene(parent)
    , engine(new GameEngine(this))
    , playerCar(nullptr)
    , lanesCount(2)
    , isNight(false)
    , twoWayMode(false) {
    setSceneRect(0, 0, 400, 600);

    engine->setScene(this);

    connect(engine, &GameEngine::scoreUpdated, this, &GameScene::onScoreUpdated);
    connect(engine, &GameEngine::gameOver, this, &GameScene::onGameOver);
}

GameScene::~GameScene() {
    stopGame();
}

void GameScene::startNewGame(int numLanes, bool nightMode, bool isTwoWay) {
    stopGame();

    lanesCount = numLanes;
    isNight = nightMode;
    twoWayMode = isTwoWay;

    setupPlayer(numLanes, nightMode);
    engine->startGame(numLanes, nightMode, isTwoWay, playerCar);
}

void GameScene::stopGame() {
    engine->stopGame();

    for (CarItem* bot : engine->getBotCars()) {
        removeItem(bot);
    }

    if (playerCar) {
        removeItem(playerCar);
        delete playerCar;
        playerCar = nullptr;
    }
}

void GameScene::setupPlayer(int numLanes, bool nightMode) {
    if (playerCar) {
        removeItem(playerCar);
        delete playerCar;
        playerCar = nullptr;
    }

    playerCar = new CarItem(true, false);
    playerCar->setNightMode(nightMode);
    playerCar->currentLane = numLanes - 1;
    addItem(playerCar);
    playerCar->setPos(getLaneX(playerCar->currentLane), 480);
}

qreal GameScene::getLaneX(int laneIndex) {
    qreal roadWidth = 300;
    qreal startX = (400 - roadWidth) / 2;
    qreal laneWidth = roadWidth / lanesCount;
    return startX + (laneIndex * laneWidth) + (laneWidth / 2);
}

void GameScene::keyPressEvent(QKeyEvent *event) {
    if (!playerCar || playerCar->isChangingLane) return;

    if (event->key() == Qt::Key_Left || event->key() == Qt::Key_A) {
        if (playerCar->currentLane > 0) {
            playerCar->currentLane--;
            playerCar->setTargetX(getLaneX(playerCar->currentLane));
        }
    }
    else if (event->key() == Qt::Key_Right || event->key() == Qt::Key_D) {
        if (playerCar->currentLane < lanesCount - 1) {
            playerCar->currentLane++;
            playerCar->setTargetX(getLaneX(playerCar->currentLane));
        }
    }
}

void GameScene::drawBackground(QPainter *painter, const QRectF &rect) {
    Q_UNUSED(rect);

    painter->fillRect(0, 0, 400, 600, QColor(35, 110, 35));

    qreal roadWidth = 300.0;
    qreal startX = (400.0 - roadWidth) / 2.0;
    qreal endX = startX + roadWidth;

    painter->fillRect(startX, 0, roadWidth, 600, QColor(50, 50, 50));

    qreal roadOffset = engine ? engine->getRoadOffset() : 0.0;

    int pillarStep = 240;
    int pillarOffset = static_cast<int>(roadOffset) % pillarStep;
    if (pillarOffset < 0) pillarOffset += pillarStep;

    qreal startPillarY = pillarOffset - pillarStep;

    for (qreal pillarY = startPillarY; pillarY < 900.0; pillarY += pillarStep) {
        painter->fillRect(QRectF(startX - 14.0, pillarY, 6.0, 25.0), QColor(245, 245, 245));
        painter->fillRect(QRectF(startX - 14.0, pillarY + 8.0, 6.0, 8.0), QColor(210, 30, 30));

        painter->fillRect(QRectF(endX + 8.0, pillarY, 6.0, 25.0), QColor(245, 245, 245));
        painter->fillRect(QRectF(endX + 8.0, pillarY + 8.0, 6.0, 8.0), QColor(210, 30, 30));
    }

    qreal laneWidth = roadWidth / lanesCount;
    int dashStep = 60;
    int dashOffset = static_cast<int>(roadOffset) % dashStep;
    if (dashOffset < 0) dashOffset += dashStep;

    qreal startDashY = dashOffset - dashStep;

    for (int i = 1; i < lanesCount; ++i) {
        qreal xPos = startX + i * laneWidth;
        bool isCenterLine = false;
        if (twoWayMode) {
            int centerLaneIndex = lanesCount / 2;
            if (i == centerLaneIndex) {
                isCenterLine = true;
            }
        }

        if (isCenterLine) {
            QPen pen(QColor(230, 175, 20), 3.0, Qt::SolidLine);
            painter->setPen(pen);
            painter->drawLine(QLineF(xPos, 0, xPos, 600));
        } else {
            QPen pen(Qt::white, 2.0, Qt::SolidLine);
            painter->setPen(pen);

            for (qreal currentY = startDashY; currentY < 700.0; currentY += dashStep) {
                painter->drawLine(QLineF(xPos, currentY, xPos, currentY + 30.0));
            }
        }
    }

    if (isNight) {
        painter->fillRect(0, 0, 400, 600, QColor(10, 15, 30, 140));
    }
}

void GameScene::onScoreUpdated(int score) {
    emit scoreUpdated(score);
}

void GameScene::onGameOver() {
    emit gameOver();
}
