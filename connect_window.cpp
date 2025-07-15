#include "connect_window.h"
#include "ui_connect_window.h"
#include "GameClient.h"
#include <QMessageBox>
#include <QDebug>

ConnectWindow::ConnectWindow(GameClient* client, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ConnectWindow)
    , m_client(client)
    , playerID(0)
{
    setFixedSize(800, 588);
    ui->setupUi(this);

    labelStatus = ui->labelStatus;
    labelStatus->setText("Введите данные для подключения.");

    // Проверяем, что клиент не нулевой, прежде чем подключаться
    if (m_client) {
        connect(m_client, &GameClient::connected, this, &ConnectWindow::onConnected);
        connect(m_client, &GameClient::disconnected, this, &ConnectWindow::onDisconnected);
        connect(m_client, &GameClient::dataReceived, this, &ConnectWindow::onDataReceived);
        connect(m_client, &GameClient::errorOccurred, this, [this](const QString& error) {
            QMessageBox::critical(this, "Ошибка подключения", error);
            setStatusText("Ошибка: " + error);
        });
    }
}

ConnectWindow::~ConnectWindow()
{
    // Убеждаемся, что отписались от сигналов при удалении окна
    disconnectFromClient();
    delete ui;
}

void ConnectWindow::setStatusText(const QString& text) {
    if (labelStatus) {
        labelStatus->setText(text);
    }
}

void ConnectWindow::setConnectButtonEnabled(bool enabled) {
    ui->btnConnect->setEnabled(enabled);
}

void ConnectWindow::disconnectFromClient()
{
    if (m_client) {
        // Отключаем все сигналы от m_client, которые были подключены к this (ConnectWindow)
        m_client->disconnect(this);
    }
}

void ConnectWindow::on_btnConnect_clicked()
{
    QString ip = ui->lineEditIP->text().trimmed();
    quint16 port = ui->lineEditPort->text().toUShort();

    if (ip.isEmpty() || port == 0) {
        QMessageBox::warning(this, "Ошибка", "Введите корректные IP и порт.");
        return;
    }

    // Проверяем, что клиент существует
    if(m_client) {
        setStatusText("Подключение к серверу...");
        setConnectButtonEnabled(false);
        m_client->connectToServer(ip, port);
    }
}

void ConnectWindow::on_btnBack_clicked()
{
    emit backtoMenu();
    this->close();
}

void ConnectWindow::onConnected()
{
    setStatusText("Успешно подключено. Ожидание второго игрока...");
    emit connectedToServer();
}

void ConnectWindow::onDisconnected()
{
    // Теперь это сообщение будет появляться только если связь реально оборвалась
    // на этапе подключения, а не после завершения игры.
    QMessageBox::warning(this, "Отключено", "Соединение с сервером потеряно.");
    setStatusText("Соединение потеряно.");
    setConnectButtonEnabled(true);
}

void ConnectWindow::onDataReceived(const QByteArray &data)
{
    QString msg = QString::fromUtf8(data);
    qDebug() << "Received message:" << msg;

    if (msg.startsWith("ID:")) {
        playerID = msg.split(':').last().toInt();
    } else if (msg == "both connected") {
        onTwoPlayersConnected();
    }
}

void ConnectWindow::onTwoPlayersConnected()
{
    setStatusText("Оба игрока подключены. Можно начинать игру!");
    emit bothPlayersReady(playerID);
}
