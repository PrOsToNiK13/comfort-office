 #include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSql>
#include "database.h"
#include <QMessageBox>
#include "dynamicFields.h"
#include <QFileDialog>
#include <QDesktopServices>
#include <QCompleter>
#include <QList>
#include <QCloseEvent>
#include <QScreen>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    connect(&auth, &AuthWindow::outName, this, &MainWindow::AuthResults);
    connect(this, &MainWindow::OpenInfoAboutDublicates, &dublicatesOutput, &DublicatesOutput::updateInformation);

    ui->setupUi(this);
    ui->stackedWidget_main->setCurrentIndex(0);
    MainThemeOfActiveClients();
    usernames = new QSqlQueryModel(this);
    ui->comboBox_redactClientUsername->setModel(usernames);

    ui->lineEdit_newClientPhone->setInputMask("+7\\(999\\)999\\-99\\-99;_");
    ui->lineEdit_newClientPhone->installEventFilter(this);
    ui->lineEdit_newProviderPhone->setInputMask("+7\\(999\\)999\\-99\\-99;_");
    ui->lineEdit_newProviderPhone->installEventFilter(this);
    ui->lineEdit_redactClientPhone->setInputMask("+7\\(999\\)999\\-99\\-99;_");
    ui->lineEdit_redactClientPhone->installEventFilter(this);
    ui->lineEdit_redactProviderPhone->setInputMask("+7\\(999\\)999\\-99\\-99;_");
    ui->lineEdit_redactProviderPhone->installEventFilter(this);

    clientsModel = new QSqlQueryModel(this);
    SortFilterProxyClientsModel = new QSortFilterProxyModel(this);
    SortFilterProxyClientsModel->setSourceModel(clientsModel);
    SortFilterProxyClientsModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    SortFilterProxyClientsModel->sort(1);
    ui->listView->setModel(SortFilterProxyClientsModel);
    ui->listView->setModelColumn(1);

    potencialClientsModel = new QSqlQueryModel(this);
    SortFilterProxyPotencialClientsModel = new QSortFilterProxyModel(this);
    SortFilterProxyPotencialClientsModel->setSourceModel(potencialClientsModel);
    SortFilterProxyPotencialClientsModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    SortFilterProxyPotencialClientsModel->sort(1);

    dublicatesModel = new QSqlQueryModel(this);
    dublicatesProxy = new QSortFilterProxyModel(this);
    dublicatesProxy->setSourceModel(dublicatesModel);
}

QRegularExpression numberRegex ("^\\+\\d{1,1}\\(\\d{3,3}\\)\\d{3,3}\\-\\d{2,2}\\-\\d{2,2}$");
QRegularExpressionValidator numberValidator(numberRegex);
QFont defaultfont = QFont("Segoe UI Variable Display Semib",12,2);
QString username;


bool ClientIsOpened = false; //переменная открытой инфы о компании
bool newNoteOpen = false; bool checkLeftMenuOpened = true; bool ContactIsOpened = false, newCliendPhoneOpened = false, redactClientPhoneOpened = false,
        newClientAddressIsOpened = false, redactClientAddressIsOpened = false;

bool clientsAreOpened = true, potencialClientsAreOpened = false, providersAreOpened = false;  //текущий выбор main
bool activeClientsAreOpened = true, inactiveCLientsAreOpened = false; //подвыбор клиентов

int tempListView_activeClients_id = 0, tempListView_inactiveClients_id = 0, tempListView_potencialClients_id = 0, tempListView_providers_id = 0; //id выбранной компании
int tempComboBoxUser;
int lastComboBoxSelecteUser = 0;

QList<QString> newClientFields, redactClientFields, newClientPhones, redactClientPhones, newClientAddresses, redactClientAddresses; //поля новой/редактируемой компании
QList<QList<QString>> findedPhoneDublicates, phonesWithDublicates;

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::display()
{
    auth.show();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    ClearNotes();
    ClearDocuments();
    ClearNewClientPhones(); ClearRedactClientPhones(); ClearOutputClientPhones();
    ClearClientAddresses(1); ClearClientAddresses(2); ClearClientAddresses(3);
    event->accept();
}

bool MainWindow::eventFilter(QObject *object, QEvent *event)
{
    if(object == ui->lineEdit_newClientPhone && event->type() == QEvent::MouseButtonPress){
        QTimer::singleShot(0,ui->lineEdit_newClientPhone,[this]
        {
            QString line = ui->lineEdit_newClientPhone->displayText();
            ui->lineEdit_newClientPhone->setFocus();
            bool isValid = true;
            for(int i = 0; i<=15; i++){
                if(line[i] == '_'){
                    ui->lineEdit_newClientPhone->setCursorPosition(i);
                    isValid = false;
                    break;
                }
            }
            if(isValid){
                ui->lineEdit_newClientPhone->setCursorPosition(16);
            }
        });
    }
    if(object == ui->lineEdit_redactClientPhone && event->type() == QEvent::MouseButtonPress){
        QTimer::singleShot(0,ui->lineEdit_redactClientPhone,[this]
        {
            QString line = ui->lineEdit_redactClientPhone->displayText();
            ui->lineEdit_redactClientPhone->setFocus();
            bool isValid = true;
            for(int i = 0; i<=15; i++){
                if(line[i] == '_'){
                    ui->lineEdit_redactClientPhone->setCursorPosition(i);
                    isValid = false;
                    break;
                }
            }
            if(isValid){
                ui->lineEdit_redactClientPhone->setCursorPosition(16);
            }
        });
    }
    if(object == ui->lineEdit_newProviderPhone && event->type() == QEvent::MouseButtonPress){
        QTimer::singleShot(0,ui->lineEdit_newProviderPhone,[this]
        {
            QString line = ui->lineEdit_newProviderPhone->displayText();
            ui->lineEdit_newProviderPhone->setFocus();
            bool isValid = true;
            for(int i = 0; i<=15; i++){
                if(line[i] == '_'){
                    ui->lineEdit_newProviderPhone->setCursorPosition(i);
                    isValid = false;
                    break;
                }
            }
            if(isValid){
                ui->lineEdit_newProviderPhone->setCursorPosition(16);
            }
        });
    }
    if(object == ui->lineEdit_redactProviderPhone && event->type() == QEvent::MouseButtonPress){
        QTimer::singleShot(0,ui->lineEdit_redactProviderPhone,[this]
        {
            QString line = ui->lineEdit_redactProviderPhone->displayText();
            ui->lineEdit_redactProviderPhone->setFocus();
            bool isValid = true;
            for(int i = 0; i<=15; i++){
                if(line[i] == '_'){
                    ui->lineEdit_redactProviderPhone->setCursorPosition(i);
                    isValid = false;
                    break;
                }
            }
            if(isValid){
                ui->lineEdit_redactProviderPhone->setCursorPosition(16);
            }
        });
    }

    return QMainWindow::eventFilter(object, event);
}

void MainWindow::AuthResults(QString name)
{
    UpdateComboBoxUsers();
    auth.close();
    this->show();
    username = name;
    QSqlQuery query;
    query.prepare("SELECT showAllClientsInDefault FROM users WHERE name = :user");
    query.bindValue(":user", username);
    query.exec();
    query.next();
    int showAllClientsInDefault = query.value(0).toInt();
    if(showAllClientsInDefault == 0){
        for(int i = 0; i <= ui->comboBox_selectedUser->count(); i++){
            if(ui->comboBox_selectedUser->itemText(i) == username){
                ui->comboBox_selectedUser->setCurrentIndex(i);
                lastComboBoxSelecteUser = i;
                break;
            }
        }
    } else {
        ui->comboBox_selectedUser->setCurrentIndex(0);
    }
    UpdateActiveClients();
    QMessageBox box(QMessageBox::Information, "Авторизация успешна!", "<FONT COLOR = '#000000'> Добро пожаловать, " + name  + "</FONT>", QMessageBox::NoButton, this);
    QScreen *screen = QGuiApplication::primaryScreen();
    const QRect screen_rect = screen->geometry();
    QPoint point = screen_rect.center() - box.geometry().bottomRight() - QPoint(6, 25);
    box.move(point);
    box.exec();
}




// ===Менюшки / сброс виджетов===
void MainWindow::MainThemeOfActiveClients(){
    ui->stackedWidget_clients->setCurrentIndex(0);
    ui->label_CMPname->setText("Активные клиенты");

    ui->pushButton_Update->hide(); ui->pushButton_DeleteClient->hide();
    ui->scrollArea->hide();
    ui->textEdit->hide(); ui->pushButton_saveNote->hide();//заметки

    ClearDocuments();
    ClearOutputClientPhones();
    ClearNotes();
    ClearClientAddresses(0);

    newNoteOpen = false;
    ClientIsOpened = false;
}

void MainWindow::MainThemeOfInactiveClients()
{

    ui->stackedWidget_clients->setCurrentIndex(0);
    ui->label_CMPname->setText("Неактивные клиенты");

    ui->pushButton_Update->hide(); ui->pushButton_DeleteClient->hide();
    ui->scrollArea->hide();
    ui->textEdit->hide(); ui->pushButton_saveNote->hide();//заметки

    ClearDocuments();
    ClearOutputClientPhones();
    ClearNotes();
    ClearClientAddresses(0);

    newNoteOpen = false;
    ClientIsOpened = false;
}

void MainWindow::MainThemeOfPotencialClients()
{
    ui->stackedWidget_clients->setCurrentIndex(0);
    ui->label_CMPname->setText("Потенциальные клиенты");

    ui->pushButton_Update->hide(); ui->pushButton_DeleteClient->hide();
    ui->scrollArea->hide();
    ui->textEdit->hide(); ui->pushButton_saveNote->hide();//заметки

    ClearDocuments();
    ClearOutputClientPhones();
    ClearNotes();
    ClearClientAddresses(0);

    newNoteOpen = false;
    ClientIsOpened = false;
}

void MainWindow::MainThemeOfProviders()
{
    ui->stackedWidget_clients->setCurrentIndex(0);
    ui->label_CMPname->setText("Поставщики");

    ui->pushButton_Update->hide(); ui->pushButton_DeleteClient->hide();
    ui->scrollArea->hide();
    ui->textEdit->hide(); ui->pushButton_saveNote->hide();//заметки

    ClearDocuments();
    ClearOutputClientPhones();
    ClearNotes();
    ClearClientAddresses(0);

    newNoteOpen = false;
    ClientIsOpened = false;
}

void MainWindow::on_pushButton_MainMenu_clicked()
{
    if(checkLeftMenuOpened){
        QPixmap pix(":/images/images/arrow_right.png");
        QIcon icon(pix);
        ui->pushButton_MainMenu->setIcon(icon);
        ui->pushButton_MainMenu->setGeometry(5,10,20,24);
        ui->frame_2->hide(); ui->pushButton_potencial_clients->hide(); ui->pushButton_providers->hide();
        ui->widget->setGeometry(1, 1, 30, ui->widget->y());
        ui->widget->setMinimumHeight(600);
        ui->widget->setMinimumWidth(30);
        checkLeftMenuOpened = false;
    } else {
        QPixmap pix(":/images/images/arrow_left.png");
        QIcon icon(pix);
        ui->pushButton_MainMenu->setIcon(icon);
        ui->frame_2->show(); ui->pushButton_potencial_clients->show(); ui->pushButton_providers->show();
        ui->pushButton_MainMenu->setGeometry(100,10,20,24);
        ui->widget->setMinimumHeight(600);
        ui->widget->setMinimumWidth(140);
        checkLeftMenuOpened = true;
    }
}

void MainWindow::on_pushButton_clients_clicked()
{
    if(!clientsAreOpened){
        bool isOK = true;
        if(ui->stackedWidget_clients->currentIndex() == 1){
            QMessageBox::StandardButton reply =
            QMessageBox::question(this, "Подтвердите действие", "<FONT COLOR = '#000000'>Отменить создание новой компании? Внесённые данные не сохранятся! </FONT>");
            if (reply == QMessageBox::Yes){
                if(newCliendPhoneOpened == true){
                    on_pushButton_AddNewClientPhone_clicked();
                }
                if(newClientAddressIsOpened == true){
                    on_pushButton_AddNewClientAddress_clicked();
                }
                ui->pushButton_CreateNewClient->setStyleSheet("QPushButton{color:white;"
                                                              "background-color:rgb(91,95,199);"
                                                              "border-radius:3px;"
                                                              "}"
                                                              "QPushButton:hover{"
                                                              "color:white;"
                                                              "background-color:rgb(68,71,145);"
                                                              "border-radius:3px;"
                                                              "}");
                ui->pushButton_CreateNewClient->setIcon(QIcon(":/images/images/plus-8-12.png"));
                ui->lineEdit_2->clear(); ui->lineEdit_newClientPhone->clear();
                newClientPhones.clear(); newClientFields.clear(); newClientAddresses.clear();
                ui->stackedWidget_clients->setCurrentIndex(0);
            } else {
                isOK = false;
            }
        } else if(ui->stackedWidget_clients->currentIndex() == 2 && isOK){
            QMessageBox::StandardButton reply =
            QMessageBox::question(this, "Подтвердите действие", "<FONT COLOR = '#000000'>Отменить редактирование контакта? Внесённые изменения не сохранятся! </FONT>");
            if (reply == QMessageBox::Yes){
                on_pushButton_CancelRedactOfClient_clicked();
            } else {
                isOK = false;
            }
        }

        if(isOK){
            if(ui->stackedWidget_main->currentIndex() != 0){
                ui->stackedWidget_main->setCurrentIndex(0);
            }
            ui->frame->show();
            ui->label_create_x->setText("Создать компанию:");
            ui->pushButton_clients->setStyleSheet("QPushButton{color:white;"
                                                            "background-color:rgb(91,95,199);"
                                                            "border: 2px solid rgb(91,95,199);"
                                                            "border-radius:3px;"
                                                            "}"
                                                            "QPushButton:hover{"
                                                            "color:white;"
                                                            "background-color:rgb(68,71,145);"
                                                            "border: 2px solid rgb(91,95,199);"
                                                            "border-radius:3px;"
                                                            "}");
            ui->pushButton_activeClients->show();
            ui->pushButton_inactiveClients->show();
            ui->frame_2->setStyleSheet("border: 2px solid rgb(91,95,199);"
                                       "border-radius:3px;");
            if(potencialClientsAreOpened){
                ui->pushButton_potencial_clients->setStyleSheet("QPushButton{color:black;"
                                                      "background-color:white;"
                                                      "border: 2px solid rgb(91,95,199);"
                                                      "border-radius:3px;"
                                                      "}"
                                                      "QPushButton:hover{"
                                                      "Color:black;"
                                                      "background-color:rgb(217,217,217);"
                                                      "border: 2px solid rgb(91,95,199);"
                                                      "border-radius:3px;"
                                                      "}");
                potencialClientsAreOpened = false;
            } else if(providersAreOpened){
                ui->pushButton_providers->setStyleSheet("QPushButton{color:black;"
                                                        "background-color:white;"
                                                        "border: 2px solid rgb(91,95,199);"
                                                        "border-radius:3px;"
                                                        "}"
                                                        "QPushButton:hover{"
                                                        "Color:black;"
                                                        "background-color:rgb(217,217,217);"
                                                        "border: 2px solid rgb(91,95,199);"
                                                        "border-radius:3px;"
                                                        "}");
                providersAreOpened = false;
            }
            ui->verticalLayout_45->setContentsMargins(0,0,0,6);
            clientsAreOpened = true;

            bool finded = false;
            if(activeClientsAreOpened){
                UpdateActiveClients();
                if(tempListView_activeClients_id != 0){
                    for(int row = 0; row < clientsModel->rowCount(); row++){
                        if(ui->listView->model()->data(ui->listView->model()->index(row, 0)).toInt() == tempListView_activeClients_id){
                            ui->listView->setCurrentIndex(ui->listView->model()->index(row, 1));
                            on_listView_clicked(ui->listView->model()->index(row, 1));
                            ui->listView->scrollTo(ui->listView->model()->index(row, 1)); 
                            finded = true;
                            break;
                        }
                    }
                    if(!finded){
                        MainThemeOfActiveClients();
                    }
                } else {
                    MainThemeOfActiveClients();
                }
            } else if (inactiveCLientsAreOpened){
                UpdateInactiveClients();
                if(tempListView_inactiveClients_id != 0){
                    for(int row = 0; row < clientsModel->rowCount(); row++){
                        if(ui->listView->model()->data(ui->listView->model()->index(row, 0)).toInt() == tempListView_inactiveClients_id){
                            ui->listView->setCurrentIndex(ui->listView->model()->index(row, 1));
                            on_listView_clicked(ui->listView->model()->index(row, 1));
                            ui->listView->scrollTo(ui->listView->model()->index(row, 1));
                            finded = true;
                            break;
                        }
                    }
                    if(!finded){
                        MainThemeOfInactiveClients();
                    }
                } else {
                    MainThemeOfInactiveClients();
                }
            }
        }
    }
}

void MainWindow::on_pushButton_activeClients_clicked()
{
    if(!activeClientsAreOpened){
        bool isOK = true;
        if(ui->stackedWidget_clients->currentIndex() == 1){
            QMessageBox::StandardButton reply =
            QMessageBox::question(this, "Подтвердите действие", "<FONT COLOR = '#000000'>Отменить создание новой компании? Внесённые данные не сохранятся! </FONT>");
            if (reply == QMessageBox::Yes){
                on_pushButton_CreateNewClient_clicked();
            } else {
                isOK = false;
            }
        } else if(ui->stackedWidget_clients->currentIndex() == 2){
            QMessageBox::StandardButton reply =
            QMessageBox::question(this, "Подтвердите действие", "<FONT COLOR = '#000000'>Отменить редактирование компании? Внесённые изменения не сохранятся! </FONT>");
            if (reply == QMessageBox::Yes){
                on_pushButton_CancelRedactOfClient_clicked();
            } else {
                isOK = false;
            }
        }
        if(isOK){
            ui->pushButton_activeClients->setStyleSheet("QPushButton{color:white;"
                                                            "background-color:rgb(91,95,199);"
                                                            "border: 2px solid rgb(91,95,199);"
                                                            "border-radius:3px;"
                                                            "}"
                                                            "QPushButton:hover{"
                                                            "color:white;"
                                                            "background-color:rgb(68,71,145);"
                                                            "border: 2px solid rgb(91,95,199);"
                                                            "border-radius:3px;"
                                                            "}");
            ui->pushButton_inactiveClients->setStyleSheet("QPushButton{color:black;"
                                                  "background-color:white;"
                                                  "border: 2px solid rgb(91,95,199);"
                                                  "border-radius:3px;"
                                                  "}"
                                                  "QPushButton:hover{"
                                                  "Color:black;"
                                                  "background-color:rgb(217,217,217);"
                                                  "border: 2px solid rgb(91,95,199);"
                                                  "border-radius:3px;"
                                                  "}");
            MainThemeOfActiveClients();
            activeClientsAreOpened = true;
            inactiveCLientsAreOpened = false;
            UpdateActiveClients();
            bool finded = false;
            if(tempListView_activeClients_id == 0){
                MainThemeOfActiveClients();
            } else {
                for(int row = 0; row < clientsModel->rowCount(); row++){
                    if(ui->listView->model()->data(ui->listView->model()->index(row, 0)).toInt() == tempListView_activeClients_id){
                        ui->listView->setCurrentIndex(ui->listView->model()->index(row, 1));
                        on_listView_clicked(ui->listView->model()->index(row, 1));
                        ui->listView->scrollTo(ui->listView->model()->index(row, 1));
                        finded = true;
                        break;
                    }
                }
                if(!finded){
                    MainThemeOfActiveClients();
                }
            }
        }
    }
}

void MainWindow::on_pushButton_inactiveClients_clicked()
{
    if(!inactiveCLientsAreOpened){
        bool isOK = true;
        if(ui->stackedWidget_clients->currentIndex() == 1){
            QMessageBox::StandardButton reply =
            QMessageBox::question(this, "Подтвердите действие", "<FONT COLOR = '#000000'>Отменить создание новой компании? Внесённые данные не сохранятся! </FONT>");
            if (reply == QMessageBox::Yes){
                on_pushButton_CreateNewClient_clicked();
            } else {
                isOK = false;
            }
        } else if(ui->stackedWidget_clients->currentIndex() == 2){
            QMessageBox::StandardButton reply =
            QMessageBox::question(this, "Подтвердите действие", "<FONT COLOR = '#000000'>Отменить редактирование компании? Внесённые изменения не сохранятся! </FONT>");
            if (reply == QMessageBox::Yes){
                on_pushButton_CancelRedactOfClient_clicked();
            } else {
                isOK = false;
            }
        }
        if(isOK){
            ui->pushButton_inactiveClients->setStyleSheet("QPushButton{color:white;"
                                                            "background-color:rgb(91,95,199);"
                                                            "border: 2px solid rgb(91,95,199);"
                                                            "border-radius:3px;"
                                                            "}"
                                                            "QPushButton:hover{"
                                                            "color:white;"
                                                            "background-color:rgb(68,71,145);"
                                                            "border: 2px solid rgb(68,71,145);"
                                                            "border-radius:3px;"
                                                            "}");
            ui->pushButton_activeClients->setStyleSheet("QPushButton{color:black;"
                                                  "background-color:white;"
                                                  "border: 2px solid rgb(91,95,199);"
                                                  "border-radius:3px;"
                                                  "}"
                                                  "QPushButton:hover{"
                                                  "Color:black;"
                                                  "background-color:rgb(217,217,217);"
                                                  "border: 2px solid rgb(91,95,199);"
                                                  "border-radius:3px;"
                                                  "}");
            MainThemeOfInactiveClients();
            inactiveCLientsAreOpened = true;
            activeClientsAreOpened = false;
            UpdateInactiveClients();
            bool finded = false;
            if(tempListView_inactiveClients_id == 0){
                MainThemeOfInactiveClients();
            } else {
                for(int row = 0; row < clientsModel->rowCount(); row++){
                    if(ui->listView->model()->data(ui->listView->model()->index(row, 0)).toInt() == tempListView_inactiveClients_id){
                        ui->listView->setCurrentIndex(ui->listView->model()->index(row, 1));
                        on_listView_clicked(ui->listView->model()->index(row, 1));
                        ui->listView->scrollTo(ui->listView->model()->index(row, 1));
                        finded = true;
                        break;
                    }
                }
                if(!finded){
                    MainThemeOfInactiveClients();
                }
            }
        }
    }
}

