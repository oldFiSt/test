#ifndef SHIPPLACEMENTWINDOW_H
#define SHIPPLACEMENTWINDOW_H

#include <QDialog>
#include <QList>
#include <QPoint>
#include <QPushButton>

namespace Ui {
class ShipPlacementWindow;
}

class ShipPlacementWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ShipPlacementWindow(QWidget *parent = nullptr);
    ~ShipPlacementWindow();

signals:
    void backToConnectWindow();
    void startGame(const QList<QList<QPoint>>& ships); // Сигнал передает корабли

private slots:
    void selectShip1();
    void selectShip2();
    void selectShip3();
    void selectShip4();
    void handleCellClick();
    void onStartBattleClicked();
    void onBackClicked();

private:
    Ui::ShipPlacementWindow *ui;
    QPushButton* gridButtons[10][10];
    int currentShipSize;
    QList<QPoint> currentPlacement;
    QList<QList<QPoint>> allPlacedShips;
    int shipsPlaced[5];
    const int maxShips[5] = {0, 4, 3, 2, 1};

    void initializeShipCounts();
    void selectShip(int size);
    void updateShipCountDisplay();
    bool isValidShipPlacement(const QList<QPoint>& ship);
    bool isAdjacentToAnotherShip(const QList<QPoint>& ship);
    void resetCurrentPlacement();
};

#endif // SHIPPLACEMENTWINDOW_H
