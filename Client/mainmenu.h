#ifndef MAINMENU_H
#define MAINMENU_H

#include <QWidget>
#include <QString>

namespace Ui {
class MainMenu;
}

class MainMenu : public QWidget
{
    Q_OBJECT

public:
explicit MainMenu(const QString &username, QWidget *parent = nullptr);
    ~MainMenu();

private slots:
    void on_playButton_clicked();

    void on_profileButton_clicked();

    void on_exitButton_clicked();

private:
    Ui::MainMenu *ui;
    QString m_username;
};

#endif // MAINMENU_H
