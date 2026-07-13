#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include <QGraphicsView>
#include "gamescene.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onPlayClicked();
    void onGameOver();
    void onBackToMenu();
    void updateHud(int score);
    void applySkinSettings();

private:
    void loadLeaderboard();
    void saveRecord(const QString& nick, const QString& score);
    QString getScoresFilePath();
    void setupColorCombo();

    Ui::MainWindow *ui;
    GameScene *gameScene;
};