void MainWindow::on_pushButton_potencial_clients_clicked()
{
    if(!potencialClientsAreOpened){
        bool isOK = true;
        if(ui->stackedWidget_clients->currentIndex() == 1){
            QMessageBox::StandardButton reply;
            if(providersAreOpened){
                reply = QMessageBox::question(this, "Подтвердите действие", "<FONT COLOR = '#000000'>Отменить создание нового поставщика? Внесённые данные не сохранятся! </FONT>");
            }else{
                reply = QMessageBox::question(this, "Подтвердите действие", "<FONT COLOR = '#000000'>Отменить создание новой компании? Внесённые данные не сохранятся! </FONT>");
            }
            if (reply == QMessageBox::Yes){
                if(newCliendPhoneOpened == true){
                    on_pushButton_AddNewClientPhone_clicked();
                }
                if(newClientAddressIsOpened == true){
                    on_pushButton_AddNewClientAddress_clicked();
                }
                ui->pushButton_CreateNewClient->setStyleSheet("QPushButton{color:white;"
                                                              "background-color:rgb(91,95,199);"
                                                              "border-radius:3px;"
                                                              "}"
                                                              "QPushButton:hover{"
                                                              "color:white;"
                                                              "background-color:rgb(68,71,145);"
                                                              "border-radius:3px;"
                                                              "}");
                ui->pushButton_CreateNewClient->setIcon(QIcon(":/images/images/plus-8-12.png"));
                ui->lineEdit_2->clear(); ui->lineEdit_newClientPhone->clear();
                newClientPhones.clear(); newClientFields.clear(); newClientAddresses.clear();
                ui->stackedWidget_clients->setCurrentIndex(0);
            } else {
                isOK = false;
            }
        } else if(ui->stackedWidget_clients->currentIndex() == 2){
            QMessageBox::StandardButton reply;
            if(providersAreOpened){
                reply = QMessageBox::question(this, "Подтвердите действие", "<FONT COLOR = '#000000'>Отменить редактирование поставщика? Внесённые данные не сохранятся! </FONT>");
            }else{
                reply = QMessageBox::question(this, "Подтвердите действие", "<FONT COLOR = '#000000'>Отменить редактирование клиента? Внесённые изменения не сохранятся! </FONT>");
            }
            if (reply == QMessageBox::Yes){
                on_pushButton_CancelRedactOfClient_clicked();
            } else {
                isOK = false;
            }
        }

        if(isOK){
            if(ui->stackedWidget_main->currentIndex() != 0){
                ui->stackedWidget_main->setCurrentIndex(0);
            }
            ui->frame->show();
            ui->label_create_x->setText("Создать компанию:");
            ui->pushButton_potencial_clients->setStyleSheet("QPushButton{color:white;"
                                                            "background-color:rgb(91,95,199);"
                                                            "border: 2px solid rgb(91,95,199);"
                                                            "border-radius:3px;"
                                                            "}"
                                                            "QPushButton:hover{"
                                                            "color:white;"
                                                            "background-color:rgb(68,71,145);"
                                                            "border: 2px solid rgb(68,71,145);"
                                                            "border-radius:3px;"
                                                            "}");
            if(clientsAreOpened){
                ui->pushButton_clients->setStyleSheet("QPushButton{color:black;"
                                                      "background-color:white;"
                                                      "border: 2px solid rgb(91,95,199);"
                                                      "border-radius:3px;"
                                                      "}"
                                                      "QPushButton:hover{"
                                                      "Color:black;"
                                                      "background-color:rgb(217,217,217);"
                                                      "border: 2px solid rgb(91,95,199);"
                                                      "border-radius:3px;"
                                                      "}");
                ui->verticalLayout_45->setContentsMargins(0,0,0,0);
                clientsAreOpened = false;
                ui->pushButton_activeClients->hide();
                ui->pushButton_inactiveClients->hide();
                ui->frame_2->setStyleSheet("border:0px;");
            } else if(providersAreOpened){
                ui->pushButton_providers->setStyleSheet("QPushButton{color:black;"
                                                      "background-color:white;"
                                                      "border: 2px solid rgb(91,95,199);"
                                                      "border-radius:3px;"
                                                      "}"
                                                      "QPushButton:hover{"
                                                      "Color:black;"
                                                      "background-color:rgb(217,217,217);"
                                                      "border: 2px solid rgb(91,95,199);"
                                                      "border-radius:3px;"
                                                      "}");
                tempListView_providers_id = ui->listView->model()->data(ui->listView->model()->index(ui->listView->currentIndex().row(), 0)).toInt();
                providersAreOpened = false;
            }

            potencialClientsAreOpened = true;
            bool finded = false;
            UpdatePotencialClients();
            if(tempListView_potencialClients_id != 0){
                for(int row = 0; row < clientsModel->rowCount(); row++){
                    if(ui->listView->model()->data(ui->listView->model()->index(row, 0)).toInt() == tempListView_potencialClients_id){
                        ui->listView->setCurrentIndex(ui->listView->model()->index(row, 1));
                        on_listView_clicked(ui->listView->model()->index(row, 1));
                        ui->listView->scrollTo(ui->listView->model()->index(row, 1));
                        finded = true;
                        break;
                    }
                }
                if(!finded){
                    MainThemeOfPotencialClients();
                }
            } else {
                MainThemeOfPotencialClients();
            }
        }
    }
}

void MainWindow::on_pushButton_providers_clicked()
{
    if(!providersAreOpened){
        bool isOK = true;
        if(ui->stackedWidget_clients->currentIndex() == 1){
            QMessageBox::StandardButton reply;
            if(providersAreOpened){
                reply = QMessageBox::question(this, "Подтвердите действие", "<FONT COLOR = '#000000'>Отменить создание нового поставщика? Внесённые данные не сохранятся! </FONT>");
            }else{
                reply = QMessageBox::question(this, "Подтвердите действие", "<FONT COLOR = '#000000'>Отменить создание новой компании? Внесённые данные не сохранятся! </FONT>");
            }
            if (reply == QMessageBox::Yes){
                if(newCliendPhoneOpened == true){
                    on_pushButton_AddNewClientPhone_clicked();
                }
                if(newClientAddressIsOpened == true){
                    on_pushButton_AddNewClientAddress_clicked();
                }
                ui->pushButton_CreateNewClient->setStyleSheet("QPushButton{color:white;"
                                                              "background-color:rgb(91,95,199);"
                                                              "border-radius:3px;"
                                                              "}"
                                                              "QPushButton:hover{"
                                                              "color:white;"
                                                              "background-color:rgb(68,71,145);"
                                                              "border-radius:3px;"
                                                              "}");
                ui->pushButton_CreateNewClient->setIcon(QIcon(":/images/images/plus-8-12.png"));
                ui->lineEdit_2->clear(); ui->lineEdit_newClientPhone->clear();
                newClientPhones.clear(); newClientFields.clear(); newClientAddresses.clear();
                ui->stackedWidget_clients->setCurrentIndex(0);
            } else {
                isOK = false;
            }
        } else if(ui->stackedWidget_clients->currentIndex() == 2){
            QMessageBox::StandardButton reply;
            if(providersAreOpened){
                reply = QMessageBox::question(this, "Подтвердите действие", "<FONT COLOR = '#000000'>Отменить редактирование поставщика? Внесённые данные не сохранятся! </FONT>");
            }else{
                reply = QMessageBox::question(this, "Подтвердите действие", "<FONT COLOR = '#000000'>Отменить редактирование клиента? Внесённые изменения не сохранятся! </FONT>");
            }
            if (reply == QMessageBox::Yes){
                on_pushButton_CancelRedactOfClient_clicked();
            } else {
                isOK = false;
            }
        }
        if(isOK){
            if(newNoteOpen){

            }
            if(ui->stackedWidget_main->currentIndex() != 0){
                ui->stackedWidget_main->setCurrentIndex(0);
            }
            ui->frame->hide();
            ui->label_create_x->setText("Создать поставщика:");
            ui->pushButton_providers->setStyleSheet("QPushButton{color:white;"
                                                            "background-color:rgb(91,95,199);"
                                                            "border: 2px solid rgb(91,95,199);"
                                                            "border-radius:3px;"
                                                            "}"
                                                            "QPushButton:hover{"
                                                            "color:white;"
                                                            "background-color:rgb(68,71,145);"
                                                            "border: 2px solid rgb(68,71,145);"
                                                            "border-radius:3px;"
                                                            "}");
            if(clientsAreOpened){
                ui->pushButton_clients->setStyleSheet("QPushButton{color:black;"
                                                      "background-color:white;"
                                                      "border: 2px solid rgb(91,95,199);"
                                                      "border-radius:3px;"
                                                      "}"
                                                      "QPushButton:hover{"
                                                      "Color:black;"
                                                      "background-color:rgb(217,217,217);"
                                                      "border: 2px solid rgb(91,95,199);"
                                                      "border-radius:3px;"
                                                      "}");
                ui->verticalLayout_45->setContentsMargins(0,0,0,0);
                clientsAreOpened = false;
                ui->pushButton_activeClients->hide();
                ui->pushButton_inactiveClients->hide();
                ui->frame_2->setStyleSheet("border:0px;");
            } else if(potencialClientsAreOpened){
                ui->pushButton_potencial_clients->setStyleSheet("QPushButton{color:black;"
                                                      "background-color:white;"
                                                      "border: 2px solid rgb(91,95,199);"
                                                      "border-radius:3px;"
                                                      "}"
                                                      "QPushButton:hover{"
                                                      "Color:black;"
                                                      "background-color:rgb(217,217,217);"
                                                      "border: 2px solid rgb(91,95,199);"
                                                      "border-radius:3px;"
                                                      "}");
                potencialClientsAreOpened = false;
            }
        }
        providersAreOpened = true;
        bool finded = false;
        UpdateProviders();
        if(tempListView_providers_id != 0){
            for(int row = 0; row < clientsModel->rowCount(); row++){
                if(ui->listView->model()->data(ui->listView->model()->index(row, 0)).toInt() == tempListView_providers_id){
                    ui->listView->setCurrentIndex(ui->listView->model()->index(row, 1));
                    on_listView_clicked(ui->listView->model()->index(row, 1));
                    ui->listView->scrollTo(ui->listView->model()->index(row, 1));
                    finded = true;
                    break;
                }
            }
            if(!finded){
                MainThemeOfProviders();
            }
        } else {
            MainThemeOfProviders();
        }
    }
}





//Работа с клиентами(компаниями)
void MainWindow::on_comboBox_selectedUser_activated(int index)
{
    if(lastComboBoxSelecteUser != index){
        if(clientsAreOpened){
            if(activeClientsAreOpened){
                UpdateActiveClients();
                MainThemeOfActiveClients();
            } else if(inactiveCLientsAreOpened){
                UpdateInactiveClients();
                MainThemeOfInactiveClients();
            }
        } else if(potencialClientsAreOpened){
            UpdatePotencialClients();
            MainThemeOfPotencialClients();
        }
        lastComboBoxSelecteUser = index;
    }
}

void MainWindow::UpdateComboBoxUsers()
{
    ui->comboBox_selectedUser->clear();
    ui->comboBox_selectedUser->addItem("Не указано");
    QSqlQuery query;
    query.prepare("SELECT name FROM users");
    query.exec();
    while(query.next()){
        ui->comboBox_selectedUser->addItem(query.value(0).toString());
    }
}

void MainWindow::UpdateActiveClients()
{
    QSqlQuery query;
    if(ui->comboBox_selectedUser->currentIndex()!= 0){
        query.prepare("SELECT id, companyName FROM clients WHERE isActive = 1 AND username = :name");
        query.bindValue(":name", ui->comboBox_selectedUser->currentText());
    } else {
        query.prepare("SELECT id, companyName FROM clients WHERE isActive = 1");
    }
    query.exec();
    clientsModel->setQuery(std::move(query));
    while (clientsModel->canFetchMore()){
         clientsModel->fetchMore();
    }
    ui->listView->setModelColumn(1);
    SortFilterProxyClientsModel->sort(1);
}

void MainWindow::UpdateInactiveClients()
{
    QSqlQuery query;
    if(ui->comboBox_selectedUser->currentIndex()!= 0){
        query.prepare("SELECT id, companyName FROM clients WHERE isActive = 0 AND username = :name");
        query.bindValue(":name", ui->comboBox_selectedUser->currentText());
    } else {
        query.prepare("SELECT id, companyName FROM clients WHERE isActive = 0");
    }
    query.exec();
    clientsModel->setQuery(std::move(query));
    while (clientsModel->canFetchMore()){
         clientsModel->fetchMore();
    }
    ui->listView->setModelColumn(1);
    SortFilterProxyClientsModel->sort(1);
}

void MainWindow::UpdatePotencialClients(){
    QSqlQuery query;
    if(ui->comboBox_selectedUser->currentIndex()!= 0){
        query.prepare("SELECT id, companyName FROM clients WHERE isActive = 2 AND username = :name");
        query.bindValue(":name", ui->comboBox_selectedUser->currentText());
    } else {
        query.prepare("SELECT id, companyName FROM clients WHERE isActive = 2");
    }
    query.exec();
    clientsModel->setQuery(std::move(query));
    while (clientsModel->canFetchMore()){
         clientsModel->fetchMore();
    }
    ui->listView->setModelColumn(1);
    SortFilterProxyClientsModel->sort(1);
}

void MainWindow::UpdateProviders()
{
    clientsModel->setQuery("SELECT id, companyName FROM clients WHERE isActive = 3");
    while (clientsModel->canFetchMore()){
         clientsModel->fetchMore();
    }
    ui->listView->setModelColumn(1);
    SortFilterProxyClientsModel->sort(1);
}

void MainWindow::on_pushButton_CreateNewClient_clicked()
{
    bool isOK = true;
    if (ui->stackedWidget_clients->currentIndex() == 1){
        QMessageBox::StandardButton reply =
        QMessageBox::question(this, "Подтвердите действие", "<FONT COLOR = '#000000'>Отменить создание новой компании? Внесённые данные не сохранятся! </FONT>");
        if (reply == QMessageBox::Yes){
            if(newCliendPhoneOpened == true){
                on_pushButton_AddNewClientPhone_clicked();
            }
            if(newClientAddressIsOpened == true){
                on_pushButton_AddNewClientAddress_clicked();
            }
            ui->pushButton_CreateNewClient->setStyleSheet("QPushButton{color:white;"
                                                          "background-color:rgb(91,95,199);"
                                                          "border-radius:3px;"
                                                          "}"
                                                          "QPushButton:hover{"
                                                          "color:white;"
                                                          "background-color:rgb(68,71,145);"
                                                          "border-radius:3px;"
                                                          "}");
            ui->pushButton_CreateNewClient->setIcon(QIcon(":/images/images/plus-8-12.png"));
            ui->lineEdit_2->clear(); ui->lineEdit_newClientPhone->clear();
            newClientPhones.clear(); newClientFields.clear(); newClientAddresses.clear();
            ui->stackedWidget_clients->setCurrentIndex(0);
            isOK = false;
        }
    } else if(ui->stackedWidget_clients->currentIndex() == 2){
        QMessageBox::StandardButton reply =
        QMessageBox::question(this, "Подтвердите действие", "<FONT COLOR = '#000000'>Отменить редактирование компании? Внесённые изменения не сохранятся! </FONT>");
        if (reply == QMessageBox::Yes){
            on_pushButton_CancelRedactOfClient_clicked();
        } else {
            isOK = false;
        }
    }
    if(isOK){
        ui->widget_6->hide(); ui->widget_newClientPhone_fields->hide(); ui->widget_newProviderPhoneFields_fullNamePhone->hide();
        ui->pushButton_saveNewClientPhone->hide(); ui->pushButton_saveNewClientAddress->hide();
        ui->stackedWidget_clients->setCurrentIndex(1);
        if(clientsAreOpened){
            if(activeClientsAreOpened){
                ui->widget_3->show(); ui->widget_5->show();
                ui->widget_newClientMainPhone_main->show(); ui->widget_newClientPhones_main->show();
                ui->stackedWidget_active_innactive_new->setCurrentIndex(1);
            } else if (inactiveCLientsAreOpened){
                ui->widget_3->show(); ui->widget_5->show();
                ui->widget_newClientMainPhone_main->show(); ui->widget_newClientPhones_main->show();
                ui->stackedWidget_active_innactive_new->setCurrentIndex(0);
            }
        } else if(potencialClientsAreOpened){
            ui->widget_3->hide(); ui->widget_5->show();
            ui->widget_newClientMainPhone_main->show(); ui->widget_newClientPhones_main->show();
        } else if(providersAreOpened){
            ui->widget_3->hide(); ui->widget_5->hide();
            ui->widget_newClientMainPhone_main->hide(); ui->widget_newClientPhones_main->show();
        }
        newClientFields.clear();
        ui->pushButton_CreateNewClient->setStyleSheet("QPushButton{color:white;"
                                                      "background-color:red;"
                                                      "border-radius:3px;"
                                                      "}"
                                                      "QPushButton:hover{"
                                                      "color:white;"
                                                      "background-color:rgb(180,0,0);"
                                                      "border-radius:3px;"
                                                      "}");
        ui->pushButton_CreateNewClient->setIcon(QIcon(":/images/images/minus-white-12.png"));

        QSqlQuery query;
        int i = 0;
        query.exec("SELECT name FROM users");
        QSqlRecord rec = query.record();
        ui->comboBox_newClientUser->clear();
        while(query.next()){
            ui->comboBox_newClientUser->addItem(query.value(rec.indexOf("name")).toString());
            if(query.value(rec.indexOf("name")).toString() == username){
                ui->comboBox_newClientUser->setCurrentIndex(i);
            }
            i++;
        }

        UpdateNewClientPhones();
        UpdateNewClientAddresses();
        ui->stackedWidget_clients->setCurrentIndex(1);
    }
}

void MainWindow::on_pushButton_active_newClient_clicked()
{
    ui->stackedWidget_active_innactive_new->setCurrentIndex(0);
}
void MainWindow::on_pushButton_inactive_newClient_clicked()
{
    ui->stackedWidget_active_innactive_new->setCurrentIndex(1);
}

