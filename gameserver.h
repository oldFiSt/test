// GameServer.h
#pragma once

#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>

class GameServer : public QObject {
    Q_OBJECT
public:
    explicit GameServer(QObject *parent = nullptr);
    bool startServer(quint16 port);

signals:
    void twoPlayersConnected();

private slots:
    void onNewConnection();
    void onReadyRead();
    void onDisconnected();

private:
    QTcpServer *server;
    QVector<QTcpSocket*> clients;
};
