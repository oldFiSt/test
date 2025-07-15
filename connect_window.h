#ifndef CONNECT_WINDOW_H
#define CONNECT_WINDOW_H

#include <QDialog>
#include <QLabel>

class GameClient;

namespace Ui {
class ConnectWindow;
}

class ConnectWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectWindow(GameClient* client, QWidget *parent = nullptr);
    ~ConnectWindow();

    void disconnectFromClient();
    void setStatusText(const QString& text);
    void setConnectButtonEnabled(bool enabled);

signals:
    void backtoMenu();
    void connectedToServer();
    void bothPlayersReady(int playerID);

private slots:
    void on_btnConnect_clicked();
    void on_btnBack_clicked();
    void onTwoPlayersConnected();
    void onConnected();
    void onDisconnected();
    void onDataReceived(const QByteArray &data);

private:
    Ui::ConnectWindow *ui;
    GameClient *m_client;
    QLabel *labelStatus;
    int playerID;
};

#endif // CONNECT_WINDOW_H
