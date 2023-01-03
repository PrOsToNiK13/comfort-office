#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QRegularExpressionValidator>
#include <QSqlTableModel>
#include <QSqlRelationalTableModel>
#include <QListWidgetItem>
#include <QSortFilterProxyModel>
#include <QCompleter>
#include <QKeyEvent>
#include <QStandardItemModel >
#include <createnewcontact.h>
#include <QList>
#include "authwindow.h"
#include "dublicatesoutput.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    //menu
    void on_pushButton_MainMenu_clicked();
    void on_pushButton_clients_clicked();
    void on_pushButton_potencial_clients_clicked();
    void on_pushButton_providers_clicked();
    void MainThemeOfActiveClients();
    void MainThemeOfInactiveClients();
    void MainThemeOfPotencialClients();
    void MainThemeOfProviders();

    //клиенты
    void UpdateComboBoxUsers();
    void UpdateInactiveClients();
    void UpdateActiveClients();
    void UpdatePotencialClients();
    void UpdateProviders();
    void on_pushButton_CreateNewClient_clicked();
    void on_pushButton_SaveNewClient_clicked();
    void on_lineEdit_SearchOfClients_textEdited(const QString &arg1);
    void on_pushButton_DeleteClient_clicked();
    void on_pushButton_Update_clicked();
    void on_pushButton_SaveRedactedClient_clicked();
    void on_pushButton_CancelRedactOfClient_clicked();
    void on_listView_clicked(const QModelIndex &index);
    void on_pushButton_active_newClient_clicked();
    void on_pushButton_inactive_newClient_clicked();



    //заметки
    void UpdateNotes();
    void on_pushButton_addNote_clicked();
    void on_pushButton_saveNote_clicked();
    void ClearNotes();
    void deleteDynamicButton_deleteNote();



    //контактные лица
    QList<QList<QString>> CheckForDuplicate(QList<QString> list, int companyId);
    void UpdateNewClientPhones();
    void UpdateOutputCLientPhones();
    void UpdateRedactClientPhones();
    void CheckUpdatesForRedactClientPhones();
    void ClearNewClientPhones();
    void ClearOutputClientPhones();
    void ClearRedactClientPhones();
    void deleteDynamicButton_deleteClientsPhone();
    void setMainOrNotMainPhone();
    void on_pushButton_AddNewClientPhone_clicked();
    void on_pushButton_saveNewClientPhone_clicked();
    void on_pushButton_AddRedactClientPhone_clicked();
    void on_pushButton_saveRedactClientPhone_clicked();




    //адреса
    void CheckUpdatesForRedactClientAddresses();
    void UpdateNewClientAddresses();
    void UpdateOutputClientAddresses();
    void UpdateRedactClientAddresses();
    void ClearClientAddresses(int place);
    void DeleteDybamicButton_deleteClientsAddress();
    void on_pushButton_AddNewClientAddress_clicked();
    void on_pushButton_saveNewClientAddress_clicked();
    void on_pushButton_AddRedactClientAddress_clicked();
    void on_pushButton_saveRedactClientAddress_clicked();



    //документы
    void UpdateDocuments();
    void on_pushButton_AddDocument_clicked();
    void DeleteDynamicButton_deleteDocument();
    void ClearDocuments();
    void OpenDocumentById();



   //пользователи
    void on_action_4_triggered();
    void on_pushButton_redactUserName_clicked();
    void on_pushButton_saveUserName_clicked();
    void on_pushButton_redactPassword_clicked();
    void on_pushButton_saveNewPassword_clicked();
    void on_pushButton_outputClients_inactive_clicked();
    void on_pushButton_outputClient_active_clicked();



    void on_pushButton_setClientActiveFromPotencial_clicked();

    void on_comboBox_selectedUser_activated(int index);

    void on_checkBox_showAllClients_stateChanged(int arg1);

    void on_pushButton_activeClients_clicked();

    void on_pushButton_inactiveClients_clicked();

signals:
    void OpenInfoAboutDublicates(QList<QList<QString> > findedPhoneDublicates, QList<QList<QString>> phonesWithDublicates);

private:
    Ui::MainWindow *ui;
    AuthWindow auth;

    QSqlQueryModel *clientsModel;
    QSqlQueryModel *potencialClientsModel;
    QSqlQueryModel *usernames;
    QSqlQueryModel *dublicatesModel;

    QSortFilterProxyModel *SortFilterProxyClientsModel;
    QSortFilterProxyModel *SortFilterProxyPotencialClientsModel;
    QSortFilterProxyModel *dublicatesProxy;

    QCompleter *newClientsCompleter;
    QCompleter *editClientsCompleter;

    DublicatesOutput dublicatesOutput;

    bool eventFilter(QObject *object, QEvent *event);

public slots:
    void display();
    void AuthResults(QString name);

protected:
     void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
