#pragma once
#include <QGraphicsObject>
#include <QColor>
#include <QVector>
#include <QPointF>

struct SmokeParticle {
    QPointF position;
    qreal opacity;
    qreal size;
};

class CarItem : public QGraphicsObject {
    Q_OBJECT
public:
    CarItem(bool isPlayer = false, bool incoming = false);

    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void setBodyColor(const QColor &color) { bodyColor = color; update(); }
    void setLineThickness(int thickness) { lineThickness = thickness; update(); }
    void setNightMode(bool enabled) { isNightMode = enabled; update(); }

    void setTargetX(qreal x);
    void updatePhysics();

    bool isPlayerControlled;
    bool isChangingLane;
    int currentLane;
    qreal speedY;

private:
    void drawDoubleHeadlights(QPainter *painter);

    QColor bodyColor;
    int lineThickness;
    bool isNightMode;
    bool isIncomingLane;

    qreal targetX;
    qreal speedX;
    qreal currentRotation;

    QVector<SmokeParticle> particles;
    int smokeSpawnTimer;
};
