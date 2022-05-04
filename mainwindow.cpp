#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSql>
#include "database.h"
#include <QMessageBox>
#include "dynamicFields.h"
#include <QFileDialog>
#include <QDesktopServices>
#include <QCompleter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->stackedWidget_main->setCurrentIndex(0);
    MainThemeOfClients();
    MainThemeOfContacts();

    ui->lineEdit_newClientPhone->setInputMask("+7\\(999\\)999\\-99\\-99;_");
    ui->lineEdit_contact_phone->setInputMask("+7\\(999\\)999\\-99\\-99;_");
    ui->lineEdit_newClientPhone->installEventFilter(this);
    ui->lineEdit_redactClientPhone->installEventFilter(this);
    ui->lineEdit_redactedContactPhone->installEventFilter(this);
    ui->lineEdit_contact_phone->installEventFilter(this);


    clientsModel = new QSqlQueryModel(this);
    SortFilterProxyClientsModel = new QSortFilterProxyModel(this);
    SortFilterProxyClientsModel->setSourceModel(clientsModel);
    SortFilterProxyClientsModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    SortFilterProxyClientsModel->sort(1);
    ui->listView->setModel(SortFilterProxyClientsModel);
    ui->listView->setModelColumn(1);

    UpdateActiveClients();

    contactsModel = new QSqlQueryModel(this);
    SortFilterProxyContactsModel = new QSortFilterProxyModel(this);
    SortFilterProxyContactsModel->setSourceModel(contactsModel);
    SortFilterProxyContactsModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    SortFilterProxyContactsModel->sort(1);
    ui->listView_contacts->setModel(SortFilterProxyContactsModel);
    ui->listView_contacts->setModelColumn(1);

    UpdateContacts(); //+



    assignment_contacts = new QSqlRelationalTableModel(this);
    proxyAssignment = new QSortFilterProxyModel(this);
    proxyAssignment->setSourceModel(assignment_contacts);

    newClientsCompleter = new QCompleter(this);
    newClientsCompleter->setModel(proxyAssignment);
    newClientsCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    newClientsCompleter->setFilterMode(Qt::MatchContains);
    newClientsCompleter->setCompletionMode(QCompleter::CompletionMode::PopupCompletion);
    newClientsCompleter->popup()->setStyleSheet("background-color:rgb(54, 57, 63);"
                                      "color:white;");


    editClientsCompleter = new QCompleter(this);
    editClientsCompleter->setModel(proxyAssignment);
    editClientsCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    editClientsCompleter->setFilterMode(Qt::MatchContains);
    editClientsCompleter->setCompletionMode(QCompleter::CompletionMode::PopupCompletion);
    editClientsCompleter->popup()->setStyleSheet("background-color:rgb(54, 57, 63);"
                                      "color:white;");

    QFont popupFont = QFont("Segoe UI",12,2);
    newClientsCompleter->popup()->setFont(popupFont);
    editClientsCompleter->popup()->setFont(popupFont);


    ui->comboBox_NewClientContacts->setEditable(true);
    ui->comboBox_NewClientContacts->setInsertPolicy(QComboBox::NoInsert);
    ui->comboBox_NewClientContacts->setModel(assignment_contacts);
    ui->comboBox_NewClientContacts->setModelColumn(1);
    ui->comboBox_NewClientContacts->completer()->setCompletionColumn(1);
    ui->comboBox_NewClientContacts->setCompleter(newClientsCompleter);
    ui->comboBox_NewClientContacts->installEventFilter(this);
    ui->comboBox_NewClientContacts->lineEdit()->installEventFilter(this);


    ui->comboBox_EditClientContacts->setEditable(true);
    ui->comboBox_EditClientContacts->setInsertPolicy(QComboBox::NoInsert);
    ui->comboBox_EditClientContacts->setModel(assignment_contacts);
    ui->comboBox_EditClientContacts->setModelColumn(1);
    ui->comboBox_EditClientContacts->completer()->setCompletionColumn(1);
    ui->comboBox_EditClientContacts->setCompleter(editClientsCompleter);
    ui->comboBox_EditClientContacts->installEventFilter(this);
    ui->comboBox_EditClientContacts->lineEdit()->installEventFilter(this);

    ui->comboBox_NewClientContacts->lineEdit()->setPlaceholderText("Выбрать...");
    ui->comboBox_EditClientContacts->lineEdit()->setPlaceholderText("Выбрать...");
}

QRegularExpression numberRegex ("^\\+\\d{1,1}\\(\\d{3,3}\\)\\d{3,3}\\-\\d{2,2}\\-\\d{2,2}$");
QRegularExpressionValidator *numberValidator = new QRegularExpressionValidator (numberRegex);
QFont defaultfont = QFont("Segoe UI",12,2);
QString username;
QString companyName, phone, address, addField, ID;

bool ClientIsOpened = false; //переменная открытой инфы о компании
bool newNoteOpen = false; //переменная открытия новой заметки
bool ContactIsOpened = false;
bool checkLeftMenuOpened = true;

bool StackedWithActiveClientsIsOpen = true;
bool StackedWithContactsIsOpen = false;
bool StackedWithInactiveClientsIsOpen = false;

int tempListView_contacts_id = 0, tempListView_activeClients_id = 0, tempListView_inactiveClients_id = 0;

QModelIndex tempIndex;
int nextId;

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::closeEvent(QCloseEvent *event) {
    delete numberValidator;
    ClearNotes();
    ClearClientOutputContacts();
    ClearClientRedactContacts();
    ClearContactOutputCompanies();
    ClearDocuments();

    QSqlQuery query;
    query.prepare("DELETE FROM assignment WHERE companyId = :nextId");
    query.bindValue(":nextId", nextId);
    query.exec();
    ClearNewClientContacts();
}


bool MainWindow::eventFilter(QObject *object, QEvent *event)
{
    if (object == ui->comboBox_NewClientContacts->lineEdit()){
        if(event->type() == QEvent::MouseButtonPress){
            ui->comboBox_NewClientContacts->completer()->setCompletionPrefix(ui->comboBox_NewClientContacts->lineEdit()->text());
            ui->comboBox_NewClientContacts->completer()->complete();
        }
    } else if (object == ui->comboBox_NewClientContacts){
        if(event->type() == QEvent::KeyRelease && ui->comboBox_NewClientContacts->currentText().isEmpty())
        {
            ui->comboBox_NewClientContacts->completer()->setCompletionPrefix("");
            ui->comboBox_NewClientContacts->completer()->complete();
        }
    }
    if(object == ui->comboBox_EditClientContacts->lineEdit()){
        if(event->type() == QEvent::MouseButtonPress){
            ui->comboBox_EditClientContacts->completer()->setCompletionPrefix(ui->comboBox_EditClientContacts->lineEdit()->text());
            ui->comboBox_EditClientContacts->completer()->complete();
        }
    } else if (object == ui->comboBox_EditClientContacts){
        if(event->type() == QEvent::KeyRelease && ui->comboBox_EditClientContacts->currentText().isEmpty())
        {
            ui->comboBox_EditClientContacts->completer()->setCompletionPrefix("");
            ui->comboBox_EditClientContacts->completer()->complete();
        }
    }
    if(object == ui->lineEdit_newClientPhone){
        if(event->type() != QEvent::Paint && event->type() != QEvent::MouseMove && event->type() != QEvent::HoverMove){
        }
     }
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

    if(object == ui->lineEdit_contact_phone && event->type() == QEvent::MouseButtonPress){
        QTimer::singleShot(0,ui->lineEdit_contact_phone,[this]
        {
            QString line = ui->lineEdit_contact_phone->displayText();
            ui->lineEdit_contact_phone->setFocus();
            bool isValid = true;
            for(int i = 0; i<=15; i++){
                if(line[i] == '_'){
                    ui->lineEdit_contact_phone->setCursorPosition(i);
                    isValid = false;
                    break;
                }
            }
            if(isValid){
                ui->lineEdit_contact_phone->setCursorPosition(16);
            }
        });
    }
    if(object == ui->lineEdit_redactedContactPhone && event->type() == QEvent::MouseButtonPress){
        QTimer::singleShot(0,ui->lineEdit_redactedContactPhone,[this]
        {
            QString line = ui->lineEdit_redactedContactPhone->displayText();
            ui->lineEdit_redactedContactPhone->setFocus();
            bool isValid = true;
            for(int i = 0; i<=15; i++){
                if(line[i] == '_'){
                    ui->lineEdit_redactedContactPhone->setCursorPosition(i);
                    isValid = false;
                    break;
                }
            }
            if(isValid){
                ui->lineEdit_redactedContactPhone->setCursorPosition(16);
            }
        });
    }
    return QMainWindow::eventFilter(object, event);
}

void MainWindow::AuthResults(QString name)
{
    username = name;
    QMessageBox::information(this, "Авторизация успешна!", "<FONT COLOR = '#ffffff'> Добро пожаловать, " + name  + "</FONT>");
}