void MainWindow::on_pushButton_SaveNewClient_clicked()
{
    bool isOK = true;
    if(ui->lineEdit_2->text() == ""){
        QMessageBox::warning(this, "Ошибка", "<FONT COLOR = '#000000'>Введите заголовок компании!</FONT> \n");
    } else {
        if(newClientAddressIsOpened){
            QMessageBox::StandardButton reply =
            QMessageBox::question(this, "Подтвердите действие", "<FONT COLOR = '#000000'>Отменить добавление нового адреса? </FONT>");
            if (reply == QMessageBox::Yes){
                on_pushButton_AddNewClientAddress_clicked();
            } else {
                isOK = false;
            }
        }
        if(newCliendPhoneOpened){
            QMessageBox::StandardButton reply =
            QMessageBox::question(this, "Подтвердите действие", "<FONT COLOR = '#000000'>Отменить добавление нового телефона? </FONT>");
            if (reply == QMessageBox::Yes){
                on_pushButton_AddNewClientPhone_clicked();
            } else {
                isOK = false;
            }
        }
        if(isOK){
            int activeStatus;
            if(potencialClientsAreOpened){
                activeStatus = 2;
            } else if(clientsAreOpened){
                activeStatus = ui->stackedWidget_active_innactive_new->currentIndex();
            } else {
                activeStatus = 3;
            }
            database::InsertToClients(ui->lineEdit_2->text(), ui->comboBox_newClientUser->currentText(), QString::number(activeStatus));
            QSqlQuery query;
            query.prepare("SELECT seq FROM sqlite_sequence where name = :name");
            query.bindValue(":name", "clients");
            query.exec(); query.next();
            int nextId = query.value(0).toInt();
            if(!newClientPhones.empty()){
                if(!providersAreOpened){
                    for (int i = 0; i < newClientPhones.size() ; i+=5) {
                        database::InsertToClientPhones(newClientPhones[i], QString::number(nextId), newClientPhones[i+1],newClientPhones[i+2], newClientPhones[i+3], newClientPhones[i+4]);
                    }
                } else {
                    for (int i = 0; i < newClientPhones.size() ; i+=2) {
                        database::InsertToClientPhones("-", QString::number(nextId), newClientPhones[i], "-", newClientPhones[i+1], "-");
                    }
                }

            }
            if(!newClientAddresses.empty()){
                for (int i = 0; i < newClientAddresses.size() ; i+=2) {
                    database::InsertToClientAddresses(QString::number(nextId), newClientAddresses[i], newClientAddresses[i+1]);
                }
            }

            ui->pushButton_CreateNewClient->setStyleSheet("QPushButton{color:white;"
                                                          "background-color:rgb(91,95,199);"
                                                          "border-radius:3px;"
                                                          "}"
                                                          "QPushButton:hover{"
                                                          "color:white;"
                                                          "background-color:rgb(68,71,145);"
                                                          "border-radius:3px;"
                                                          "}");
            ui->pushButton_CreateNewClient->setIcon(QIcon(":/images/images/plus-8-12.png"));
            ui->lineEdit_2->clear(); ui->lineEdit_newClientPhone->clear();
            newClientPhones.clear(); newClientFields.clear(); newClientAddresses.clear();
            ui->stackedWidget_clients->setCurrentIndex(0);

            if(lastComboBoxSelecteUser != 0){
                 ui->comboBox_selectedUser->setCurrentIndex(ui->comboBox_newClientUser->currentIndex() + 1);
            }

            if(potencialClientsAreOpened){
                UpdatePotencialClients();
            } else if(clientsAreOpened){
                if(ui->stackedWidget_active_innactive_new->currentIndex() == 0){
                    UpdateInactiveClients();
                    activeClientsAreOpened = false; inactiveCLientsAreOpened = true;
                    ui->pushButton_inactiveClients->setStyleSheet("QPushButton{color:white;"
                                                                    "background-color:rgb(91,95,199);"
                                                                    "border: 2px solid rgb(91,95,199);"
                                                                    "border-radius:3px;"
                                                                    "}"
                                                                    "QPushButton:hover{"
                                                                    "color:white;"
                                                                    "background-color:rgb(68,71,145);"
                                                                    "border: 2px solid rgb(68,71,145);"
                                                                    "border-radius:3px;"
                                                                    "}");
                    ui->pushButton_activeClients->setStyleSheet("QPushButton{color:black;"
                                                          "background-color:white;"
                                                          "border: 2px solid rgb(91,95,199);"
                                                          "border-radius:3px;"
                                                          "}"
                                                          "QPushButton:hover{"
                                                          "Color:black;"
                                                          "background-color:rgb(217,217,217);"
                                                          "border: 2px solid rgb(91,95,199);"
                                                          "border-radius:3px;"
                                                          "}");
                } else {
                    UpdateActiveClients();
                    activeClientsAreOpened = true; inactiveCLientsAreOpened = false;
                    ui->pushButton_activeClients->setStyleSheet("QPushButton{color:white;"
                                                                    "background-color:rgb(91,95,199);"
                                                                    "border: 2px solid rgb(91,95,199);"
                                                                    "border-radius:3px;"
                                                                    "}"
                                                                    "QPushButton:hover{"
                                                                    "color:white;"
                                                                    "background-color:rgb(68,71,145);"
                                                                    "border: 2px solid rgb(68,71,145);"
                                                                    "border-radius:3px;"
                                                                    "}");
                    ui->pushButton_inactiveClients->setStyleSheet("QPushButton{color:black;"
                                                          "background-color:white;"
                                                          "border: 2px solid rgb(91,95,199);"
                                                          "border-radius:3px;"
                                                          "}"
                                                          "QPushButton:hover{"
                                                          "Color:black;"
                                                          "background-color:rgb(217,217,217);"
                                                          "border: 2px solid rgb(91,95,199);"
                                                          "border-radius:3px;"
                                                          "}");
                }
            } else {
                UpdateProviders();
            }

            ui->lineEdit_SearchOfClients->setText("");
            SortFilterProxyClientsModel->setFilterRegularExpression(NULL);

            ui->lineEdit_2->clear(); ui->lineEdit_newClientPhone->clear();  //очищаем все поля
            for(int row = 0; row < clientsModel->rowCount(); row++){
                if(ui->listView->model()->data(ui->listView->model()->index(row, 0)).toInt() == nextId){
                    ui->listView->setCurrentIndex(ui->listView->model()->index(row, 1));
                    on_listView_clicked(ui->listView->model()->index(row, 1));
                    ui->listView->scrollTo(ui->listView->model()->index(row, 1));
                    break;
                }
            }
        }
    }
}

void MainWindow::on_lineEdit_SearchOfClients_textEdited(const QString &arg1)
{
    if(ui->stackedWidget_clients->currentIndex()==1){
        on_pushButton_CreateNewClient_clicked();
    }

    if(clientsAreOpened){
        if(activeClientsAreOpened){
            MainThemeOfActiveClients();
        } else if(inactiveCLientsAreOpened){
            MainThemeOfInactiveClients();
        }
    } else if (potencialClientsAreOpened){
        MainThemeOfPotencialClients();
    } else if(providersAreOpened){
        MainThemeOfProviders();
    }

    if(arg1 == " "){
        ui->lineEdit_SearchOfClients->setText("");
    } else {
        QRegularExpression filter ("^.{0,}(?i)"+ arg1 + ".{0,}$");
        SortFilterProxyClientsModel->setFilterKeyColumn(1);
        SortFilterProxyClientsModel->setFilterRegularExpression(filter);
        SortFilterProxyClientsModel->sort(1);
    }
}

void MainWindow::on_pushButton_DeleteClient_clicked()
{
    ui->pushButton_DeleteClient->setStyleSheet("QPushButton{color:white;"
                                               "background-color:red;"
                                               "border-radius:3px;"
                                               "}");
    QMessageBox::StandardButton reply =
    QMessageBox::question(this, "Подтвердите действие", "<FONT COLOR = '#000000'>Вы уверены, что хотите удалить объект из базы данных? Это действие необратимо! </FONT>");
    if (reply == QMessageBox::Yes){
        ui->pushButton_DeleteClient->setStyleSheet("QPushButton{color:white;"
                                                   "background-color:rgb(91,95,199);"
                                                   "border-radius:3px;"
                                                   "}"
                                                   "QPushButton:hover{"
                                                   "color:white;"
                                                   "background-color:rgb(68,71,145);"
                                                   "border-radius:3px;"
                                                   "}");
        QVariant data = ui->listView->model()->data(ui->listView->model()->index(ui->listView->currentIndex().row(),0),Qt::DisplayRole).toString();
        database::DeleteFromActiveClients(data.toString());
        database::DeleteFromNotes(data.toString());
        database::DeleteCompanyDocuments(data.toString());
        database::DeleteAllAddressesFromClient(data.toString());
        database::DeletaAllPhonesFromClient(data.toString());
        on_lineEdit_SearchOfClients_textEdited(ui->lineEdit_SearchOfClients->text());
        ClientIsOpened = false;
        if(clientsAreOpened){
            if(activeClientsAreOpened){
                tempListView_activeClients_id = 0;
                UpdateActiveClients();
            } else if(inactiveCLientsAreOpened){
                tempListView_inactiveClients_id = 0;
                UpdateInactiveClients();
            }
        } else if(potencialClientsAreOpened){
            tempListView_potencialClients_id = 0;
            UpdatePotencialClients();
        } else if(providersAreOpened){
            tempListView_providers_id = 0;
            UpdateProviders();
        }
    } else {
        ui->pushButton_DeleteClient->setStyleSheet("QPushButton{color:white;"
                                                   "background-color:rgb(91,95,199);"
                                                   "border-radius:3px;"
                                                   "}"
                                                   "QPushButton:hover{"
                                                   "color:white;"
                                                   "background-color:rgb(68,71,145);"
                                                   "border-radius:3px;"
                                                   "}");
    }
}

void MainWindow::on_listView_clicked(const QModelIndex &index)
{
    bool isOK = true;
    if(ui->stackedWidget_clients->currentIndex() == 1){
        QMessageBox::StandardButton reply =
        QMessageBox::question(this, "Подтвердите действие", "<FONT COLOR = '#000000'>Отменить создание новой компании? Внесённые данные не сохранятся! </FONT>");
        if (reply == QMessageBox::Yes){
            if(newCliendPhoneOpened == true){
                on_pushButton_AddNewClientPhone_clicked();
            }
            if(newClientAddressIsOpened == true){
                on_pushButton_AddNewClientAddress_clicked();
            }
            ui->pushButton_CreateNewClient->setStyleSheet("QPushButton{color:white;"
                                                          "background-color:rgb(91,95,199);"
                                                          "border-radius:3px;"
                                                          "}"
                                                          "QPushButton:hover{"
                                                          "color:white;"
                                                          "background-color:rgb(68,71,145);"
                                                          "border-radius:3px;"
                                                          "}");
            ui->pushButton_CreateNewClient->setIcon(QIcon(":/images/images/plus-8-12.png"));
            ui->lineEdit_2->clear(); ui->lineEdit_newClientPhone->clear();
            newClientPhones.clear(); newClientFields.clear(); newClientAddresses.clear();
            ui->stackedWidget_clients->setCurrentIndex(0);
        } else {
            isOK = false;
        }
    } else if(ui->stackedWidget_clients->currentIndex() == 2){
        QMessageBox::StandardButton reply =
        QMessageBox::question(this, "Подтвердите действие", "<FONT COLOR = '#000000'>Отменить редактирование компании? Внесённые изменения не сохранятся! </FONT>");
        if (reply == QMessageBox::Yes){
            on_pushButton_CancelRedactOfClient_clicked();
        } else {
            isOK = false;
        }
    }
    if(isOK){
        if(ClientIsOpened == false){
            ui->pushButton_Update->show(); ui->pushButton_DeleteClient->show();
            ui->scrollArea->show();
            ClientIsOpened = true;
        }
        if(newNoteOpen){
            on_pushButton_addNote_clicked();
        }
        ui->textEdit->setFont(defaultfont);
        QSqlQuery query;
        query.prepare("SELECT * FROM clients WHERE id = :search");
        QVariant data =  ui->listView->model()->data(ui->listView->model()->index(index.row(),0),Qt::DisplayRole).toString();
        query.bindValue(":search", data.toString()); //поиск по id
        if(!query.exec()){
            qDebug() << query.lastError().text();
        }
        QSqlRecord rec = query.record();
        query.first();
        ui->label_CMPname->setText(query.value(rec.indexOf("companyName")).toString());
        ui->label_CMPname->setTextInteractionFlags(Qt::TextSelectableByMouse);
        ui->label_username->setText(query.value(rec.indexOf("username")).toString());
        ui->label_username->setTextInteractionFlags(Qt::TextSelectableByMouse);

        if(query.value(rec.indexOf("isActive")).toInt() == 0){
            ui->stackedWidget_2->setCurrentIndex(1); ui->widget_clientMainPhones_main->show(); ui->widget_8->show();
        } else if(query.value(rec.indexOf("isActive")).toInt() == 1) {
            ui->stackedWidget_2->setCurrentIndex(0); ui->widget_clientMainPhones_main->show(); ui->widget_8->show();
        } else if(query.value(rec.indexOf("isActive")).toInt() == 2){
            ui->stackedWidget_2->setCurrentIndex(2); ui->widget_clientMainPhones_main->show(); ui->widget_8->show();
        } else if(query.value(rec.indexOf("isActive")).toInt() == 3){
            ui->widget_clientMainPhones_main->hide(); ui->widget_8->hide();
        }

        UpdateNotes(); //обновление заметок
        UpdateDocuments();
        UpdateOutputCLientPhones();
        UpdateOutputClientAddresses();

        if(clientsAreOpened){
            if(activeClientsAreOpened){
                tempListView_activeClients_id = ui->listView->model()->data(ui->listView->model()->index(ui->listView->currentIndex().row(), 0)).toInt();
            } else if (inactiveCLientsAreOpened){
                tempListView_inactiveClients_id = ui->listView->model()->data(ui->listView->model()->index(ui->listView->currentIndex().row(), 0)).toInt();
            }
        } else if(potencialClientsAreOpened){
            tempListView_potencialClients_id = ui->listView->model()->data(ui->listView->model()->index(ui->listView->currentIndex().row(), 0)).toInt();
        } else if(providersAreOpened){
            tempListView_providers_id = ui->listView->model()->data(ui->listView->model()->index(ui->listView->currentIndex().row(), 0)).toInt();
        }
    }
}

void MainWindow::on_pushButton_outputClient_active_clicked()
{
    QString data = ui->listView->model()->data(ui->listView->model()->index(ui->listView->currentIndex().row(),0),Qt::DisplayRole).toString();
    database::UpdateActiveStatusOfCompany(data, QString::number(0));

    UpdateInactiveClients();

    on_lineEdit_SearchOfClients_textEdited("");

    tempListView_inactiveClients_id = 0;
    ui->pushButton_inactiveClients->setStyleSheet("QPushButton{color:white;"
                                                    "background-color:rgb(91,95,199);"
                                                    "border: 2px solid rgb(91,95,199);"
                                                    "border-radius:3px;"
                                                    "}"
                                                    "QPushButton:hover{"
                                                    "color:white;"
                                                    "background-color:rgb(68,71,145);"
                                                    "border: 2px solid rgb(68,71,145);"
                                                    "border-radius:3px;"
                                                    "}");
    ui->pushButton_activeClients->setStyleSheet("QPushButton{color:black;"
                                          "background-color:white;"
                                          "border: 2px solid rgb(91,95,199);"
                                          "border-radius:3px;"
                                          "}"
                                          "QPushButton:hover{"
                                          "Color:black;"
                                          "background-color:rgb(217,217,217);"
                                          "border: 2px solid rgb(91,95,199);"
                                          "border-radius:3px;"
                                          "}");

    for(int row = 0; row <= clientsModel->rowCount(); row++){
        if(ui->listView->model()->data(ui->listView->model()->index(row, 0)) == data){
            activeClientsAreOpened = false; inactiveCLientsAreOpened = true;
            ui->listView->setCurrentIndex(ui->listView->model()->index(row, 1));
            on_listView_clicked(ui->listView->model()->index(row, 1));
            ui->listView->scrollTo(ui->listView->model()->index(row, 1));
            break;
        }
    }
}
void MainWindow::on_pushButton_outputClients_inactive_clicked()
{
    QString data = ui->listView->model()->data(ui->listView->model()->index(ui->listView->currentIndex().row(),0),Qt::DisplayRole).toString();
    database::UpdateActiveStatusOfCompany(data, QString::number(1));

    UpdateActiveClients();
    ui->lineEdit_SearchOfClients->setText("");
    on_lineEdit_SearchOfClients_textEdited("");

    tempListView_activeClients_id = 0;
    ui->pushButton_activeClients->setStyleSheet("QPushButton{color:white;"
                                                    "background-color:rgb(91,95,199);"
                                                    "border: 2px solid rgb(91,95,199);"
                                                    "border-radius:3px;"
                                                    "}"
                                                    "QPushButton:hover{"
                                                    "color:white;"
                                                    "background-color:rgb(68,71,145);"
                                                    "border: 2px solid rgb(68,71,145);"
                                                    "border-radius:3px;"
                                                    "}");
    ui->pushButton_inactiveClients->setStyleSheet("QPushButton{color:black;"
                                          "background-color:white;"
                                          "border: 2px solid rgb(91,95,199);"
                                          "border-radius:3px;"
                                          "}"
                                          "QPushButton:hover{"
                                          "Color:black;"
                                          "background-color:rgb(217,217,217);"
                                          "border: 2px solid rgb(91,95,199);"
                                          "border-radius:3px;"
                                          "}");


    for(int row = 0; row <= clientsModel->rowCount(); row++){
        if(ui->listView->model()->data(ui->listView->model()->index(row, 0)) == data){
            activeClientsAreOpened = true; inactiveCLientsAreOpened = false;
            ui->listView->setCurrentIndex(ui->listView->model()->index(row, 1));
            on_listView_clicked(ui->listView->model()->index(row, 1));
            ui->listView->scrollTo(ui->listView->model()->index(row, 1));
            break;
        }
    }
}
void MainWindow::on_pushButton_setClientActiveFromPotencial_clicked()
{
    if(potencialClientsAreOpened){
        QString data = ui->listView->model()->data(ui->listView->model()->index(ui->listView->currentIndex().row(),0),Qt::DisplayRole).toString();
        database::UpdateActiveStatusOfCompany(data, QString::number(1));
        ui->lineEdit_SearchOfClients->setText("");
        SortFilterProxyClientsModel->setFilterRegularExpression(NULL);
        tempListView_activeClients_id = data.toInt();
        if(inactiveCLientsAreOpened){
            inactiveCLientsAreOpened = false; activeClientsAreOpened = true;
            ui->pushButton_activeClients->setStyleSheet("QPushButton{color:white;"
                                                            "background-color:rgb(91,95,199);"
                                                            "border: 2px solid rgb(91,95,199);"
                                                            "border-radius:3px;"
                                                            "}"
                                                            "QPushButton:hover{"
                                                            "color:white;"
                                                            "background-color:rgb(68,71,145);"
                                                            "border: 2px solid rgb(68,71,145);"
                                                            "border-radius:3px;"
                                                            "}");
            ui->pushButton_inactiveClients->setStyleSheet("QPushButton{color:black;"
                                                  "background-color:white;"
                                                  "border: 2px solid white;"
                                                  "border-radius:3px;"
                                                  "border-color:rgb(217,217,217);"
                                                  "}"
                                                  "QPushButton:hover{"
                                                  "Color:black;"
                                                  "background-color:rgb(217,217,217);"
                                                  "border: 2px solid white;"
                                                  "border-radius:3px;"
                                                  "border-color:rgb(217,217,217);"
                                                  "}");
        }
        on_pushButton_clients_clicked();
        tempListView_potencialClients_id = 0;
    }
}




//Редактирование компании
void MainWindow::on_pushButton_Update_clicked()
{
    ui->widget_redactProviderPhones_Fields->hide(); ui->widget_redactClientPhones_Fields->hide(); ui->pushButton_saveRedactClientPhone->hide();
    ui->widget_7->hide(); ui->pushButton_saveRedactClientAddress->hide();
    if(!providersAreOpened){
        ui->widget_redactClientPhones_main->show(); ui->widget_redactClientPhones->show();
    } else {
        ui->widget_redactClientPhones_main->hide(); ui->widget_redactClientPhones->show();
    }
    QSqlQuery query;
    query.prepare("SELECT * FROM clients WHERE id = :search");
    QVariant data =  ui->listView->model()->data(ui->listView->model()->index(ui->listView->currentIndex().row(),0),Qt::DisplayRole).toString();
    query.bindValue(":search", data.toString()); //поиск по id
    if(!query.exec()){
        qDebug() << query.lastError().text();
    }
    QSqlRecord rec = query.record();
    query.first();
    ui->lineEdit_6->setText(ui->listView->model()->data(ui->listView->model()->index(ui->listView->currentIndex().row(), 1), Qt::DisplayRole).toString());
    ui->lineEdit_redactClientPhone->setInputMask("+7\\(999\\)999\\-99\\-99;_");
    usernames->setQuery("SELECT name FROM users");
    ui->comboBox_redactClientUsername->setCurrentText(query.value(rec.indexOf("username")).toString());

    query.prepare("SELECT address, information FROM clientAddresses WHERE companyId = :search");
    query.bindValue(":search", data);
    query.exec();
    while(query.next()){
        redactClientAddresses.push_back(query.value(0).toString());
        redactClientAddresses.push_back(query.value(1).toString());
    }

    if(!providersAreOpened){
        query.prepare("SELECT fullName, information, phone, email FROM clientPhones WHERE companyId = :search AND mainStatus = :mainStatus");
        query.bindValue(":search", data);
        query.bindValue(":mainStatus", 1);
        query.exec();
        while(query.next()){
            redactClientPhones.push_back("1");
            redactClientPhones.push_back(query.value(0).toString());
            redactClientPhones.push_back(query.value(1).toString());
            redactClientPhones.push_back(query.value(2).toString());
            redactClientPhones.push_back(query.value(3).toString());
        }

        query.prepare("SELECT fullName, information, phone, email FROM clientPhones WHERE companyId = :search AND mainStatus = :mainStatus");
        query.bindValue(":search", data);
        query.bindValue(":mainStatus", 0);
        query.exec();
        while(query.next()){
            redactClientPhones.push_back("0");
            redactClientPhones.push_back(query.value(0).toString());
            redactClientPhones.push_back(query.value(1).toString());
            redactClientPhones.push_back(query.value(2).toString());
            redactClientPhones.push_back(query.value(3).toString());
        }
    } else {
        query.prepare("SELECT fullName, phone FROM clientPhones WHERE companyId = :search");
        query.bindValue(":search", data);
        query.exec();
        while(query.next()){
            redactClientPhones.push_back(query.value(0).toString());
            redactClientPhones.push_back(query.value(1).toString());
        }
    }
    UpdateRedactClientAddresses();
    UpdateRedactClientPhones();
    ui->stackedWidget_clients->setCurrentIndex(2);
}

