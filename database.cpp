#include "database.h"
#include <QtSql>
#include <QMessageBox>

database::database()
{

}

void database::connectToDataBase()
{
       if(!QFile("clients.db").exists()){
           restoreDataBase();
       } else {
           openDataBase();
       }
}

bool database::openDataBase()
{
    QSqlDatabase sdb = QSqlDatabase::addDatabase("QSQLITE");
    sdb.setDatabaseName("clients.db");
    if(sdb.open()){
         return true;
    } else {
        return false;
    }
}

bool database::restoreDataBase()
{
        if(openDataBase()){
            return (createTables() ? true : false);
        } else {
            qDebug() << "Не удалось восстановить базу данных";
            return false;
        }
        return false;
}

bool database::createTables()
{
    QSqlQuery query;
    bool isOK = true;
    if(!query.exec( "CREATE TABLE users("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "name VARCHAR(50),"
                    "password VARCHAR(50));")){
        isOK = false;
    }
    if(!query.exec( "CREATE TABLE clients ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "companyName VARCHAR(50),"
                    "phone VARCHAR(50),"
                    "address VARCHAR (50),"
                    "username VARCHAR(50),"
                    "isActive INTEGER);")){
        isOK= false;
    }
    if(!query.exec( "CREATE TABLE notes("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "companyID VARCHAR(20),"
                    "text VARCHAR(200),"
                    "date VARCHAR(30));")){
        isOK = false;
    }
    if(!query.exec(" CREATE TABLE contacts("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                   "fullName VARCHAR(150),"
                   "surname VARCHAR(50),"
                   "name VARCHAR(50),"
                   "middleName VARCHAR(50),"
                   "phone VARCHAR(50),"
                   "email VARCHAR(100),"
                   "username VARCHAR(50));")){
        isOK = false;
    }
    if(!query.exec("CREATE TABLE assignment("
                   "companyId INTEGER,"
                   "contactId INTEGER);")){
        isOK = false;
    }
    if(!query.exec("CREATE TABLE documents("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                   "companyId INTEGER,"
                   "fileName VARCHAR(100),"
                   "file BLOB NOT NULL,"
                   "extention VARCHAR(50));")){
        isOK = false;
    }
    if (isOK){
        qDebug() << "Таблицы созданы!";
        return true;
    } else {
        qDebug() << "DataBase: error of create ";
        return false;
    }
    return false;
}




void database::DeleteFromActiveClients(QString id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM clients WHERE id= :ID;");
    query.bindValue(":ID", id);
    if(!query.exec()){
        qDebug() << "Ошибка удаления из таблицы clients";
    }
}

void database::RedactActiveClients(QString name, QString phone, QString address, QString id)
{
    QSqlQuery query;
    query.prepare("UPDATE clients SET companyName = :name, phone = :phone, address = :address WHERE id = :id");
    query.bindValue(":name", name);
    query.bindValue(":phone", phone);
    query.bindValue(":address", address);
    query.bindValue(":id", id);
    if (!query.exec()){
        qDebug() << "Ошибка редактирования компании!";
    }
}

void database::UpdateActiveStatusOfCompany(QString id, QString status)
{
    QSqlQuery query;
    query.prepare("UPDATE clients SET isActive = :status WHERE id = :id");
    query.bindValue(":status", status);
    query.bindValue(":id", id);
    if (!query.exec()){
        qDebug() << "Ошибка редактирования компании!";
    }
}

void database::InsertToActiveClients(QString name, QString phone, QString address, QString username, QString activeStatus)
{
    QSqlQuery query;
    query.prepare("INSERT INTO clients (companyName, phone, address, username, isActive)"
                  "VALUES (:companyName, :phone, :address, :username, :isActive);");
    query.bindValue(":companyName", name);
    query.bindValue(":phone", phone);
    query.bindValue(":address", address);
    query.bindValue(":username", username);
    query.bindValue(":isActive", activeStatus);
    if (!query.exec()){
        qDebug() << "Ошибка записи в таблицу clients!";
    }
}






void database::InsertToNotes(QString id, QString text, QString date)
{
    QSqlQuery query;
    query.prepare("INSERT INTO notes (companyID, text, date)"
                  "VALUES (:companyID, :text, :date);");
    query.bindValue(":companyID", id);
    query.bindValue(":text", text);
    query.bindValue(":date", date);
    if (!query.exec()){
        qDebug() << "Ошибка записи в таблицу notes!";
    }
}

void database::DeleteFromNotes(QString companyID)
{
    QSqlQuery query;
    query.prepare("DELETE FROM notes WHERE companyID = :ID;");
    query.bindValue(":ID", companyID);
    if(!query.exec()){
        qDebug() << "Ошибка удаления из таблицы notes";
    }
}






void database::InsertToUsers(QString name, QString password)
{
    QSqlQuery query;
    query.prepare("INSERT INTO users (name, password)"
                  "VALUES (:name, :password);");
    query.bindValue(":name", name);
    query.bindValue(":password", password);
    if (!query.exec()){
        qDebug() << name << password << query.lastError();
        qDebug() << "Ошибка записи в таблицу users";
    }
}