void MainWindow::MainThemeOfClients(){

    ui->stackedWidget_clients->setCurrentIndex(0);
    ui->label_CMPname->setText("Активные компании");

    ui->frame->hide();ui->frame_2->hide(); //номер, адрес
    ui->pushButton_addNote->hide(); ui->label_2->hide(); ui->textEdit->hide(); ui->pushButton_saveNote->hide();//заметки
    ui->widget_2->hide();
    ui->pushButton_Update->hide(); ui->pushButton_DeleteClient->hide(); //редакт-делет
    ui->label_username->hide(); ui->label_9->hide(); //создатель
    ui->frame_6->hide(); ui->frame_7->hide();

    ui->stackedWidget_2->hide();

    ui->pushButton_AddDocument->hide(); ui->frame_13->hide();

    ClearDocuments();
    ClearClientRedactContacts();
    ClearNewClientContacts();
    ClearClientOutputContacts();
    newNoteOpen = false;
    ClearNotes();
    ClientIsOpened = false;
}

void MainWindow::MainThemeOfInactiveClients(){

    ui->stackedWidget_clients->setCurrentIndex(0);
    ui->label_CMPname->setText("Неактивные компании");

    ui->stackedWidget_2->hide();

    ui->frame->hide();ui->frame_2->hide(); //номер, адрес
    ui->pushButton_addNote->hide(); ui->label_2->hide(); ui->textEdit->hide(); ui->pushButton_saveNote->hide();//заметки
    ui->widget_2->hide();
    ui->pushButton_Update->hide(); ui->pushButton_DeleteClient->hide(); //редакт-делет
    ui->label_username->hide(); ui->label_9->hide(); //создатель
    ui->frame_6->hide(); ui->frame_7->hide();

    ui->pushButton_AddDocument->hide(); ui->frame_13->hide();

    ClearDocuments();
    ClearClientRedactContacts();
    ClearNewClientContacts();
    ClearClientOutputContacts();
    newNoteOpen = false;
    ClearNotes();
    ClientIsOpened = false;
}

void MainWindow::MainThemeOfContacts()
{
    ui->stackedWidget_contacts->setCurrentIndex(0);
    ui->label_CONTname->setText("Контакты");

    ui->frame_8->hide(); ui->frame_9->hide(); ui->frame_11->hide(); ui->frame_12->hide();
    ui->pushButton_updateContact->hide(); ui->pushButton_deleteContact->hide();
    ui->label_30->hide(); ui->label_username_2->hide();

    ClearContactOutputCompanies();
    ContactIsOpened = false;
}

void MainWindow::on_pushButton_MainMenu_clicked()
{
    if(checkLeftMenuOpened){
        ui->pushButton_MainMenu->setGeometry(5,10,20,24);
        ui->label_12->hide(); ui->frame_15->hide(); ui->pushButton_activeClients->hide(); ui->pushButton_activeClients->hide();
        ui->label_13->hide(); ui->frame_14->hide(); ui->pushButton_contacts->hide();
        ui->widget->setGeometry(1, 1, 30, ui->widget->y());
        ui->widget->setMinimumHeight(600);
        ui->widget->setMinimumWidth(30);
        checkLeftMenuOpened = false;
    } else {
        ui->label_12->show(); ui->frame_15->show(); ui->pushButton_activeClients->show(); ui->pushButton_activeClients->show();
        ui->label_13->show(); ui->frame_14->show(); ui->pushButton_contacts->show();
        ui->pushButton_MainMenu->setGeometry(100,10,20,24);
        ui->widget->setMinimumHeight(600);
        ui->widget->setMinimumWidth(130);
        checkLeftMenuOpened = true;
    }
}


//выбор стака с активными клиентами
void MainWindow::on_pushButton_activeClients_clicked()
{
    if(!StackedWithActiveClientsIsOpen){
        QSqlQuery query;
        if(StackedWithInactiveClientsIsOpen == true){
             tempListView_inactiveClients_id = ui->listView->model()->data(ui->listView->model()->index(ui->listView->currentIndex().row(),0),Qt::DisplayRole).toInt();
        } else if(StackedWithContactsIsOpen == true){
            tempListView_contacts_id = ui->listView_contacts->model()->data(ui->listView_contacts->model()->index(ui->listView_contacts->currentIndex().row(),0),Qt::DisplayRole).toInt();
        }
        ui->pushButton_activeClients->setStyleSheet("color:black;"
                                               "background-color:green;"
                                               "border-width:1px;"
                                               "border-radius:8px;"
                                               "border-color: black;");

        ui->pushButton_contacts->setStyleSheet("QPushButton{color:black;"
                                               "background-color:white;"
                                               "border-width:1px;"
                                               "border-radius:8px;"
                                               "border-color: black;"
                                               "}"
                                               "QPushButton:hover{ "
                                               "background-color:rgb(122, 122, 122);"
                                               "border-width:1px;"
                                               "border-radius:8px;"
                                               "border-color: black;"
                                               "}");

        ui->pushButton_inactiveClients->setStyleSheet("QPushButton{color:black;"
                                                      "background-color:white;"
                                                      "border-width:1px;"
                                                      "border-radius:8px;"
                                                      "border-color: black;"
                                                      "}"
                                                      "QPushButton:hover{ "
                                                      "background-color:rgb(122, 122, 122);"
                                                      "border-width:1px;"
                                                      "border-radius:8px;"
                                                      "border-color: black;"
                                                      "}");
        ui->stackedWidget_main->setCurrentIndex(0);
        UpdateActiveClients();
        ui->lineEdit_SearchOfClients->setText("");
        on_lineEdit_SearchOfClients_textEdited("");
        switch(ui->stackedWidget_clients->currentIndex()){
        case 1:
            if(tempListView_activeClients_id!= 0){
                for(int row = 0; row <= ui->listView->model()->rowCount(); row++){
                    if(ui->listView->model()->data(ui->listView->model()->index(row, 0)) == tempListView_activeClients_id){
                        QModelIndex index = ui->listView->model()->index(row, 1);
                        ui->listView->setCurrentIndex(index);
                        ui->listView->scrollTo(index);
                        break;
                    }
                }
            }
            query.prepare("SELECT id, fullName FROM contacts WHERE contacts.rowid NOT IN (SELECT contactId FROM assignment WHERE companyId = :nextId);");
            query.bindValue(":nextId", nextId);
            query.exec();
            assignment_contacts->setQuery(std::move(query));
            ui->comboBox_NewClientContacts->setCurrentIndex(-1);

            UpdateClientContacts(nextId, 1);
            break;
        case 0:
            if(tempListView_activeClients_id == 0){
                MainThemeOfClients();
                break;
            } else {
                for(int row = 0; row <= ui->listView->model()->rowCount(); row++){
                    if(ui->listView->model()->data(ui->listView->model()->index(row, 0)) == tempListView_activeClients_id){
                        QModelIndex index = ui->listView->model()->index(row, 1);
                        ui->listView->setCurrentIndex(index);
                        on_listView_clicked(index);
                        ui->listView->scrollTo(index);
                        break;
                    }
                }
            }
            break;
        case 2:
            for(int row = 0; row <= ui->listView->model()->rowCount(); row++){
                if(ui->listView->model()->data(ui->listView->model()->index(row, 0)) == tempListView_activeClients_id){
                    QModelIndex index = ui->listView->model()->index(row, 1);
                    ui->listView->setCurrentIndex(index);
                    ui->listView->scrollTo(index);
                    break;
                }
            }
            query.prepare("SELECT id, fullName FROM contacts WHERE contacts.rowid NOT IN (SELECT contactId FROM assignment WHERE companyId = :nextId);");
            query.bindValue(":nextId", tempListView_activeClients_id);
            query.exec();
            assignment_contacts->setQuery(std::move(query));

            ui->comboBox_EditClientContacts->setCurrentIndex(-1);
            UpdateClientContacts(tempListView_activeClients_id, 2);
            break;
        }
        StackedWithActiveClientsIsOpen = true;
        StackedWithContactsIsOpen = false;
        StackedWithInactiveClientsIsOpen = false;
    }
}

