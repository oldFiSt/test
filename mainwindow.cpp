#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "secondwindow.h"
#include "connect_window.h"
#include "shipplacementwindow.h"
#include "battlewindow.h"
#include "GameClient.h"
#include "GameServer.h"
#include "multiplayermodewindow.h"
#include "hostlobbywindow.h"
#include "hostsetupwindow.h"
#include <QNetworkInterface>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , secondWindow(nullptr)
    , connectWindow(nullptr)
    , shipPlacementWindow(nullptr)
    , battleWindow(nullptr)
    , gameServer(nullptr)
    , gameClient(nullptr)
    , multiplayerModeWindow(nullptr)
    , hostLobbyWindow(nullptr)
    , hostSetupWindow(nullptr)
    , playerID(0)
    , m_iAmReady(false)
    , m_opponentIsReady(false)
{
    setFixedSize(800, 588);
    ui->setupUi(this);

    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::on_pushButton_clicked);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::on_pushButton_2_clicked);
    connect(ui->btnExitToDesktop, &QPushButton::clicked, this, &MainWindow::on_btnExitToDesktop_clicked);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::cleanupNetwork(bool force) {
    if (gameClient) {
        gameClient->deleteLater();
        gameClient = nullptr;
    }
    if (gameServer) {
        gameServer->deleteLater();
        gameServer = nullptr;
    }
    if (connectWindow && force) {
        connectWindow->deleteLater();
        connectWindow = nullptr;
    }
    m_iAmReady = false;
    m_opponentIsReady = false;
    m_myShips.clear();
}

void MainWindow::on_pushButton_clicked() {
    this->hide();
    if (!secondWindow) secondWindow = new SecondWindow(this);
    secondWindow->show();
}

void MainWindow::on_pushButton_2_clicked()
{
    this->hide();
    if (!multiplayerModeWindow) {
        multiplayerModeWindow = new MultiplayerModeWindow(this);
        connect(multiplayerModeWindow, &MultiplayerModeWindow::hostGameClicked, this, &MainWindow::onHostGameClicked);
        connect(multiplayerModeWindow, &MultiplayerModeWindow::joinGameClicked, this, &MainWindow::onJoinGameClicked);
        connect(multiplayerModeWindow, &MultiplayerModeWindow::backClicked, this, [this](){ onMultiplayerBackClicked(multiplayerModeWindow); });
    }
    multiplayerModeWindow->show();
}

void MainWindow::onHostGameClicked()
{
    multiplayerModeWindow->hide();

    cleanupNetwork(true);

    if (!hostSetupWindow) {
        hostSetupWindow = new HostSetupWindow(this);
        connect(hostSetupWindow, &HostSetupWindow::hostGame, this, &MainWindow::startServerAndLobby);
        connect(hostSetupWindow, &HostSetupWindow::backToMenu, this, [this](){
            hostSetupWindow->hide();
            multiplayerModeWindow->show();
            cleanupNetwork(true);
        });
    }
    hostSetupWindow->show();
}

void MainWindow::startServerAndLobby(quint16 port)
{
    hostSetupWindow->hide();

    gameServer = new GameServer(this);
    gameClient = new GameClient(this);

    // Добавлена обработка ошибок подключения
    connect(gameClient, &GameClient::errorOccurred, this, [this](const QString& error) {
        QMessageBox::critical(this, "Ошибка подключения", "Не удалось подключиться к серверу: " + error);
        if (hostLobbyWindow) hostLobbyWindow->hide();
        cleanupNetwork(true);
        hostSetupWindow->show();
    });

    // Проверка успешного запуска сервера
    if (!gameServer->startServer(port)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось запустить сервер на порту " + QString::number(port));
        cleanupNetwork(true);
        hostSetupWindow->show();
        return;
    }

    connect(gameClient, &GameClient::dataReceived, this, &MainWindow::handleNetworkData);

    // Подключаемся к собственному серверу
    gameClient->connectToServer("127.0.0.1", port);

    if (!hostLobbyWindow) {
        hostLobbyWindow = new HostLobbyWindow(this);
        connect(hostLobbyWindow, &HostLobbyWindow::cancelled, this, [this](){
            hostLobbyWindow->hide();
            cleanupNetwork(true);
            this->show();
        });
    }

    QString ipAddress;
    const QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    for (const QHostAddress &address : ipAddressesList) {
        if (address != QHostAddress::LocalHost && address.toIPv4Address()) {
            ipAddress = address.toString();
            break;
        }
    }
    if (ipAddress.isEmpty()) ipAddress = "127.0.0.1";
    hostLobbyWindow->setIpAndPort(ipAddress, port);
    hostLobbyWindow->show();
}