void database::RedactUserName(QString name, QString username)
{
    QSqlQuery query;
    query.prepare("UPDATE users SET name = :name WHERE name = :username");
    query.bindValue(":name", name);
    query.bindValue(":username", username);
    if (!query.exec()){
        qDebug() << "Ошибка редактирования имени пользователя!";
    }
}

bool database::RedactPass(QString username, QString oldpass, QString newpass)
{
    QSqlQuery query;
    query.prepare("SELECT * FROM users WHERE name =:name");
    query.bindValue(":name", username);
    if (!query.exec()){
        qDebug() << "Ошибка редактирования пароля!";
    }
    QSqlRecord rec = query.record();
    query.first();
    if(oldpass == query.value(rec.indexOf("password")).toString()){
        QSqlQuery query_2;
        query_2.prepare("UPDATE users SET password = :password WHERE name = :username");
        query_2.bindValue(":username", username);
        query_2.bindValue(":password", newpass);
        if (!query_2.exec()){
            qDebug() << "Ошибка редактирования пароля!";
        }
        return true;
    } else {
        return false;
    }
}






void database::InsertToContacts(QString fullName, QString surname, QString name, QString middleName, QString phone, QString email, QString username)
{
    QSqlQuery query;
    query.prepare("INSERT INTO contacts (fullName, surname, name, middleName, phone, email, username)"
                  "VALUES (:fullName, :surname, :name, :middleName, :phone, :email, :username);");
    query.bindValue(":fullName", fullName);
    query.bindValue(":surname", surname);
    query.bindValue(":name", name);
    query.bindValue(":middleName", middleName);
    query.bindValue(":phone", phone);
    query.bindValue(":email", email);
    query.bindValue(":username", username);
    if (!query.exec()){
        qDebug() << "Ошибка записи в таблицу contacts!";
    }
}

void database::RedactContacts(QString fullName, QString surname, QString name, QString middleName, QString phone, QString email, QString id)
{
    QSqlQuery query;
    query.prepare("UPDATE contacts SET fullName = :fullName, surname = :surname, name = :name, middleName = :middleName, phone = :phone, email = :email WHERE id = :id");
    query.bindValue(":fullName", fullName);
    query.bindValue(":surname", surname);
    query.bindValue(":name", name);
    query.bindValue(":middleName", middleName);
    query.bindValue(":phone", phone);
    query.bindValue(":email", email);
    query.bindValue(":id", id);
    if (!query.exec()){
        qDebug() << "Ошибка редактирования контакта!";
    }
}

void database::DeleteFromContacts(QString id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM contacts WHERE id= :ID;");
    query.bindValue(":ID", id);
    if(!query.exec()){
        qDebug() << "Ошибка удаления из таблицы contacts";
    }
}





void database::AddAssignment(QString companyId, QString contactId)
{
    QSqlQuery query;
    query.prepare("INSERT INTO assignment (companyId, contactId)"
                      "VALUES (:companyId, :contactId);");
    query.bindValue(":companyId", companyId);
    query.bindValue(":contactId", contactId);
    if (!query.exec()){
        qDebug() << "Ошибка записи в таблицу assignment!";
    }
}

void database::DeleteContactsAssignments(QString contactId)
{
    QSqlQuery query;
    query.prepare("DELETE  FROM assignment WHERE contactId = :contactsId");
    query.bindValue(":contactId", contactId);
    if (!query.exec()){
        qDebug() << "Ошибка удаления из таблицы assignment!";
    }
}

void database::DeleteClientsAssignments(QString companyId)
{
    QSqlQuery query;
    query.prepare("DELETE  FROM assignment WHERE companyId = :companyId");
    query.bindValue(":companyId", companyId);
    if (!query.exec()){
        qDebug() << "Ошибка удаления из таблицы assignment!";
    }
}

void database::DeleteAssignmentFromClient(QString companyId, QString contactId)
{
    QSqlQuery query;
    query.prepare("DELETE FROM assignment WHERE contactId = :contactId AND companyId = :companyId");
    query.bindValue(":contactId", contactId);
    query.bindValue(":companyId", companyId);
    if (!query.exec()){
        qDebug() << "Ошибка удаления из таблицы assignment!";
    }
}




bool database::AddDocument(QString companyId, QString name, QByteArray arr, QString ext)
{
    QSqlQuery query;
    query.prepare("INSERT INTO documents (companyId, fileName, file, extention)"
                  "VALUES (:companyId, :fileName, :file, :extention);");
    query.bindValue(":companyId", companyId);
    query.bindValue(":fileName", name);
    query.bindValue(":file", arr);
    query.bindValue(":extention", ext);

    if(!query.exec()){
        qDebug() << "Ошибка в записи документа!";
    }
    return true;
}

void database::DeleteCompanyDocuments(QString id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM documents WHERE companyId = :id");
    query.bindValue(":id", id);
    if(!query.exec()){
        qDebug() << "Ошибка при удалении документов компании";
    }
}


