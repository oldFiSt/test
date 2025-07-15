#include "GameClient.h"
#include <QDataStream>
#include <QDebug>

GameClient::GameClient(QObject *parent)
    : QObject(parent),
    socket(new QTcpSocket(this))
{
    connect(socket, &QTcpSocket::connected, this, &GameClient::onConnected);
    connect(socket, &QTcpSocket::readyRead, this, &GameClient::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &GameClient::onDisconnected);
    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred),
            this, &GameClient::onSocketError);
}

void GameClient::connectToServer(const QString &host, quint16 port)
{
    socket->abort(); // На случай, если подключение уже активно
    socket->connectToHost(host, port);
}

void GameClient::sendData(const QByteArray &data)
{
    if (socket->state() == QAbstractSocket::ConnectedState) {
        // Добавляем символ новой строки для разделения сообщений
        QByteArray newData = data;
        if (!newData.endsWith('\n')) {
            newData.append('\n');
        }
        socket->write(newData);
    } else {
        qDebug() << "Не удалось отправить данные: нет соединения.";
    }
}

void GameClient::onConnected()
{
    qDebug() << "Успешное подключение к серверу.";
    emit connected();
}

void GameClient::onDisconnected()
{
    qDebug() << "Отключено от сервера.";
    emit disconnected();
}

void GameClient::onSocketError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError)
    QString error = socket->errorString();
    qDebug() << "Ошибка сокета:" << error;
    emit errorOccurred(error);
}

void GameClient::onReadyRead() {
    buffer.append(socket->readAll());

    // Разделяем сообщения по символу новой строки
    int pos;
    while ((pos = buffer.indexOf('\n')) != -1) {
        QByteArray message = buffer.left(pos);
        buffer = buffer.mid(pos + 1);

        if (!message.isEmpty()) {
            emit dataReceived(message);

            QString msg = QString::fromUtf8(message);
            if (msg == "both connected") {
                emit twoPlayersConnected();
            }
        }
    }
}
