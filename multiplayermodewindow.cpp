#include "multiplayermodewindow.h"
#include "ui_multiplayermodewindow.h"

MultiplayerModeWindow::MultiplayerModeWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MultiplayerModeWindow)
{
    setFixedSize(800, 588);
    ui->setupUi(this);
    connect(ui->btnHost, &QPushButton::clicked, this, &MultiplayerModeWindow::hostGameClicked);
    connect(ui->btnJoin, &QPushButton::clicked, this, &MultiplayerModeWindow::joinGameClicked);
    connect(ui->btnBack, &QPushButton::clicked, this, &MultiplayerModeWindow::backClicked);
}

MultiplayerModeWindow::~MultiplayerModeWindow()
{
    delete ui;
}
