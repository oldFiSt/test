#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QPoint>

class SecondWindow;
class ConnectWindow;
class ShipPlacementWindow;
class BattleWindow;
class GameServer;
class GameClient;
class MultiplayerModeWindow;
class HostLobbyWindow;
class HostSetupWindow;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_btnExitToDesktop_clicked();

    void onPlayerShipsPlaced(const QList<QList<QPoint>>& ships);

    void onHostGameClicked();
    void onJoinGameClicked();
    void onMultiplayerBackClicked(QWidget* senderWindow); // ИЗМЕНЕНА СИГНАТУРА
    void startServerAndLobby(quint16 port);
    void proceedToShipPlacement(int id);

    void handleNetworkData(const QByteArray& data);

private:
    Ui::MainWindow *ui;
    SecondWindow *secondWindow;
    ConnectWindow *connectWindow;
    ShipPlacementWindow *shipPlacementWindow;
    BattleWindow *battleWindow;
    GameServer *gameServer;
    GameClient *gameClient;
    MultiplayerModeWindow* multiplayerModeWindow;
    HostLobbyWindow* hostLobbyWindow;
    HostSetupWindow* hostSetupWindow;
    int playerID;

    QList<QList<QPoint>> m_myShips;
    bool m_iAmReady;
    bool m_opponentIsReady;

    void tryStartMultiplayerGame();
    void cleanupNetwork(bool force = false); // ДОБАВЛЕН МЕТОД
};

#endif // MAINWINDOW_H
