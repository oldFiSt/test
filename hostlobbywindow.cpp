#include "hostlobbywindow.h"
#include "ui_hostlobbywindow.h"

HostLobbyWindow::HostLobbyWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HostLobbyWindow)
{
    ui->setupUi(this);
    setFixedSize(900, 200);
    connect(ui->btnCancel, &QPushButton::clicked, this, &HostLobbyWindow::cancelled);
}

HostLobbyWindow::~HostLobbyWindow()
{
    delete ui;
}

void HostLobbyWindow::setIpAndPort(const QString &ip, quint16 port)
{
    QString ipText = ip;
    if (ip.isEmpty() || ip == "0.0.0.0") {
        ipText = "127.0.0.1 (локальный)";
    }
    ui->labelIp->setText(QString("Сообщите другу ваш IP: %1 и Порт: %2").arg(ipText).arg(port));
}