void MainWindow::on_pushButton_SaveRedactedClient_clicked()
{
    bool isOK = true;
    if(redactClientAddressIsOpened){
        QMessageBox::StandardButton reply =
        QMessageBox::question(this, "Подтвердите действие", "<FONT COLOR = '#000000'>Отменить добавление нового адреса? </FONT>");
        if (reply == QMessageBox::Yes){
            on_pushButton_AddRedactClientAddress_clicked();
        } else {
            isOK = false;
        }
    }
    if(redactClientPhoneOpened){
        QMessageBox::StandardButton reply =
        QMessageBox::question(this, "Подтвердите действие", "<FONT COLOR = '#000000'>Отменить добавление нового телефона? </FONT>");
        if (reply == QMessageBox::Yes){
            on_pushButton_AddRedactClientPhone_clicked();
        } else {
            isOK = false;
        }
    }
    QList<QString> wrongPhones;
    if(isOK){
        CheckUpdatesForRedactClientPhones(); CheckUpdatesForRedactClientAddresses();
        if(!providersAreOpened){
            for (int i = 3; i<redactClientPhones.size(); i+=5 ) {
                QString a = redactClientPhones[i];
                int b = 0;
                if(redactClientPhones[i] != "" && numberValidator.validate(a, b) != QValidator::Acceptable){
                    wrongPhones.push_back(redactClientPhones[i-2]);
                }
            }
        } else {
            for (int i = 1; i<redactClientPhones.size(); i+=2 ) {
                QString a = redactClientPhones[i];
                int b = 0;
                if(redactClientPhones[i] != "" && numberValidator.validate(a, b) != QValidator::Acceptable){
                    wrongPhones.push_back(redactClientPhones[i]);
                }
            }
        }

        if(wrongPhones.size() != 0){
            QString information = "<FONT COLOR = '#000000'>Проверьте правильность следующих введенных номеров телефона: <br> ";
            for(int i = 0; i < wrongPhones.size(); i++){
                information += wrongPhones[i];
                information += " <br> ";
            }
            information += "Очистите поле, либо введите валидные номера.</FONT>";
            QMessageBox::warning(this, "Ошибка", information);
        } else {
            QVariant data = ui->listView->model()->data(ui->listView->model()->index(ui->listView->currentIndex().row(),0),Qt::DisplayRole).toString();

            bool isOK = true;

            if(!providersAreOpened){
                if(!CheckForDuplicate(redactClientPhones, data.toInt()).isEmpty()){
                    emit OpenInfoAboutDublicates(findedPhoneDublicates, phonesWithDublicates);
                    if(dublicatesOutput.exec() == 1){
                        isOK = true;
                    } else {
                        isOK = false;
                    }
                }
            }

            if(isOK){
                QString name = ui->lineEdit_6->text();
                QString username = ui->comboBox_redactClientUsername->currentText();
                database::RedactActiveClients(name, data.toString(), username);
                database::DeletaAllPhonesFromClient(data.toString());

                if(providersAreOpened){
                    for(int i = 0; i < redactClientPhones.size(); i+=2){
                        if(redactClientPhones[i] != "" || redactClientPhones[i+1] != ""){
                            database::InsertToClientPhones("-", data.toString(), redactClientPhones[i], "-", redactClientPhones[i+1], "-");
                        }
                    }
                } else {
                    for(int i = 0; i < redactClientPhones.size(); i+=5){
                        if(redactClientPhones[i+1] != "" || redactClientPhones[i+2] != "" || redactClientPhones[i+3] != "" || redactClientPhones[i+4] != ""){
                            database::InsertToClientPhones(redactClientPhones[i], data.toString(), redactClientPhones[i+1],redactClientPhones[i+2], redactClientPhones[i+3], redactClientPhones[i+4]);
                        }
                    }
                }
                redactClientPhones.clear();

                database::DeleteAllAddressesFromClient(data.toString());
                for(int i = 0; i < redactClientAddresses.size(); i+=2){
                    if(redactClientAddresses[i] != "" || redactClientAddresses[i+1] != ""){
                        database::InsertToClientAddresses(data.toString(), redactClientAddresses[i], redactClientAddresses[i+1]);
                    }
                }
                redactClientAddresses.clear();

                if(lastComboBoxSelecteUser != 0){
                     ui->comboBox_selectedUser->setCurrentIndex(ui->comboBox_redactClientUsername->currentIndex() + 1);
                }

                if(clientsAreOpened){
                    if(activeClientsAreOpened){
                        UpdateActiveClients();
                    } else if (inactiveCLientsAreOpened){
                        UpdateInactiveClients();
                    }
                } else if (potencialClientsAreOpened){
                    UpdatePotencialClients();
                } else if(providersAreOpened){
                    UpdateProviders();
                }

                ui->lineEdit_SearchOfClients->setText("");
                SortFilterProxyClientsModel->setFilterRegularExpression(NULL);
                ui->stackedWidget_clients->setCurrentIndex(0);

                for(int row = 0; row <= ui->listView->model()->rowCount(); row++){
                        if(ui->listView->model()->data(ui->listView->model()->index(row, 0)) == data){
                            ui->listView->setCurrentIndex(ui->listView->model()->index(row, 1));
                            on_listView_clicked(ui->listView->model()->index(row, 1));
                            ui->listView->scrollTo(ui->listView->model()->index(row, 1));
                            break;
                        }
                }
                ClearRedactClientPhones();
            }
        }
    }
}

void MainWindow::on_pushButton_CancelRedactOfClient_clicked()
{
    ui->lineEdit_6->setText("");
    if(redactClientAddressIsOpened){
        on_pushButton_AddRedactClientAddress_clicked();
    }
    if(redactClientPhoneOpened){
        on_pushButton_AddRedactClientPhone_clicked();
    }

    ui->lineEdit_redactClientAddress->setText(""); ui->lineEdit_redactClientAddress_information->setText("");

    redactClientFields.clear();
    ui->stackedWidget_clients->setCurrentIndex(0);
    on_listView_clicked(ui->listView->currentIndex());
    redactClientAddresses.clear();
    redactClientPhones.clear();
}





//Телефоны компаний
void MainWindow::on_pushButton_AddNewClientPhone_clicked()
{
    if(newCliendPhoneOpened == false){
        if(providersAreOpened){
            ui->widget_newProviderPhoneFields_fullNamePhone->show();
        } else if (clientsAreOpened || potencialClientsAreOpened){
            ui->widget_newClientPhone_fields->show();
        }

        ui->pushButton_saveNewClientPhone->show();
        ui->pushButton_AddNewClientPhone->setStyleSheet("QPushButton{color:white;"
                                                        "background-color:red;"
                                                        "border-radius:3px;"
                                                        "}"
                                                        "QPushButton:hover{"
                                                        "color:white;"
                                                        "background-color:rgb(180,0,0);"
                                                        "border-radius:3px;"
                                                        "}");
      ui->pushButton_AddNewClientPhone->setIcon(QIcon(":/images/images/minus-white-12.png"));
        newCliendPhoneOpened = true;
    } else if(newCliendPhoneOpened == true){
        ui->widget_newClientPhone_fields->hide(); ui->widget_newProviderPhoneFields_fullNamePhone->hide();
        ui->pushButton_saveNewClientPhone->hide();
        ui->pushButton_AddNewClientPhone->setStyleSheet("QPushButton{color:white;"
                                                          "background-color:rgb(91,95,199);"
                                                          "border-radius:3px;"
                                                          "}"
                                                          "QPushButton:hover{"
                                                          "color:white;"
                                                          "background-color:rgb(68,71,145);"
                                                          "border-radius:3px;"
                                                          "}");
        ui->pushButton_AddNewClientPhone->setIcon(QIcon(":/images/images/plus-8-12.png"));
        if(!providersAreOpened){
            ui->lineEdit_newClientPhone->clear(); ui->lineEdit_newClientPhone_information->clear(); ui->lineEdit_newClientPhone_FullName->clear(); ui->lineEdit_newClientPhone_email->clear();
        } else {
            ui->lineEdit_newProviderPhone->clear(); ui->lineEdit_newProviderPhone_FullName->clear();
        }
        newCliendPhoneOpened = false;
    }
}

void MainWindow::on_pushButton_saveNewClientPhone_clicked()
{
    bool isOK = true;
    if(!providersAreOpened){
        QString a = ui->lineEdit_newClientPhone->text();
        int b = ui->lineEdit_newClientPhone->cursorPosition();
        if(!ui->lineEdit_newClientPhone_FullName->text().isEmpty() || !ui->lineEdit_newClientPhone_information->text().isEmpty()
                || ui->lineEdit_newClientPhone->text() != "+7()--" || !ui->lineEdit_newClientPhone_email->text().isEmpty()){
            if(a != "+7()--"){
                if(numberValidator.validate(a, b) != QValidator::Acceptable){
                    QMessageBox::warning(this, "Ошибка", "<FONT COLOR = '#000000'>Проверьте правильность введенного номера телефона.</FONT> \n");
                    isOK = false;
                }
            }
            if(isOK){
                newClientPhones.push_back("0");
                newClientPhones.push_back(ui->lineEdit_newClientPhone_FullName->text());
                newClientPhones.push_back(ui->lineEdit_newClientPhone_information->text());
                if(a == "+7()--"){
                    newClientPhones.push_back("");
                } else {
                    newClientPhones.push_back(ui->lineEdit_newClientPhone->text());
                }
                newClientPhones.push_back(ui->lineEdit_newClientPhone_email->text());
                on_pushButton_AddNewClientPhone_clicked();
                UpdateNewClientPhones();
            }
        } else {
            QMessageBox::warning(this, "Ошибка", "<FONT COLOR = '#000000'>Хотя бы одно поле не должно быть пустым.</FONT> \n");
        }
    } else {
        QString a = ui->lineEdit_newProviderPhone->text();
        int b = ui->lineEdit_newProviderPhone->cursorPosition();
        if(!ui->lineEdit_newProviderPhone->text().isEmpty() || !ui->lineEdit_newProviderPhone_FullName->text().isEmpty()){
            if(a != "+7()--"){
                if(numberValidator.validate(a, b) != QValidator::Acceptable){
                    QMessageBox::warning(this, "Ошибка", "<FONT COLOR = '#000000'>Проверьте правильность введенного номера телефона.</FONT> \n");
                    isOK = false;
                }
            }
            if(isOK){
                newClientPhones.push_back(ui->lineEdit_newProviderPhone_FullName->text());
                if(a == "+7()--"){
                    newClientPhones.push_back("");
                } else {
                    newClientPhones.push_back(ui->lineEdit_newProviderPhone->text());
                }
                on_pushButton_AddNewClientPhone_clicked();
                UpdateNewClientPhones();
            }
        } else {
            QMessageBox::warning(this, "Ошибка", "<FONT COLOR = '#000000'>Хотя бы одно поле не должно быть пустым.</FONT> \n");
        }
    }
}

void MainWindow::UpdateNewClientPhones(){
    ClearNewClientPhones();
    bool mainIsThere = false, notMainIsThere = false;
    if(!providersAreOpened){
        for(int i = 0; i < newClientPhones.size(); i+=5){
            dynamicFrame *frame = new dynamicFrame(this);
            frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            frame->setStyleSheet("background-color:rgb(232,235,250);"
                                 "color:rgb(36,36,36);"
                                 "border-width:1px;"
                                 "border-radius:8px;"
                                 "border-color: black;");
            QHBoxLayout *lay = new QHBoxLayout(frame);
            QHBoxLayout *hLay_1 = new QHBoxLayout();
            QHBoxLayout *hLay_2 = new QHBoxLayout();
            QHBoxLayout *hLay_3 = new QHBoxLayout();
            QHBoxLayout *hLay_4 = new QHBoxLayout();
            QHBoxLayout *upperH = new QHBoxLayout();
            QHBoxLayout *downH = new QHBoxLayout();
            QVBoxLayout *vLay = new QVBoxLayout();


            dynamicLabel *icon_fullName = new dynamicLabel(frame);
            icon_fullName->setFixedSize(24,24);
            QPixmap pix_fullName(":/images/images/administrator-black-24.png");
            icon_fullName->setPixmap(pix_fullName);
            dynamicLineEdit *fullName = new dynamicLineEdit(frame);
            fullName->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
            fullName->setFont(defaultfont);
            if(newClientPhones[i+1] != ""){
                fullName->setText(newClientPhones[i+1]);
                fullName->setToolTip(newClientPhones[i+1]);
            } else {
                fullName->setText("—");
                fullName->setToolTip("Не указано");
            }

            fullName->setReadOnly(true);
            fullName->setFrame(false);
            fullName->setCursorPosition(0);

            hLay_1->addWidget(icon_fullName); hLay_1->addWidget(fullName);

            dynamicLabel *icon_info = new dynamicLabel(frame);
            icon_info->setFixedSize(24,24);
            QPixmap pix_info(":/images/images/info-black-24.png");
            icon_info->setPixmap(pix_info);
            dynamicLineEdit *information = new dynamicLineEdit(frame);
            information->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
            information->setFont(defaultfont);
            if(newClientPhones[i+2] != ""){
                information->setText(newClientPhones[i+2]);
                information->setToolTip(newClientPhones[i+2]);
            } else {
                information->setText("—");
                information->setToolTip("Не указано");
            }

            information->setReadOnly(true);
            information->setFrame(false);
            information->setCursorPosition(0);
            hLay_2->addWidget(icon_info); hLay_2->addWidget(information);

            upperH->addLayout(hLay_1); upperH->addLayout(hLay_2);
            //fullName->setMinimumWidth(104);
            //QSpacerItem *spacer = new QSpacerItem(80, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

            dynamicLabel *icon_phone = new dynamicLabel(frame);
            icon_phone->setFixedSize(24,24);
            QPixmap pix_phone(":/images/images/phone-black-24.png");
            icon_phone->setPixmap(pix_phone);
            dynamicLineEdit *phone = new dynamicLineEdit(frame);
            phone->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
            phone->setFont(defaultfont);
            if(newClientPhones[i+3] != ""){
                phone->setText(newClientPhones[i+3]);
                phone->setToolTip(newClientPhones[i+3]);
            } else {
                phone->setText("—");
                phone->setToolTip("Не указано");
            }

            phone->setReadOnly(true);
            phone->setFrame(false);
            phone->setCursorPosition(0);

            hLay_3->addWidget(icon_phone); hLay_3->addWidget(phone);

            dynamicLabel *icon_email = new dynamicLabel(frame);
            icon_email->setFixedSize(24,24);
            QPixmap pix_email(":/images/images/email-black-24.png");
            icon_email->setPixmap(pix_email);
            dynamicLineEdit *email = new dynamicLineEdit(frame);
            email->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
            email->setFont(defaultfont);
            if(newClientPhones[i+4] != ""){
                email->setText(newClientPhones[i+4]);
                email->setToolTip(newClientPhones[i+4]);
            } else {
                email->setText("—");
                email->setToolTip("Не указано");
            }

            email->setReadOnly(true);
            email->setFrame(false);
            email->setCursorPosition(0);

            hLay_4->addWidget(icon_email); hLay_4->addWidget(email);

            downH->addLayout(hLay_3); downH->addLayout(hLay_4);
            vLay->addLayout(upperH); vLay->addLayout(downH);

            dynamicButtonMainOrNotMainPhone *button_main = new dynamicButtonMainOrNotMainPhone(frame);
            button_main->setID(QString::number(i));
            button_main->setMainStatus(newClientPhones[i]);
            button_main->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
            button_main->setFixedSize(60,24);
            if(newClientPhones[i] == "1"){
                button_main->setStyleSheet("QPushButton{color:black;"
                                        "border-style: solid;"
                                        "border-width:1px;"
                                        "border-radius:3px;"
                                        "border-color: black;"
                                        "max-width:60px;"
                                        "max-height:24px;"
                                        "min-width:60px;"
                                        "min-height:24px;"
                                        "color:black;"
                                        "background-color:rgb(5, 255, 26);"
                                        "}"

                                        "QPushButton:hover{ color:black;"
                                        "border-style: solid;"
                                        "border-width:1px;"
                                        "border-radius:3px;"
                                        "border-color: black;"
                                        "max-width:60px;"
                                        "max-height:24px;"
                                        "min-width:60px;"
                                        "min-height:24px;"
                                        "color:black;"
                                        "background-color:rgb(5, 156, 16);"
                                        "}");
            } else {
                button_main->setStyleSheet("QPushButton{color:black;"
                                        "border-style: solid;"
                                        "border-width:1px;"
                                        "border-radius:3px;"
                                        "border-color: black;"
                                        "max-width:60px;"
                                        "max-height:24px;"
                                        "min-width:60px;"
                                        "min-height:24px;"
                                        "color:black;"
                                        "background-color:rgb(255, 0 , 0);"
                                        "}"

                                        "QPushButton:hover{ color:black;"
                                        "border-style: solid;"
                                        "border-width:1px;"
                                        "border-radius:3px;"
                                        "border-color: black;"
                                        "max-width:60px;"
                                        "max-height:24px;"
                                        "min-width:60px;"
                                        "min-height:24px;"
                                        "color:black;"
                                        "background-color:rgb(145, 0, 0);"
                                        "}");
            }
            button_main->setText("Основной");
            connect(button_main, SIGNAL(clicked()), this, SLOT(setMainOrNotMainPhone()));


            DynamicButton_Delete_clientPhone *button_x = new DynamicButton_Delete_clientPhone(frame);
            button_x->setID(QString::number(i));
            button_x->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
            button_x->setFixedSize(24,24);
            button_x->setStyleSheet("QPushButton{color:black;"
                                    "border-style: solid;"
                                    "border-width:1px;"
                                    "border-radius:3px;"
                                    "border-color: black;"
                                    "max-width:24px;"
                                    "max-height:24px;"
                                    "min-width:24px;"
                                    "min-height:24px;"
                                    "color:black;"
                                    "background-color:rgb(255, 0 , 0);"
                                    "}"

                                    "QPushButton:hover{ color:black;"
                                    "border-style: solid;"
                                    "border-width:1px;"
                                    "border-radius:3px;"
                                    "border-color: black;"
                                    "max-width:24px;"
                                    "max-height:24px;"
                                    "min-width:24px;"
                                    "min-height:24px;"
                                    "color:black;"
                                    "background-color:rgb(145, 0, 0);"
                                    "}");
            button_x->setText("X");
            connect(button_x, SIGNAL(clicked()), this, SLOT(deleteDynamicButton_deleteClientsPhone()));
            lay->addLayout(vLay); lay->addWidget(button_main); lay->addWidget(button_x);
            frame->setLayout(lay);
            if(newClientPhones[i] == "1"){
                if(mainIsThere == false){
                    mainIsThere = true;
                }
                ui->verticalLayout_3->addWidget(frame);
            } else if(newClientPhones[i] == "0"){
                if(notMainIsThere == false){
                    notMainIsThere = true;
                }
                ui->verticalLayout_6->addWidget(frame);
            }
        }
        if(!mainIsThere){
            dynamicFrame *frame = new dynamicFrame(this);
            frame->setStyleSheet("background-color:rgb(232,235,250);"
                                 "color:rgb(36,36,36);"
                                 "border-width:1px;"
                                 "border-radius:8px;"
                                 "border-color: black;");
            frame->setFixedHeight(44);
            QHBoxLayout *lay = new QHBoxLayout(frame);
            dynamicLabel *isClear = new dynamicLabel(frame);
            isClear->setText("Не указано");
            isClear->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
            isClear->setFont(defaultfont);
            lay->addStretch();
            lay->addWidget(isClear);
            lay->addStretch();
            frame->setLayout(lay);
            ui->verticalLayout_3->addWidget(frame);
        }
        if(!notMainIsThere){
            dynamicFrame *frame = new dynamicFrame(this);
            frame->setStyleSheet("background-color:rgb(232,235,250);"
                                 "color:rgb(36,36,36);"
                                 "border-width:1px;"
                                 "border-radius:8px;"
                                 "border-color: black;");
            frame->setFixedHeight(44);
            QHBoxLayout *lay = new QHBoxLayout(frame);
            dynamicLabel *isClear = new dynamicLabel(frame);
            isClear->setText("Не указано");
            isClear->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
            isClear->setFont(defaultfont);
            lay->addStretch();
            lay->addWidget(isClear);
            lay->addStretch();
            frame->setLayout(lay);
            ui->verticalLayout_6->addWidget(frame);
        }
    } else {
        for(int i = 0; i < newClientPhones.size(); i+=2){
            notMainIsThere = true;
            dynamicFrame *frame = new dynamicFrame(this);
            frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            frame->setStyleSheet("background-color:rgb(232,235,250);"
                                 "color:rgb(36,36,36);"
                                 "border-width:1px;"
                                 "border-radius:8px;"
                                 "border-color: black;");
            frame->setFixedHeight(44);
            QHBoxLayout *lay = new QHBoxLayout(frame);
            QHBoxLayout *hLay_1 = new QHBoxLayout();
            QHBoxLayout *hLay_2 = new QHBoxLayout();


            dynamicLabel *icon_fullName = new dynamicLabel(frame);
            icon_fullName->setFixedSize(24,24);
            QPixmap pix_fullName(":/images/images/administrator-black-24.png");
            icon_fullName->setPixmap(pix_fullName);
            dynamicLineEdit *fullName = new dynamicLineEdit(frame);
            fullName->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
            fullName->setFont(defaultfont);
            if(newClientPhones[i] != ""){
                fullName->setText(newClientPhones[i]);
                fullName->setToolTip(newClientPhones[i]);
            } else {
                fullName->setText("—");
                fullName->setToolTip("Не указано");
            }

            fullName->setReadOnly(true);
            fullName->setFrame(false);
            fullName->setCursorPosition(0);

            hLay_1->addWidget(icon_fullName); hLay_1->addWidget(fullName);


            dynamicLabel *icon_phone = new dynamicLabel(frame);
            icon_phone->setFixedSize(26,26);
            QPixmap pix_phone(":/images/images/phone-black-24.png");
            icon_phone->setPixmap(pix_phone);
            dynamicLineEdit *phone = new dynamicLineEdit(frame);
            phone->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
            phone->setFont(defaultfont);
            if(newClientPhones[i+1] != ""){
                phone->setText(newClientPhones[i+1]);
                phone->setToolTip(newClientPhones[i+1]);
            } else {
                phone->setText("—");
                phone->setToolTip("Не указано");
            }

            phone->setReadOnly(true);
            phone->setFrame(false);
            phone->setCursorPosition(0);

            hLay_2->addWidget(icon_phone); hLay_2->addWidget(phone);

            DynamicButton_Delete_clientPhone *button_x = new DynamicButton_Delete_clientPhone(frame);
            button_x->setID(QString::number(i));
            button_x->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
            button_x->setFixedSize(24,24);
            button_x->setStyleSheet("QPushButton{color:black;"
                                    "border-style: solid;"
                                    "border-width:1px;"
                                    "border-radius:3px;"
                                    "border-color: black;"
                                    "max-width:24px;"
                                    "max-height:24px;"
                                    "min-width:24px;"
                                    "min-height:24px;"
                                    "color:black;"
                                    "background-color:rgb(255, 0 , 0);"
                                    "}"

                                    "QPushButton:hover{ color:black;"
                                    "border-style: solid;"
                                    "border-width:1px;"
                                    "border-radius:3px;"
                                    "border-color: black;"
                                    "max-width:24px;"
                                    "max-height:24px;"
                                    "min-width:24px;"
                                    "min-height:24px;"
                                    "color:black;"
                                    "background-color:rgb(145, 0, 0);"
                                    "}");
            button_x->setText("X");
            connect(button_x, SIGNAL(clicked()), this, SLOT(deleteDynamicButton_deleteClientsPhone()));

            lay->addLayout(hLay_1); lay->addLayout(hLay_2); lay->addWidget(button_x);

            ui->verticalLayout_6->addWidget(frame);
        }
        if(!notMainIsThere){
            dynamicFrame *frame = new dynamicFrame(this);
            frame->setStyleSheet("background-color:rgb(232,235,250);"
                                 "color:rgb(36,36,36);"
                                 "border-width:1px;"
                                 "border-radius:8px;"
                                 "border-color: black;");
            QHBoxLayout *lay = new QHBoxLayout(frame);
            dynamicLabel *isClear = new dynamicLabel(frame);
            frame->setFixedHeight(44);
            isClear->setText("Не указано");
            isClear->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
            isClear->setFont(defaultfont);
            lay->addStretch();
            lay->addWidget(isClear);
            lay->addStretch();
            frame->setLayout(lay);
            ui->verticalLayout_6->addWidget(frame);
        }
    }
}

