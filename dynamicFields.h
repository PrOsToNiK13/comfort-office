#ifndef DYNAMICFIELDS_H
#define DYNAMICFIELDS_H
#include <QTextEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
#include <QListWidget>
#include <QLineEdit>

class dynamicLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit dynamicLineEdit(QWidget *parent = 0);
    ~dynamicLineEdit();
    dynamicLineEdit();
    static int IdUp;
    int getID(); // Функция для возврата локального номера кнопки
private:
    int frameID = 0;
};


class dynamicFrame : public QFrame
{
    Q_OBJECT
public:
    explicit dynamicFrame(QWidget *parent = 0);
    ~dynamicFrame();
    dynamicFrame();
    static int IdUp;
    int getID(); // Функция для возврата локального номера кнопки
private:
    int frameID = 0;
};


class dynamicButton : public QPushButton
{
    Q_OBJECT
public:
    explicit dynamicButton(QWidget *parent = 0);
    ~dynamicButton();
    dynamicButton();
    int id;
    void setID(QString a);   // Статическая переменная, счетчик номеров кнопок
    int getID();// Функция для возврата локального номера кнопки
private:
    int buttonID = 0;
};

class dynamicButtonMainOrNotMainPhone : public QPushButton
{
    Q_OBJECT
public:
    explicit dynamicButtonMainOrNotMainPhone(QWidget *parent = 0);
    ~dynamicButtonMainOrNotMainPhone();
    dynamicButtonMainOrNotMainPhone();
    int id, mainStatus;
    void setID(QString a);   // Статическая переменная, счетчик номеров кнопок
    void setMainStatus(QString a);
    int getID();// Функция для возврата локального номера кнопки
    int getMainStatus();
private:
    int buttonID = 0;
    int MainStatus = 0;
};

class dynamicButton_Delete_contacts : public QPushButton
{
    Q_OBJECT
public:
    explicit dynamicButton_Delete_contacts(QWidget *parent = 0);
    ~dynamicButton_Delete_contacts();
    dynamicButton_Delete_contacts();
    int id;
    void setID(QString a);   // Статическая переменная, счетчик номеров кнопок
    int getID();// Функция для возврата локального номера кнопки
private:
    int buttonID = 0;
};

class dynamicButton_Delete_Document : public QPushButton
{
    Q_OBJECT
public:
    explicit dynamicButton_Delete_Document(QWidget *parent = 0);
    ~dynamicButton_Delete_Document();
    dynamicButton_Delete_Document();
    int id;
    void setID(QString a);   // Статическая переменная, счетчик номеров кнопок
    int getID();// Функция для возврата локального номера кнопки
private:
    int buttonID = 0;
};

class DynamicButton_Delete_clientPhone : public QPushButton{
    Q_OBJECT
public:
    explicit DynamicButton_Delete_clientPhone(QWidget *parent = 0);
    ~DynamicButton_Delete_clientPhone();
    DynamicButton_Delete_clientPhone();
    int id;
    void setID(QString a);   // Статическая переменная, счетчик номеров кнопок
    int getID();// Функция для возврата локального номера кнопки
private:
    int buttonID = 0;
};

class DynamicButton_Delete_clientAddress : public QPushButton{
    Q_OBJECT
public:
    explicit DynamicButton_Delete_clientAddress(QWidget *parent = 0);
    ~DynamicButton_Delete_clientAddress();
    DynamicButton_Delete_clientAddress();
    int id;
    void setID(QString a);   // Статическая переменная, счетчик номеров кнопок
    int getID();// Функция для возврата локального номера кнопки
private:
    int buttonID = 0;
};






class dynamicButton_Link_to_contacts : public QPushButton
{
    Q_OBJECT
public:
    explicit dynamicButton_Link_to_contacts(QWidget *parent = 0);
    ~dynamicButton_Link_to_contacts();
    dynamicButton_Link_to_contacts();
    int id;
    void setID(QString a);   // Статическая переменная, счетчик номеров кнопок
    int getID();// Функция для возврата локального номера кнопки
private:
    int buttonID = 0;
};

class dynamicButton_openDocument : public QPushButton
{
    Q_OBJECT
public:
    explicit dynamicButton_openDocument(QWidget *parent = 0);
    ~dynamicButton_openDocument();
    dynamicButton_openDocument();
    int id;
    void setID(QString a);   // Статическая переменная, счетчик номеров кнопок
    int getID();// Функция для возврата локального номера кнопки
private:
    int buttonID = 0;
};


class dynamicText : public QTextEdit
{
    Q_OBJECT
public:
    explicit dynamicText(QWidget *parent = 0);
    ~dynamicText();
    dynamicText();
    static int IdUp;   // Статическая переменная, счетчик номеров кнопок
    int getID();
private:
    int textID = 0;
};



class dynamicLabel : public QLabel
{
    Q_OBJECT
public:
    explicit dynamicLabel(QWidget *parent = 0);
    ~dynamicLabel();
    dynamicLabel();
    static int IdUp;   // Статическая переменная, счетчик номеров кнопок
    int getID();
private:
    int labelID = 0;
};



#endif // DYNAMICFIELDS_H
