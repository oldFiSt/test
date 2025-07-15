#include "battlewindow.h"
#include "ui_battlewindow.h"
#include "mainwindow.h"
#include "secondwindow.h"
#include "GameClient.h"
#include <QRandomGenerator>
#include <QDebug>
#include <QMessageBox>
#include <QSet>
#include <algorithm>

// Конструктор для игры с ботом
BattleWindow::BattleWindow(const QList<QList<QPoint>>& playerShipsGroups, MainWindow* mainWindow, QWidget *parent)
    : QDialog(parent),
    ui(new Ui::BattleWindow),
    mainWindowPtr(mainWindow),
    gameClient(nullptr),
    playerGridLayout(nullptr),
    enemyGridLayout(nullptr),
    botTimer(nullptr),
    playerShipsAlive(0),
    enemyShipsAlive(0),
    playerTurn(true),
    isNetworkGame(false),
    myPlayerID(0),
    botHuntingMode(false)
{
    setFixedSize(900, 650);
    ui->setupUi(this);

    playerGridLayout = new QGridLayout(ui->gridLayoutPlayer);
    enemyGridLayout = new QGridLayout(ui->gridLayoutEnemy);
    playerGridLayout->setSpacing(1);
    enemyGridLayout->setSpacing(1);

    ui->btnReplay->setVisible(false);
    ui->btnExitToMainMenu->setVisible(false);

    connect(ui->btnReplay, &QPushButton::clicked, this, [=]() {
        this->close();
        SecondWindow* window = new SecondWindow(mainWindowPtr);
        window->show();
    });
    connect(ui->btnExitToMainMenu, &QPushButton::clicked, this, &BattleWindow::on_btnExitToMainMenu_clicked);

    playerShipsGrouped = playerShipsGroups;
    playerShipsGroupedFOREVER = playerShipsGroups;

    setupFields();
    placePlayerShips();

    playerShipsAlive = playerShipsGrouped.size();
    enemyShipsAlive = 10;
    ui->labelPlayerShips->setText("Ваши корабли: " + QString::number(playerShipsAlive));
    ui->labelEnemyShips->setText("Корабли противника: " + QString::number(enemyShipsAlive));

    placeEnemyShipsRandomly();

    botTimer = new QTimer(this);
    botTimer->setInterval(500); // Интервал для хода бота (500 мс = 0.5 секунды)
    connect(botTimer, &QTimer::timeout, this, &BattleWindow::botShoot);

    updateTurnStatus();
}

// Новый конструктор для сетевой игры
BattleWindow::BattleWindow(const QList<QList<QPoint>>& playerShipsGroups, GameClient* client, int playerID, MainWindow* mainWindow, QWidget *parent)
    : QDialog(parent),
    ui(new Ui::BattleWindow),
    mainWindowPtr(mainWindow),
    gameClient(client),
    playerGridLayout(nullptr),
    enemyGridLayout(nullptr),
    botTimer(nullptr),
    playerShipsAlive(0),
    enemyShipsAlive(0),
    playerTurn(playerID == 1),
    isNetworkGame(true),
    myPlayerID(playerID),
    botHuntingMode(false)
{
    setFixedSize(900, 650);
    ui->setupUi(this);

    playerGridLayout = new QGridLayout(ui->gridLayoutPlayer);
    enemyGridLayout = new QGridLayout(ui->gridLayoutEnemy);
    playerGridLayout->setSpacing(1);
    enemyGridLayout->setSpacing(1);

    ui->btnReplay->setVisible(false);
    ui->btnExitToMainMenu->setVisible(false);
    connect(ui->btnExitToMainMenu, &QPushButton::clicked, this, &BattleWindow::on_btnExitToMainMenu_clicked);
    ui->btnReplay->hide();

    playerShipsGrouped = playerShipsGroups;
    playerShipsGroupedFOREVER = playerShipsGroups;

    setupFields();
    placePlayerShips();

    playerShipsAlive = playerShipsGrouped.size();
    enemyShipsAlive = 10;
    ui->labelPlayerShips->setText("Ваши корабли: " + QString::number(playerShipsAlive));
    ui->labelEnemyShips->setText("Корабли противника: " + QString::number(enemyShipsAlive));

    if (gameClient) {
        connect(gameClient, &GameClient::dataReceived, this, &BattleWindow::handleNetworkData);
        connect(gameClient, &GameClient::disconnected, this, [this]() {
            QMessageBox::warning(this, "Соединение потеряно", "Связь с сервером была прервана.");
            ui->btnExitToMainMenu->setVisible(true);
            setEnemyFieldEnabled(false);
        });
    }

    updateTurnStatus();
}

BattleWindow::~BattleWindow()
{
    delete ui;
}

