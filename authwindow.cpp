#include "authwindow.h"
#include "ui_authwindow.h"
#include <QMessageBox>
#include "database.h"
#include <QtSql>
#include <QSettings>
#include <QFileDialog>

AuthWindow::AuthWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AuthWindow)
{
    ui->setupUi(this);
    UpdateFromBD();
    ui->stackedWidget->setCurrentIndex(0);
}

AuthWindow::~AuthWindow()
{
    delete ui;
}

QString name, password;

void AuthWindow::UpdateFromBD()
{
    ui->comboBox_user->clear();
    QSqlQuery query("SELECT COUNT(id) FROM users;");
    query.exec(); query.first();
    if(query.value(0).toInt() == 0){
        ui->comboBox_user->addItem("<Пусто>");
    } else {
        query.exec("SELECT * FROM users");
        QSqlRecord rec = query.record();
        while(query.next()){
            name = query.value(rec.indexOf("name")).toString();
            ui->comboBox_user->addItem(name);
        }
    }
}

void AuthWindow::on_action_StackedWidgetNewUserOpen_triggered()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void AuthWindow::on_pushButton_back_clicked()
{
    ui->lineEdit->clear();
    ui->lineEdit_2->clear();
    UpdateFromBD();
    ui->stackedWidget->setCurrentIndex(0);

}

void AuthWindow::on_pushButton_addNewUserInAuth_clicked()
{
    bool isOK = true;
    if(ui->lineEdit->text() == "" || ui->lineEdit_2->text() == ""){
        isOK = false;
        QMessageBox::warning(this, "Ошибка", "<FONT COLOR = '#000000'>Введите имя пользователя и пароль. </FONT> \n");
    } else if(ui->lineEdit->text().count() < 6){
        isOK = false;
        QMessageBox::warning(this, "Ошибка", "<FONT COLOR = '#000000'>Имя пользователя должно содержать 6 и более символов. </FONT> \n");
    } else if(ui->lineEdit_2->text().count() < 3){
        isOK = false;
        QMessageBox::warning(this, "Ошибка", "<FONT COLOR = '#000000'>Пароль должен содержать 3 и более символов.</FONT> \n");
    }
    if(isOK){
        database::InsertToUsers(ui->lineEdit->text(), ui->lineEdit_2->text());
        QMessageBox::information(this, "Успех!", "<FONT COLOR = '#000000'>Пользователь " + ui->lineEdit->text() + " создан! </FONT>");
        ui->lineEdit->clear();
        ui->lineEdit_2->clear();
        UpdateFromBD();
        ui->stackedWidget->setCurrentIndex(0);
    }
}

void AuthWindow::on_pushButton_auth_clicked()
{
    bool isOK = true;
    if( ui->comboBox_user->currentText() == "" && ui->lineEdit_password->text() == ""){
        isOK = false;
        QMessageBox::warning(this, "Ошибка", "<FONT COLOR = '#000000'>Выберите пользователя и пароль. </FONT> \n");
    }else if(ui->comboBox_user->currentText() == ""){
        isOK = false;
        QMessageBox::warning(this, "Ошибка", "<FONT COLOR = '#000000'>Выберите пользователя для авторизации. </FONT> \n");
    } else if(ui->lineEdit_password->text() == ""){
        isOK = false;
        QMessageBox::warning(this, "Ошибка", "<FONT COLOR = '#000000'>Введите пароль.</FONT> \n");
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
             QMessageBox::critical(this, "Ошибка", "<FONT COLOR = '#000000'>Неверный пароль!</FONT> \n");
         } else{
            emit outName(ui->comboBox_user->currentText());
         }
    }
}

void AuthWindow::on_action_triggered()
{
    QMessageBox::information(this, "", "<FONT COLOR = '#000000'>Выберите ПУТЬ к файлу БД</FONT>");
    QString DBPath = QFileDialog::getExistingDirectory(nullptr, "Выберите путь к базе данных", "C:\\") + "/clients.db";
    if(DBPath == "/clients.db"){
        QMessageBox::warning(this, "Ошибка", "<FONT COLOR = '#000000'>Вы не указали путь к БД!</FONT>");
    } else {
        QSettings settings("Comfort-office", "Comfort-CRM");
        settings.beginGroup("Main");
        settings.setValue("DataBasePath", DBPath);
        settings.endGroup();
        if (!database::connectToDataBase(DBPath)){
            QMessageBox::warning(this, "Ошибка", "<FONT COLOR = '#000000'>Не удалось подключиться к базе данных по указанному пути.</FONT>");
        }
        UpdateFromBD();
    }
}



void AuthWindow::on_action_2_triggered()
{
    if(ui->stackedWidget->currentIndex() != 2){
        ui->stackedWidget->setCurrentIndex(2);
        ui->radioButton->setChecked(true);
    }
}

void AuthWindow::on_pushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

