#include "secondwindow.h"
#include "ui_secondwindow.h"
#include "battlewindow.h"
#include "mainwindow.h"

#include <QMessageBox>
#include <QPushButton>
#include <algorithm>

SecondWindow::SecondWindow(QWidget *parent)
    : QDialog(parent), ui(new Ui::SecondWindow), currentShipSize(0)
{
    setFixedSize(944, 600);
    ui->setupUi(this);

    for (int row = 0; row < 10; ++row) {
        for (int col = 0; col < 10; ++col) {
            gridButtons[row][col] = new QPushButton(this);
            gridButtons[row][col]->setStyleSheet("background-color: lightblue;");
            gridButtons[row][col]->setProperty("row", row);
            gridButtons[row][col]->setProperty("col", col);
            gridButtons[row][col]->setProperty("occupied", false);

            connect(gridButtons[row][col], &QPushButton::clicked, this, &SecondWindow::handleCellClick);
            ui->gridLayoutField->addWidget(gridButtons[row][col], row, col);
        }
    }

    connect(ui->btnShip1, &QPushButton::clicked, this, &SecondWindow::selectShip1);
    connect(ui->btnShip2, &QPushButton::clicked, this, &SecondWindow::selectShip2);
    connect(ui->btnShip3, &QPushButton::clicked, this, &SecondWindow::selectShip3);
    connect(ui->btnShip4, &QPushButton::clicked, this, &SecondWindow::selectShip4);
    connect(ui->btnStartBattle, &QPushButton::clicked, this, &SecondWindow::startBattle);
    connect(ui->btnBackToMainMenu, &QPushButton::clicked, this, &SecondWindow::returnToMainMenu);

    initializeShipCounts();
}

SecondWindow::~SecondWindow()
{
    delete ui;
}

void SecondWindow::initializeShipCounts()
{
    shipsPlaced[1] = 0;
    shipsPlaced[2] = 0;
    shipsPlaced[3] = 0;
    shipsPlaced[4] = 0;
    updateShipCountDisplay();
}

void SecondWindow::selectShip(int size)
{
    currentShipSize = size;
    currentPlacement.clear();
    QMessageBox::information(this, "Выбор корабля", QString("Вы выбрали %1-палубный корабль").arg(size));
}

void SecondWindow::selectShip1() { selectShip(1); }
void SecondWindow::selectShip2() { selectShip(2); }
void SecondWindow::selectShip3() { selectShip(3); }
void SecondWindow::selectShip4() { selectShip(4); }

void SecondWindow::handleCellClick()
{
    if (currentShipSize == 0) return;

    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    int row = btn->property("row").toInt();
    int col = btn->property("col").toInt();

    if (!currentPlacement.isEmpty()) {
        bool valid = false;
        QPoint last = currentPlacement.last();

        if ((std::abs(row - last.x()) == 1 && col == last.y()) ||
            (std::abs(col - last.y()) == 1 && row == last.x())) {
            valid = true;
        }

        if (!valid) {
            QMessageBox::warning(this, "Ошибка", "Корабль должен быть непрерывным!");
            return;
        }
    }

    if (btn->property("occupied").toBool()) return;

    currentPlacement.append(QPoint(row, col));
    btn->setStyleSheet("background-color: gray;");
    btn->setProperty("occupied", true);

    if (currentPlacement.size() == currentShipSize) {
        if (shipsPlaced[currentShipSize] >= maxShips[currentShipSize]) {
            QMessageBox::warning(this, "Ошибка", "Достигнут лимит кораблей этого типа!");
            resetCurrentPlacement();
            return;
        }

        if (!isValidShipPlacement(currentPlacement)) {
            QMessageBox::warning(this, "Ошибка", "Невозможно разместить корабль здесь.");
            resetCurrentPlacement();
            return;
        }

        shipsPlaced[currentShipSize]++;
        allPlacedShips.append(currentPlacement);
        updateShipCountDisplay();
        currentPlacement.clear();
    }
}

void SecondWindow::resetCurrentPlacement()
{
    for (const QPoint& point : currentPlacement) {
        gridButtons[point.x()][point.y()]->setStyleSheet("background-color: lightblue;");
        gridButtons[point.x()][point.y()]->setProperty("occupied", false);
    }
    currentPlacement.clear();
}

void SecondWindow::updateShipCountDisplay()
{
    ui->labelShip1Count->setText(QString("1-палубные: %1/4").arg(shipsPlaced[1]));
    ui->labelShip2Count->setText(QString("2-палубные: %1/3").arg(shipsPlaced[2]));
    ui->labelShip3Count->setText(QString("3-палубные: %1/2").arg(shipsPlaced[3]));
    ui->labelShip4Count->setText(QString("4-палубные: %1/1").arg(shipsPlaced[4]));
}

bool SecondWindow::isValidShipPlacement(const QList<QPoint>& ship)
{
    if (ship.size() == 1) {
        return !isAdjacentToAnotherShip(ship);
    }

    QList<QPoint> sorted = ship;
    std::sort(sorted.begin(), sorted.end(), [](const QPoint &a, const QPoint &b) {
        return a.x() == b.x() ? a.y() < b.y() : a.x() < b.x();
    });

    bool isVertical = true, isHorizontal = true;
    int firstRow = sorted[0].x(), firstCol = sorted[0].y();

    for (int i = 1; i < sorted.size(); ++i) {
        if (sorted[i].x() != firstRow) isHorizontal = false;
        if (sorted[i].y() != firstCol) isVertical = false;
    }

    if (!isVertical && !isHorizontal) return false;

    if (isHorizontal) {
        for (int i = 1; i < sorted.size(); ++i) {
            if (sorted[i].y() != sorted[i-1].y() + 1) {
                return false;
            }
        }
    } else {
        for (int i = 1; i < sorted.size(); ++i) {
            if (sorted[i].x() != sorted[i-1].x() + 1) {
                return false;
            }
        }
    }

    return !isAdjacentToAnotherShip(ship);
}

bool SecondWindow::isAdjacentToAnotherShip(const QList<QPoint>& ship)
{
    for (const QPoint& point : ship) {
        int row = point.x(), col = point.y();

        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                int newRow = row + i, newCol = col + j;
                if (newRow >= 0 && newRow < 10 && newCol >= 0 && newCol < 10) {
                    if (gridButtons[newRow][newCol]->property("occupied").toBool() &&
                        !ship.contains(QPoint(newRow, newCol))) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

void SecondWindow::startBattle()
{
    if (shipsPlaced[1] != 4 || shipsPlaced[2] != 3 || shipsPlaced[3] != 2 || shipsPlaced[4] != 1) {
        QMessageBox::warning(this, "Ошибка", "Сначала расставьте все корабли!");
        return;
    }

    MainWindow* mainWindow = qobject_cast<MainWindow*>(parent());

    BattleWindow* battle = new BattleWindow(allPlacedShips, mainWindow, this);
    battle->show();
    this->hide();
}

void SecondWindow::returnToMainMenu()
{
    this->hide();
    if (parentWidget()) {
        parentWidget()->show();
    }
}
