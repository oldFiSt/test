#include "hostsetupwindow.h"
#include "ui_hostsetupwindow.h" // <-- Добавьте и сюда, на всякий случай
#include <QMessageBox>

HostSetupWindow::HostSetupWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HostSetupWindow)
{
    ui->setupUi(this);
    setFixedSize(400, 250);
    connect(ui->btnCreate, &QPushButton::clicked, this, &HostSetupWindow::onCreateClicked);
    // ИСПРАВЛЕНИЕ ОПЕЧАТКИ
    connect(ui->btnBack, &QPushButton::clicked, this, &HostSetupWindow::backToMenu);
}

HostSetupWindow::~HostSetupWindow()
{
    delete ui;
}

void HostSetupWindow::onCreateClicked()
{
    bool ok;
    quint16 port = ui->lineEditPort->text().toUShort(&ok);
    if (!ok || port == 0) {
        QMessageBox::warning(this, "Ошибка", "Введите корректный номер порта (1-65535).");
        return;
    }
    emit hostGame(port);
}
