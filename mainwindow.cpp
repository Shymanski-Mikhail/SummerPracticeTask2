#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "caritem.h"
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>
#include <QDir>
#include <QMap>
#include <QPair>
#include <QVector>
#include <QPixmap>
#include <QPainter>
#include <algorithm>
#include <QLabel>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , gameScene(nullptr) {
    ui->setupUi(this);

    setupColorCombo();

    gameScene = new GameScene(this);
    ui->gameView->setScene(gameScene);
    ui->gameView->setFixedSize(402, 602);

    connect(ui->playButton, &QPushButton::clicked, this, &MainWindow::onPlayClicked);
    connect(ui->backButton, &QPushButton::clicked, this, &MainWindow::onBackToMenu);
    connect(gameScene, &GameScene::scoreUpdated, this, &MainWindow::updateHud);
    connect(gameScene, &GameScene::gameOver, this, &MainWindow::onGameOver);

    loadLeaderboard();

    QLabel *imageLabel = new QLabel(this);

    QPixmap carPixmap("race_car.jpg");

    if (carPixmap.isNull()) {
        carPixmap = QPixmap(200, 120);
        carPixmap.fill(Qt::gray);
        QPainter p(&carPixmap);
        p.setPen(Qt::black);
        p.drawText(carPixmap.rect(), Qt::AlignCenter, "Нет JPG");
    }

    imageLabel->setPixmap(carPixmap.scaled(450, 380, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    ui->leftMenuLayout->insertWidget(ui->leftMenuLayout->count() - 1, imageLabel);
    ui->leaderboardTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::setupColorCombo() {
    QVector<QPair<QString, QColor>> colors = {
        {"Красный", Qt::red},
        {"Синий", QColor(0, 102, 204)},
        {"Зеленый", QColor(51, 204, 51)},
        {"Желтый", QColor(255, 204, 0)},
        {"Белый", Qt::white},
        {"Фиолетовый", QColor(153, 51, 255)}
    };

    for (const auto& colorPair : colors) {
        QPixmap pix(16, 16);
        pix.fill(Qt::transparent);
        QPainter p(&pix);
        p.setRenderHint(QPainter::Antialiasing);
        p.setBrush(colorPair.second);
        p.setPen(QPen(Qt::black, 1));
        p.drawRoundedRect(0, 0, 15, 15, 3, 3);
        p.end();
        ui->colorCombo->addItem(QIcon(pix), colorPair.first, colorPair.second);
    }
}

QString MainWindow::getScoresFilePath() {
    return QDir(QCoreApplication::applicationDirPath()).filePath("scores.txt");
}

void MainWindow::loadLeaderboard() {
    ui->leaderboardTable->setRowCount(0);
    ui->top3List->clear();
    ui->leaderboardTable->setSortingEnabled(false);

    QFile file(getScoresFilePath());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QTextStream in(&file);
    int row = 0;
    QMap<QString, int> userMaxScores;
    QMap<QString, QString> userMaxDist;

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        QStringList parts = line.split(",");
        if (parts.size() == 3) {
            QString name = parts[0];
            int score = parts[1].toInt();
            QString dist = parts[2];

            ui->leaderboardTable->insertRow(row);
            ui->leaderboardTable->setItem(row, 0, new QTableWidgetItem(name));
            ui->leaderboardTable->setItem(row, 1, new QTableWidgetItem(parts[1]));
            ui->leaderboardTable->setItem(row, 2, new QTableWidgetItem(dist));
            row++;

            if (!userMaxScores.contains(name) || score > userMaxScores[name]) {
                userMaxScores[name] = score;
                userMaxDist[name] = dist;
            }
        }
    }
    file.close();

    ui->leaderboardTable->setSortingEnabled(true);
    ui->leaderboardTable->sortItems(1, Qt::DescendingOrder);

    QVector<QPair<QString, int>> sortedUsers;
    for (auto it = userMaxScores.begin(); it != userMaxScores.end(); ++it) {
        sortedUsers.append({it.key(), it.value()});
    }

    std::sort(sortedUsers.begin(), sortedUsers.end(),
              [](const QPair<QString, int>& a, const QPair<QString, int>& b) {
                  return a.second > b.second;
              });

    int limit = std::min(3, static_cast<int>(sortedUsers.size()));
    for (int i = 0; i < limit; ++i) {
        QString name = sortedUsers[i].first;
        int score = sortedUsers[i].second;
        QString dist = userMaxDist[name];
        QString rankText = QString("%1. %2 — %3 очков (%4)").arg(i + 1).arg(name).arg(score).arg(dist);
        ui->top3List->addItem(rankText);
    }
}

void MainWindow::saveRecord(const QString& nick, const QString& score) {
    QFile file(getScoresFilePath());
    if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << nick.trimmed() << "," << score.trimmed() << ","  << "\n";
        file.close();
    }
}

void MainWindow::onPlayClicked() {
    ui->stackedWidget->setCurrentIndex(1);

    if (ui->scoreLabel) ui->scoreLabel->setText("Очки: 0");

    int numLanes = ui->lanesCombo->currentIndex() + 2;
    bool isNight = (ui->timeCombo->currentIndex() == 1);
    bool isTwoWay = (ui->trafficDirectionCombo->currentIndex() == 1);

    gameScene->startNewGame(numLanes, isNight, isTwoWay);
    applySkinSettings();
    ui->gameView->setFocus();
}

void MainWindow::onBackToMenu() {
    gameScene->stopGame();
    ui->stackedWidget->setCurrentIndex(0);
    loadLeaderboard();
}

void MainWindow::applySkinSettings() {
    if (!gameScene || !gameScene->getPlayer()) return;

    CarItem* player = gameScene->getPlayer();
    player->setLineThickness(ui->thicknessSlider->value());

    QColor selectedColor = ui->colorCombo->currentData().value<QColor>();
    player->setBodyColor(selectedColor);
}

void MainWindow::updateHud(int score) {
    ui->scoreLabel->setText(QString("Очки: %1").arg(score));
}

void MainWindow::onGameOver() {
    static bool insideGameOver = false;
    if (insideGameOver) return;
    insideGameOver = true;

    ui->gameView->clearFocus();

    QString playerNick = ui->nicknameEdit->text().trimmed();
    if (playerNick.isEmpty()) playerNick = "Driver";

    QString finalScore = ui->scoreLabel->text().split(" ").last();

    QMessageBox::information(this, "Игра Окончена",
                             QString("%1, Вы врезались!\nВаш счет: %2").arg(playerNick).arg(finalScore));

    saveRecord(playerNick, finalScore);
    loadLeaderboard();

    ui->stackedWidget->setCurrentIndex(0);
    insideGameOver = false;
}
