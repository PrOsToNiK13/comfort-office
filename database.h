#ifndef DATABASE_H
#define DATABASE_H
#include <QWidget>

class database
{
public:
    database();
    //основное
    static bool connectToDataBase(QString path);
    static bool openDataBase(QString path);
    static bool restoreDataBase(QString path);
    static bool createTables();

    //клиенты (компании)
    static void InsertToClients(QString name, QString username, QString activeStatus);
    static void DeleteFromActiveClients(QString id);
    static void RedactActiveClients(QString name, QString id, QString username);
    static void UpdateActiveStatusOfCompany(QString id, QString status);

    //заметки
    static void InsertToNotes(QString id, QString text, QString date);
    static void DeleteFromNotes(QString companyID);
    static void DeleteNote(QString id, QString text);

    //телефоны  клиентов
    static void InsertToClientPhones(QString mainStatus, QString companyId, QString fullName, QString information, QString phone, QString email);
    static void DeletaAllPhonesFromClient(QString companyId);

    //адреса клиентов
    static void InsertToClientAddresses(QString companyId, QString address, QString information);
    static void DeleteAllAddressesFromClient(QString companyId);

    //пользователи
    static void InsertToUsers(QString name, QString password);
    static void RedactUserName(QString name, QString username);
    static bool RedactPass(QString username, QString oldpass, QString newpass);

    //документы
    static bool AddDocument(QString companyId, QString name, QByteArray arr, QString ext);
    static void DeleteCompanyDocuments(QString id);

};

#endif // DATABASE_H
