#ifndef SECONDWINDOW_H
#define SECONDWINDOW_H

#include <QDialog>
#include <QList>
#include <QPoint>
#include <QPushButton>

class MainWindow;

namespace Ui {
class SecondWindow;
}

class SecondWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SecondWindow(QWidget *parent = nullptr);
    ~SecondWindow();

private slots:
    void selectShip1();
    void selectShip2();
    void selectShip3();
    void selectShip4();
    void handleCellClick();
    void startBattle();
    void returnToMainMenu();

private:
    Ui::SecondWindow *ui;
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

#endif // SECONDWINDOW_H