void MainWindow::setMainOrNotMainPhone(){
    dynamicButtonMainOrNotMainPhone *button = (dynamicButtonMainOrNotMainPhone*) sender();
    QString temp;
    if(ui->stackedWidget_clients->currentIndex() == 1){
        if(button->getMainStatus() == 0){
            bool isOK = true;
            for(int i = 0; i < newClientPhones.size(); i+=5){
                if(newClientPhones[i] == "1"){
                    isOK = false;
                }
            }
            if(!isOK){
                QMessageBox::StandardButton reply =
                QMessageBox::question(this, "Подтвердите действие", "<FONT COLOR = '#000000'>Основной контакт уже существует, заменить его? </FONT>");
                if (reply == QMessageBox::Yes){
                    temp = newClientPhones[button->getID()+4];
                    newClientPhones.removeAt(button->getID()+4);
                    newClientPhones.push_front(temp);

                    temp = newClientPhones[button->getID()+4];
                    newClientPhones.removeAt(button->getID()+4);
                    newClientPhones.push_front(temp);

                    temp = newClientPhones[button->getID()+4];
                    newClientPhones.removeAt(button->getID()+4);
                    newClientPhones.push_front(temp);

                    temp = newClientPhones[button->getID()+4];
                    newClientPhones.removeAt(button->getID()+4);
                    newClientPhones.push_front(temp);

                    newClientPhones.removeAt(button->getID()+4);
                    newClientPhones.push_front("1");

                    newClientPhones.push_back("0");
                    newClientPhones.removeAt(5);
                    for(int i = 0; i< 4; i++){
                        newClientPhones.push_back(newClientPhones[5]);
                        newClientPhones.removeAt(5);
                    }
                }
            } else {
                temp = newClientPhones[button->getID()+4];
                newClientPhones.removeAt(button->getID()+4);
                newClientPhones.push_front(temp);

                temp = newClientPhones[button->getID()+4];
                newClientPhones.removeAt(button->getID()+4);
                newClientPhones.push_front(temp);

                temp = newClientPhones[button->getID()+4];
                newClientPhones.removeAt(button->getID()+4);
                newClientPhones.push_front(temp);

                temp = newClientPhones[button->getID()+4];
                newClientPhones.removeAt(button->getID()+4);
                newClientPhones.push_front(temp);

                newClientPhones.removeAt(button->getID()+4);
                newClientPhones.push_front("1");
            }
        } else if(button->getMainStatus() == 1){
            newClientPhones.push_back("0");
            newClientPhones.removeAt(0);
            newClientPhones.push_back(newClientPhones[0]);
            newClientPhones.removeAt(0);
            newClientPhones.push_back(newClientPhones[0]);
            newClientPhones.removeAt(0);
            newClientPhones.push_back(newClientPhones[0]);
            newClientPhones.removeAt(0);
            newClientPhones.push_back(newClientPhones[0]);
            newClientPhones.removeAt(0);
        }
        UpdateNewClientPhones();

    } else if(ui->stackedWidget_clients->currentIndex() == 2){
        CheckUpdatesForRedactClientPhones();
        if(button->getMainStatus() == 0){
            bool isOK = true;
            for(int i = 0; i < redactClientPhones.size(); i+=5){
                if(redactClientPhones[i] == "1"){
                    isOK = false;
                }
            }
            if(!isOK){
                QMessageBox::StandardButton reply =
                QMessageBox::question(this, "Подтвердите действие", "<FONT COLOR = '#000000'>Основной контакт уже существует, заменить его? </FONT>");
                if (reply == QMessageBox::Yes){
                    temp = redactClientPhones[button->getID()+4];
                    redactClientPhones.removeAt(button->getID()+4);
                    redactClientPhones.push_front(temp);

                    temp = redactClientPhones[button->getID()+4];
                    redactClientPhones.removeAt(button->getID()+4);
                    redactClientPhones.push_front(temp);

                    temp = redactClientPhones[button->getID()+4];
                    redactClientPhones.removeAt(button->getID()+4);
                    redactClientPhones.push_front(temp);

                    temp = redactClientPhones[button->getID()+4];
                    redactClientPhones.removeAt(button->getID()+4);
                    redactClientPhones.push_front(temp);

                    redactClientPhones.removeAt(button->getID()+4);
                    redactClientPhones.push_front("1");

                    redactClientPhones.push_back("0");
                    redactClientPhones.removeAt(5);
                    for(int i = 0; i< 4; i++){
                        redactClientPhones.push_back(redactClientPhones[5]);
                        redactClientPhones.removeAt(5);
                    }
                }
            } else {
                temp = redactClientPhones[button->getID()+4];
                redactClientPhones.removeAt(button->getID()+4);
                redactClientPhones.push_front(temp);

                temp = redactClientPhones[button->getID()+4];
                redactClientPhones.removeAt(button->getID()+4);
                redactClientPhones.push_front(temp);

                temp = redactClientPhones[button->getID()+4];
                redactClientPhones.removeAt(button->getID()+4);
                redactClientPhones.push_front(temp);

                temp = redactClientPhones[button->getID()+4];
                redactClientPhones.removeAt(button->getID()+4);
                redactClientPhones.push_front(temp);

                redactClientPhones.removeAt(button->getID()+4);
                redactClientPhones.push_front("1");
            }
        } else if(button->getMainStatus() == 1){
            redactClientPhones.push_back("0");
            redactClientPhones.removeAt(0);
            redactClientPhones.push_back(redactClientPhones[0]);
            redactClientPhones.removeAt(0);
            redactClientPhones.push_back(redactClientPhones[0]);
            redactClientPhones.removeAt(0);
            redactClientPhones.push_back(redactClientPhones[0]);
            redactClientPhones.removeAt(0);
            redactClientPhones.push_back(redactClientPhones[0]);
            redactClientPhones.removeAt(0);
        }
        UpdateRedactClientPhones();
    }
}

void MainWindow::UpdateOutputCLientPhones()
{
    ClearOutputClientPhones();
    QVariant data = ui->listView->model()->data(ui->listView->model()->index(ui->listView->currentIndex().row(),0),Qt::DisplayRole).toString();
    QSqlQuery query;
    bool mainIsThere = false, notMainIsThere = false;
    query.prepare("SELECT mainStatus, fullName, information, phone, email FROM clientPhones WHERE companyId = :search");
    query.bindValue(":search", data); //поиск по id
    if(!query.exec()){
        qDebug() << query.lastError().text();
    }
    QSqlRecord rec = query.record();
    if(!providersAreOpened){
        while(query.next()){
            dynamicFrame *frame = new dynamicFrame(this);
            frame->setStyleSheet("background-color:rgb(232,235,250);"
                                 "color:rgb(36,36,36);"
                                 "border-width:1px;"
                                 "border-radius:8px;"
                                 "border-color: black;");
            QHBoxLayout *lay = new QHBoxLayout(frame);
            QHBoxLayout *hLay_1 = new QHBoxLayout();
            QHBoxLayout *hLay_2 = new QHBoxLayout();
            QHBoxLayout *hLay_3 = new QHBoxLayout();
            QHBoxLayout *hLay_4 = new QHBoxLayout();
            QHBoxLayout *upperH = new QHBoxLayout();
            QHBoxLayout *downH = new QHBoxLayout();
            QVBoxLayout *vLay = new QVBoxLayout();


            dynamicLabel *icon_fullName = new dynamicLabel(frame);
            icon_fullName->setFixedSize(24,24);
            QPixmap pix_fullName(":/images/images/administrator-black-24.png");
            icon_fullName->setPixmap(pix_fullName);
            dynamicLineEdit *fullName = new dynamicLineEdit(frame);
            fullName->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
            fullName->setFont(defaultfont);
            if(query.value(rec.indexOf("fullName")).toString() != ""){
                fullName->setText(query.value(rec.indexOf("fullName")).toString());
                fullName->setToolTip(query.value(rec.indexOf("fullName")).toString());
            } else {
                fullName->setText("—");
                fullName->setToolTip("Не указано");
            }

            fullName->setReadOnly(true);
            fullName->setFrame(false);
            fullName->setCursorPosition(0);

            hLay_1->addWidget(icon_fullName); hLay_1->addWidget(fullName);

            dynamicLabel *icon_info = new dynamicLabel(frame);
            icon_info->setFixedSize(24,24);
            QPixmap pix_info(":/images/images/info-black-24.png");
            icon_info->setPixmap(pix_info);
            dynamicLineEdit *information = new dynamicLineEdit(frame);
            information->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
            information->setFont(defaultfont);
            if(query.value(rec.indexOf("information")).toString() != ""){
                information->setText(query.value(rec.indexOf("information")).toString());
                information->setToolTip(query.value(rec.indexOf("information")).toString());
            } else {
                information->setText("—");
                information->setToolTip("Не указано");
            }

            information->setReadOnly(true);
            information->setFrame(false);
            information->setCursorPosition(0);

            hLay_2->addWidget(icon_info); hLay_2->addWidget(information);

            upperH->addLayout(hLay_1); upperH->addLayout(hLay_2);
            //fullName->setMinimumWidth(104);
            //QSpacerItem *spacer = new QSpacerItem(80, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

            dynamicLabel *icon_phone = new dynamicLabel(frame);
            icon_phone->setFixedSize(24,24);
            QPixmap pix_phone(":/images/images/phone-black-24.png");
            icon_phone->setPixmap(pix_phone);
            dynamicLineEdit *phone = new dynamicLineEdit(frame);
            phone->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
            phone->setFont(defaultfont);
            if(query.value(rec.indexOf("phone")).toString() != ""){
                phone->setText(query.value(rec.indexOf("phone")).toString());
                phone->setToolTip(query.value(rec.indexOf("phone")).toString());
            } else {
                phone->setText("—");
                phone->setToolTip("Не указано");
            }

            phone->setReadOnly(true);
            phone->setFrame(false);
            phone->setCursorPosition(0);

            hLay_3->addWidget(icon_phone); hLay_3->addWidget(phone);

            dynamicLabel *icon_email = new dynamicLabel(frame);
            icon_email->setFixedSize(24,24);
            QPixmap pix_email(":/images/images/email-black-24.png");
            icon_email->setPixmap(pix_email);
            dynamicLineEdit *email = new dynamicLineEdit(frame);
            email->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
            email->setFont(defaultfont);
            if(query.value(rec.indexOf("email")).toString() != ""){
                email->setText(query.value(rec.indexOf("email")).toString());
                email->setToolTip(query.value(rec.indexOf("email")).toString());
            } else {
                email->setText("—");
                email->setToolTip("Не указано");
            }

            email->setReadOnly(true);
            email->setFrame(false);
            email->setCursorPosition(0);

            hLay_4->addWidget(icon_email); hLay_4->addWidget(email);

            downH->addLayout(hLay_3); downH->addLayout(hLay_4);
            vLay->addLayout(upperH); vLay->addLayout(downH);
            lay->addLayout(vLay);
            frame->setLayout(lay);

            if(query.value(0).toString() == "1"){
                if(mainIsThere == false){
                    mainIsThere = true;
                }
                ui->verticalLayout_22->addWidget(frame);
            } else if(query.value(0).toString() == "0"){
                if(notMainIsThere == false){
                    notMainIsThere = true;
                }
                ui->verticalLayout_48->addWidget(frame);
            }
        }
        if(!mainIsThere){
            dynamicFrame *frame = new dynamicFrame(this);
            frame->setStyleSheet("background-color:rgb(232,235,250);"
                                 "color:rgb(36,36,36);"
                                 "border-width:1px;"
                                 "border-radius:8px;"
                                 "border-color: black;");
            frame->setFixedHeight(44);
            QHBoxLayout *lay = new QHBoxLayout(frame);
            dynamicLabel *isClear = new dynamicLabel(frame);
            isClear->setText("Не указано");
            isClear->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
            isClear->setFont(defaultfont);
            lay->addStretch();
            lay->addWidget(isClear);
            lay->addStretch();
            frame->setLayout(lay);
            ui->verticalLayout_22->addWidget(frame);
        }
        if(!notMainIsThere){
            dynamicFrame *frame = new dynamicFrame(this);
            frame->setStyleSheet("background-color:rgb(232,235,250);"
                                 "color:rgb(36,36,36);"
                                 "border-width:1px;"
                                 "border-radius:8px;"
                                 "border-color: black;");
            frame->setFixedHeight(44);
            QHBoxLayout *lay = new QHBoxLayout(frame);
            dynamicLabel *isClear = new dynamicLabel(frame);
            isClear->setText("Не указано");
            isClear->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
            isClear->setFont(defaultfont);
            lay->addStretch();
            lay->addWidget(isClear);
            lay->addStretch();
            frame->setLayout(lay);
            ui->verticalLayout_48->addWidget(frame);
        }
    } else {
        while(query.next()){
            notMainIsThere = true;
            dynamicFrame *frame = new dynamicFrame(this);
            frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            frame->setStyleSheet("background-color:rgb(232,235,250);"
                                 "color:rgb(36,36,36);"
                                 "border-width:1px;"
                                 "border-radius:8px;"
                                 "border-color: black;");
            frame->setFixedHeight(44);
            QHBoxLayout *lay = new QHBoxLayout(frame);
            QHBoxLayout *hLay_1 = new QHBoxLayout();
            QHBoxLayout *hLay_2 = new QHBoxLayout();


            dynamicLabel *icon_fullName = new dynamicLabel(frame);
            icon_fullName->setFixedSize(24,24);
            QPixmap pix_fullName(":/images/images/administrator-black-24.png");
            icon_fullName->setPixmap(pix_fullName);
            dynamicLineEdit *fullName = new dynamicLineEdit(frame);
            fullName->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
            fullName->setFont(defaultfont);
            if(query.value(rec.indexOf("fullName")).toString() != ""){
                fullName->setText(query.value(rec.indexOf("fullName")).toString());
                fullName->setToolTip(query.value(rec.indexOf("fullName")).toString());
            } else {
                fullName->setText("—");
                fullName->setToolTip("Не указано");
            }

            fullName->setReadOnly(true);
            fullName->setFrame(false);
            fullName->setCursorPosition(0);

            hLay_1->addWidget(icon_fullName); hLay_1->addWidget(fullName);


            dynamicLabel *icon_phone = new dynamicLabel(frame);
            icon_phone->setFixedSize(24,24);
            QPixmap pix_phone(":/images/images/phone-black-24.png");
            icon_phone->setPixmap(pix_phone);
            dynamicLineEdit *phone = new dynamicLineEdit(frame);
            phone->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
            phone->setFont(defaultfont);
            if(query.value(rec.indexOf("phone")).toString() != ""){
                phone->setText(query.value(rec.indexOf("phone")).toString());
                phone->setToolTip(query.value(rec.indexOf("phone")).toString());
            } else {
                phone->setText("—");
                phone->setToolTip("Не указано");
            }

            phone->setReadOnly(true);
            phone->setFrame(false);
            phone->setCursorPosition(0);

            hLay_2->addWidget(icon_phone); hLay_2->addWidget(phone);

            lay->addLayout(hLay_1); lay->addLayout(hLay_2);

            ui->verticalLayout_48->addWidget(frame);
        }
        if(!notMainIsThere){
            dynamicFrame *frame = new dynamicFrame(this);
            frame->setStyleSheet("background-color:rgb(232,235,250);"
                                 "color:rgb(36,36,36);"
                                 "border-width:1px;"
                                 "border-radius:8px;"
                                 "border-color: black;");
            frame->setFixedHeight(44);
            QHBoxLayout *lay = new QHBoxLayout(frame);
            dynamicLabel *isClear = new dynamicLabel(frame);
            isClear->setFixedHeight(26);
            isClear->setText("Не указано");
            isClear->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
            isClear->setFont(defaultfont);
            lay->addStretch();
            lay->addWidget(isClear);
            lay->addStretch();
            frame->setLayout(lay);
            ui->verticalLayout_48->addWidget(frame);
        }
    }
}

