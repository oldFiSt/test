#ifndef MULTIPLAYERMODEWINDOW_H
#define MULTIPLAYERMODEWINDOW_H

#include <QDialog>

namespace Ui {
class MultiplayerModeWindow;
}

class MultiplayerModeWindow : public QDialog
{
    Q_OBJECT

public:
    explicit MultiplayerModeWindow(QWidget *parent = nullptr);
    ~MultiplayerModeWindow();

signals:
    void hostGameClicked();
    void joinGameClicked();
    void backClicked();

private:
    Ui::MultiplayerModeWindow *ui;
};

#endif // MULTIPLAYERMODEWINDOW_H
