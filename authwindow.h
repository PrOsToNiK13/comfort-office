#ifndef AUTHWINDOW_H
#define AUTHWINDOW_H

#include <QWidget>

namespace Ui {
class AuthWindow;
}

class AuthWindow : public QWidget
{
    Q_OBJECT

public:
    explicit AuthWindow(QWidget *parent = nullptr);
    ~AuthWindow();

private slots:
    void UpdateFromBD();

    void on_action_StackedWidgetNewUserOpen_triggered();

    void on_pushButton_back_clicked();

    void on_pushButton_addNewUserInAuth_clicked();

    void on_pushButton_auth_clicked();

    void on_action_triggered();

    void on_action_2_triggered();

    void on_pushButton_clicked();

signals:
    void outName(QString name);

private:
    Ui::AuthWindow *ui;
};

#endif // AUTHWINDOW_H
