#include "dynamicFields.h"
#include <QTextEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QListWidget>

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


