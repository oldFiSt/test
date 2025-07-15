#ifndef HOSTSETUPWINDOW_H
#define HOSTSETUPWINDOW_H

#include <QDialog>

namespace Ui {
class HostSetupWindow;
}

class HostSetupWindow : public QDialog
{
    Q_OBJECT

public:
    explicit HostSetupWindow(QWidget *parent = nullptr);
    ~HostSetupWindow();

signals:
    void hostGame(quint16 port);
    void backToMenu();

private slots:
    void onCreateClicked();

private:
    Ui::HostSetupWindow *ui;
};

#endif // HOSTSETUPWINDOW_H
