// GameServer.cpp
#include "GameServer.h"
#include <QDebug>
#include <QTimer>

GameServer::GameServer(QObject *parent)
    : QObject(parent), server(new QTcpServer(this)) {
    connect(server, &QTcpServer::newConnection, this, &GameServer::onNewConnection);
}

bool GameServer::startServer(quint16 port) {
    if (!server->listen(QHostAddress::Any, port)) {
        qDebug() << "Не удалось запустить сервер:" << server->errorString();
        return false;
    } else {
        qDebug() << "Сервер запущен на порту" << port;
        return true;
    }
}

void GameServer::onNewConnection() {
    if (clients.size() >= 2) {
        QTcpSocket *extra = server->nextPendingConnection();
        extra->disconnectFromHost();
        qDebug() << "Отклонено подключение: максимум 2 игрока";
        return;
    }

    QTcpSocket *client = server->nextPendingConnection();
    clients.append(client);
    qDebug() << "Player connected. Currently players:" << clients.size();

    connect(client, &QTcpSocket::readyRead, this, &GameServer::onReadyRead);
    connect(client, &QTcpSocket::disconnected, this, &GameServer::onDisconnected);

    // Когда подключились два игрока, назначаем им ID и отправляем сигнал старта
    if (clients.size() == 2) {
        // Добавлена задержка для стабильности подключения
        QTimer::singleShot(100, this, [this]() {
            emit twoPlayersConnected();
            // Отправляем первому игроку ID=1, второму ID=2
            clients.at(0)->write("ID:1\n");
            clients.at(1)->write("ID:2\n");

            // Отправляем обоим сигнал, что можно начинать
            for (QTcpSocket *c : clients) {
                c->write("both connected\n");
            }
        });
    }
}

void GameServer::onReadyRead() {
    QTcpSocket *senderClient = qobject_cast<QTcpSocket*>(sender());
    if (!senderClient) return;

    QByteArray data = senderClient->readAll();

    // Пересылаем данные другому клиенту
    for (QTcpSocket *client : clients) {
        if (client != senderClient && client->state() == QAbstractSocket::ConnectedState) {
            client->write(data);
        }
    }
}

void GameServer::onDisconnected() {
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    if (!client) return;

    clients.removeAll(client);
    client->deleteLater();
    qDebug() << "Клиент отключился. Осталось:" << clients.size();
}
