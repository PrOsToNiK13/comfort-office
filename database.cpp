#include "database.h"
#include <QtSql>
#include <QMessageBox>

database::database()
{

}

bool database::connectToDataBase(QString path)
{
       if(!QFile(path).exists()){
           return restoreDataBase(path);
       } else {
           return openDataBase(path);
       }
}

bool database::openDataBase(QString path)
{
    QSqlDatabase sdb = QSqlDatabase::addDatabase("QSQLITE");
    sdb.setDatabaseName(path);
    if(sdb.open()){
         return true;
    } else {
        return false;
    }
}

bool database::restoreDataBase(QString path)
{
        if(openDataBase(path)){
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
                    "showAllClientsInDefault INTEGER,"
                    "name VARCHAR(50),"
                    "password VARCHAR(50));")){
        isOK = false;
        qDebug() << "users created";
    }
    if(!query.exec( "CREATE TABLE clients("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "companyName VARCHAR(50),"
                    "username VARCHAR(50),"
                    "isActive INTEGER);")){
        isOK= false;
        qDebug() << "clients created";
    }
    if(!query.exec( "CREATE TABLE notes("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "companyID VARCHAR(20),"
                    "text VARCHAR(200),"
                    "date VARCHAR(30));")){
        isOK = false;
        qDebug() << "notes created";
    }
    if(!query.exec("CREATE TABLE documents("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                   "companyId INTEGER,"
                   "fileName VARCHAR(100),"
                   "file BLOB NOT NULL,"
                   "extention VARCHAR(50));")){
        isOK = false;
        qDebug() << "documents created";
    }
    if(!query.exec("CREATE TABLE clientPhones("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                   "mainStatus INTEGER,"
                   "companyId INTEGER,"
                   "fullName VARCHAR(50),"
                   "information VARCHAR(50),"
                   "phone VARCHAR(20),"
                   "email VARCHAR(50));")){
        isOK = false;
        qDebug() << "clientPhones created";
    }
    if(!query.exec("CREATE TABLE clientAddresses("
                   "companyId INTEGER,"
                   "address VARCHAR(50),"
                   "information VARCHAR(50));")){
        isOK = false;
        qDebug() << "clientAddresses created";
    }
    if (isOK){
        qDebug() << "Таблицы созданы!";
        return true;
    } else {
        qDebug() << "DataBase: error of create";
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

void database::RedactActiveClients(QString name, QString id, QString username)
{
    QSqlQuery query;
    query.prepare("UPDATE clients SET companyName = :name, username = :username WHERE id = :id");
    query.bindValue(":name", name);
    query.bindValue(":username", username);
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

void database::InsertToClients(QString name, QString username, QString activeStatus)
{
    QSqlQuery query;
    query.prepare("INSERT INTO clients (companyName, username, isActive)"
                  "VALUES (:companyName, :username, :isActive);");
    query.bindValue(":companyName", name);
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






void database::InsertToClientPhones(QString mainStatus, QString companyId, QString fullName, QString information, QString phone, QString email)
{
    QSqlQuery query;
    query.prepare("INSERT INTO clientPhones (mainStatus, companyId, fullName, information, phone, email)"
                  "VALUES (:mainStatus, :companyId, :fullName, :information, :phone, :email);");
    query.bindValue(":mainStatus", mainStatus);
    query.bindValue(":companyId", companyId);
    query.bindValue(":fullName", fullName);
    query.bindValue(":information", information);
    query.bindValue(":phone", phone);
    query.bindValue(":email", email);
    if (!query.exec()){
        qDebug() << "Ошибка записи в таблицу clientPhones!";
    }
}

void database::DeletaAllPhonesFromClient(QString companyId){
    QSqlQuery query;
    query.prepare("DELETE FROM clientPhones WHERE companyId = :companyId");
    query.bindValue(":companyId", companyId);
    if (!query.exec()){
        qDebug() << "Ошибка удаления из таблицы clientPhones!";
    }
}





void database::InsertToClientAddresses(QString companyId, QString address, QString information)
{
    QSqlQuery query;
    query.prepare("INSERT INTO clientAddresses (companyId, address, information)"
                  "VALUES (:companyId, :address, :information);");
    query.bindValue(":companyId", companyId);
    query.bindValue(":address", address);
    query.bindValue(":information", information);
    if (!query.exec()){
        qDebug() << "Ошибка записи в таблицу clientPhones!";
    }
}

void database::DeleteAllAddressesFromClient(QString companyId)
{
    QSqlQuery query;
    query.prepare("DELETE FROM clientAddresses WHERE companyId = :companyId");
    query.bindValue(":companyId", companyId);
    if (!query.exec()){
        qDebug() << "Ошибка удаления из таблицы clientAddresses!";
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


