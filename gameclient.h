#ifndef GAMECLIENT_H
#define GAMECLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QString>

class GameClient : public QObject
{
    Q_OBJECT

public:
    explicit GameClient(QObject *parent = nullptr);
    void connectToServer(const QString &host, quint16 port);
    void sendData(const QByteArray &data);

signals:
    void connected();                         // Успешное подключение
    void disconnected();                      // Отключение от сервера
    void dataReceived(const QByteArray &data); // Получены данные от сервера
    void errorOccurred(const QString &error); // Ошибка подключения
    void twoPlayersConnected();

private slots:
    void onConnected();
    void onReadyRead();
    void onDisconnected();
    void onSocketError(QAbstractSocket::SocketError socketError);

private:
    QTcpSocket *socket;
    QByteArray buffer; // Буфер для сообщений
};

#endif // GAMECLIENT_H