void MainWindow::UpdateRedactClientPhones()
{
    ClearRedactClientPhones();
    bool mainIsThere = false, notMainIsThere = false;
    if(!providersAreOpened){
        for(int i = 0; i < redactClientPhones.size(); i+=5){
            dynamicFrame *frame = new dynamicFrame(this);
            frame->setStyleSheet("background-color:rgb(232,235,250);"
                                 "color:rgb(36,36,36);"
                                 "border-width:1px;"
                                 "border-radius:8px;"
                                 "border-color: black;");
            frame->setFixedHeight(72);
            QHBoxLayout *lay = new QHBoxLayout(frame);
            lay->setContentsMargins(0,0,0,0);
            lay->setSpacing(12);
            QHBoxLayout *hLay_1 = new QHBoxLayout();
            hLay_1->setContentsMargins(0,0,4,0);
            hLay_1->setSpacing(3);
            QHBoxLayout *hLay_2 = new QHBoxLayout();
            hLay_2->setContentsMargins(0,0,4,0);
            hLay_2->setSpacing(3);
            QHBoxLayout *hLay_3 = new QHBoxLayout();
            hLay_3->setContentsMargins(0,0,4,0);
            hLay_3->setSpacing(3);
            QHBoxLayout *hLay_4 = new QHBoxLayout();
            hLay_4->setContentsMargins(0,0,4,0);
            hLay_4->setSpacing(3);
            QHBoxLayout *upperH = new QHBoxLayout();
            upperH->setContentsMargins(4,0,4,0);
            upperH->setSpacing(12);
            QHBoxLayout *downH = new QHBoxLayout();
            downH->setContentsMargins(4,0,4,0);
            downH->setSpacing(12);
            QVBoxLayout *vLay = new QVBoxLayout();
            vLay->setContentsMargins(4,3,4,3);
            vLay->setSpacing(6);



            dynamicLabel *icon_fullName = new dynamicLabel(frame);
            icon_fullName->setFixedSize(24,24);
            QPixmap pix_fullName(":/images/images/administrator-black-24.png");
            icon_fullName->setPixmap(pix_fullName);
            dynamicLineEdit *fullName = new dynamicLineEdit(frame);
            fullName->setStyleSheet("color:rgb(40,40,40);"
                                    "background-color:rgb(255,255,255);"
                                    "border:1px solid rgb(224,224,224);"
                                    "border-radius:4px;"
                                    "border-bottom:2px solid rgb(91,95,199);");
            fullName->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            fullName->setFont(defaultfont);
            fullName->setPlaceholderText("ФИО...");
            if(redactClientPhones[i+1] != ""){
                fullName->setText(redactClientPhones[i+1]);
            }
            hLay_1->addWidget(icon_fullName); hLay_1->addWidget(fullName);

            dynamicLabel *icon_info = new dynamicLabel(frame);
            icon_info->setFixedSize(24,24);
            QPixmap pix_info(":/images/images/info-black-24.png");
            icon_info->setPixmap(pix_info);
            dynamicLineEdit *information = new dynamicLineEdit(frame);
            information->setStyleSheet("color:rgb(40,40,40);"
                                       "background-color:rgb(255,255,255);"
                                       "border:1px solid rgb(224,224,224);"
                                       "border-radius:4px;"
                                       "border-bottom:2px solid rgb(91,95,199);");
            information->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            information->setFont(defaultfont);
            information->setPlaceholderText("Описание...");
            if(redactClientPhones[i+2] != ""){
                information->setText(redactClientPhones[i+2]);
            }
            hLay_2->addWidget(icon_info); hLay_2->addWidget(information);

            upperH->addLayout(hLay_1); upperH->addLayout(hLay_2);

            dynamicLabel *icon_phone = new dynamicLabel(frame);
            icon_phone->setFixedSize(24,24);
            QPixmap pix_phone(":/images/images/phone-black-24.png");
            icon_phone->setPixmap(pix_phone);
            dynamicLineEdit *phone = new dynamicLineEdit(frame);
            phone->setStyleSheet("color:rgb(40,40,40);"
                                 "background-color:rgb(255,255,255);"
                                 "border:1px solid rgb(224,224,224);"
                                 "border-radius:4px;"
                                 "border-bottom:2px solid rgb(91,95,199);");
            phone->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            phone->setFont(defaultfont);
            phone->setInputMask("+7\\(999\\)999\\-99\\-99;_");
            if(redactClientPhones[i+3] != "+7()--"){
                phone->setText(redactClientPhones[i+3]);
            }
            hLay_3->addWidget(icon_phone); hLay_3->addWidget(phone);

            dynamicLabel *icon_email = new dynamicLabel(frame);
            icon_email->setFixedSize(24,24);
            QPixmap pix_email(":/images/images/email-black-24.png");
            icon_email->setPixmap(pix_email);
            dynamicLineEdit *email = new dynamicLineEdit(frame);
            email->setStyleSheet("color:rgb(40,40,40);"
                                 "background-color:rgb(255,255,255);"
                                 "border:1px solid rgb(224,224,224);"
                                 "border-radius:4px;"
                                 "border-bottom:2px solid rgb(91,95,199);");
            email->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            email->setFont(defaultfont);
            email->setPlaceholderText("Email...");
            if(redactClientPhones[i+4] != ""){
                email->setText(redactClientPhones[i+4]);
            }
            hLay_4->addWidget(icon_email); hLay_4->addWidget(email);

            downH->addLayout(hLay_3); downH->addLayout(hLay_4);
            vLay->addLayout(upperH); vLay->addLayout(downH);

            dynamicButtonMainOrNotMainPhone *button_main = new dynamicButtonMainOrNotMainPhone(frame);
            button_main->setID(QString::number(i));
            button_main->setMainStatus(redactClientPhones[i]);
            button_main->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
            button_main->setFixedSize(60,24);
            if(redactClientPhones[i] == "1"){
                button_main->setStyleSheet("QPushButton{color:black;"
                                        "border-style: solid;"
                                        "border-width:1px;"
                                        "border-radius:3px;"
                                        "border-color: black;"
                                        "max-width:60px;"
                                        "max-height:24px;"
                                        "min-width:60px;"
                                        "min-height:24px;"
                                        "color:black;"
                                        "background-color:rgb(5, 255, 26);"
                                        "}"

                                        "QPushButton:hover{ color:black;"
                                        "border-style: solid;"
                                        "border-width:1px;"
                                        "border-radius:3px;"
                                        "border-color: black;"
                                        "max-width:60px;"
                                        "max-height:24px;"
                                        "min-width:60px;"
                                        "min-height:24px;"
                                        "color:black;"
                                        "background-color:rgb(5, 156, 16);"
                                        "}");
            } else {
                button_main->setStyleSheet("QPushButton{color:black;"
                                        "border-style: solid;"
                                        "border-width:1px;"
                                        "border-radius:3px;"
                                        "border-color: black;"
                                        "max-width:60px;"
                                        "max-height:24px;"
                                        "min-width:60px;"
                                        "min-height:24px;"
                                        "color:black;"
                                        "background-color:rgb(255, 0 , 0);"
                                        "}"

                                        "QPushButton:hover{ color:black;"
                                        "border-style: solid;"
                                        "border-width:1px;"
                                        "border-radius:3px;"
                                        "border-color: black;"
                                        "max-width:60px;"
                                        "max-height:24px;"
                                        "min-width:60px;"
                                        "min-height:24px;"
                                        "color:black;"
                                        "background-color:rgb(145, 0, 0);"
                                        "}");
            }
            button_main->setText("Основной");
            connect(button_main, SIGNAL(clicked()), this, SLOT(setMainOrNotMainPhone()));

            DynamicButton_Delete_clientPhone *button_x = new DynamicButton_Delete_clientPhone(frame);
            button_x->setID(QString::number(i));
            button_x->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
            button_x->setFixedSize(24,24);
            button_x->setStyleSheet("QPushButton{color:black;"
                                    "border-style: solid;"
                                    "border-width:1px;"
                                    "border-radius:3px;"
                                    "border-color: black;"
                                    "max-width:24px;"
                                    "max-height:24px;"
                                    "min-width:24px;"
                                    "min-height:24px;"
                                    "color:black;"
                                    "background-color:rgb(255, 0 , 0);"
                                    "}"

                                    "QPushButton:hover{ color:black;"
                                    "border-style: solid;"
                                    "border-width:1px;"
                                    "border-radius:3px;"
                                    "border-color: black;"
                                    "max-width:24px;"
                                    "max-height:24px;"
                                    "min-width:24px;"
                                    "min-height:24px;"
                                    "color:black;"
                                    "background-color:rgb(145, 0, 0);"
                                    "}");
            button_x->setText("X");
            connect(button_x, SIGNAL(clicked()), this, SLOT(deleteDynamicButton_deleteClientsPhone()));
            lay->addLayout(vLay); lay->addWidget(button_main); lay->addWidget(button_x);
            frame->setLayout(lay);
            if(redactClientPhones[i] == "1"){
                if(mainIsThere == false){
                    mainIsThere = true;
                }
                ui->verticalLayout_78->addWidget(frame);
            } else if(redactClientPhones[i] == "0"){
                if(notMainIsThere == false){
                    notMainIsThere = true;
                }
                ui->verticalLayout_80->addWidget(frame);
            }
        }
        if(!mainIsThere){
            dynamicFrame *frame = new dynamicFrame(this);
            frame->setStyleSheet("background-color:rgb(232,235,250);"
                                 "color:rgb(36,36,36);"
                                 "border-width:1px;"
                                 "border-radius:8px;"
                                 "border-color: black;");
            frame->setFixedHeight(44);
            QHBoxLayout *lay = new QHBoxLayout(frame);
            dynamicLabel *isClear = new dynamicLabel(frame);
            isClear->setText("Не указано");
            isClear->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
            isClear->setFont(defaultfont);
            lay->addStretch();
            lay->addWidget(isClear);
            lay->addStretch();
            frame->setLayout(lay);
            ui->verticalLayout_78->addWidget(frame);
        }
        if(!notMainIsThere){
            dynamicFrame *frame = new dynamicFrame(this);
            frame->setStyleSheet("background-color:rgb(232,235,250);"
                                 "color:rgb(36,36,36);"
                                 "border-width:1px;"
                                 "border-radius:8px;"
                                 "border-color: black;");
            frame->setFixedHeight(44);
            QHBoxLayout *lay = new QHBoxLayout(frame);
            dynamicLabel *isClear = new dynamicLabel(frame);
            isClear->setText("Не указано");
            isClear->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
            isClear->setFont(defaultfont);
            lay->addStretch();
            lay->addWidget(isClear);
            lay->addStretch();
            frame->setLayout(lay);
            ui->verticalLayout_80->addWidget(frame);
        }
    } else {
        for(int i = 0; i < redactClientPhones.size(); i+=2){
            notMainIsThere = true;
            dynamicFrame *frame = new dynamicFrame(this);
            frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            frame->setStyleSheet("background-color:rgb(232,235,250);"
                                 "color:rgb(36,36,36);"
                                 "border-width:1px;"
                                 "border-radius:8px;"
                                 "border-color: black;");
            frame->setFixedHeight(44);
            QHBoxLayout *lay = new QHBoxLayout(frame);
            lay->setContentsMargins(4,3,4,3);
            lay->setSpacing(12);
            QHBoxLayout *hLay_1 = new QHBoxLayout();
            hLay_1->setContentsMargins(0,2,4,2);
            hLay_1->setSpacing(3);
            QHBoxLayout *hLay_2 = new QHBoxLayout();
            hLay_2->setContentsMargins(0,2,4,2);
            hLay_2->setSpacing(3);


            dynamicLabel *icon_fullName = new dynamicLabel(frame);
            icon_fullName->setFixedSize(24,24);
            QPixmap pix_fullName(":/images/images/administrator-black-24.png");
            icon_fullName->setPixmap(pix_fullName);
            dynamicLineEdit *fullName = new dynamicLineEdit(frame);
            fullName->setStyleSheet("color:rgb(40,40,40);"
                                    "background-color:rgb(255,255,255);"
                                    "border:1px solid rgb(224,224,224);"
                                    "border-radius:4px;"
                                    "border-bottom:2px solid rgb(91,95,199);");
            fullName->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            fullName->setFont(defaultfont);
            fullName->setPlaceholderText("ФИО...");
            if(redactClientPhones[i] != ""){
                fullName->setText(redactClientPhones[i]);
            }
            hLay_1->addWidget(icon_fullName); hLay_1->addWidget(fullName);

            dynamicLabel *icon_phone = new dynamicLabel(frame);
            icon_phone->setFixedSize(24,24);
            QPixmap pix_phone(":/images/images/phone-black-24.png");
            icon_phone->setPixmap(pix_phone);
            dynamicLineEdit *phone = new dynamicLineEdit(frame);
            phone->setStyleSheet("color:rgb(40,40,40);"
                                 "background-color:rgb(255,255,255);"
                                 "border:1px solid rgb(224,224,224);"
                                 "border-radius:4px;"
                                 "border-bottom:2px solid rgb(91,95,199);");
            phone->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            phone->setFont(defaultfont);
            phone->setInputMask("+7\\(999\\)999\\-99\\-99;_");
            if(redactClientPhones[i+1] != "+7()--"){
                phone->setText(redactClientPhones[i+1]);
            }
            hLay_2->addWidget(icon_phone); hLay_2->addWidget(phone);

            DynamicButton_Delete_clientPhone *button_x = new DynamicButton_Delete_clientPhone(frame);
            button_x->setID(QString::number(i));
            button_x->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
            button_x->setFixedSize(24,24);
            button_x->setStyleSheet("QPushButton{color:black;"
                                    "border-style: solid;"
                                    "border-width:1px;"
                                    "border-radius:3px;"
                                    "border-color: black;"
                                    "max-width:24px;"
                                    "max-height:24px;"
                                    "min-width:24px;"
                                    "min-height:24px;"
                                    "color:black;"
                                    "background-color:rgb(255, 0 , 0);"
                                    "}"

                                    "QPushButton:hover{ color:black;"
                                    "border-style: solid;"
                                    "border-width:1px;"
                                    "border-radius:3px;"
                                    "border-color: black;"
                                    "max-width:24px;"
                                    "max-height:24px;"
                                    "min-width:24px;"
                                    "min-height:24px;"
                                    "color:black;"
                                    "background-color:rgb(145, 0, 0);"
                                    "}");
            button_x->setText("X");
            connect(button_x, SIGNAL(clicked()), this, SLOT(deleteDynamicButton_deleteClientsPhone()));

            lay->addLayout(hLay_1); lay->addLayout(hLay_2); lay->addWidget(button_x);

            ui->verticalLayout_80->addWidget(frame);
        }
        if(!notMainIsThere){
            dynamicFrame *frame = new dynamicFrame(this);
            frame->setStyleSheet("background-color:rgb(232,235,250);"
                                 "color:rgb(36,36,36);"
                                 "border-width:1px;"
                                 "border-radius:8px;"
                                 "border-color: black;");
            frame->setFixedHeight(44);
            QHBoxLayout *lay = new QHBoxLayout(frame);
            dynamicLabel *isClear = new dynamicLabel(frame);
            isClear->setText("Не указано");
            isClear->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
            isClear->setFont(defaultfont);
            lay->addStretch();
            lay->addWidget(isClear);
            lay->addStretch();
            frame->setLayout(lay);
            ui->verticalLayout_80->addWidget(frame);
        }
    }
}

void MainWindow::CheckUpdatesForRedactClientPhones()
{
    int temp = 0;
    bool mainIsThere = false, notMainIsThere = false;
    if(!providersAreOpened){
        if(redactClientPhones.count() != 0){
            for(int i = 0; i < redactClientPhones.count(); i+=5){
                if(redactClientPhones[i] == "1"){
                    mainIsThere = true;
                } else if (redactClientPhones[i] == "0"){
                    notMainIsThere = true;
                }
            }
            if(mainIsThere){
                dynamicFrame *frame = qobject_cast<dynamicFrame*>(ui->verticalLayout_78->itemAt(0)->widget());
                QList<dynamicLineEdit*> widgetList = frame->findChildren<dynamicLineEdit*>();
                if(redactClientPhones[temp+1] != widgetList[0]->text()){
                    redactClientPhones[temp+1] = widgetList[0]->text();
                }
                if(redactClientPhones[temp+2] != widgetList[1]->text()){
                    redactClientPhones[temp+2] = widgetList[1]->text();
                }
                if(widgetList[2]->text() == "+7()--"){
                    redactClientPhones[temp+3] = "";
                } else if(redactClientPhones[temp+3] != widgetList[2]->text()){
                    redactClientPhones[temp+3] = widgetList[2]->text();
                }
                if(redactClientPhones[temp+4] != widgetList[3]->text()){
                    redactClientPhones[temp+4] = widgetList[3]->text();
                }
                temp+=5;
            }
            if(notMainIsThere){
                for(int i = 0; i < ui->verticalLayout_80->count(); i++){
                    dynamicFrame *frame = qobject_cast<dynamicFrame*>(ui->verticalLayout_80->itemAt(i)->widget());
                    QList<dynamicLineEdit*> widgetList = frame->findChildren<dynamicLineEdit*>();
                    if(redactClientPhones[temp+1] != widgetList[0]->text()){
                        redactClientPhones[temp+1] = widgetList[0]->text();
                    }
                    if(redactClientPhones[temp+2] != widgetList[1]->text()){
                        redactClientPhones[temp+2] = widgetList[1]->text();
                    }
                    if(widgetList[2]->text() == "+7()--"){
                        redactClientPhones[temp+3] = "";
                    } else if(redactClientPhones[temp+3] != widgetList[2]->text()){
                        redactClientPhones[temp+3] = widgetList[2]->text();
                    }
                    if(redactClientPhones[temp+4] != widgetList[3]->text()){
                        redactClientPhones[temp+4] = widgetList[3]->text();
                    }
                    temp+=5;
                }
            }
        }
    } else {
        for(int i = 0; i < redactClientPhones.count()/2; i++){
            dynamicFrame *frame = qobject_cast<dynamicFrame*>(ui->verticalLayout_80->itemAt(i)->widget());
            QList<dynamicLineEdit*> widgetList = frame->findChildren<dynamicLineEdit*>();
            if(redactClientPhones[temp] != widgetList[0]->text()){
                redactClientPhones[temp] = widgetList[0]->text();
            }
            if(widgetList[1]->text() == "+7()--"){
                redactClientPhones[temp+1] = "";
            } else if(redactClientPhones[temp+1] != widgetList[1]->text()){
                redactClientPhones[temp+1] = widgetList[1]->text();
            }
            temp+=2;
        }
    }
}

void MainWindow::deleteDynamicButton_deleteClientsPhone()
{
     dynamicButton *button = (dynamicButton*) sender();
     if(!providersAreOpened){
         if(ui->stackedWidget_clients->currentIndex() == 1){
             newClientPhones.removeAt(button->getID() + 4);
             newClientPhones.removeAt(button->getID() + 3);
             newClientPhones.removeAt(button->getID() + 2);
             newClientPhones.removeAt(button->getID() + 1);
             newClientPhones.removeAt(button->getID());
             UpdateNewClientPhones();
         } else if (ui->stackedWidget_clients->currentIndex() == 2){
             CheckUpdatesForRedactClientPhones();
             redactClientPhones.removeAt(button->getID() + 4);
             redactClientPhones.removeAt(button->getID() + 3);
             redactClientPhones.removeAt(button->getID() + 2);
             redactClientPhones.removeAt(button->getID() + 1);
             redactClientPhones.removeAt(button->getID());
             UpdateRedactClientPhones();
         }
     } else {
         if(ui->stackedWidget_clients->currentIndex() == 1){
             newClientPhones.removeAt(button->getID() + 1);
             newClientPhones.removeAt(button->getID());
             UpdateNewClientPhones();
         } else if (ui->stackedWidget_clients->currentIndex() == 2){
             CheckUpdatesForRedactClientPhones();
             redactClientPhones.removeAt(button->getID() + 1);
             redactClientPhones.removeAt(button->getID());
             UpdateRedactClientPhones();
         }
     }
}

void MainWindow::ClearNewClientPhones()
{
    if(ui->verticalLayout_3->count() != 0){
        int count = ui->verticalLayout_3->count();
        for(int i = 0; i < count; i++){
            dynamicFrame *frame = qobject_cast<dynamicFrame*>(ui->verticalLayout_3->itemAt(ui->verticalLayout_3->count()- 1)->widget());
            delete frame;
        }
    }
    if(ui->verticalLayout_6->count() != 0){
        int count = ui->verticalLayout_6->count();
        for(int i = 0; i < count; i++){
            dynamicFrame *frame = qobject_cast<dynamicFrame*>(ui->verticalLayout_6->itemAt(ui->verticalLayout_6->count()- 1)->widget());
            delete frame;
        }
    }
}

void MainWindow::ClearOutputClientPhones()
{
    if(ui->verticalLayout_22->count() != 0){
        int count = ui->verticalLayout_22->count();
        for(int i = 0; i < count; i++){
            dynamicFrame *frame = qobject_cast<dynamicFrame*>(ui->verticalLayout_22->itemAt(ui->verticalLayout_22->count()- 1)->widget());
            delete frame;
        }
    }
    if(ui->verticalLayout_48->count() != 0){
        int count = ui->verticalLayout_48->count();
        for(int i = 0; i < count; i++){
            dynamicFrame *frame = qobject_cast<dynamicFrame*>(ui->verticalLayout_48->itemAt(ui->verticalLayout_48->count()- 1)->widget());
            delete frame;
        }
    }
}

void MainWindow::ClearRedactClientPhones()
{
    QLayoutItem* child;
    while ((child = ui->verticalLayout_78->takeAt(0)) != 0)
    {
        delete child->widget();
        delete child;
    }
    while ((child = ui->verticalLayout_80->takeAt(0)) != 0)
    {
        delete child->widget();
        delete child;
    }
}

void MainWindow::on_pushButton_AddRedactClientPhone_clicked()
{
    if(redactClientPhoneOpened == false){
        if(!providersAreOpened){
            ui->widget_redactClientPhones_Fields->show();
        } else {
            ui->widget_redactProviderPhones_Fields->show();
        }
        ui->pushButton_saveRedactClientPhone->show();
        ui->pushButton_AddRedactClientPhone->setStyleSheet("QPushButton{color:white;"
                                                           "background-color:red;"
                                                           "border-radius:3px;"
                                                           "}"
                                                           "QPushButton:hover{"
                                                           "color:white;"
                                                           "background-color:rgb(180,0,0);"
                                                           "border-radius:3px;"
                                                           "}");
        ui->pushButton_AddRedactClientPhone->setIcon(QIcon(":/images/images/minus-white-12.png"));
        CheckUpdatesForRedactClientPhones();
        redactClientPhoneOpened = true;
    } else if(redactClientPhoneOpened == true){
        if(!providersAreOpened){
            ui->widget_redactClientPhones_Fields->hide();
        } else {
            ui->widget_redactProviderPhones_Fields->hide();
        }
        ui->pushButton_saveRedactClientPhone->hide();
        ui->pushButton_AddRedactClientPhone->setStyleSheet("QPushButton{color:white;"
                                                           "background-color:rgb(91,95,199);"
                                                           "border-radius:3px;"
                                                           "}"
                                                           "QPushButton:hover{"
                                                           "color:white;"
                                                           "background-color:rgb(68,71,145);"
                                                           "border-radius:3px;"
                                                           "}");
        ui->pushButton_AddRedactClientPhone->setIcon(QIcon(":/images/images/plus-8-12.png"));
        if(!providersAreOpened){
            ui->lineEdit_redactClientPhone->clear(); ui->lineEdit_redactClientPhone_information->clear(); ui->lineEdit_redactClientPhone_FullName->clear(); ui->lineEdit_redactClientPhone_email->clear();
        } else {
            ui->lineEdit_redactProviderPhone->clear(); ui->lineEdit_redactProviderPhone_FullName->clear();
        }
        redactClientPhoneOpened = false;
    }
}

void MainWindow::on_pushButton_saveRedactClientPhone_clicked()
{
    QString a; int b; bool isOK = true;
    if(!providersAreOpened){
        a = ui->lineEdit_redactClientPhone->text();
        b = ui->lineEdit_redactClientPhone->cursorPosition();
        if(!ui->lineEdit_redactClientPhone_FullName->text().isEmpty() || !ui->lineEdit_redactClientPhone_information->text().isEmpty()
                || ui->lineEdit_redactClientPhone->text() != "+7()--" || !ui->lineEdit_redactClientPhone_email->text().isEmpty()){
            if(a != "+7()--"){
                if(numberValidator.validate(a, b) != QValidator::Acceptable){
                    QMessageBox::warning(this, "Ошибка", "<FONT COLOR = '#000000'>Проверьте правильность введенного номера телефона.</FONT> \n");
                    isOK = false;
                }
            }
            if(isOK){
                CheckUpdatesForRedactClientPhones();
                redactClientPhones.push_back("0");
                redactClientPhones.push_back(ui->lineEdit_redactClientPhone_FullName->text());
                redactClientPhones.push_back(ui->lineEdit_redactClientPhone_information->text());
                if(a == "+7()--"){
                    redactClientPhones.push_back("");
                } else {
                    redactClientPhones.push_back(ui->lineEdit_redactClientPhone->text());
                }
                redactClientPhones.push_back(ui->lineEdit_redactClientPhone_email->text());
                on_pushButton_AddRedactClientPhone_clicked();
                UpdateRedactClientPhones();
            }
        } else {
            QMessageBox::warning(this, "Ошибка", "<FONT COLOR = '#000000'>Хотя бы одно поле не должно быть пустым.</FONT> \n");
        }
    } else {
        a = ui->lineEdit_redactProviderPhone->text();
        b = ui->lineEdit_redactProviderPhone->cursorPosition();
        if(!ui->lineEdit_redactProviderPhone_FullName->text().isEmpty() || ui->lineEdit_redactProviderPhone->text() != "+7()--"){
            if(a != "+7()--"){
                if(numberValidator.validate(a, b) != QValidator::Acceptable){
                    QMessageBox::warning(this, "Ошибка", "<FONT COLOR = '#000000'>Проверьте правильность введенного номера телефона.</FONT> \n");
                    isOK = false;
                }
            }
            if(isOK){
                CheckUpdatesForRedactClientPhones();
                redactClientPhones.push_back(ui->lineEdit_redactProviderPhone_FullName->text());
                if(a == "+7()--"){
                    redactClientPhones.push_back("");
                } else {
                    redactClientPhones.push_back(ui->lineEdit_redactProviderPhone->text());
                }
                on_pushButton_AddRedactClientPhone_clicked();
                UpdateRedactClientPhones();
            }
        } else {
            QMessageBox::warning(this, "Ошибка", "<FONT COLOR = '#000000'>Хотя бы одно поле не должно быть пустым.</FONT> \n");
        }
    }
}

