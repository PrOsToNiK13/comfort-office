#ifndef DATABASE_H
#define DATABASE_H
#include <QWidget>

class database
{
public:
    database();
    static void connectToDataBase();
    static bool openDataBase();
    static bool restoreDataBase();
    static bool createTables();

    static void InsertToActiveClients(QString name, QString phone, QString address, QString username, QString activeStatus);
    static void DeleteFromActiveClients(QString id);
    static void RedactActiveClients(QString name, QString phone, QString address, QString id);
    static void UpdateActiveStatusOfCompany(QString id, QString status);

    static void InsertToNotes(QString id, QString text, QString date);
    static void DeleteFromNotes(QString companyID);
    static void DeleteNote(QString id, QString text);

    static void InsertToUsers(QString name, QString password);
    static void RedactUserName(QString name, QString username);
    static bool RedactPass(QString username, QString oldpass, QString newpass);

    static void InsertToContacts(QString fullName, QString surname, QString name, QString middleName, QString phone, QString email, QString username);
    static void RedactContacts(QString fullName, QString surname, QString name, QString middleName, QString phone, QString email, QString id);
    static void DeleteFromContacts(QString id);

    static void AddAssignment(QString companyId, QString contactId);
    static void DeleteContactsAssignments(QString contactId);
    static void DeleteClientsAssignments(QString companyId);
    static void DeleteAssignmentFromClient(QString companyId, QString contactId);

    static bool AddDocument(QString companyId, QString name, QByteArray arr, QString ext);
    static void DeleteCompanyDocuments(QString id);

};

#endif // DATABASE_H