//выбор стака с контактами
void MainWindow::on_pushButton_contacts_clicked()
{
    if (!StackedWithContactsIsOpen){
        if(StackedWithInactiveClientsIsOpen == true){
             tempListView_inactiveClients_id = ui->listView->model()->data(ui->listView->model()->index(ui->listView->currentIndex().row(),0),Qt::DisplayRole).toInt();
        } else if(StackedWithActiveClientsIsOpen == true){
            tempListView_activeClients_id = ui->listView->model()->data(ui->listView->model()->index(ui->listView->currentIndex().row(),0),Qt::DisplayRole).toInt();
        }
        ui->pushButton_activeClients->setStyleSheet("QPushButton{color:black;"
                                                    "background-color:white;"
                                                    "border-width:1px;"
                                                    "border-radius:8px;"
                                                    "border-color: black;"
                                                    "}"
                                                    "QPushButton:hover{ "
                                                    "background-color:rgb(122, 122, 122);"
                                                    "border-width:1px;"
                                                    "border-radius:8px;"
                                                    "border-color: black;"
                                                    "}");

        ui->pushButton_contacts->setStyleSheet("color:black;"
                                               "background-color:green;"
                                               "border-width:1px;"
                                               "border-radius:8px;"
                                               "border-color: black;");

        ui->pushButton_inactiveClients->setStyleSheet("QPushButton{color:black;"
                                                      "background-color:white;"
                                                      "border-width:1px;"
                                                      "border-radius:8px;"
                                                      "border-color: black;"
                                                      "}"
                                                      "QPushButton:hover{ "
                                                      "background-color:rgb(122, 122, 122);"
                                                      "border-width:1px;"
                                                      "border-radius:8px;"
                                                      "border-color: black;"
                                                      "}");
        ui->stackedWidget_main->setCurrentIndex(1);

        UpdateContacts();
        ui->lineEdit_search_contacts->setText("");
        on_lineEdit_search_contacts_textEdited("");

        switch(ui->stackedWidget_contacts->currentIndex()){
        case 1:
            if(tempListView_contacts_id!= 0){
                for(int row = 0; row <= ui->listView_contacts->model()->rowCount(); row++){
                    if(ui->listView_contacts->model()->data(ui->listView_contacts->model()->index(row, 0)) == tempListView_contacts_id){
                        QModelIndex index = ui->listView_contacts->model()->index(row, 1);
                        ui->listView_contacts->setCurrentIndex(index);
                        ui->listView_contacts->scrollTo(index);
                        break;
                    }
                }
            }
            break;
        case 0:
            if(tempListView_contacts_id == 0){
                MainThemeOfContacts();
            } else {
                for(int row = 0; row <= ui->listView_contacts->model()->rowCount(); row++){
                    if(ui->listView_contacts->model()->data(ui->listView_contacts->model()->index(row, 0)) == tempListView_contacts_id){
                        QModelIndex index = ui->listView_contacts->model()->index(row, 1);
                        ui->listView_contacts->setCurrentIndex(index);
                        on_listView_contacts_clicked(index);
                        ui->listView_contacts->scrollTo(index);
                        break;
                    }
                }
            }break;
        case 2:
            for(int row = 0; row <= ui->listView_contacts->model()->rowCount(); row++){
                if(ui->listView_contacts->model()->data(ui->listView_contacts->model()->index(row, 0)) == tempListView_contacts_id){
                    QModelIndex index = ui->listView_contacts->model()->index(row, 1);
                    ui->listView_contacts->setCurrentIndex(index);
                    ui->listView_contacts->scrollTo(index);
                    break;
                }
            }
            break;
        }
        StackedWithContactsIsOpen = true;
        StackedWithActiveClientsIsOpen = false;
        StackedWithInactiveClientsIsOpen = false;
    }
}

//выбор стака с неактивными клиентами
void MainWindow::on_pushButton_inactiveClients_clicked()
{
    if (!StackedWithInactiveClientsIsOpen){
        QSqlQuery query;
        if(StackedWithActiveClientsIsOpen == true){
             tempListView_activeClients_id = ui->listView->model()->data(ui->listView->model()->index(ui->listView->currentIndex().row(),0),Qt::DisplayRole).toInt();
        } else if(StackedWithContactsIsOpen == true){
            tempListView_contacts_id = ui->listView_contacts->model()->data(ui->listView_contacts->model()->index(ui->listView_contacts->currentIndex().row(),0),Qt::DisplayRole).toInt();
        }
        ui->pushButton_activeClients->setStyleSheet("QPushButton{color:black;"
                                                    "background-color:white;"
                                                    "border-width:1px;"
                                                    "border-radius:8px;"
                                                    "border-color: black;"
                                                    "}"
                                                    "QPushButton:hover{ "
                                                    "background-color:rgb(122, 122, 122);"
                                                    "border-width:1px;"
                                                    "border-radius:8px;"
                                                    "border-color: black;"
                                                    "}");

        ui->pushButton_contacts->setStyleSheet("QPushButton{color:black;"
                                               "background-color:white;"
                                               "border-width:1px;"
                                               "border-radius:8px;"
                                               "border-color: black;"
                                               "}"
                                               "QPushButton:hover{ "
                                               "background-color:rgb(122, 122, 122);"
                                               "border-width:1px;"
                                               "border-radius:8px;"
                                               "border-color: black;"
                                               "}");

        ui->pushButton_inactiveClients->setStyleSheet("color:black;"
                                               "background-color:green;"
                                               "border-width:1px;"
                                               "border-radius:8px;"
                                               "border-color: black;");
        ui->stackedWidget_main->setCurrentIndex(0);

        UpdateInactiveClients();
        ui->lineEdit_SearchOfClients->setText("");
        on_lineEdit_SearchOfClients_textEdited("");

        switch(ui->stackedWidget_clients->currentIndex()){
        case 1:
            if(tempListView_inactiveClients_id!= 0){
                for(int row = 0; row <= ui->listView->model()->rowCount(); row++){
                    if(ui->listView->model()->data(ui->listView->model()->index(row, 0)) == tempListView_inactiveClients_id){
                        QModelIndex index = ui->listView->model()->index(row, 1);
                        ui->listView->setCurrentIndex(index);
                        ui->listView->scrollTo(index);
                        break;
                    }
                }
            }
            query.prepare("SELECT id, fullName FROM contacts WHERE contacts.rowid NOT IN (SELECT contactId FROM assignment WHERE companyId = :nextId);");
            query.bindValue(":nextId", nextId);
            query.exec();
            assignment_contacts->setQuery(std::move(query));

            ui->comboBox_NewClientContacts->setCurrentIndex(-1);

            UpdateClientContacts(nextId, 1);
            break;
        case 0:
            if(tempListView_inactiveClients_id == 0){
                MainThemeOfInactiveClients();
                break;
            } else {
                for(int row = 0; row <= ui->listView->model()->rowCount(); row++){
                    if(ui->listView->model()->data(ui->listView->model()->index(row, 0)) == tempListView_inactiveClients_id){
                        QModelIndex index = ui->listView->model()->index(row, 1);
                        ui->listView->setCurrentIndex(index);
                        on_listView_clicked(index);
                        ui->listView->scrollTo(index);
                        break;
                    }
                }
            }
            break;
        case 2:
            for(int row = 0; row <= ui->listView->model()->rowCount(); row++){
                if(ui->listView->model()->data(ui->listView->model()->index(row, 0)) == tempListView_inactiveClients_id){
                    QModelIndex index = ui->listView->model()->index(row, 1);
                    ui->listView->setCurrentIndex(index);
                    ui->listView->scrollTo(index);
                    break;
                }
            }
            query.prepare("SELECT id, fullName FROM contacts WHERE contacts.rowid NOT IN (SELECT contactId FROM assignment WHERE companyId = :nextId);");
            query.bindValue(":nextId", tempListView_inactiveClients_id);
            query.exec();
            assignment_contacts->setQuery(std::move(query));

            ui->comboBox_EditClientContacts->setCurrentIndex(-1);
            UpdateClientContacts(tempListView_inactiveClients_id, 2);
            break;
        }
        StackedWithInactiveClientsIsOpen = true;
        StackedWithContactsIsOpen = false;
        StackedWithActiveClientsIsOpen = false;
    }
}




//Работа с клиентами(компаниями)
void MainWindow::UpdateActiveClients()
{
    clientsModel->setQuery("SELECT id, companyName FROM clients WHERE isActive = 1");
    while (clientsModel->canFetchMore()){
         clientsModel->fetchMore();
    }
    ui->listView->setModelColumn(1);
    SortFilterProxyClientsModel->sort(1);
}

void MainWindow::UpdateInactiveClients()
{
    clientsModel->setQuery("SELECT id, companyName FROM clients WHERE isActive = 0");
    while (clientsModel->canFetchMore()){
         clientsModel->fetchMore();
    }
    ui->listView->setModelColumn(1);
    SortFilterProxyClientsModel->sort(1);
}