void BattleWindow::on_btnExitToMainMenu_clicked()
{
    this->close();
    if (mainWindowPtr) {
        mainWindowPtr->show();
    }
}

void BattleWindow::setupFields()
{
    for (int row = 0; row < 10; ++row) {
        for (int col = 0; col < 10; ++col) {
            playerField[row][col] = new QPushButton(this);
            playerField[row][col]->setFixedSize(30, 30);
            playerField[row][col]->setEnabled(true);
            playerField[row][col]->setStyleSheet("background-color: lightblue;");
            playerGridLayout->addWidget(playerField[row][col], row, col);
            playerField[row][col]->setProperty("hasShip", false);

            enemyField[row][col] = new QPushButton(this);
            enemyField[row][col]->setFixedSize(30, 30);
            enemyField[row][col]->setStyleSheet("background-color: lightblue;");
            enemyGridLayout->addWidget(enemyField[row][col], row, col);
            enemyField[row][col]->setProperty("row", row);
            enemyField[row][col]->setProperty("col", col);
            enemyField[row][col]->setProperty("hasShip", false);

            connect(enemyField[row][col], &QPushButton::clicked, this, &BattleWindow::handlePlayerShot);
        }
    }
}

void BattleWindow::handlePlayerShot()
{
    if (!playerTurn) return;

    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    if (!btn || !btn->isEnabled()) return;

    int row = btn->property("row").toInt();
    int col = btn->property("col").toInt();

    btn->setEnabled(false);

    if (isNetworkGame) {
        QString message = QString("SHOOT:%1:%2").arg(row).arg(col);
        gameClient->sendData(message.toUtf8());
    } else {
        bool hit = btn->property("hasShip").toBool();
        if (hit) {
            btn->setStyleSheet("background-color: red;");
            checkAndHandleDestroyedShip(QPoint(row, col), true);
            updateTurnStatus();
        } else {
            btn->setStyleSheet("background-color: white;");
            playerTurn = false;
            updateTurnStatus();
            botTimer->start();
        }
    }
}

void BattleWindow::handleNetworkData(const QByteArray &data)
{
    QString msg = QString::fromUtf8(data);
    QStringList parts = msg.split(':');
    QString command = parts[0];

    if (command == "SHOOT") {
        int row = parts[1].toInt();
        int col = parts[2].toInt();

        QPushButton* cell = playerField[row][col];
        bool hit = cell->property("hasShip").toBool();

        if (hit) {
            cell->setStyleSheet("background-color: red;");
            gameClient->sendData(QString("HIT:%1:%2").arg(row).arg(col).toUtf8());
            checkAndHandleDestroyedShip(QPoint(row, col), false);
        } else {
            cell->setStyleSheet("background-color: white;");
            gameClient->sendData(QString("MISS:%1:%2").arg(row).arg(col).toUtf8());
            playerTurn = true;
            updateTurnStatus();
        }
    } else if (command == "HIT") {
        int row = parts[1].toInt();
        int col = parts[2].toInt();
        enemyField[row][col]->setStyleSheet("background-color: red;");
        enemyField[row][col]->setEnabled(false);
    } else if (command == "MISS") {
        int row = parts[1].toInt();
        int col = parts[2].toInt();
        enemyField[row][col]->setStyleSheet("background-color: white;");
        enemyField[row][col]->setEnabled(false);
        playerTurn = false;
        updateTurnStatus();
    } else if (command == "SUNK") {
        enemyShipsAlive--;
        ui->labelEnemyShips->setText("Корабли противника: " + QString::number(enemyShipsAlive));
        QList<QPoint> sunkShip;
        for (int i = 1; i < parts.size(); i += 2) {
            sunkShip.append(QPoint(parts[i].toInt(), parts[i+1].toInt()));
        }
        markSurroundingArea(sunkShip, enemyField);
        if (enemyShipsAlive == 0) {
            QMessageBox::information(this, "Победа", "Вы уничтожили все корабли противника!");
            ui->btnExitToMainMenu->setVisible(true);
            setEnemyFieldEnabled(false);
        }
    }
}


void BattleWindow::placePlayerShips()
{
    for (const auto& ship : playerShipsGroupedFOREVER) {
        for (const QPoint& pt : ship) {
            if (pt.x() >= 0 && pt.x() < 10 && pt.y() >= 0 && pt.y() < 10) {
                playerField[pt.x()][pt.y()]->setStyleSheet("background-color: green;");
                playerField[pt.x()][pt.y()]->setProperty("hasShip", true);
            }
        }
    }
}

