#include <QtTest>
#include <QGraphicsScene>
#include "gameengine.h"
#include "caritem.h"

class TestGameEngine : public QObject {

private slots:
    void init() {
        engine = new GameEngine();
        player = new CarItem(true, false);
        player->currentLane = 1;
        player->setPos(200, 480);
    }

    void cleanup() {
        delete engine;
        delete player;
    }

    void testInitialization() {
        QCOMPARE(engine->getScore(), 0);
        QCOMPARE(engine->getDistance(), 0);
        QVERIFY(!engine->isGameRunning());
    }

    void testGameStart() {
        engine->startGame(2, false, false, player);
        QVERIFY(engine->isGameRunning());
        QCOMPARE(engine->getScore(), 0);
        QCOMPARE(engine->getDistance(), 0);
        engine->stopGame();
        QVERIFY(!engine->isGameRunning());
    }

    void testScoreIncrement() {
        engine->startGame(2, false, false, player);
        for (int i = 0; i < 100; i++) {
            engine->gameTick();
        }
        QVERIFY(engine->getScore() > 0);
        engine->stopGame();
    }

    void testLaneXCalculation() {
        const int numLanes = 2;
        const qreal roadWidth = 300.0;
        const qreal expectedLaneWidth = roadWidth / numLanes;

        engine->startGame(numLanes, false, false, player);
        qreal lane0 = engine->getLaneX(0);
        qreal lane1 = engine->getLaneX(1);
        QVERIFY(std::abs(lane1 - lane0 - expectedLaneWidth) < 2);
        engine->stopGame();
    }

    void testReset() {
        engine->startGame(2, false, false, player);
        engine->gameTick();
        QVERIFY(engine->getScore() >= 0);
        engine->reset();
        QCOMPARE(engine->getScore(), 0);
        QCOMPARE(engine->getDistance(), 0);
    }

private:
    GameEngine* engine;
    CarItem* player;
};

QTEST_MAIN(TestGameEngine)