void MainWindow::on_pushButton_CreateNewClient_clicked()
{
    QSqlQuery query;

    if (ui->stackedWidget_clients->currentIndex() == 1){
        ui->pushButton_CreateNewClient->setStyleSheet("border-style: solid;"
                                        "border-width:1px;"
                                        "border-radius:3px;"
                                        "border-color: black;"
                                        "max-width:24px;"
                                        "max-height:24px;"
                                        "min-width:24px;"
                                        "min-height:24px;"
                                        "color:black;"
                                        "background-color:rgb(5, 255, 26)");
        ui->pushButton_CreateNewClient->setText("+");
        ui->lineEdit_2->clear(); ui->lineEdit_5->clear(); ui->lineEdit_newClientPhone->clear();
        database::DeleteClientsAssignments(QString::number(nextId));
        ClearNewClientContacts();
        ui->stackedWidget_clients->setCurrentIndex(0);
    } else {
        switch(ui->stackedWidget_clients->currentIndex()){
        case 0:
            ClearClientOutputContacts();
            break;
        case 2:
            ClearClientRedactContacts();
            break;
        }
        ui->pushButton_CreateNewClient->setStyleSheet("border-style: solid;"
                                        "border-width:1px;"
                                        "border-radius:3px;"
                                        "border-color: black;"
                                        "max-width:24px;"
                                        "max-height:24px;"
                                        "min-width:24px;"
                                        "min-height:24px;"
                                        "color:black;"
                                        "background-color:rgb(255,0,0)");
        ui->pushButton_CreateNewClient->setText("-");

        query.prepare("SELECT seq FROM sqlite_sequence where name = :name");
        query.bindValue(":name", "clients");
        query.exec();
        while(query.next()){
            nextId = query.value(0).toInt();
        }
        nextId++;

        query.prepare("SELECT id, fullName FROM contacts WHERE contacts.rowid NOT IN (SELECT contactId FROM assignment WHERE companyId = :nextId);");
        query.bindValue(":nextId", nextId);
        query.exec();
        assignment_contacts->setQuery(std::move(query));
        ui->comboBox_NewClientContacts->setModel(assignment_contacts);
        ui->comboBox_NewClientContacts->setModelColumn(1);
        ui->stackedWidget_clients->setCurrentIndex(1);
        ui->comboBox_NewClientContacts->setCurrentIndex(-1);
    }
}

void MainWindow::on_pushButton_SaveNewClient_clicked()
{
    QString a = ui->lineEdit_newClientPhone->text();
    int b = ui->lineEdit_newClientPhone->cursorPosition();
    if(numberValidator->validate(a, b) == QValidator::Acceptable){
        int isActive = 1;
        if(StackedWithInactiveClientsIsOpen == true){
            isActive = 0;
        } else if(StackedWithActiveClientsIsOpen == true){
            isActive = 1;
        }
        database::InsertToActiveClients(ui->lineEdit_2->text(),ui->lineEdit_newClientPhone->text(), ui->lineEdit_5->text(), username, QString::number(isActive));
        nextId++;
        on_pushButton_CreateNewClient_clicked();
        if(StackedWithInactiveClientsIsOpen == true){
            UpdateInactiveClients();
        } else if(StackedWithActiveClientsIsOpen == true){
            UpdateActiveClients();
        }

        ui->lineEdit_SearchOfClients->setText("");
        on_lineEdit_SearchOfClients_textEdited("");

        ui->lineEdit_2->clear(); ui->lineEdit_newClientPhone->clear(); ui->lineEdit_5->clear(); //очищаем все поля
        for(int row = 0; row <= clientsModel->rowCount(); row++){
            if(ui->listView->model()->data(ui->listView->model()->index(row, 0)) == nextId - 1){
                QModelIndex index= ui->listView->model()->index(row, 1);
                on_listView_clicked(index);
                ui->listView->setCurrentIndex(index);
                ui->listView->scrollTo(index);
                break;
            }
        }
    } else {
        QString warning = "Проверьте правильность введенного номера телефона. \n";

        QMessageBox::warning(this, "Ошибка", "<FONT COLOR='#ffffff'>Проверьте правильность введенного номера телефона.</FONT> \n");
    }
}

