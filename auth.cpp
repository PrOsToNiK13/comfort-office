#include "auth.h"
#include "ui_auth.h"
#include <QMessageBox>
#include <QMainWindow>
#include "database.h"
#include <QtSql>

auth::auth(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::auth)
{
    mainwindow = new MainWindow();
    ui->setupUi(this);
    UpdateFromBD();
    ui->stackedWidget->setCurrentIndex(0);
    ui->lineEdit_password->setEchoMode(QLineEdit::Password);
    connect(this, &auth::outName, mainwindow, &MainWindow::AuthResults);
}

QString name, password;
bool TEMP = false; //переменная авторизации

auth::~auth()
{
    delete ui;
}

void auth::UpdateFromBD()
{
    ui->comboBox_user->clear();
    ui->comboBox_user->addItem("");
    QSqlQuery query("SELECT * FROM users");
    QSqlRecord rec = query.record();
    while(query.next()){
        name = query.value(rec.indexOf("name")).toString();
        ui->comboBox_user->addItem(name);
    }
}

void auth::on_action_StackedWidgetNewUserOpen_triggered()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void auth::on_pushButton_back_clicked()
{
    ui->lineEdit->clear();
    ui->lineEdit_2->clear();
    UpdateFromBD();
    ui->stackedWidget->setCurrentIndex(0);

}

void auth::on_pushButton_addNewUserInAuth_clicked()
{
    bool isOK = true;
    if(ui->lineEdit->text() == "" || ui->lineEdit_2->text() == ""){
        isOK = false;
        QMessageBox::warning(this, "Ошибка", "Введите имя пользователя и пароль. \n");
    } else if(ui->lineEdit->text().count() < 6){
        isOK = false;
        QMessageBox::warning(this, "Ошибка", "Имя пользователя должно содержать 6 и более символов. \n");
    } else if(ui->lineEdit_2->text().count() < 3){
        isOK = false;
        QMessageBox::warning(this, "Ошибка", "Пароль должен содержать 3 и более символов. \n");
    }
    if(isOK){
        database::InsertToUsers(ui->lineEdit->text(), ui->lineEdit_2->text());
        QMessageBox msgBox;
        msgBox.setText("Пользователь " + ui->lineEdit->text() + " создан!      ");
        msgBox.exec();
        ui->lineEdit->clear();
        ui->lineEdit_2->clear();
        UpdateFromBD();
        ui->stackedWidget->setCurrentIndex(0);
    }
}


void auth::on_pushButton_auth_clicked()
{
    bool isOK = true;
    if( ui->comboBox_user->currentText() == "" && ui->lineEdit_password->text() == ""){
        isOK = false;
        QMessageBox::warning(this, "Ошибка", "Выберите пользователя и пароль. \n");
    }else if(ui->comboBox_user->currentText() == ""){
        isOK = false;
        QMessageBox::warning(this, "Ошибка", "Выберите пользователя для авторизации. \n");
    } else if(ui->lineEdit_password->text() == ""){
        isOK = false;
        QMessageBox::warning(this, "Ошибка", "Введите пароль. \n");
    }
    if(isOK){
         QSqlQuery query;
         query.prepare("SELECT * FROM users WHERE name LIKE :search;");
         query.bindValue(":search", ui->comboBox_user->currentText());
         if(!query.exec()){
             qDebug() << query.lastError().text();
         }
         QSqlRecord rec = query.record();
         query.first();
         if(ui->lineEdit_password->text() != query.value(rec.indexOf("password")).toString()){
             QMessageBox::critical(this, "Ошибка", "Неверный пароль! \n");
         } else{
            mainwindow->show();
            emit outName(ui->comboBox_user->currentText());
            this->close();
         }
    }
}


void auth::on_pushButton_2_clicked()
{
    mainwindow->show();
    emit outName("ADMIN");
    this->close();
}

