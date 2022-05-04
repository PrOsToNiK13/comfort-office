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
    void MainThemeOfClients();
    void MainThemeOfContacts();
    void MainThemeOfInactiveClients();

    void UpdateNotes() ;
    void UpdateActiveClients();
    void UpdateInactiveClients();
    void UpdateClientContacts(int id, int where);
    void UpdateDocuments();

    void ClearNotes();
    void ClearNewClientContacts();
    void ClearClientOutputContacts();
    void ClearClientRedactContacts();

    void deleteDynamicButton_deleteNote();
    void deleteDynamicButton_deleteNewClientsContact();
    void deleteDynamicButton_deleteRedactedClientsContact();
    void LinkToContactByButton();

    void on_pushButton_MainMenu_clicked();

    void on_pushButton_CreateNewClient_clicked();
    void on_pushButton_SaveNewClient_clicked();

    void on_lineEdit_SearchOfClients_textEdited(const QString &arg1);

    void on_pushButton_DeleteClient_clicked();

    void on_pushButton_Update_clicked();
    void on_pushButton_SaveRedactedClient_clicked();
    void on_pushButton_CancelRedactOfClient_clicked();

    void on_pushButton_addNote_clicked();
    void on_pushButton_saveNote_clicked();

    void on_action_4_triggered();


    void on_listView_clicked(const QModelIndex &index);


    void on_pushButton_activeClients_clicked();
    void on_pushButton_inactiveClients_clicked();
    void on_pushButton_contacts_clicked();



    void on_listView_contacts_clicked(const QModelIndex &index);

    void UpdateContacts();

    void on_lineEdit_search_contacts_textEdited(const QString &arg1);

    void on_pushButton_CreateNewContact_clicked();

    void on_pushButton_updateContact_clicked();

    void on_pushButton_cancelContactUpdate_clicked();

    void on_pushButton_saveContactUpdate_clicked();

    void on_pushButton_saveNewContact_clicked();

    void on_pushButton_deleteContact_clicked();

    void on_comboBox_NewClientContacts_activated(int index);


    void on_comboBox_EditClientContacts_activated(int index);


    void UpdateContactsCompanies(int id);
    void ClearContactOutputCompanies();
    void LinkToCompanyByButton();

    void SearchForContactIndexById(int id);
    void SearchForCompanyIndexById(int id);
    void on_pushButton_redactUserName_clicked();

    void on_pushButton_saveUserName_clicked();

    void on_pushButton_redactPassword_clicked();

    void on_pushButton_saveNewPassword_clicked();

    void on_pushButton_AddDocument_clicked();
    void DeleteDynamicButton_deleteDocument();
    void ClearDocuments();
    void OpenDocumentById();

    bool eventFilter(QObject *object, QEvent *event);

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::MainWindow *ui;
    QSqlQueryModel *clientsModel;
    QSqlQueryModel *contactsModel;
    QSqlQueryModel *assignment_contacts;

    QSortFilterProxyModel *SortFilterProxyClientsModel;
    QSortFilterProxyModel *SortFilterProxyContactsModel;
    QSortFilterProxyModel *proxyAssignment;

    QCompleter *newClientsCompleter;
    QCompleter *editClientsCompleter;

public slots:
    void AuthResults(QString name);

protected:
     void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
