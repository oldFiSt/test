#ifndef HOSTLOBBYWINDOW_H
#define HOSTLOBBYWINDOW_H

#include <QDialog>

namespace Ui {
class HostLobbyWindow;
}

class HostLobbyWindow : public QDialog
{
    Q_OBJECT

public:
    explicit HostLobbyWindow(QWidget *parent = nullptr);
    ~HostLobbyWindow();
    void setIpAndPort(const QString& ip, quint16 port);

signals:
    void cancelled();

private:
    Ui::HostLobbyWindow *ui;
};

#endif // HOSTLOBBYWINDOW_H