void BattleWindow::placeEnemyShipsRandomly()
{
    if (isNetworkGame) return;

    enemyShipsGrouped.clear();
    enemyShipsGroupedFOREVER.clear();

    int shipSizes[] = {4, 3, 3, 2, 2, 2, 1, 1, 1, 1};
    for (int size : shipSizes) {
        bool placed = false;
        while (!placed) {
            bool vertical = QRandomGenerator::global()->bounded(2);
            int row = QRandomGenerator::global()->bounded(10);
            int col = QRandomGenerator::global()->bounded(10);
            QList<QPoint> newShip;
            for (int i = 0; i < size; ++i) {
                int r = vertical ? row + i : row;
                int c = vertical ? col : col + i;
                if (r < 10 && c < 10) {
                    newShip.append(QPoint(r, c));
                }
            }
            if (newShip.size() == size && isValidEnemyShipPlacement(newShip)) {
                enemyShipsGrouped.append(newShip);
                for (const QPoint& pt : newShip) {
                    enemyField[pt.x()][pt.y()]->setProperty("hasShip", true);
                }
                placed = true;
            }
        }
    }
    enemyShipsGroupedFOREVER = enemyShipsGrouped;
}

bool BattleWindow::isValidEnemyShipPlacement(const QList<QPoint>& ship)
{
    for (const QPoint& point : ship) {
        for (int dr = -1; dr <= 1; ++dr) {
            for (int dc = -1; dc <= 1; ++dc) {
                int nr = point.x() + dr;
                int nc = point.y() + dc;
                if (nr >= 0 && nr < 10 && nc >= 0 && nc < 10) {
                    if (enemyField[nr][nc]->property("hasShip").toBool())
                        return false;
                }
            }
        }
    }
    return true;
}

void BattleWindow::checkAndHandleDestroyedShip(QPoint hitPoint, bool isPlayerShot)
{
    auto& ships = isPlayerShot ? enemyShipsGrouped : playerShipsGrouped;
    auto& shipsForever = isPlayerShot ? enemyShipsGroupedFOREVER : playerShipsGroupedFOREVER;

    for (int i = 0; i < ships.size(); ++i) {
        if (ships[i].contains(hitPoint)) {
            ships[i].removeOne(hitPoint);

            if (ships[i].isEmpty()) {
                QList<QPoint> originalShip;
                for (const auto& ship : shipsForever) {
                    if (ship.contains(hitPoint)) {
                        originalShip = ship;
                        break;
                    }
                }

                if (isPlayerShot) {
                    enemyShipsAlive--;
                    ui->labelEnemyShips->setText("Корабли противника: " + QString::number(enemyShipsAlive));
                    markSurroundingArea(originalShip, enemyField);

                    if (isNetworkGame) {
                        QString msg = "SUNK";
                        for (const QPoint& p : originalShip) {
                            msg += QString(":%1:%2").arg(p.x()).arg(p.y());
                        }
                        gameClient->sendData(msg.toUtf8());
                    }

                    if (enemyShipsAlive == 0) {
                        QMessageBox::information(this, "Победа", "Вы уничтожили все корабли противника!");
                        ui->btnReplay->setVisible(!isNetworkGame);
                        ui->btnExitToMainMenu->setVisible(true);
                        setEnemyFieldEnabled(false);
                        if (botTimer) botTimer->stop();
                    }
                } else { // Это выстрел бота
                    playerShipsAlive--;
                    ui->labelPlayerShips->setText("Ваши корабли: " + QString::number(playerShipsAlive));
                    markSurroundingArea(originalShip, playerField);

                    // Сброс режима охоты после потопления корабля
                    botHuntingMode = false;
                    targetQueue.clear();

                    if (isNetworkGame) {
                        QString msg = "SUNK";
                        for(const QPoint& p : originalShip) {
                            msg += QString(":%1:%2").arg(p.x()).arg(p.y());
                        }
                        gameClient->sendData(msg.toUtf8());
                    }
                    if (playerShipsAlive == 0) {
                        QMessageBox::information(this, "Поражение", "Все ваши корабли уничтожены!");
                        ui->btnReplay->setVisible(!isNetworkGame);
                        ui->btnExitToMainMenu->setVisible(true);
                        setEnemyFieldEnabled(false);
                        if (botTimer) botTimer->stop();
                    }
                }
            }
            break;
        }
    }
}

void BattleWindow::markSurroundingArea(const QList<QPoint>& ship, QPushButton* field[10][10])
{
    for (const QPoint& pt : ship) {
        for (int dr = -1; dr <= 1; ++dr) {
            for (int dc = -1; dc <= 1; ++dc) {
                int nr = pt.x() + dr;
                int nc = pt.y() + dc;
                if (nr >= 0 && nr < 10 && nc >= 0 && nc < 10) {
                    QPushButton* btn = field[nr][nc];
                    bool isShipPart = false;
                    for (const auto& s : ship) {
                        if (s == QPoint(nr, nc)) {
                            isShipPart = true;
                            break;
                        }
                    }
                    if (!isShipPart && btn->isEnabled()) {
                        btn->setStyleSheet("background-color: white;");
                        btn->setEnabled(false); // Делаем соседние клетки неактивными
                    }
                }
            }
        }
    }
}


