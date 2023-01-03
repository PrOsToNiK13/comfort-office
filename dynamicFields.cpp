#include "dynamicFields.h"
#include <QTextEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QListWidget>

dynamicLineEdit::dynamicLineEdit(QWidget *parent) :
    QLineEdit(parent)
{
    IdUp++;            // Увеличение счетчика на единицу
    frameID = IdUp;   /* Присвоение кнопке номера, по которому будет производиться
                         * дальнейшая работа с кнопок
                         * */
}
int dynamicLineEdit::IdUp = 0;

int dynamicLineEdit::getID()
{
    return frameID;
}

dynamicLineEdit::~dynamicLineEdit()
{

}

//---------------------------------------------------------------------------------

dynamicFrame::dynamicFrame(QWidget *parent) :
    QFrame(parent)
{
    IdUp++;            // Увеличение счетчика на единицу
    frameID = IdUp;   /* Присвоение кнопке номера, по которому будет производиться
                         * дальнейшая работа с кнопок
                         * */
}
int dynamicFrame::IdUp = 0;

int dynamicFrame::getID()
{
    return frameID;
}

dynamicFrame::~dynamicFrame()
{

}

//--------------------------------------------------------------------------------

dynamicButton::dynamicButton(QWidget *parent) :
    QPushButton(parent)
{

}

void dynamicButton::setID(QString a){
     id = a.toInt();
}

int dynamicButton::getID()
{
    return id;
}

dynamicButton::~dynamicButton()
{

}

//--------------------------------------------------------------------------------

dynamicButtonMainOrNotMainPhone::dynamicButtonMainOrNotMainPhone(QWidget *parent) :
    QPushButton(parent)
{

}

void dynamicButtonMainOrNotMainPhone::setID(QString a){
     id = a.toInt();
}

int dynamicButtonMainOrNotMainPhone::getID()
{
    return id;
}

void dynamicButtonMainOrNotMainPhone::setMainStatus(QString a){
     mainStatus = a.toInt();
}

int dynamicButtonMainOrNotMainPhone::getMainStatus()
{
    return mainStatus;
}

dynamicButtonMainOrNotMainPhone::~dynamicButtonMainOrNotMainPhone()
{

}

//-----------------------------------------------------------------------------------

dynamicButton_Delete_contacts::dynamicButton_Delete_contacts(QWidget *parent) :
    QPushButton(parent)
{

}

void dynamicButton_Delete_contacts::setID(QString a){
     id = a.toInt();
}

int dynamicButton_Delete_contacts::getID()
{
    return id;
}

dynamicButton_Delete_contacts::~dynamicButton_Delete_contacts()
{

}

//-----------------------------------------------------------------------------------

DynamicButton_Delete_clientPhone::DynamicButton_Delete_clientPhone(QWidget *parent) :
    QPushButton(parent)
{

}

void DynamicButton_Delete_clientPhone::setID(QString a){
     id = a.toInt();
}

int DynamicButton_Delete_clientPhone::getID()
{
    return id;
}

DynamicButton_Delete_clientPhone::~DynamicButton_Delete_clientPhone()
{

}

//-----------------------------------------------------------------------------------

DynamicButton_Delete_clientAddress::DynamicButton_Delete_clientAddress(QWidget *parent) :
    QPushButton(parent)
{

}

void DynamicButton_Delete_clientAddress::setID(QString a){
     id = a.toInt();
}

int DynamicButton_Delete_clientAddress::getID()
{
    return id;
}

DynamicButton_Delete_clientAddress::~DynamicButton_Delete_clientAddress()
{

}

//-----------------------------------------------------------------------------------

dynamicButton_Delete_Document::dynamicButton_Delete_Document(QWidget *parent) :
    QPushButton(parent)
{

}

void dynamicButton_Delete_Document::setID(QString a){
     id = a.toInt();
}

int dynamicButton_Delete_Document::getID()
{
    return id;
}

dynamicButton_Delete_Document::~dynamicButton_Delete_Document()
{

}

//-----------------------------------------------------------------------------------

dynamicButton_Link_to_contacts::dynamicButton_Link_to_contacts(QWidget *parent) :
    QPushButton(parent)
{

}

void dynamicButton_Link_to_contacts::setID(QString a){
     id = a.toInt();
}

int dynamicButton_Link_to_contacts::getID()
{
    return id;
}

dynamicButton_Link_to_contacts::~dynamicButton_Link_to_contacts()
{

}
//---------------------------------------------------------------------------------------

dynamicButton_openDocument::dynamicButton_openDocument(QWidget *parent) :
    QPushButton(parent)
{

}

void dynamicButton_openDocument::setID(QString a){
     id = a.toInt();
}

int dynamicButton_openDocument::getID()
{
    return id;
}

dynamicButton_openDocument::~dynamicButton_openDocument()
{

}

//---------------------------------------------------------------------------------------

dynamicText::dynamicText(QWidget *parent) :
    QTextEdit(parent)
{
    IdUp++;            // Увеличение счетчика на единицу
    textID = IdUp;   /* Присвоение кнопке номера, по которому будет производиться
                         * дальнейшая работа с кнопок
                         * */
}
int dynamicText::IdUp = 0;

int dynamicText::getID()
{
    return textID;
}


dynamicText::~dynamicText()
{

}

//----------------------------------------------------------------------------------


dynamicLabel::dynamicLabel(QWidget *parent) :
    QLabel(parent)
{
    IdUp++;            // Увеличение счетчика на единицу
    labelID = IdUp;   /* Присвоение кнопке номера, по которому будет производиться
                         * дальнейшая работа с кнопок
                         * */
}
int dynamicLabel::IdUp = 0;

int dynamicLabel::getID()
{
    return labelID;
}


dynamicLabel::~dynamicLabel()
{

}

//-----------------------------------------------------------------------------