void MainWindow::onJoinGameClicked()
{
    multiplayerModeWindow->hide();

    cleanupNetwork(true);

    gameClient = new GameClient(this);
    connect(gameClient, &GameClient::errorOccurred, this, [this](const QString& error) {
        QMessageBox::critical(this, "Ошибка подключения", "Не удалось подключиться к серверу: " + error);
        if (connectWindow) {
            connectWindow->setStatusText("Ошибка: " + error);
            connectWindow->setConnectButtonEnabled(true);
        }
    });
    connect(gameClient, &GameClient::dataReceived, this, &MainWindow::handleNetworkData);

    connectWindow = new ConnectWindow(gameClient, this);
    connect(connectWindow, &ConnectWindow::backtoMenu, this, [this]() {
        cleanupNetwork(true);
        multiplayerModeWindow->show();
    });
    connect(connectWindow, &ConnectWindow::bothPlayersReady, this, &MainWindow::proceedToShipPlacement);

    connectWindow->show();
}

void MainWindow::onMultiplayerBackClicked(QWidget* senderWindow) {
    if (senderWindow) senderWindow->hide();
    this->show();
    cleanupNetwork(true);
}

void MainWindow::handleNetworkData(const QByteArray &data)
{
    QString msg = QString::fromUtf8(data);
    qDebug() << "Network data received:" << msg;

    if (msg.startsWith("ID:")) {
        this->playerID = msg.split(':').last().toInt();
    } else if (msg == "both connected") {
        if (hostLobbyWindow) hostLobbyWindow->hide();
        if (playerID == 1) {
            proceedToShipPlacement(this->playerID);
        }
    } else if (msg == "SHIPS_READY") {
        m_opponentIsReady = true;
        tryStartMultiplayerGame();
    }
}

void MainWindow::proceedToShipPlacement(int id)
{
    this->playerID = id;

    if (connectWindow) {
        connectWindow->disconnectFromClient();
        connectWindow->hide();
    }
    if (hostLobbyWindow) {
        hostLobbyWindow->hide();
    }

    if (shipPlacementWindow) {
        shipPlacementWindow->deleteLater();
    }
    shipPlacementWindow = new ShipPlacementWindow(this);
    connect(shipPlacementWindow, &ShipPlacementWindow::startGame, this, &MainWindow::onPlayerShipsPlaced);
    connect(shipPlacementWindow, &ShipPlacementWindow::backToConnectWindow, this, [this]() {
        shipPlacementWindow->hide();
        cleanupNetwork(true);
        multiplayerModeWindow->show();
    });

    shipPlacementWindow->show();
}

void MainWindow::onPlayerShipsPlaced(const QList<QList<QPoint>>& ships)
{
    m_myShips = ships;
    m_iAmReady = true;

    if(gameClient) {
        gameClient->sendData("SHIPS_READY");
    }

    tryStartMultiplayerGame();
}

void MainWindow::tryStartMultiplayerGame()
{
    if (m_iAmReady && m_opponentIsReady) {
        if (shipPlacementWindow) {
            shipPlacementWindow->hide();
        }

        if (battleWindow) {
            battleWindow->deleteLater();
            battleWindow = nullptr;
        }

        if (connectWindow) {
            connectWindow->deleteLater();
            connectWindow = nullptr;
        }

        battleWindow = new BattleWindow(m_myShips, gameClient, this->playerID, this, this);
        battleWindow->show();
    }
}

void MainWindow::on_btnExitToDesktop_clicked()
{
    QApplication::quit();
}