void BattleWindow::botShoot()
{
    if (playerShipsAlive == 0) {
        botTimer->stop();
        return;
    }

    QPoint target;
    bool targetFound = false;

    // 1. Режим "Охоты": если есть цели в очереди, берем их
    while (botHuntingMode && !targetQueue.isEmpty()) {
        target = targetQueue.takeFirst();
        if (target.x() >= 0 && target.x() < 10 && target.y() >= 0 && target.y() < 10) {
            if (playerField[target.x()][target.y()]->isEnabled()) {
                targetFound = true;
                break;
            }
        }
    }

    // Если в режиме охоты цели закончились, но корабль не потоплен, сбрасываем
    if (botHuntingMode && !targetFound) {
        botHuntingMode = false;
        targetQueue.clear();
    }

    // 2. Режим "Поиска": если мы не на охоте, ищем случайную цель
    if (!targetFound) {
        QVector<QPoint> availableCells;
        for (int r = 0; r < 10; ++r) {
            for (int c = 0; c < 10; ++c) {
                if (playerField[r][c]->isEnabled()) {
                    availableCells.append(QPoint(r, c));
                }
            }
        }
        if (!availableCells.isEmpty()) {
            target = availableCells.at(QRandomGenerator::global()->bounded(availableCells.size()));
            targetFound = true;
        }
    }

    if (!targetFound) { // На поле не осталось доступных клеток
        botTimer->stop();
        return;
    }


    // Совершаем выстрел
    QPushButton* cell = playerField[target.x()][target.y()];
    cell->setEnabled(false);

    bool hit = cell->property("hasShip").toBool();
    if (hit) {
        cell->setStyleSheet("background-color: red;");
        lastHitPoint = target;

        if (!botHuntingMode) { // Первое попадание по новому кораблю
            botHuntingMode = true;
            firstHitPoint = target;
            // Добавляем соседние клетки в очередь
            int r = target.x(), c = target.y();
            if (r > 0) targetQueue.append(QPoint(r - 1, c));
            if (r < 9) targetQueue.append(QPoint(r + 1, c));
            if (c > 0) targetQueue.append(QPoint(r, c - 1));
            if (c < 9) targetQueue.append(QPoint(r, c + 1));
        } else { // Второе (и последующее) попадание
            targetQueue.clear(); // Очищаем старые цели, т.к. теперь знаем направление
            int dx = lastHitPoint.x() - firstHitPoint.x();
            int dy = lastHitPoint.y() - firstHitPoint.y();

            // Добавляем точки по прямой в обе стороны от крайних попаданий
            QPoint p1 = firstHitPoint, p2 = lastHitPoint;
            if (dx == 0 && p1.y() > p2.y()) std::swap(p1, p2);
            if (dy == 0 && p1.x() > p2.x()) std::swap(p1, p2);

            if (p1.x() > 0 && dy == 0) targetQueue.append(QPoint(p1.x() - 1, p1.y()));
            if (p2.x() < 9 && dy == 0) targetQueue.append(QPoint(p2.x() + 1, p2.y()));
            if (p1.y() > 0 && dx == 0) targetQueue.append(QPoint(p1.x(), p1.y() - 1));
            if (p2.y() < 9 && dx == 0) targetQueue.append(QPoint(p1.x(), p2.y() + 1));
        }

        checkAndHandleDestroyedShip(target, false);
        if (playerShipsAlive > 0) {
            botTimer->start(); // Продолжаем ход, т.к. попали
        }
    } else { // Промах - в любом случае передаем ход игроку
        cell->setStyleSheet("background-color: white;");
        playerTurn = true;      // Передаем ход игроку
        updateTurnStatus();     // Обновляем надписи
        botTimer->stop();       // Останавливаем таймер бота, т.к. его ход закончен
    }
}


void BattleWindow::updateTurnStatus() {
    QString turnText = playerTurn ? "(Ваш ход)" : "(Ход противника)";
    ui->labelPlayer->setText("Ваше поле " + turnText);
    ui->labelEnemy->setText("Поле противника");
    setEnemyFieldEnabled(playerTurn);
}

void BattleWindow::setEnemyFieldEnabled(bool enabled) {
    for (int r = 0; r < 10; ++r) {
        for (int c = 0; c < 10; ++c) {
            // Разрешаем нажимать только на "голубые" (неизведанные) клетки
            if (enemyField[r][c]->styleSheet().contains("lightblue")) {
                enemyField[r][c]->setEnabled(enabled);
            }
        }
    }
}