QList<QList<QString>> MainWindow::CheckForDuplicate(QList<QString> list, int companyId)
{
    findedPhoneDublicates.clear();
    phonesWithDublicates.clear();
    QList<QString> temp;
    bool phoneIsOk = false, emailIsOk = false, phoneIsOk_2 = false, emailIsOk_2 = false;
    dublicatesModel->setQuery("SELECT * FROM clientPhones");
    while (dublicatesModel->canFetchMore()){
         dublicatesModel->fetchMore();
    }
    dublicatesProxy->setSourceModel(dublicatesModel);
    dublicatesProxy->sort(2);
    for(int i = 0; i < list.size() / 5; i++){
        phonesWithDublicates.push_back(temp);
    }
    temp.clear();
    for(int i = 0; i < dublicatesProxy->rowCount(); i++){
        phoneIsOk = false, emailIsOk = false;
        if(companyId != dublicatesProxy->data(dublicatesProxy->index(i,2)).toInt()){
            for(int j = 0; j < list.size(); j+=5){
                phoneIsOk_2 = false, emailIsOk_2 = false;
                if(list[j+3] != "" && list[j+3] == dublicatesProxy->data(dublicatesProxy->index(i,5))){ 
                    phoneIsOk = true; phoneIsOk_2 = true;
                    if(temp.size() < 5){
                        temp.push_back(dublicatesProxy->data(dublicatesProxy->index(i,2)).toString());
                        temp.push_back(dublicatesProxy->data(dublicatesProxy->index(i,3)).toString());
                        temp.push_back(dublicatesProxy->data(dublicatesProxy->index(i,4)).toString());
                        temp.push_back(dublicatesProxy->data(dublicatesProxy->index(i,5)).toString());
                        temp.push_back(dublicatesProxy->data(dublicatesProxy->index(i,6)).toString());
                    }
                }

                if(list[j+4] != "" && list[j+4] == dublicatesProxy->data(dublicatesProxy->index(i,6))){
                    emailIsOk = true; emailIsOk_2 = true;
                    if(temp.size() < 5){
                        temp.push_back(dublicatesProxy->data(dublicatesProxy->index(i,2)).toString());
                        temp.push_back(dublicatesProxy->data(dublicatesProxy->index(i,3)).toString());
                        temp.push_back(dublicatesProxy->data(dublicatesProxy->index(i,4)).toString());
                        temp.push_back(dublicatesProxy->data(dublicatesProxy->index(i,5)).toString());
                        temp.push_back(dublicatesProxy->data(dublicatesProxy->index(i,6)).toString());
                    }
                }


                if(phoneIsOk_2 || emailIsOk_2){
                    if(phonesWithDublicates[j/5].isEmpty()){
                        phonesWithDublicates[j/5].insert(0, list[j + 1]);
                        phonesWithDublicates[j/5].insert(1, list[j + 2]);
                        phonesWithDublicates[j/5].insert(2 ,list[j + 3]);
                        phonesWithDublicates[j/5].insert(3, list[j + 4]);
                    }
                    if(phoneIsOk_2){
                        if(phonesWithDublicates[j/5].size() > 4) {
                                if(phonesWithDublicates[j/5][4] != "1"){
                                    phonesWithDublicates[j/5].insert(4, "1");
                                }
                        } else {
                            phonesWithDublicates[j/5].insert(4, "1");
                        }
                    }
                    if(emailIsOk_2){
                        if(phonesWithDublicates[j/5].size() > 4){
                            if(phonesWithDublicates[j/5][4] == "1"){
                                phonesWithDublicates[j/5].insert(5, "2");
                            }
                        } else {
                            phonesWithDublicates[j/5].insert(4, "2");
                        }
                    }
                }
            }
            if(phoneIsOk){
                temp.push_back("1");
            }
            if(emailIsOk){
                temp.push_back("2");
            }
            if(phoneIsOk || emailIsOk){
                findedPhoneDublicates.push_back(temp); temp.clear();
            }
        }
    }
    return findedPhoneDublicates;
}







//адреса компаний
void MainWindow::on_pushButton_AddNewClientAddress_clicked()
{
    QLabel icon;
    if(newClientAddressIsOpened == false){
        ui->widget_6->show();
        ui->pushButton_saveNewClientAddress->show();
        ui->pushButton_AddNewClientAddress->setStyleSheet("QPushButton{color:white;"
                                                          "background-color:red;"
                                                          "border-radius:3px;"
                                                          "}"
                                                          "QPushButton:hover{"
                                                          "color:white;"
                                                          "background-color:rgb(180,0,0);"
                                                          "border-radius:3px;"
                                                          "}");
        ui->pushButton_AddNewClientAddress->setIcon(QIcon(":/images/images/minus-white-12.png"));
        newClientAddressIsOpened = true;
    } else if(newClientAddressIsOpened == true){
        ui->widget_6->hide();
        ui->pushButton_saveNewClientAddress->hide();
        ui->pushButton_AddNewClientAddress->setStyleSheet("QPushButton{color:white;"
                                                          "background-color:rgb(91,95,199);"
                                                          "border-radius:3px;"
                                                          "}"
                                                          "QPushButton:hover{"
                                                          "color:white;"
                                                          "background-color:rgb(68,71,145);"
                                                          "border-radius:3px;"
                                                          "}");
        ui->pushButton_AddNewClientAddress->setIcon(QIcon(":/images/images/plus-8-12.png"));
        ui->lineEdit_newClientAddress->clear(); ui->lineEdit_newClientAddress_information->clear();
        newClientAddressIsOpened = false;
    }
}

void MainWindow::on_pushButton_saveNewClientAddress_clicked()
{
    if(!ui->lineEdit_newClientAddress->text().isEmpty() || !ui->lineEdit_newClientAddress_information->text().isEmpty()){
        newClientAddresses.push_back(ui->lineEdit_newClientAddress->text());
        newClientAddresses.push_back(ui->lineEdit_newClientAddress_information->text());
        on_pushButton_AddNewClientAddress_clicked();
        UpdateNewClientAddresses();
    } else {
        QMessageBox::warning(this, "Ошибка", "<FONT COLOR = '#000000'>Хотя бы одно поле не должно быть пустым.</FONT> \n");
    }
}

void MainWindow::UpdateNewClientAddresses()
{
    ClearClientAddresses(1);
    if(newClientAddresses.size() != 0){
        for(int i = 0; i < newClientAddresses.size(); i+=2){
            dynamicFrame *frame = new dynamicFrame(this);
            frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            frame->setStyleSheet("background-color:rgb(232,235,250);"
                                 "color:rgb(36,36,36);"
                                 "border-width:1px;"
                                 "border-radius:8px;"
                                 "border-color: black;");
            frame->setFixedHeight(44);
            QHBoxLayout *lay = new QHBoxLayout(frame);
            QHBoxLayout *hLay_1 = new QHBoxLayout();
            QHBoxLayout *hLay_2 = new QHBoxLayout();



            dynamicLabel *icon_address = new dynamicLabel(frame);
            icon_address->setFixedSize(24,24);
            QPixmap pix_address(":/images/images/map-marker-black-24.png");
            icon_address->setPixmap(pix_address);

            dynamicLineEdit *address = new dynamicLineEdit(frame);
            address->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
            address->setFont(defaultfont);
            if(newClientAddresses[i] != ""){
                address->setText(newClientAddresses[i]);
                address->setToolTip(newClientAddresses[i]);
            } else {
                address->setText("—");
                address->setToolTip("Не указано");
            }
            address->setReadOnly(true);
            address->setFrame(false);
            address->setCursorPosition(0);

            hLay_1->addWidget(icon_address); hLay_1->addWidget(address);

            dynamicLabel *icon_info = new dynamicLabel(frame);
            icon_info->setFixedSize(24,24);
            QPixmap pix_info(":/images/images/info-black-24.png");
            icon_info->setPixmap(pix_info);

            dynamicLineEdit *information = new dynamicLineEdit(frame);
            information->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
            information->setFont(defaultfont);
            if(newClientAddresses[i+1] != ""){
                information->setText(newClientAddresses[i+1]);
                information->setToolTip(newClientAddresses[i+1]);
            } else {
                information->setText("—");
                information->setToolTip("Не указано");
            }

            information->setReadOnly(true);
            information->setFrame(false);
            information->setCursorPosition(0);
            hLay_2->addWidget(icon_info); hLay_2->addWidget(information);

            DynamicButton_Delete_clientAddress *button_x = new DynamicButton_Delete_clientAddress(frame);
            button_x->setID(QString::number(i));
            button_x->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
            button_x->setFixedSize(24,24);
            button_x->setStyleSheet("QPushButton{color:black;"
                                    "border-style: solid;"
                                    "border-width:1px;"
                                    "border-radius:3px;"
                                    "border-color: black;"
                                    "max-width:24px;"
                                    "max-height:24px;"
                                    "min-width:24px;"
                                    "min-height:24px;"
                                    "color:black;"
                                    "background-color:rgb(255, 0 , 0);"
                                    "}"

                                    "QPushButton:hover{ color:black;"
                                    "border-style: solid;"
                                    "border-width:1px;"
                                    "border-radius:3px;"
                                    "border-color: black;"
                                    "max-width:24px;"
                                    "max-height:24px;"
                                    "min-width:24px;"
                                    "min-height:24px;"
                                    "color:black;"
                                    "background-color:rgb(145, 0, 0);"
                                    "}");
            button_x->setText("X");
            connect(button_x, SIGNAL(clicked()), this, SLOT(DeleteDybamicButton_deleteClientsAddress()));
            lay->addLayout(hLay_1); lay->addLayout(hLay_2); lay->addWidget(button_x);
            frame->setLayout(lay);
            ui->verticalLayout_84->addWidget(frame);
        }
    } else {
        dynamicFrame *frame = new dynamicFrame(this);
        frame->setStyleSheet("background-color:rgb(232,235,250);"
                             "color:rgb(36,36,36);"
                             "border-width:1px;"
                             "border-radius:8px;"
                             "border-color: black;");
        frame->setFixedHeight(44);
        QHBoxLayout *lay = new QHBoxLayout(frame);
        dynamicLabel *isClear = new dynamicLabel(frame);
        isClear->setText("Не указано");
        isClear->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        isClear->setFont(defaultfont);
        lay->addStretch();
        lay->addWidget(isClear);
        lay->addStretch();
        frame->setLayout(lay);
        ui->verticalLayout_84->addWidget(frame);
    }
}

void MainWindow::UpdateOutputClientAddresses()
{
    ClearClientAddresses(0);
    QVariant data = ui->listView->model()->data(ui->listView->model()->index(ui->listView->currentIndex().row(),0),Qt::DisplayRole).toString();
    QSqlQuery query;
    query.prepare("SELECT address, information FROM clientAddresses WHERE companyId = :search");
    query.bindValue(":search", data); //поиск по id
    if(!query.exec()){
        qDebug() << query.lastError().text();
    }
    QSqlRecord rec = query.record();
    bool isHere = false;
    while(query.next()){
        isHere = true;
        dynamicFrame *frame = new dynamicFrame(this);
        frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        frame->setStyleSheet("background-color:rgb(232,235,250);"
                             "color:rgb(36,36,36);"
                             "border-width:1px;"
                             "border-radius:8px;"
                             "border-color: black;");
        frame->setFixedHeight(44);
        QHBoxLayout *lay = new QHBoxLayout(frame);
        QHBoxLayout *hLay_1 = new QHBoxLayout();
        QHBoxLayout *hLay_2 = new QHBoxLayout();



        dynamicLabel *icon_address = new dynamicLabel(frame);
        icon_address->setFixedSize(24,24);
        QPixmap pix_address(":/images/images/map-marker-black-24.png");
        icon_address->setPixmap(pix_address);

        dynamicLineEdit *address = new dynamicLineEdit(frame);
        address->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        address->setFont(defaultfont);
        if(query.value(rec.indexOf("address")).toString() != ""){
            address->setText(query.value(rec.indexOf("address")).toString());
            address->setToolTip(query.value(rec.indexOf("address")).toString());
        } else {
            address->setText("—");
            address->setToolTip("Не указано");
        }
        address->setReadOnly(true);
        address->setFrame(false);
        address->setCursorPosition(0);

        hLay_1->addWidget(icon_address); hLay_1->addWidget(address);

        dynamicLabel *icon_info = new dynamicLabel(frame);
        icon_info->setFixedSize(24,24);
        QPixmap pix_info(":/images/images/info-black-24.png");
        icon_info->setPixmap(pix_info);

        dynamicLineEdit *information = new dynamicLineEdit(frame);
        information->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        information->setFont(defaultfont);
        if(query.value(rec.indexOf("information")).toString() != ""){
            information->setText(query.value(rec.indexOf("information")).toString());
            information->setToolTip(query.value(rec.indexOf("information")).toString());
        } else {
            information->setText("—");
            information->setToolTip("Не указано");
        }

        information->setReadOnly(true);
        information->setFrame(false);
        information->setCursorPosition(0);
        hLay_2->addWidget(icon_info); hLay_2->addWidget(information);

        lay->addLayout(hLay_1); lay->addLayout(hLay_2);
        frame->setLayout(lay);
        ui->verticalLayout_43->addWidget(frame);
    }
    if(!isHere){
        dynamicFrame *frame = new dynamicFrame(this);
        frame->setStyleSheet("background-color:rgb(232,235,250);"
                             "color:rgb(36,36,36);"
                             "border-width:1px;"
                             "border-radius:8px;"
                             "border-color: black;");
        QHBoxLayout *lay = new QHBoxLayout(frame);
        dynamicLabel *isClear = new dynamicLabel(frame);
        frame->setFixedHeight(44);
        isClear->setText("Не указано");
        isClear->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        isClear->setFont(defaultfont);
        lay->addStretch();
        lay->addWidget(isClear);
        lay->addStretch();
        frame->setLayout(lay);
        ui->verticalLayout_43->addWidget(frame);
    }
}

void MainWindow::UpdateRedactClientAddresses()
{
    ClearClientAddresses(2);
    bool isHere = false;
    for(int i = 0; i < redactClientAddresses.size(); i+=2){
        if(isHere != true){
            isHere = true;
        }
        dynamicFrame *frame = new dynamicFrame(this);
        frame->setStyleSheet("background-color:rgb(232,235,250);"
                             "color:rgb(36,36,36);"
                             "border-width:1px;"
                             "border-radius:8px;"
                             "border-color: black;");
        frame->setMaximumHeight(44); frame->setMinimumHeight(44);
        QHBoxLayout *lay = new QHBoxLayout(frame);
        lay->setContentsMargins(4,2,4,2);
        lay->setSpacing(12);
        QHBoxLayout *hLay_1 = new QHBoxLayout();
        hLay_1->setContentsMargins(0,2,4,2);
        hLay_1->setSpacing(3);
        QHBoxLayout *hLay_2 = new QHBoxLayout();
        hLay_2->setContentsMargins(0,2,4,2);
        hLay_2->setSpacing(3);

        dynamicLabel *icon_address = new dynamicLabel(frame);
        icon_address->setFixedSize(24,24);
        QPixmap pix_fullName(":/images/images/map-marker-black-24.png");
        icon_address->setPixmap(pix_fullName);

        dynamicLineEdit *address = new dynamicLineEdit(frame);
        address->setStyleSheet("color:rgb(40,40,40);"
                               "background-color:rgb(255,255,255);"
                               "border:1px solid rgb(224,224,224);"
                               "border-radius:4px;"
                               "border-bottom:2px solid rgb(91,95,199);");
        address->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        address->setFont(defaultfont);
        address->setPlaceholderText("Адрес...");
        if(redactClientAddresses[i] != ""){
            address->setText(redactClientAddresses[i]);
        }
        hLay_1->addWidget(icon_address); hLay_1->addWidget(address);

        dynamicLabel *icon_info = new dynamicLabel(frame);
        icon_info->setFixedSize(24,24);
        QPixmap pix_info(":/images/images/info-black-24.png");
        icon_info->setPixmap(pix_info);

        dynamicLineEdit *information = new dynamicLineEdit(frame);
        information->setStyleSheet("color:rgb(40,40,40);"
                                   "background-color:rgb(255,255,255);"
                                   "border:1px solid rgb(224,224,224);"
                                   "border-radius:4px;"
                                   "border-bottom:2px solid rgb(91,95,199);");
        information->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        information->setFont(defaultfont);
        information->setPlaceholderText("Описание...");
        if(redactClientAddresses[i+1] != ""){
            information->setText(redactClientAddresses[i+1]);
        }
        hLay_2->addWidget(icon_info); hLay_2->addWidget(information);

        DynamicButton_Delete_clientAddress *button_x = new DynamicButton_Delete_clientAddress(frame);
        button_x->setID(QString::number(i));
        button_x->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        button_x->setFixedSize(24,24);
        button_x->setStyleSheet("QPushButton{color:black;"
                                "border-style: solid;"
                                "border-width:1px;"
                                "border-radius:3px;"
                                "border-color: black;"
                                "max-width:24px;"
                                "max-height:24px;"
                                "min-width:24px;"
                                "min-height:24px;"
                                "color:black;"
                                "background-color:rgb(255, 0 , 0);"
                                "}"

                                "QPushButton:hover{ color:black;"
                                "border-style: solid;"
                                "border-width:1px;"
                                "border-radius:3px;"
                                "border-color: black;"
                                "max-width:24px;"
                                "max-height:24px;"
                                "min-width:24px;"
                                "min-height:24px;"
                                "color:black;"
                                "background-color:rgb(145, 0, 0);"
                                "}");
        button_x->setText("X");
        connect(button_x, SIGNAL(clicked()), this, SLOT(DeleteDybamicButton_deleteClientsAddress()));
        lay->addLayout(hLay_1); lay->addLayout(hLay_2); lay->addWidget(button_x);
        frame->setLayout(lay);
        ui->verticalLayout_85->addWidget(frame);
    }
    if(!isHere){
        dynamicFrame *frame = new dynamicFrame(this);
        frame->setStyleSheet("background-color:rgb(232,235,250);"
                             "color:rgb(36,36,36);"
                             "border-width:1px;"
                             "border-radius:8px;"
                             "border-color: black;");
        QHBoxLayout *lay = new QHBoxLayout(frame);
        dynamicLabel *isClear = new dynamicLabel(frame);
        isClear->setText("Не указано");
        isClear->setFixedHeight(26);
        isClear->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        isClear->setFont(defaultfont);
        lay->addStretch();
        lay->addWidget(isClear);
        lay->addStretch();
        frame->setLayout(lay);
        ui->verticalLayout_85->addWidget(frame);
    }
}

void MainWindow::ClearClientAddresses(int place)
{
    switch(place){
    case 1:{
        QLayoutItem* child;
        while ((child = ui->verticalLayout_84->takeAt(0)) != 0)
        {
            delete child->widget();
            delete child;
        }
        break;
        }
    case 0:{
        QLayoutItem* child;
        while ((child = ui->verticalLayout_43->takeAt(0)) != 0)
        {
            delete child->widget();
            delete child;
        }
        break;
        }
    case 2: {
        QLayoutItem* child;
        while ((child = ui->verticalLayout_85->takeAt(0)) != 0)
        {
            delete child->widget();
            delete child;
        }
        break;
        }
    }
}

void MainWindow::CheckUpdatesForRedactClientAddresses()
{
    int temp = 0;
     if(redactClientAddresses.count() != 0){
         for(int i = 0; i < ui->verticalLayout_85->count(); i++){
             dynamicFrame *frame = qobject_cast<dynamicFrame*>(ui->verticalLayout_85->itemAt(i)->widget());
             QList<dynamicLineEdit*> widgetList = frame->findChildren<dynamicLineEdit*>();
             if(redactClientAddresses[temp] != widgetList[0]->text()){
                 redactClientAddresses[temp] = widgetList[0]->text();
             }
             if(redactClientAddresses[temp+1] != widgetList[1]->text()){
                 redactClientAddresses[temp+1] = widgetList[1]->text();
             }
             temp+=2;
         }
     }
}

void MainWindow::DeleteDybamicButton_deleteClientsAddress()
{
    DynamicButton_Delete_clientAddress *button = (DynamicButton_Delete_clientAddress*) sender();
    if(ui->stackedWidget_clients->currentIndex() == 1){
        newClientAddresses.removeAt(button->getID() + 1);
        newClientAddresses.removeAt(button->getID());
        UpdateNewClientAddresses();
    } else if (ui->stackedWidget_clients->currentIndex() == 2){
        CheckUpdatesForRedactClientAddresses();
        redactClientAddresses.removeAt(button->getID() + 1);
        redactClientAddresses.removeAt(button->getID());
        UpdateRedactClientAddresses();
    }
}

