#ifndef AUTH_H
#define AUTH_H

#include <QDialog>
#include "mainwindow.h"

namespace Ui {
class auth;
}

class auth : public QMainWindow
{
    Q_OBJECT

public:
    explicit auth(QWidget *parent = nullptr);
    ~auth();

private slots:
    void UpdateFromBD();

    void on_action_StackedWidgetNewUserOpen_triggered();

    void on_pushButton_back_clicked();

    void on_pushButton_addNewUserInAuth_clicked();

    void on_pushButton_auth_clicked();

    void on_pushButton_2_clicked();

signals:
    void outName(QString name);
private:
    Ui::auth *ui;
    MainWindow *mainwindow;
};

#endif // AUTH_H