void MainWindow::on_lineEdit_SearchOfClients_textEdited(const QString &arg1)
{
    if(ui->stackedWidget_clients->currentIndex()==1){
        on_pushButton_CreateNewClient_clicked();
    }

    if(StackedWithActiveClientsIsOpen){
        MainThemeOfClients();
    } else if(StackedWithInactiveClientsIsOpen){
        MainThemeOfInactiveClients();
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
    QMessageBox::StandardButton reply =
    QMessageBox::question(this, "Подтвердите действие", "<FONT COLOR='#ffffff'>Вы уверены, что хотите удалить объект из базы данных? Это действие необратимо! </FONT>");
    if (reply == QMessageBox::Yes){
        QVariant data = ui->listView->model()->data(ui->listView->model()->index(ui->listView->currentIndex().row(),0),Qt::DisplayRole).toString();
        database::DeleteFromActiveClients(data.toString());
        database::DeleteFromNotes(data.toString());
        database::DeleteClientsAssignments(data.toString());
        database::DeleteCompanyDocuments(data.toString());
        ClearDocuments();
        if(ui->verticalLayout_11->count() != 0){
            int count = ui->verticalLayout_11->count();
            for(int i = 0; i < count; i++){
                dynamicFrame *frame = qobject_cast<dynamicFrame*>(ui->verticalLayout_11->itemAt(ui->verticalLayout_11->count()- 1)->widget());
                delete frame;//очищаем из памяти фрейм
            }
        }
        on_lineEdit_SearchOfClients_textEdited(ui->lineEdit_SearchOfClients->text());
        ClientIsOpened = false;
        if(StackedWithInactiveClientsIsOpen == true){
            UpdateInactiveClients();
        } else if(StackedWithActiveClientsIsOpen == true){
            UpdateActiveClients();
        }
    }
}

void MainWindow::on_listView_clicked(const QModelIndex &index)
{
    if(ui->stackedWidget_clients->currentIndex() == 1){
        on_pushButton_CreateNewClient_clicked();
    } else {
        ui->stackedWidget_clients->setCurrentIndex(0);
    }
    if(ClientIsOpened == false){
        ui->stackedWidget_2->show();
        ui->label_2->show();  ui->pushButton_addNote->show();
        ui->frame->show();
        ui->frame_2->show();
        ui->pushButton_DeleteClient->show();
        ui->pushButton_Update->show();
        ui->frame_13->show(); ui->pushButton_AddDocument->show();
        ui->label_username->show(); ui->label_9->show();
        ui->frame_6->show(); ui->frame_7->show();
        ClientIsOpened = true;
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
    ui->label_phone->setText(query.value(rec.indexOf("phone")).toString());
    ui->label_phone->setTextInteractionFlags(Qt::TextSelectableByMouse);
    ui->label_address->setText(query.value(rec.indexOf("address")).toString());
    ui->label_address->setTextInteractionFlags(Qt::TextSelectableByMouse);
    ui->label_username->setText(query.value(rec.indexOf("username")).toString());
    ui->label_username->setTextInteractionFlags(Qt::TextSelectableByMouse);

    if(query.value(rec.indexOf("isActive")).toInt() == 0){
        ui->stackedWidget_2->setCurrentIndex(1);
    } else {
        ui->stackedWidget_2->setCurrentIndex(0);
    }

    UpdateNotes(); //обновление заметок
    UpdateDocuments();
    UpdateClientContacts(data.toInt(), 0);

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
    while(query.next()){
        dynamicFrame *frame = new dynamicFrame(this);
        frame->setStyleSheet("background-color:rgb(47,49,54);"
                             "color:white;"
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
        button_open->setStyleSheet("background-color:white; color:black");
        button_open->setText("->");
        connect(button_open, SIGNAL(clicked()), this, SLOT(OpenDocumentById()));
        lay->addWidget(button_open);


        dynamicButton_Delete_Document *button_x = new dynamicButton_Delete_Document(frame);
        button_x->setID(query.value(rec.indexOf("id")).toString());
        button_x->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        button_x->setFixedSize(24,24);
        button_x->setStyleSheet("background-color:red; color:black");
        button_x->setText("X");
        connect(button_x, SIGNAL(clicked()), this, SLOT(DeleteDynamicButton_deleteDocument()));
        lay->addWidget(button_x);

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




//Контакты компаний
void MainWindow::deleteDynamicButton_deleteNewClientsContact()
{
    dynamicButton_Delete_contacts *button = (dynamicButton_Delete_contacts*) sender();
    QSqlQuery query;
    database::DeleteAssignmentFromClient(QString::number(nextId), QString::number(button->getID()));
    query.prepare("SELECT id, fullName FROM contacts WHERE contacts.rowid NOT IN (SELECT contactId FROM assignment WHERE companyId = :nextId);");
    query.bindValue(":nextId", nextId);
    query.exec();
    assignment_contacts->setQuery(std::move(query));
    UpdateClientContacts(nextId, 1);
    ui->comboBox_NewClientContacts->setCurrentIndex(-1);
}

void MainWindow::deleteDynamicButton_deleteRedactedClientsContact()
{
    dynamicButton_Delete_contacts *button = (dynamicButton_Delete_contacts*) sender();
    QSqlQuery query;
    database::DeleteAssignmentFromClient(ui->listView->model()->data(ui->listView->model()->index(ui->listView->currentIndex().row(),0),Qt::DisplayRole).toString(), QString::number(button->getID()));
    query.prepare("SELECT id, fullName FROM contacts WHERE contacts.rowid NOT IN (SELECT contactId FROM assignment WHERE companyId = :nextId);");
    query.bindValue(":nextId", ui->listView->model()->data(ui->listView->model()->index(ui->listView->currentIndex().row(),0),Qt::DisplayRole).toString());
    query.exec();
    assignment_contacts->setQuery(std::move(query));
    UpdateClientContacts(ui->listView->model()->data(ui->listView->model()->index(ui->listView->currentIndex().row(),0),Qt::DisplayRole).toInt(), 2);

    ui->comboBox_EditClientContacts->setCurrentIndex(-1);
}

void MainWindow::LinkToContactByButton()
{
    dynamicButton_Link_to_contacts* button = (dynamicButton_Link_to_contacts*) sender();
    SearchForContactIndexById(button->getID());
}
void MainWindow::SearchForContactIndexById(int id)
{
    if(ui->lineEdit_search_contacts->text() != ""){
        ui->lineEdit_search_contacts->setText("");
        on_lineEdit_search_contacts_textEdited("");
    }
    ui->lineEdit_search_contacts->clearFocus();
    for(int row = 0; row <= contactsModel->rowCount(); row++){
        if(ui->listView_contacts->model()->data(ui->listView_contacts->model()->index(row, 0)) == id){
            QModelIndex index = ui->listView_contacts->model()->index(row, 1);
            tempListView_contacts_id = ui->listView_contacts->model()->data(ui->listView_contacts->model()->index(ui->listView_contacts->currentIndex().row(),0),Qt::DisplayRole).toInt();
            if(StackedWithActiveClientsIsOpen){
                ui->pushButton_activeClients->setStyleSheet("color:black;"
                                                       "background-color:white;"
                                                       "border-width:1px;"
                                                       "border-radius:8px;"
                                                       "border-color: black;");
                 StackedWithActiveClientsIsOpen = false;
            } else if(StackedWithInactiveClientsIsOpen){
                ui->pushButton_inactiveClients->setStyleSheet("color:black;"
                                                       "background-color:white;"
                                                       "border-width:1px;"
                                                       "border-radius:8px;"
                                                       "border-color: black;");
                StackedWithInactiveClientsIsOpen = false;
            }
            ui->pushButton_contacts->setStyleSheet("color:black;"
                                                   "background-color:green;"
                                                   "border-width:1px;"
                                                   "border-radius:8px;"
                                                   "border-color: black;");
            ui->stackedWidget_main->setCurrentIndex(1);
            ui->listView_contacts->setCurrentIndex(index);
            on_listView_contacts_clicked(index);
            ui->listView_contacts->scrollTo(index);
            StackedWithContactsIsOpen = true;
            break;
        }
    }
}

void MainWindow::ClearNewClientContacts()
{
    if(ui->verticalLayout_12->count() != 0){
        int count = ui->verticalLayout_12->count();
        for(int i = 0; i < count; i++){
            dynamicFrame *frame = qobject_cast<dynamicFrame*>(ui->verticalLayout_12->itemAt(ui->verticalLayout_12->count()- 1)->widget());
            delete frame;
        }
    }
}

void MainWindow::ClearClientOutputContacts()
{
    if(ui->verticalLayout_50->count() != 0){
        int count = ui->verticalLayout_50->count();
        for(int i = 0; i < count; i++){
            dynamicFrame *frame = qobject_cast<dynamicFrame*>(ui->verticalLayout_50->itemAt(ui->verticalLayout_50->count()- 1)->widget());
            delete frame;
        }
    }
}

void MainWindow::ClearClientRedactContacts()
{
    if(ui->verticalLayout_52->count() != 0){
        int count = ui->verticalLayout_52->count();
        for(int i = 0; i < count; i++){
            dynamicFrame *frame = qobject_cast<dynamicFrame*>(ui->verticalLayout_52->itemAt(ui->verticalLayout_52->count()- 1)->widget());
            delete frame;
        }
    }
}

void MainWindow::UpdateClientContacts(int id, int where)
{
    switch(where){
    case 0:
        ClearClientOutputContacts();
        break;
    case 1:
        ClearNewClientContacts();
        break;
    case 2:
        ClearClientRedactContacts();
        break;
    }
    QSqlQuery query;
    query.prepare("SELECT id, fullName FROM contacts WHERE contacts.rowid IN (SELECT contactId FROM assignment WHERE companyId = :nextId);");
    query.bindValue(":nextId", id);
    if(!query.exec()){
        qDebug() << query.lastError().text();
    }
    QSqlRecord rec = query.record();
    while(query.next()){
        dynamicFrame *frame = new dynamicFrame(this);
        frame->setStyleSheet("background-color:rgb(47,49,54);"
                             "color:white;"
                             "border-width:1px;"
                             "border-radius:8px;"
                             "border-color: black;");
        frame->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
        QHBoxLayout *lay = new QHBoxLayout(frame);

        dynamicLabel *fullName = new dynamicLabel(frame);
        fullName->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        fullName->setFont(defaultfont);
        fullName->setText(query.value(rec.indexOf("fullName")).toString());
        fullName->setToolTip(query.value(rec.indexOf("fullName")).toString());
        lay->addWidget(fullName);

        dynamicButton_Link_to_contacts *button_link = new dynamicButton_Link_to_contacts(frame);
        button_link->setID(query.value(rec.indexOf("id")).toString());
        button_link->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        button_link->setFixedSize(24,24);
        button_link->setStyleSheet("background-color:white; color:black");
        button_link->setText("->");
        connect(button_link, SIGNAL(clicked()), this, SLOT(LinkToContactByButton()));
        lay->addWidget(button_link);

        if(where == 1 || where == 2){
            dynamicButton_Delete_contacts *button_x = new dynamicButton_Delete_contacts(frame);
            button_x->setID(query.value(rec.indexOf("id")).toString());
            button_x->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
            button_x->setFixedSize(24,24);
            button_x->setStyleSheet("background-color:red; color:black");
            button_x->setText("X");
            switch(where){
            case 1:
                connect(button_x, SIGNAL(clicked()), this, SLOT(deleteDynamicButton_deleteNewClientsContact()));
                break;
            case 2:
                connect(button_x, SIGNAL(clicked()), this, SLOT(deleteDynamicButton_deleteRedactedClientsContact()));
                break;
            }
            lay->addWidget(button_x);
        }

        switch(where){
        case 0:
            ui->verticalLayout_50->addWidget(frame);
            break;
        case 1:
            ui->verticalLayout_12->addWidget(frame);
           break;
        case 2:
            ui->verticalLayout_52->addWidget(frame);
            break;
        }
    }
}

void MainWindow::on_comboBox_NewClientContacts_activated(int index)
{
    database::AddAssignment(QString::number(nextId), assignment_contacts->data(assignment_contacts->index(index,0),Qt::DisplayRole).toString());
    QSqlQuery query;
    query.prepare("SELECT id, fullName FROM contacts WHERE contacts.rowid NOT IN (SELECT contactId FROM assignment WHERE companyId = :nextId);");
    query.bindValue(":nextId", nextId);
    query.exec();
    assignment_contacts->setQuery(std::move(query));
    UpdateClientContacts(nextId, 1);
    ui->comboBox_NewClientContacts->setCurrentIndex(-1);


    QTimer::singleShot(0,ui->comboBox_NewClientContacts,[this]
    {
        ui->comboBox_NewClientContacts->lineEdit()->clear();
    });

    ui->comboBox_NewClientContacts->lineEdit()->clearFocus();
}

void MainWindow::on_comboBox_EditClientContacts_activated(int index)
{
    database::AddAssignment(QString::number(ui->listView->model()->data(ui->listView->model()->index(ui->listView->currentIndex().row(),0),Qt::DisplayRole).toInt()), assignment_contacts->data(assignment_contacts->index(index,0),Qt::DisplayRole).toString());
    QSqlQuery query;
    query.prepare("SELECT id, fullName FROM contacts WHERE contacts.rowid NOT IN (SELECT contactId FROM assignment WHERE companyId = :nextId);");
    query.bindValue(":nextId", ui->listView->model()->data(ui->listView->model()->index(ui->listView->currentIndex().row(),0),Qt::DisplayRole).toString());
    query.exec();
    assignment_contacts->setQuery(std::move(query));
    UpdateClientContacts(ui->listView->model()->data(ui->listView->model()->index(ui->listView->currentIndex().row(),0),Qt::DisplayRole).toInt(), 2);
    ui->comboBox_EditClientContacts->setCurrentIndex(-1);


    QTimer::singleShot(0,ui->comboBox_EditClientContacts,[this]
    {
        ui->comboBox_EditClientContacts->lineEdit()->clear();
    });

    ui->comboBox_EditClientContacts->lineEdit()->clearFocus();
}





//Редактирование компании
void MainWindow::on_pushButton_Update_clicked()
{
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
    ui->lineEdit_redactClientPhone->setText(query.value(rec.indexOf("phone")).toString());
    ui->lineEdit_8->setText(query.value(rec.indexOf("address")).toString());

    query.prepare("SELECT id, fullName FROM contacts WHERE contacts.rowid NOT IN (SELECT contactId FROM assignment WHERE companyId = :nextId);");
    query.bindValue(":nextId", data);
    query.exec();
    assignment_contacts->setQuery(std::move(query));
    ui->comboBox_EditClientContacts->setModel(assignment_contacts);
    ui->comboBox_EditClientContacts->setModelColumn(1);
    UpdateClientContacts(data.toInt(), 2);
    ui->stackedWidget_clients->setCurrentIndex(2);
    ui->comboBox_EditClientContacts->setCurrentIndex(-1);
}

void MainWindow::on_pushButton_SaveRedactedClient_clicked()
{
    QString phone = ui->lineEdit_redactClientPhone->text();
    int b = ui->lineEdit_redactClientPhone->cursorPosition();
    if(numberValidator->validate(phone, b) == QValidator::Acceptable){
        QString name = ui->lineEdit_6->text();
        QString phone = ui->lineEdit_redactClientPhone->text();
        QString address = ui->lineEdit_8->text();
        QVariant data = ui->listView->model()->data(ui->listView->model()->index(ui->listView->currentIndex().row(),0),Qt::DisplayRole).toString();
        database::RedactActiveClients(name, phone, address, data.toString());
        if(StackedWithActiveClientsIsOpen){
            UpdateActiveClients();
        } else if (StackedWithInactiveClientsIsOpen){
            UpdateInactiveClients();
        }
        ui->lineEdit_SearchOfClients->setText("");
        on_lineEdit_SearchOfClients_textEdited("");
        for(int row = 0; row <= ui->listView->model()->rowCount(); row++){
                if(ui->listView->model()->data(ui->listView->model()->index(row, 0)) == data){
                    QModelIndex index = ui->listView->model()->index(row, 1);
                    on_listView_clicked(index);
                    ui->listView->setCurrentIndex(index);
                    ui->listView->scrollTo(index);
                    break;
                }
        }
    } else {
        QMessageBox::warning(this, "Ошибка", "<FONT COLOR='#ffffff'>Проверьте правильность введенного номера телефона.</FONT> \n");
    }
}

void MainWindow::on_pushButton_CancelRedactOfClient_clicked()
{
    ui->lineEdit_6->setText("");
    ui->lineEdit_redactClientPhone->setText("");
    ui->lineEdit_8->setText("");
    UpdateClientContacts(ui->listView->model()->data(ui->listView->model()->index(ui->listView->currentIndex().row(),0),Qt::DisplayRole).toInt(), 0);
    ui->stackedWidget_clients->setCurrentIndex(0);
}




//Заметки компании
void MainWindow::on_pushButton_addNote_clicked()
{
    if(newNoteOpen == false){
            ui->textEdit->show();
            ui->pushButton_addNote->setStyleSheet("border-style: solid;"
                                                   "border-width:1px;"
                                                   "border-radius:3px;"
                                                   "border-color: black;"
                                                   "max-width:20px;"
                                                   "max-height:20px;"
                                                   "min-width:20px;"
                                                   "min-height:20px;"
                                                   "color:black;"
                                                   "background-color:rgb(255,0,0)");
            ui->pushButton_addNote->setText("-");
            ui->pushButton_saveNote->show();
            newNoteOpen = true;
    } else {
        ui->textEdit->hide();
        ui->textEdit->setText("");
        ui->pushButton_addNote->setStyleSheet("border-style: solid;"
                                              "border-width:1px;"
                                              "border-radius:3px;"
                                              "border-color: black;"
                                              "max-width:20px;"
                                              "max-height:20px;"
                                              "min-width:20px;"
                                              "min-height:20px;"
                                              "color:black;"
                                              "background-color:rgb(5, 255, 26)");
        ui->pushButton_addNote->setText("+");
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
        frame->setStyleSheet("background-color:rgb(47,49,54);"
                             "color:white;"
                             "border-width:1px;"
                             "border-radius:8px;"
                             "border-color: black;");
        QHBoxLayout *lay = new QHBoxLayout(frame);

        dynamicText *text = new dynamicText(frame);
        text->setReadOnly(true);
        text->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
        text->setFont(defaultfont);

        dynamicLabel *date = new dynamicLabel(frame);
        date->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        date->setFont(defaultfont);

        dynamicButton *button = new dynamicButton(frame);
        button->setID(query.value(rec.indexOf("id")).toString());
        button->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        button->setFixedSize(24,24);
        button->setStyleSheet("background-color:red; color:black");
        button->setText("X");
        connect(button, SIGNAL(clicked()), this, SLOT(deleteDynamicButton_deleteNote()));

        text->setText(query.value(rec.indexOf("text")).toString());
        date->setText(query.value(rec.indexOf("date")).toString());
        QString s = text->toPlainText();
        QFontMetrics fm(text->font());
        int pixelsWide = fm.horizontalAdvance(s); //ширина текста
        int rowCount = pixelsWide/200; //кол-во строк
        if (rowCount == 0){ //если 0 - фиксированные 30
            text->setFixedHeight(30);
        } else{ //если больше 0 = формула
            int height = rowCount * 22 + 8;
            text->setFixedHeight(height);
        }
        text->setStyleSheet("background-color: transparent");
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
        ui->textEdit->hide();
        ui->textEdit->setText("");
        ui->pushButton_saveNote->hide();
        ui->pushButton_addNote->setStyleSheet("border-style: solid;"
                                              "border-width:1px;"
                                              "border-radius:3px;"
                                              "border-color: black;"
                                              "max-width:20px;"
                                              "max-height:20px;"
                                              "min-width:20px;"
                                              "min-height:20px;"
                                              "color:black;"
                                              "background-color:rgb(5, 255, 26)");
        newNoteOpen = false;
    } else {
        QVariant data = ui->listView->model()->data(ui->listView->model()->index(ui->listView->currentIndex().row(), 0)).toString();
        database::InsertToNotes(data.toString(),
                                ui->textEdit->toPlainText(),
                                QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm"));
        UpdateNotes();
        ui->textEdit->hide();
        ui->textEdit->setText("");
        ui->pushButton_saveNote->hide();
        ui->pushButton_addNote->setStyleSheet("border-style: solid;"
                                              "border-width:1px;"
                                              "border-radius:3px;"
                                              "border-color: black;"
                                              "max-width:20px;"
                                              "max-height:20px;"
                                              "min-width:20px;"
                                              "min-height:20px;"
                                              "color:black;"
                                              "background-color:rgb(5, 255, 26)");
        ui->pushButton_addNote->setText("+");
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





//Работа с контактами
void MainWindow::on_listView_contacts_clicked(const QModelIndex &index)
{
    if(ui->stackedWidget_contacts->currentIndex() == 1){
        on_pushButton_CreateNewContact_clicked();
    } else {
        ui->stackedWidget_contacts->setCurrentIndex(0);
    }
    if(ContactIsOpened == false){
        ui->frame_8->show(); ui->frame_9->show();
        ui->frame_11->show(); ui->frame_12->show();
        ui->pushButton_deleteContact->show(); ui->pushButton_updateContact->show();;
        ui->label_username_2->show(); ui->label_30->show();
        ContactIsOpened = true;
    }
    QSqlQuery query;
    query.prepare("SELECT * FROM contacts WHERE id = :search");
    QVariant data = ui->listView_contacts->model()->data(ui->listView_contacts->model()->index(index.row(),0),Qt::DisplayRole).toString();
    query.bindValue(":search", data.toString()); //поиск по id
    if(!query.exec()){
        qDebug() << query.lastError().text();
    }
    QSqlRecord rec = query.record();
    query.first();
    ui->label_CONTname->setText(query.value(rec.indexOf("fullName")).toString());
    ui->label_CONTname->setTextInteractionFlags(Qt::TextSelectableByMouse);
    ui->label_phone_2->setText(query.value(rec.indexOf("phone")).toString());
    ui->label_phone_2->setTextInteractionFlags(Qt::TextSelectableByMouse);
    ui->label_email->setText(query.value(rec.indexOf("email")).toString());
    ui->label_email->setTextInteractionFlags(Qt::TextSelectableByMouse);
    ui->label_username_2->setText(query.value(rec.indexOf("username")).toString());
    ui->label_username_2->setTextInteractionFlags(Qt::TextSelectableByMouse);

    UpdateContactsCompanies(data.toInt());
}

void MainWindow::UpdateContacts()
{
    contactsModel->setQuery("SELECT id, fullName FROM contacts");
    while (contactsModel->canFetchMore()){
         contactsModel->fetchMore();
    }
    ui->listView_contacts->setModelColumn(1);
    SortFilterProxyContactsModel->sort(1);
}

void MainWindow::on_lineEdit_search_contacts_textEdited(const QString &arg1)
{
    if(ui->stackedWidget_contacts->currentIndex()==1){
        on_pushButton_CreateNewContact_clicked();
    }
    MainThemeOfContacts();

    if(arg1 == " "){
        ui->lineEdit_search_contacts->setText("");
    } else {
        QRegularExpression filter ("^.{0,}(?i)"+ arg1 + ".{0,}$");
        SortFilterProxyContactsModel->setFilterKeyColumn(1);
        SortFilterProxyContactsModel->setFilterRegularExpression(filter);
        SortFilterProxyContactsModel->sort(1);
    }
}

void MainWindow::on_pushButton_CreateNewContact_clicked()
{
    if (ui->stackedWidget_contacts->currentIndex() == 1){
        ui->pushButton_CreateNewContact->setStyleSheet("border-style: solid;"
                                        "border-width:1px;"
                                        "border-radius:3px;"
                                        "border-color: black;"
                                        "max-width:24px;"
                                        "max-height:24px;"
                                        "min-width:24px;"
                                        "min-height:24px;"
                                        "color:black;"
                                        "background-color:rgb(5, 255, 26)");
        ui->pushButton_CreateNewContact->setText("+");
        ui->stackedWidget_contacts->setCurrentIndex(0);
    } else {
        ui->pushButton_CreateNewContact->setStyleSheet("border-style: solid;"
                                        "border-width:1px;"
                                        "border-radius:3px;"
                                        "border-color: black;"
                                        "max-width:24px;"
                                        "max-height:24px;"
                                        "min-width:24px;"
                                        "min-height:24px;"
                                        "color:black;"
                                        "background-color:rgb(255,0,0)");
        ui->pushButton_CreateNewContact->setText("-");
        ui->stackedWidget_contacts->setCurrentIndex(1);
    }

}

void MainWindow::on_pushButton_updateContact_clicked()
{
    QSqlQuery query;
    query.prepare("SELECT * FROM contacts WHERE id = :search");
    QVariant data =  ui->listView_contacts->model()->data(ui->listView_contacts->model()->index(ui->listView_contacts->currentIndex().row(),0),Qt::DisplayRole).toString();
    query.bindValue(":search", data.toString()); //поиск по id
    if(!query.exec()){
        qDebug() << query.lastError().text();
    }
    QSqlRecord rec = query.record();
    query.first();
    ui->lineEdit_contact_surname_redact->setText(query.value(rec.indexOf("surname")).toString());
    ui->lineEdit_contact_name_redact->setText(query.value(rec.indexOf("name")).toString());
    ui->lineEdit_contact_middleName_redact->setText(query.value(rec.indexOf("middleName")).toString());
    ui->lineEdit_redactedContactPhone->setText(query.value(rec.indexOf("phone")).toString());
    ui->lineEdit_redactedContactPhone->setInputMask("+7\\(999\\)999\\-99\\-99;_");
    ui->lineEdit_redactedContactEmail->setText(query.value(rec.indexOf("email")).toString());
    ui->stackedWidget_contacts->setCurrentIndex(2);
}

void MainWindow::on_pushButton_cancelContactUpdate_clicked()
{
    ui->lineEdit_contact_surname_redact->setText("");
    ui->lineEdit_contact_name_redact->setText("");
    ui->lineEdit_contact_middleName_redact->setText("");
    ui->lineEdit_redactedContactPhone->setText("");
    ui->lineEdit_redactedContactEmail->setText("");
    ui->stackedWidget_contacts->setCurrentIndex(0);
}

void MainWindow::on_pushButton_saveContactUpdate_clicked()
{
    QString phone = ui->lineEdit_redactedContactPhone->text();
    int b = ui->lineEdit_redactedContactPhone->cursorPosition();
    if(numberValidator->validate(phone, b) == QValidator::Acceptable){
        QString fullName = ui->lineEdit_contact_surname_redact->text() + " " + ui->lineEdit_contact_name_redact->text() + " " + ui->lineEdit_contact_middleName_redact->text();
        QString surname = ui->lineEdit_contact_surname_redact->text();
        QString name = ui->lineEdit_contact_name_redact->text();
        QString middleName = ui->lineEdit_contact_middleName_redact->text();
        QString email = ui->lineEdit_redactedContactEmail->text();
        QVariant data = ui->listView_contacts->model()->data(ui->listView_contacts->model()->index(ui->listView_contacts->currentIndex().row(),0),Qt::DisplayRole).toString();
        database::RedactContacts(fullName, surname, name, middleName, phone, email, data.toString());
        UpdateContacts();
        ui->lineEdit_search_contacts->setText("");
        on_lineEdit_search_contacts_textEdited("");
        for(int row = 0; row <= ui->listView_contacts->model()->rowCount(); row++){
                if(ui->listView_contacts->model()->data(ui->listView_contacts->model()->index(row, 0)) == data){
                    QModelIndex index = ui->listView_contacts->model()->index(row, 1);
                    on_listView_contacts_clicked(index);
                    ui->listView_contacts->setCurrentIndex(index);
                    ui->listView_contacts->scrollTo(index);
                    break;
                }
        }
    } else {
        QMessageBox::warning(this, "Ошибка", "<FONT COLOR='#ffffff'>Проверьте правильность введенного номера телефона.</FONT> \n");
    }
}

void MainWindow::on_pushButton_saveNewContact_clicked()
{
    QString a = ui->lineEdit_contact_phone->text();
    int b = ui->lineEdit_contact_phone->cursorPosition();
    if(numberValidator->validate(a, b) == QValidator::Acceptable){
        QString fullName = ui->lineEdit_contact_surname->text() + " " + ui->lineEdit_contact_name->text() + " " + ui->lineEdit_contact_middleName->text();
        database::InsertToContacts(fullName, ui->lineEdit_contact_surname->text(), ui->lineEdit_contact_name->text(), ui->lineEdit_contact_middleName->text(),
                                   ui->lineEdit_contact_phone->text(), ui->lineEdit_contact_email->text(), username);
        on_pushButton_CreateNewContact_clicked();

        UpdateContacts();
        ui->lineEdit_search_contacts->setText("");
        on_lineEdit_search_contacts_textEdited("");


        ui->lineEdit_contact_surname->clear(); ui->lineEdit_contact_name->clear(); ui->lineEdit_contact_middleName->clear();
        ui->lineEdit_contact_phone->clear(); ui->lineEdit_contact_email->clear(); //очищаем все поля


        QSqlQuery query;
        query.prepare("SELECT seq FROM sqlite_sequence where name = :name");
        query.bindValue(":name", "contacts");
        query.exec();
        int nextContactId;
        while(query.next()){
            nextContactId = query.value(0).toInt();
        }
        for(int row = 0; row <= contactsModel->rowCount(); row++){
            if(ui->listView_contacts->model()->data(ui->listView_contacts->model()->index(row, 0)) == nextContactId){
                QModelIndex index= ui->listView_contacts->model()->index(row, 1);
                ui->listView_contacts->setCurrentIndex(index);
                on_listView_contacts_clicked(index);
                ui->listView_contacts->scrollTo(index);
                break;
            }
        }
    } else {
        QMessageBox::warning(this, "Ошибка", "<FONT COLOR='#ffffff'>Проверьте правильность введенного номера телефона.</FONT> \n");
    }
}

void MainWindow::on_pushButton_deleteContact_clicked()
{
    QMessageBox::StandardButton reply =
    QMessageBox::question(this, "Подтвердите действие", "<FONT COLOR='#ffffff'> Вы уверены, что хотите удалить объект из базы данных? \nЭто действие необратимо! </FONT>");
    if (reply == QMessageBox::Yes){
        QVariant data = ui->listView_contacts->model()->data(ui->listView_contacts->model()->index(ui->listView_contacts->currentIndex().row(),0),Qt::DisplayRole).toString();
        database::DeleteFromContacts(data.toString());
        database::DeleteContactsAssignments(data.toString());
        on_lineEdit_search_contacts_textEdited(ui->lineEdit_search_contacts->text());
        ContactIsOpened = false;

        MainThemeOfContacts();
    }
}






//компании контактов
void MainWindow::ClearContactOutputCompanies()
{
    if(ui->verticalLayout_55->count() != 0){
        int count = ui->verticalLayout_55->count();
        for(int i = 0; i < count; i++){
            dynamicFrame *frame = qobject_cast<dynamicFrame*>(ui->verticalLayout_55->itemAt(ui->verticalLayout_55->count()- 1)->widget());
            delete frame;
        }
    }
}

void MainWindow::LinkToCompanyByButton()
{
    dynamicButton_Link_to_contacts* button = (dynamicButton_Link_to_contacts*) sender();
    SearchForCompanyIndexById(button->getID());
}
void MainWindow::SearchForCompanyIndexById(int id)
{
    QSqlQuery query;
    query.prepare("SELECT isActive FROM clients WHERE id = :companyId");
    query.bindValue(":companyId", id);
    query.exec();
    query.next();
    int activeStatus = query.value(0).toInt();
    if(ui->lineEdit_SearchOfClients->text() != ""){
        ui->lineEdit_SearchOfClients->setText("");
        on_lineEdit_SearchOfClients_textEdited("");
    }
    ui->lineEdit_SearchOfClients->clearFocus();
    if(activeStatus == 0){
        UpdateInactiveClients();
        for(int row = 0; row <= ui->listView->model()->rowCount(); row++){
            if(ui->listView->model()->data(ui->listView->model()->index(row, 0)) == id){
                QModelIndex index = ui->listView->model()->index(row, 1);
                ui->pushButton_inactiveClients->setStyleSheet("color:black;"
                                                       "background-color:green;"
                                                       "border-width:1px;"
                                                       "border-radius:8px;"
                                                       "border-color: black;");

                ui->pushButton_contacts->setStyleSheet("color:black;"
                                                       "background-color:white;"
                                                       "border-width:1px;"
                                                       "border-radius:8px;"
                                                       "border-color: black;");
                ui->stackedWidget_main->setCurrentIndex(0);
                tempListView_contacts_id = ui->listView_contacts->model()->data(ui->listView_contacts->model()->index(ui->listView_contacts->currentIndex().row(),0),Qt::DisplayRole).toInt();
                ui->listView->setCurrentIndex(index);
                on_listView_clicked(index);
                ui->listView->scrollTo(index);
                StackedWithContactsIsOpen = false;
                StackedWithInactiveClientsIsOpen = true;
                break;
            }
        }
    } else if (activeStatus == 1){
        UpdateActiveClients();
        for(int row = 0; row <= ui->listView->model()->rowCount(); row++){
            if(ui->listView->model()->data(ui->listView->model()->index(row, 0)) == id){
                QModelIndex index = ui->listView->model()->index(row, 1);
                ui->pushButton_activeClients->setStyleSheet("color:black;"
                                                       "background-color:green;"
                                                       "border-width:1px;"
                                                       "border-radius:8px;"
                                                       "border-color: black;");

                ui->pushButton_contacts->setStyleSheet("color:black;"
                                                       "background-color:white;"
                                                       "border-width:1px;"
                                                       "border-radius:8px;"
                                                       "border-color: black;");
                ui->stackedWidget_main->setCurrentIndex(0);
                tempListView_contacts_id = ui->listView_contacts->model()->data(ui->listView_contacts->model()->index(ui->listView_contacts->currentIndex().row(),0),Qt::DisplayRole).toInt();
                ui->listView->setCurrentIndex(index);
                on_listView_clicked(index);
                ui->listView->scrollTo(index);
                StackedWithContactsIsOpen = false;
                StackedWithActiveClientsIsOpen = true;
                break;
            }
        }
    }
}

void MainWindow::UpdateContactsCompanies(int id)
{
    ClearContactOutputCompanies();

    QSqlQuery query;
    query.prepare("SELECT id, companyName FROM clients WHERE clients.rowid IN (SELECT companyId FROM assignment WHERE contactId = :id);");
    query.bindValue(":id", id);
    if(!query.exec()){
        qDebug() << query.lastError().text();
    }
    QSqlRecord rec = query.record();
    while(query.next()){
        dynamicFrame *frame = new dynamicFrame(this);
        frame->setStyleSheet("background-color:rgb(47,49,54);"
                             "color:white;"
                             "border-width:1px;"
                             "border-radius:8px;"
                             "border-color: black;");
        frame->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
        QHBoxLayout *lay = new QHBoxLayout(frame);

        dynamicLabel *companyName = new dynamicLabel(frame);
        companyName->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        companyName->setFont(defaultfont);
        companyName->setText(query.value(rec.indexOf("companyName")).toString());
        lay->addWidget(companyName);

        dynamicButton_Link_to_contacts *button_link = new dynamicButton_Link_to_contacts(frame);
        button_link->setID(query.value(rec.indexOf("id")).toString());
        button_link->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        button_link->setFixedSize(24,24);
        button_link->setStyleSheet("background-color:white; color:black");
        button_link->setText("->");
        connect(button_link, SIGNAL(clicked()), this, SLOT(LinkToCompanyByButton()));
        lay->addWidget(button_link);


        ui->verticalLayout_55->addWidget(frame);

    }
}








void MainWindow::on_action_4_triggered()
{
    ui->label_UserName_User->setText(username);
    ui->stackedWidget_username_3->setCurrentIndex(1);
    ui->widget_OldNewPass_3->hide();
    ui->stackedWidget_redactpass_3->setCurrentIndex(0);
    ui->label_61->hide(); ui->label_62->hide();

    ui->stackedWidget_main->setCurrentIndex(2);
    StackedWithActiveClientsIsOpen = false; StackedWithContactsIsOpen = false;
    ui->pushButton_activeClients->setStyleSheet("color:black;"
                                           "background-color:white;"
                                           "border-width:1px;"
                                           "border-radius:8px;"
                                           "border-color: black;");

    ui->pushButton_contacts->setStyleSheet("color:black;"
                                           "background-color:white;"
                                           "border-width:1px;"
                                           "border-radius:8px;"
                                           "border-color: black;");
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
        QMessageBox::warning(this, "Ошибка!", "<FONT COLOR='#ffffff'>Введите новое имя пользователя.</FONT>");
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
            QMessageBox::warning(this, "Ошибка", "<FONT COLOR='#ffffff'>Новый пароль должен содержать 3 и более символов.</FONT> \n");
    } else {
        if(database::RedactPass(username, ui->lineEdit_oldpass->text(), ui->lineEdit_newpass->text())){
            QMessageBox::information(this, "Успех", "Пароль успешно изменен.");
            ui->stackedWidget_redactpass_3->setCurrentIndex(0);
            ui->widget_OldNewPass_3->hide();
            ui->lineEdit_newpass->clear(); ui->label_61->hide();
            ui->lineEdit_oldpass->clear(); ui->label_62->hide();

        } else {
            QMessageBox::warning(this, "Ошибка!", "<FONT COLOR='#ffffff'>Старый пароль не верный.</FONT> \n");
        }
    }
}


void MainWindow::on_pushButton_2_clicked()
{
    QString data = ui->listView->model()->data(ui->listView->model()->index(ui->listView->currentIndex().row(),0),Qt::DisplayRole).toString();
    database::UpdateActiveStatusOfCompany(data, QString::number(0));
    ui->lineEdit_SearchOfClients->setText("");

    UpdateInactiveClients();
    ui->lineEdit_SearchOfClients->setText("");
    on_lineEdit_SearchOfClients_textEdited("");

    QModelIndex index;
    for(int row = 0; row <= clientsModel->rowCount(); row++){
        if(ui->listView->model()->data(ui->listView->model()->index(row, 0)) == data){
            index = ui->listView->model()->index(row, 1);
        }
    }
    ui->listView->setCurrentIndex(index);
    on_listView_clicked(index);
    ui->listView->scrollTo(index);
    tempListView_inactiveClients_id = 0;
    StackedWithActiveClientsIsOpen = false;
    StackedWithInactiveClientsIsOpen = true;
    ui->pushButton_activeClients->setStyleSheet("color:black;"
                                           "background-color:white;"
                                           "border-width:1px;"
                                           "border-radius:8px;"
                                           "border-color: black;");

    ui->pushButton_inactiveClients->setStyleSheet("color:black;"
                                           "background-color:green;"
                                           "border-width:1px;"
                                           "border-radius:8px;"
                                           "border-color: black;");
}


void MainWindow::on_pushButton_3_clicked()
{
    QString data = ui->listView->model()->data(ui->listView->model()->index(ui->listView->currentIndex().row(),0),Qt::DisplayRole).toString();
    database::UpdateActiveStatusOfCompany(data, QString::number(1));

    UpdateActiveClients();
    ui->lineEdit_SearchOfClients->setText("");
    on_lineEdit_SearchOfClients_textEdited("");

    QModelIndex index;
    for(int row = 0; row <= clientsModel->rowCount(); row++){
        if(ui->listView->model()->data(ui->listView->model()->index(row, 0)) == data){
            index = ui->listView->model()->index(row, 1);
        }
    }
    ui->listView->setCurrentIndex(index);
    on_listView_clicked(index);
    ui->listView->scrollTo(index);
    tempListView_activeClients_id = 0;
    StackedWithActiveClientsIsOpen = true;
    StackedWithInactiveClientsIsOpen = false;
    ui->pushButton_activeClients->setStyleSheet("color:black;"
                                           "background-color:green;"
                                           "border-width:1px;"
                                           "border-radius:8px;"
                                           "border-color: black;");

    ui->pushButton_inactiveClients->setStyleSheet("color:black;"
                                           "background-color:white;"
                                           "border-width:1px;"
                                           "border-radius:8px;"
                                           "border-color: black;");
}