void MainWindow::on_pushButton_AddRedactClientAddress_clicked()
{
    if(redactClientAddressIsOpened == false){
        ui->widget_7->show();
        ui->pushButton_saveRedactClientAddress->show();
        ui->pushButton_AddRedactClientAddress->setStyleSheet("QPushButton{color:white;"
                                                             "background-color:red;"
                                                             "border-radius:3px;"
                                                             "}"
                                                             "QPushButton:hover{"
                                                             "color:white;"
                                                             "background-color:rgb(180,0,0);"
                                                             "border-radius:3px;"
                                                             "}");
        ui->pushButton_AddRedactClientAddress->setIcon(QIcon(":/images/images/minus-white-12.png"));
        CheckUpdatesForRedactClientAddresses();
        redactClientAddressIsOpened = true;
    } else if(redactClientAddressIsOpened == true){
        ui->widget_7->hide();
        ui->pushButton_saveRedactClientAddress->hide();
        ui->pushButton_AddRedactClientAddress->setStyleSheet("QPushButton{color:white;"
                                                             "background-color:rgb(91,95,199);"
                                                             "border-radius:3px;"
                                                             "}"
                                                             "QPushButton:hover{"
                                                             "color:white;"
                                                             "background-color:rgb(68,71,145);"
                                                             "border-radius:3px;"
                                                             "}");
        ui->pushButton_AddRedactClientAddress->setIcon(QIcon(":/images/images/plus-8-12.png"));
        ui->lineEdit_redactClientAddress->clear(); ui->lineEdit_redactClientAddress_information->clear();
        redactClientAddressIsOpened = false;
    }
}

void MainWindow::on_pushButton_saveRedactClientAddress_clicked()
{
    if(!ui->lineEdit_redactClientAddress->text().isEmpty() || !ui->lineEdit_redactClientAddress_information->text().isEmpty()){
        CheckUpdatesForRedactClientAddresses();
        redactClientAddresses.push_back(ui->lineEdit_redactClientAddress->text());
        redactClientAddresses.push_back(ui->lineEdit_redactClientAddress_information->text());
        on_pushButton_AddRedactClientAddress_clicked();
        UpdateRedactClientAddresses();
    } else {
        QMessageBox::warning(this, "Ошибка", "<FONT COLOR = '#000000'>Хотя бы одно поле не должно быть пустым.</FONT> \n");
    }
}






//Заметки компании(комментарии)
void MainWindow::on_pushButton_addNote_clicked()
{
    if(newNoteOpen == false){
            ui->textEdit->show();
            ui->pushButton_addNote->setStyleSheet("QPushButton{color:white;"
                                                  "background-color:red;"
                                                  "border-radius:3px;"
                                                  "}"
                                                  "QPushButton:hover{"
                                                  "color:white;"
                                                  "background-color:rgb(180,0,0);"
                                                  "border-radius:3px;"
                                                  "}");
            ui->pushButton_addNote->setIcon(QIcon(":/images/images/minus-white-12.png"));
            ui->pushButton_saveNote->show();
            newNoteOpen = true;
    } else {
        ui->textEdit->hide();
        ui->textEdit->setText("");
        ui->pushButton_addNote->setStyleSheet("QPushButton{color:white;"
                                              "background-color:rgb(91,95,199);"
                                              "border-radius:3px;"
                                              "}"
                                              "QPushButton:hover{"
                                              "color:white;"
                                              "background-color:rgb(68,71,145);"
                                              "border-radius:3px;"
                                              "}");
        ui->pushButton_addNote->setIcon(QIcon(":/images/images/plus-8-12.png"));
        ui->pushButton_saveNote->hide();
        newNoteOpen = false;
    }
}

void MainWindow::UpdateNotes(){
    ClearNotes();
    ui->widget_2->show();
    QSqlQuery query;
    query.prepare("SELECT * FROM notes WHERE companyID = :search");
    QVariant data = ui->listView->model()->data(ui->listView->model()->index(ui->listView->currentIndex().row(),0),Qt::DisplayRole).toString();
    query.bindValue(":search", data); //поиск по id
    if(!query.exec()){
        qDebug() << query.lastError().text();
    }
    QSqlRecord rec = query.record();
    while(query.next()){
        dynamicFrame *frame = new dynamicFrame(this);
        frame->setStyleSheet("background-color:rgb(232,235,250);"
                             "color:rgb(36,36,36);"
                             "border-width:1px;"
                             "border-radius:8px;"
                             "border-color: black;");
        //frame->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
        QHBoxLayout *lay = new QHBoxLayout(frame);

        QLabel *text = new QLabel(frame);
        text->setWordWrap(true);
        text->setFrameShape(QLabel::Shape::Box);
        text->setTextInteractionFlags(Qt::NoTextInteraction);
        //text->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
        text->setFont(QFont("Segoe UI Variable Display Semib",14,2));

        dynamicLabel *date = new dynamicLabel(frame);
        date->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        date->setFont(defaultfont);

        dynamicButton *button = new dynamicButton(frame);
        button->setID(query.value(rec.indexOf("id")).toString());
        button->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        button->setFixedSize(24,24);
        button->setStyleSheet("QPushButton{color:black;"
                              "border-style: solid;"
                              "border-width:1px;"
                              "border-radius:3px;"
                              "border-color: black;"
                              "max-width:24px;"
                              "max-height:24px;"
                              "min-width:24px;"
                              "min-height:24px;"
                              "color:black;"
                              "background-color:rgb(255, 0, 0)"
                              "}"

                              "QPushButton:hover{ color:black;"
                              "border-style: solid;"
                              "border-width:1px;"
                              "border-radius:3px;"
                              "border-color: black;"
                              "max-width:24px;"
                              "max-height:24px;"
                              "min-width:24px;"
                              "min-height:24px;"
                              "color:black;"
                              "background-color:rgb(145, 0, 0);"
                              "}");
        button->setText("X");
        connect(button, SIGNAL(clicked()), this, SLOT(deleteDynamicButton_deleteNote()));

        text->setText(query.value(rec.indexOf("text")).toString());
        date->setText(query.value(rec.indexOf("date")).toString());
        //text->setStyleSheet("background-color: transparent");
        text->setStyleSheet("border:0px solid black");
        lay->addWidget(text);
        lay->addWidget(date);
        lay->addWidget(button);
        ui->verticalLayout_11->addWidget(frame);
    }
}

void MainWindow::ClearNotes()
{
    if(ui->verticalLayout_11->count() != 0){
        int count = ui->verticalLayout_11->count();
        for(int i = 0; i < count; i++){
            dynamicFrame *frame = qobject_cast<dynamicFrame*>(ui->verticalLayout_11->itemAt(ui->verticalLayout_11->count()- 1)->widget());
            delete frame;
        }
    }
}

void MainWindow::on_pushButton_saveNote_clicked()
{
    if(ui->textEdit->toPlainText().count(" ") == ui->textEdit->toPlainText().size()){
        on_pushButton_addNote_clicked();
    } else {
        QVariant data = ui->listView->model()->data(ui->listView->model()->index(ui->listView->currentIndex().row(), 0)).toString();
        database::InsertToNotes(data.toString(),
                                ui->textEdit->toPlainText(),
                                QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm"));
        UpdateNotes();
        ui->textEdit->hide();
        ui->textEdit->setText("");
        ui->pushButton_saveNote->hide();
        ui->pushButton_addNote->setStyleSheet("QPushButton{color:white;"
                                              "background-color:rgb(91,95,199);"
                                              "border-radius:3px;"
                                              "}"
                                              "QPushButton:hover{"
                                              "color:white;"
                                              "background-color:rgb(68,71,145);"
                                              "border-radius:3px;"
                                              "}");
        ui->pushButton_addNote->setIcon(QIcon(":/images/images/plus-8-12.png"));
        newNoteOpen = false;
    }
}

void MainWindow::deleteDynamicButton_deleteNote()
{
     dynamicButton *button = (dynamicButton*) sender();
     QSqlQuery query;
     query.prepare("DELETE FROM notes WHERE id = :id");
     query.bindValue(":id", button->getID());
     if (!query.exec()){
         qDebug() << "Ошибка в удалении заметки";
     }
     UpdateNotes();
}






//документы компаний
void MainWindow::UpdateDocuments()
{
    ClearDocuments();
    QVariant companyId = ui->listView->model()->data(ui->listView->model()->index(ui->listView->currentIndex().row(),0),Qt::DisplayRole).toString();

    QSqlQuery query;
    query.prepare("SELECT id, fileName, file, extention FROM documents WHERE companyId = :companyId;");
    query.bindValue(":companyId", companyId);
    if(!query.exec()){
        qDebug() << query.lastError().text();
    }
    QSqlRecord rec = query.record();
    bool isHere = false;
    while(query.next()){
        if(!isHere) isHere = true;
        dynamicFrame *frame = new dynamicFrame(this);
        frame->setStyleSheet("background-color:rgb(232,235,250);"
                             "color:rgb(36,36,36);"
                             "border-width:1px;"
                             "border-radius:8px;"
                             "border-color: black;");

        frame->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
        QHBoxLayout *lay = new QHBoxLayout(frame);

        dynamicLabel *fileName = new dynamicLabel(this);
        fileName->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        fileName->setFont(defaultfont);
        fileName->setText(query.value(rec.indexOf("fileName")).toString());
        fileName->setToolTip(query.value(rec.indexOf("fileName")).toString());
        lay->addWidget(fileName);


        dynamicButton_openDocument *button_open = new dynamicButton_openDocument(frame);
        button_open->setID(query.value(rec.indexOf("id")).toString());
        button_open->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        button_open->setFixedSize(24,24);
        button_open->setStyleSheet("QPushButton{color:black;"
                                   "border-style: solid;"
                                   "border-width:1px;"
                                   "border-radius:3px;"
                                   "border-color: black;"
                                   "max-width:24px;"
                                   "max-height:24px;"
                                   "min-width:24px;"
                                   "min-height:24px;"
                                   "color:black;"
                                   "background-color:white"
                                   "}"

                                   "QPushButton:hover{ color:black;"
                                   "border-style: solid;"
                                   "border-width:1px;"
                                   "border-radius:3px;"
                                   "border-color: black;"
                                   "max-width:24px;"
                                   "max-height:24px;"
                                   "min-width:24px;"
                                   "min-height:24px;"
                                   "color:black;"
                                   "background-color:rgb(122, 122, 122);"
                                   "}");
        QPixmap pix(":/images/images/doc-icon.png");
        QIcon icon(pix);
        button_open->setIcon(icon);
        connect(button_open, SIGNAL(clicked()), this, SLOT(OpenDocumentById()));
        lay->addWidget(button_open);


        dynamicButton_Delete_Document *button_x = new dynamicButton_Delete_Document(frame);
        button_x->setID(query.value(rec.indexOf("id")).toString());
        button_x->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        button_x->setFixedSize(24,24);
        button_x->setStyleSheet("QPushButton{color:black;"
                                "border-style: solid;"
                                "border-width:1px;"
                                "border-radius:3px;"
                                "border-color: black;"
                                "max-width:24px;"
                                "max-height:24px;"
                                "min-width:24px;"
                                "min-height:24px;"
                                "color:black;"
                                "background-color:rgb(255, 0, 0)"
                                "}"

                                "QPushButton:hover{ color:black;"
                                "border-style: solid;"
                                "border-width:1px;"
                                "border-radius:3px;"
                                "border-color: black;"
                                "max-width:24px;"
                                "max-height:24px;"
                                "min-width:24px;"
                                "min-height:24px;"
                                "color:black;"
                                "background-color:rgb(145, 0, 0);"
                                "}");
        button_x->setText("X");
        connect(button_x, SIGNAL(clicked()), this, SLOT(DeleteDynamicButton_deleteDocument()));
        lay->addWidget(button_x);

        ui->verticalLayout_doc->addWidget(frame);
    }

    if(!isHere){
        dynamicFrame *frame = new dynamicFrame(this);
        frame->setStyleSheet("background-color:rgb(232,235,250);"
                             "color:rgb(36,36,36);"
                             "border-width:1px;"
                             "border-radius:8px;"
                             "border-color: black;");
        frame->setFixedHeight(44);
        QHBoxLayout *lay = new QHBoxLayout(frame);
        dynamicLabel *isClear = new dynamicLabel(frame);
        isClear->setText("Пусто");
        isClear->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        isClear->setFont(defaultfont);
        lay->addStretch();
        lay->addWidget(isClear);
        lay->addStretch();
        frame->setLayout(lay);
        ui->verticalLayout_doc->addWidget(frame);
    }
};

void MainWindow::on_pushButton_AddDocument_clicked()
{
    QVariant companyId = ui->listView->model()->data(ui->listView->model()->index(ui->listView->currentIndex().row(),0),Qt::DisplayRole);

    QString file = QFileDialog::getOpenFileName(this, "Выберите документ", "C:\\",
                                          "All Files (*.docx *.pdf *.xlsx *.txt)");
    QFile temp = QFile(file);
    QString fileName = QFileInfo(file).fileName();
    QSqlQuery query;
    query.prepare("SELECT * FROM documents WHERE fileName = :fileName");
    query.bindValue(":fileName", fileName);
    query.exec();
    temp.open(QIODevice::ReadOnly);
    QString ext = QFileInfo(temp).suffix();
    QByteArray arr = temp.readAll();
    database::AddDocument(companyId.toString(), fileName, arr, ext);

    temp.close();

    UpdateDocuments();
}

void MainWindow::DeleteDynamicButton_deleteDocument()
{
     dynamicButton_Delete_Document *button = (dynamicButton_Delete_Document*) sender();
     QSqlQuery query;
     query.prepare("DELETE FROM documents WHERE id = :id");
     query.bindValue(":id", button->getID());
     if (!query.exec()){
         qDebug() << "Ошибка в удалении документа ";
     }
     UpdateDocuments();
}

void MainWindow::ClearDocuments()
{
    if(ui->verticalLayout_doc->count() != 0){
        int count = ui->verticalLayout_doc->count();
        for(int i = 0; i < count; i++){
            dynamicFrame *frame = qobject_cast<dynamicFrame*>(ui->verticalLayout_doc->itemAt(ui->verticalLayout_doc->count()- 1)->widget());
            delete frame;
        }
    }
}

void MainWindow::OpenDocumentById()
{
    dynamicButton_openDocument *button_open = (dynamicButton_openDocument*) sender();
    QSqlQuery query;
    query.prepare("SELECT file, extention FROM documents WHERE id = :id");
    query.bindValue(":id", button_open->getID());
    query.exec();
    query.next();

    QFile file(qApp->applicationDirPath().append("/file_from_DB." + query.value(1).toString()));

    if(file.open(QIODevice::WriteOnly))
    {
        {
            QByteArray ba = query.value(0).toByteArray();
            file.write(ba);
            file.close();
        }
    }
    else
    {
        qDebug() << "Невозможно записать в файл";
    }
    file.setPermissions(QFileDevice::ReadUser);
    QDesktopServices::openUrl(qApp->applicationDirPath().append("/file_from_DB." + query.value(1).toString()));
}





//пользователи
void MainWindow::on_action_4_triggered()
{
    bool isOK = true;
    if(clientsAreOpened){
        ui->pushButton_clients->setStyleSheet("QPushButton{color:black;"
                                              "background-color:white;"
                                              "border: 2px solid rgb(91,95,199);"
                                              "border-radius:3px;"
                                              "}"
                                              "QPushButton:hover{"
                                              "Color:black;"
                                              "background-color:rgb(217,217,217);"
                                              "border: 2px solid rgb(91,95,199);"
                                              "border-radius:3px;"
                                              "}");
        ui->verticalLayout_45->setContentsMargins(0,0,0,0);
        ui->pushButton_activeClients->hide();
        ui->pushButton_inactiveClients->hide();
        ui->frame_2->setStyleSheet("border:0px;");
        if(activeClientsAreOpened){
            tempListView_activeClients_id = ui->listView->model()->data(ui->listView->model()->index(ui->listView->currentIndex().row(), 0)).toInt();
        } else if (inactiveCLientsAreOpened){
            tempListView_inactiveClients_id = ui->listView->model()->data(ui->listView->model()->index(ui->listView->currentIndex().row(), 0)).toInt();
        } 
        clientsAreOpened = false;
    } else if(providersAreOpened){
        tempListView_providers_id = ui->listView->model()->data(ui->listView->model()->index(ui->listView->currentIndex().row(), 0)).toInt();
        ui->pushButton_providers->setStyleSheet("QPushButton{color:black;"
                                                "background-color:white;"
                                                "border: 2px solid rgb(91,95,199);"
                                                "border-radius:3px;"
                                                "}"
                                                "QPushButton:hover{"
                                                "Color:black;"
                                                "background-color:rgb(217,217,217);"
                                                "border: 2px solid rgb(91,95,199);"
                                                "border-radius:3px;"
                                                "}");
        providersAreOpened = false;
    } else if (potencialClientsAreOpened){
        tempListView_potencialClients_id = ui->listView->model()->data(ui->listView->model()->index(ui->listView->currentIndex().row(), 0)).toInt();
        ui->pushButton_potencial_clients->setStyleSheet("QPushButton{color:black;"
                                              "background-color:white;"
                                              "border: 2px solid rgb(91,95,199);"
                                              "border-radius:3px;"
                                              "}"
                                              "QPushButton:hover{"
                                              "Color:black;"
                                              "background-color:rgb(217,217,217);"
                                              "border: 2px solid rgb(91,95,199);"
                                              "border-radius:3px;"
                                              "}");
        potencialClientsAreOpened = false;
    }

    if(ui->stackedWidget_clients->currentIndex() == 1){
        QMessageBox::StandardButton reply =
        QMessageBox::question(this, "Подтвердите действие", "<FONT COLOR = '#000000'>Отменить создание новой компании? Внесённые данные не сохранятся! </FONT>");
        if (reply == QMessageBox::Yes){
            on_pushButton_CreateNewClient_clicked();
        } else {
            isOK = false;
        }
    } else if(ui->stackedWidget_clients->currentIndex() == 2){
        QMessageBox::StandardButton reply =
        QMessageBox::question(this, "Подтвердите действие", "<FONT COLOR = '#000000'>Отменить редактирование компании? Внесённые изменения не сохранятся! </FONT>");
        if (reply == QMessageBox::Yes){
            on_pushButton_CancelRedactOfClient_clicked();
        } else {
            isOK = false;
        }
    }
    if(isOK){
        ui->label_UserName_User->setText(username);
        ui->stackedWidget_username_3->setCurrentIndex(1);
        ui->widget_OldNewPass_3->hide();
        ui->stackedWidget_redactpass_3->setCurrentIndex(0);
        ui->label_61->hide(); ui->label_62->hide();

        QSqlQuery query;
        query.prepare("SELECT showAllClientsInDefault FROM users WHERE name = :name");
        query.bindValue(":name", username);
        query.exec();
        query.next();
        if(query.value(0).toInt() == 0){
            ui->checkBox_showAllClients->setCheckState(Qt::Unchecked);
        } else {
            ui->checkBox_showAllClients->setCheckState(Qt::Checked);
        }
        ui->stackedWidget_main->setCurrentIndex(1);
    }
}

void MainWindow::on_pushButton_redactUserName_clicked()
{
    ui->lineEdit_userName->setText(username);
    ui->stackedWidget_username_3->setCurrentIndex(0);
    ui->stackedWidget_saveRedact_3->setCurrentIndex(1);
}

void MainWindow::on_pushButton_saveUserName_clicked()
{
    if(ui->lineEdit_userName->text().count(' ') == ui->lineEdit_userName->text().size()){
        QMessageBox::warning(this, "Ошибка!", "<FONT COLOR = '#000000'>Введите новое имя пользователя.</FONT>");
        ui->lineEdit_userName->clear();
    } else {
        database::RedactUserName(ui->lineEdit_userName->text(), username);
        username = ui->lineEdit_userName->text();
        ui->label_UserName_User->setText(username);
        ui->stackedWidget_username_3->setCurrentIndex(1);
        ui->stackedWidget_saveRedact_3->setCurrentIndex(0);
    }
}

void MainWindow::on_pushButton_redactPassword_clicked()
{
    ui->stackedWidget_redactpass_3->setCurrentIndex(1);
    ui->widget_OldNewPass_3->show();
    ui->label_61->show();
    ui->label_62->show();
}

void MainWindow::on_pushButton_saveNewPassword_clicked()
{
    if(ui->lineEdit_newpass->text().count() < 3){
            QMessageBox::warning(this, "Ошибка", "<FONT COLOR = '#000000'>Новый пароль должен содержать 3 и более символов.</FONT> \n");
    } else {
        if(database::RedactPass(username, ui->lineEdit_oldpass->text(), ui->lineEdit_newpass->text())){
            QMessageBox::information(this, "Успех", "Пароль успешно изменен.");
            ui->stackedWidget_redactpass_3->setCurrentIndex(0);
            ui->widget_OldNewPass_3->hide();
            ui->lineEdit_newpass->clear(); ui->label_61->hide();
            ui->lineEdit_oldpass->clear(); ui->label_62->hide();

        } else {
            QMessageBox::warning(this, "Ошибка!", "<FONT COLOR = '#000000'>Старый пароль не верный.</FONT> \n");
        }
    }
}

void MainWindow::on_checkBox_showAllClients_stateChanged(int arg1)
{
    qDebug() << arg1;
    QSqlQuery query;
    query.prepare("UPDATE users SET showAllClientsInDefault = :value WHERE name = :username");
    query.bindValue(":username", username);
    query.bindValue(":value", arg1);
    if (!query.exec()){
        qDebug() << "Ошибка редактирования имени пользователя!";
    }
}
