#include "caritem.h"
#include <QPainter>
#include <QLinearGradient>
#include <QRadialGradient>
#include <cmath>

CarItem::CarItem(bool isPlayer, bool incoming)
    : isPlayerControlled(isPlayer), isIncomingLane(incoming),isChangingLane(false), currentLane(1),
    speedY(0), bodyColor(Qt::red), lineThickness(2), isNightMode(false),
    targetX(0), speedX(8), currentRotation(0), smokeSpawnTimer(0)
{
    if (!isPlayerControlled) {
        bodyColor = QColor(rand() % 200 + 55, rand() % 200 + 55, rand() % 200 + 55);
    }
}

QRectF CarItem::boundingRect() const {
    return QRectF(-100, -200, 200, 400);
}

QPainterPath CarItem::shape() const {
    QPainterPath path;
    path.addRect(-20, -40, 40, 80);
    return path;
}

void CarItem::drawDoubleHeadlights(QPainter *painter) {
    painter->save();

    QPointF leftHeadlight(-12, -38);
    QPointF rightHeadlight(12, -38);
    qreal lightLength = -75.0;
    auto drawSingleCone = [&](const QPointF& origin) {
        QLinearGradient grad(origin, QPointF(origin.x(), origin.y() + lightLength));
        grad.setColorAt(0, QColor(255, 255, 210, 180));
        grad.setColorAt(0.5, QColor(255, 255, 210, 60));
        grad.setColorAt(1, QColor(255, 255, 250, 0));

        painter->setBrush(grad);
        painter->setPen(Qt::NoPen);

        QPolygonF cone;
        cone << origin
             << QPointF(origin.x() - 14, origin.y() + lightLength)
             << QPointF(origin.x() + 14, origin.y() + lightLength);
        painter->drawPolygon(cone);
    };

    drawSingleCone(leftHeadlight);
    drawSingleCone(rightHeadlight);
    painter->restore();
}

void CarItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option); Q_UNUSED(widget);

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    if (isIncomingLane) {
        painter->rotate(180);
    }

    painter->rotate(currentRotation);

    painter->rotate(currentRotation);

    bool isIncoming = (!isPlayerControlled && speedY < 0);
    if (isIncoming) {
        painter->rotate(180);
    }

    if (isNightMode) {
        drawDoubleHeadlights(painter);
    }

    QPen pen(Qt::black, lineThickness, Qt::SolidLine);
    painter->setPen(pen);
    painter->setBrush(bodyColor);
    painter->drawRoundedRect(-20, -40, 40, 80, 8, 8);

    painter->setBrush(QColor(40, 40, 40, 220));
    painter->setPen(QPen(Qt::black, 1));
    painter->drawRect(-15, -18, 30, 22);
    painter->drawRect(-12, 22, 24, 10);

    painter->setBrush(Qt::yellow);
    painter->setPen(Qt::NoPen);
    painter->drawRect(-16, -38, 7, 4);
    painter->drawRect(9, -38, 7, 4);

    painter->setBrush(Qt::red);
    painter->drawRect(-16, 36, 6, 3);
    painter->drawRect(10, 36, 6, 3);

    painter->restore();
}

void CarItem::setTargetX(qreal x) {
    targetX = x;
    isChangingLane = true;
}

void CarItem::updatePhysics() {
    if (isChangingLane) {
        qreal curX = x();
        if (std::abs(curX - targetX) < speedX) {
            setX(targetX);
            isChangingLane = false;
            currentRotation = 0;
        } else {
            if (curX < targetX) {
                setX(curX + speedX);
                currentRotation = 6;
            } else {
                setX(curX - speedX);
                currentRotation = -6;
            }
        }
    } else {
        currentRotation = 0;
    }
    update();
}
