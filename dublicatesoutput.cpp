#include "dublicatesoutput.h"
#include "ui_dublicatesoutput.h"
#include "dynamicFields.h"
#include "QSqlQuery"
#include "QThread"
#include "QObject"


DublicatesOutput::DublicatesOutput(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DublicatesOutput)
{
    ui->setupUi(this);
    connect(ui->pushButton_save, SIGNAL(clicked()), this, SLOT(accept()));
    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(reject()));
}


DublicatesOutput::~DublicatesOutput()
{
    delete ui;
}

void DublicatesOutput::on_pushButton_cancel_clicked()
{

}


void DublicatesOutput::on_pushButton_save_clicked()
{

}

void DublicatesOutput::ClearAllWidgets()
{
    QLayoutItem* child;
    while ((child = ui->verticalLayout_4->takeAt(0)) != 0)
    {
        delete child->widget();
        delete child;
    }
    while ((child = ui->verticalLayout_3->takeAt(0)) != 0)
    {
        delete child->widget();
        delete child;
    }
}

void DublicatesOutput::updateInformation(QList<QList<QString> > findedPhoneDublicates, QList<QList<QString>> phonesWithDublicates)
{
    ui->widget->hide(); ui->widget_2->hide();
    QSqlQuery query;
    int lastId = 0;
    ClearAllWidgets();
    for(int i = phonesWithDublicates.size()-1; i >= 0; i--){
        if(phonesWithDublicates[i].isEmpty()){
            phonesWithDublicates.removeAt(i);
        }
    }
    for(int i = 0; i < phonesWithDublicates.size(); i++){
        dynamicFrame *frame_phone = new dynamicFrame(this);
        frame_phone->setStyleSheet("background-color:rgb(232,235,250);"
                             "color:rgb(36,36,36);"
                             "border-width:1px;"
                             "border-radius:8px;"
                             "border-color: black;");
        QHBoxLayout *lay = new QHBoxLayout(frame_phone);
        QHBoxLayout *hLay_1 = new QHBoxLayout();
        QHBoxLayout *hLay_2 = new QHBoxLayout();
        QHBoxLayout *hLay_3 = new QHBoxLayout();
        QHBoxLayout *hLay_4 = new QHBoxLayout();
        QHBoxLayout *upperH = new QHBoxLayout();
        QHBoxLayout *downH = new QHBoxLayout();
        QVBoxLayout *vLay = new QVBoxLayout();



        dynamicLabel *icon_fullName = new dynamicLabel(frame_phone);
        icon_fullName->setFixedSize(24,24);
        QPixmap pix_fullName(":/images/images/administrator-black-24.png");
        icon_fullName->setPixmap(pix_fullName);
        dynamicLineEdit *fullName = new dynamicLineEdit(frame_phone);
        fullName->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        fullName->setFont(QFont("Segoe UI Variable Display Semib",12,2));
        if(phonesWithDublicates[i][0] != ""){
            fullName->setText(phonesWithDublicates[i][0]);
            fullName->setToolTip(phonesWithDublicates[i][0]);
        } else {
            fullName->setText("—");
            fullName->setToolTip("Не указано");
        }

        fullName->setReadOnly(true);
        fullName->setFrame(false);
        fullName->setCursorPosition(0);

        hLay_1->addWidget(icon_fullName); hLay_1->addWidget(fullName);

        dynamicLabel *icon_info = new dynamicLabel(frame_phone);
        icon_info->setFixedSize(24,24);
        QPixmap pix_info(":/images/images/info-black-24.png");
        icon_info->setPixmap(pix_info);
        dynamicLineEdit *information = new dynamicLineEdit(frame_phone);
        information->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        information->setFont(QFont("Segoe UI Variable Display Semib",12,2));
        if(phonesWithDublicates[i][1] != ""){
            information->setText(phonesWithDublicates[i][1]);
            information->setToolTip(phonesWithDublicates[i][1]);
        } else {
            information->setText("—");
            information->setToolTip("Не указано");
        }

        information->setReadOnly(true);
        information->setFrame(false);
        information->setCursorPosition(0);

        hLay_2->addWidget(icon_info); hLay_2->addWidget(information);

        upperH->addLayout(hLay_1); upperH->addLayout(hLay_2);
        //fullName->setMinimumWidth(104);
        //QSpacerItem *spacer = new QSpacerItem(80, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        dynamicLabel *icon_phone = new dynamicLabel(frame_phone);
        icon_phone->setFixedSize(24,24);
        QPixmap pix_phone(":/images/images/phone-black-24.png");
        icon_phone->setPixmap(pix_phone);
        dynamicLineEdit *phone = new dynamicLineEdit(frame_phone);
        phone->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        phone->setFont(QFont("Segoe UI Variable Display Semib",12,2));
        if(phonesWithDublicates[i][2] != ""){
            phone->setText(phonesWithDublicates[i][2]);
            phone->setToolTip(phonesWithDublicates[i][2]);
        } else {
            phone->setText("—");
            phone->setToolTip("Не указано");
        }

        phone->setReadOnly(true);
        phone->setFrame(false);
        phone->setCursorPosition(0);

        hLay_3->addWidget(icon_phone); hLay_3->addWidget(phone);

        dynamicLabel *icon_email = new dynamicLabel(frame_phone);
        icon_email->setFixedSize(24,24);
        QPixmap pix_email(":/images/images/email-black-24.png");
        icon_email->setPixmap(pix_email);
        dynamicLineEdit *email = new dynamicLineEdit(frame_phone);
        email->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        email->setFont(QFont("Segoe UI Variable Display Semib",12,2));
        if(phonesWithDublicates[i][3] != ""){
            email->setText(phonesWithDublicates[i][3]);
            email->setToolTip(phonesWithDublicates[i][3]);
        } else {
            email->setText("—");
            email->setToolTip("Не указано");
        }

        email->setReadOnly(true);
        email->setFrame(false);
        email->setCursorPosition(0);

        hLay_4->addWidget(icon_email); hLay_4->addWidget(email);
        if(phonesWithDublicates[i][4] == "1"){
            phone->setStyleSheet("border-bottom-style: solid; border-bottom-color: red; border-bottom-width: 1px;");
        }
        if(phonesWithDublicates[i].size() > 5){
            email->setStyleSheet("border-bottom-style: solid; border-bottom-color: red; border-bottom-width: 1px;");
        } else if (phonesWithDublicates[i][4] == "2"){
            email->setStyleSheet("border-bottom-style: solid; border-bottom-color: red; border-bottom-width: 1px;");
        }

        downH->addLayout(hLay_3); downH->addLayout(hLay_4);
        vLay->addLayout(upperH); vLay->addLayout(downH);
        lay->addLayout(vLay);
        frame_phone->setLayout(lay);
        ui->verticalLayout_3->addWidget(frame_phone);
    }


    for(int i = 0; i < findedPhoneDublicates.size(); i++){

        dynamicFrame *frame_dublicate = new dynamicFrame(this);
        QVBoxLayout *vLay_dublicate = new QVBoxLayout(frame_dublicate);
        vLay_dublicate->setContentsMargins(0,0,0,0);
        vLay_dublicate->setSpacing(0);

        if(findedPhoneDublicates[i][0].toInt() != lastId){
            QHBoxLayout *hLay_companyInfo = new QHBoxLayout();
            hLay_companyInfo->setContentsMargins(0,0,0,0);
            QHBoxLayout *hLay_usernameInfo = new QHBoxLayout();
            hLay_usernameInfo->setContentsMargins(0,0,0,0);
            lastId = findedPhoneDublicates[i][0].toInt();

            dynamicLabel *companyName = new dynamicLabel(frame_dublicate);
            query.prepare("SELECT companyName, username, isActive FROM clients WHERE id = :companyId;");
            query.bindValue(":companyId", findedPhoneDublicates[i][0]);
            query.exec(); query.first();
            companyName->setText(query.value(0).toString());
            companyName->setFont(QFont("Segoe UI Variable Display Semib",14,2));

            dynamicLabel *companyActiveStatus = new dynamicLabel(frame_dublicate);
            QString activeStatus;
            switch(query.value(2).toInt()){
            case 0:{
                activeStatus = "Неактивная";
                break;
               }
            case 1:{
                activeStatus = "Активная";
                break;
               }
            case 2:{
                activeStatus = "Потенциальная";
               }
            }
            companyActiveStatus->setText("(" + activeStatus + ")");
            companyActiveStatus->setFont(QFont("Segoe UI Variable Display Semib",12,2));
            companyActiveStatus->setStyleSheet("color:rgb(125,125,125);");

            hLay_companyInfo->addWidget(companyName); hLay_companyInfo->addWidget(companyActiveStatus); hLay_companyInfo->addStretch();

            dynamicLabel *username = new dynamicLabel(frame_dublicate);
            username->setText("Владелец: " + query.value(1).toString());
            username->setFont(QFont("Segoe UI Variable Display Semib",12,2));
            username->setStyleSheet("color:rgb(125,125,125);");

            hLay_usernameInfo->addWidget(username); hLay_usernameInfo->addStretch();
            vLay_dublicate->addLayout(hLay_companyInfo); vLay_dublicate->addLayout(hLay_usernameInfo);
        }

        dynamicFrame *frame_phone_2 = new dynamicFrame(frame_dublicate);
        frame_phone_2->setStyleSheet("background-color:rgb(232,235,250);"
                             "color:rgb(36,36,36);"
                             "border-width:1px;"
                             "border-radius:8px;"
                             "border-color: black;");
        QVBoxLayout *lay = new QVBoxLayout(frame_phone_2);
        QHBoxLayout *hLay_1 = new QHBoxLayout();
        QHBoxLayout *hLay_2 = new QHBoxLayout();
        QHBoxLayout *hLay_3 = new QHBoxLayout();
        QHBoxLayout *hLay_4 = new QHBoxLayout();
        QHBoxLayout *upperH = new QHBoxLayout();
        QHBoxLayout *downH = new QHBoxLayout();

        dynamicLabel *icon_fullName = new dynamicLabel(frame_phone_2);
        icon_fullName->setFixedSize(24,24);
        QPixmap pix_fullName(":/images/images/administrator-black-24.png");
        icon_fullName->setPixmap(pix_fullName);
        dynamicLineEdit *fullName = new dynamicLineEdit(frame_phone_2);
        fullName->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        fullName->setFont(QFont("Segoe UI Variable Display Semib",12,2));
        if(findedPhoneDublicates[i][1] != ""){
            fullName->setText(findedPhoneDublicates[i][1]);
            fullName->setToolTip(findedPhoneDublicates[i][1]);
        } else {
            fullName->setText("—");
            fullName->setToolTip("Не указано");
        }

        fullName->setReadOnly(true);
        fullName->setFrame(false);
        fullName->setCursorPosition(0);

        hLay_1->addWidget(icon_fullName); hLay_1->addWidget(fullName);

        dynamicLabel *icon_info = new dynamicLabel(frame_phone_2);
        icon_info->setFixedSize(24,24);
        QPixmap pix_info(":/images/images/info-black-24.png");
        icon_info->setPixmap(pix_info);
        dynamicLineEdit *information = new dynamicLineEdit(frame_phone_2);
        information->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        information->setFont(QFont("Segoe UI Variable Display Semib",12,2));
        if(findedPhoneDublicates[i][2] != ""){
            information->setText(findedPhoneDublicates[i][2]);
            information->setToolTip(findedPhoneDublicates[i][2]);
        } else {
            information->setText("—");
            information->setToolTip("Не указано");
        }

        information->setReadOnly(true);
        information->setFrame(false);
        information->setCursorPosition(0);

        hLay_2->addWidget(icon_info); hLay_2->addWidget(information);

        upperH->addLayout(hLay_1); upperH->addLayout(hLay_2);

        dynamicLabel *icon_phone = new dynamicLabel(frame_phone_2);
        icon_phone->setFixedSize(24,24);
        QPixmap pix_phone(":/images/images/phone-black-24.png");
        icon_phone->setPixmap(pix_phone);
        dynamicLineEdit *phone = new dynamicLineEdit(frame_phone_2);
        phone->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        phone->setFont(QFont("Segoe UI Variable Display Semib",12,2));
        if(findedPhoneDublicates[i][3] != ""){
            phone->setText(findedPhoneDublicates[i][3]);
            phone->setToolTip(findedPhoneDublicates[i][3]);
        } else {
            phone->setText("—");
            phone->setToolTip("Не указано");
        }

        phone->setReadOnly(true);
        phone->setFrame(false);
        phone->setCursorPosition(0);

        hLay_3->addWidget(icon_phone); hLay_3->addWidget(phone);

        dynamicLabel *icon_email = new dynamicLabel(frame_phone_2);
        icon_email->setFixedSize(24,24);
        QPixmap pix_email(":/images/images/email-black-24.png");
        icon_email->setPixmap(pix_email);
        dynamicLineEdit *email = new dynamicLineEdit(frame_phone_2);
        email->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        email->setFont(QFont("Segoe UI Variable Display Semib",12,2));
        if(findedPhoneDublicates[i][4] != ""){
            email->setText(findedPhoneDublicates[i][4]);
            email->setToolTip(findedPhoneDublicates[i][4]);
        } else {
            email->setText("—");
            email->setToolTip("Не указано");
        }

        email->setReadOnly(true);
        email->setFrame(false);
        email->setCursorPosition(0);

        hLay_4->addWidget(icon_email); hLay_4->addWidget(email);
        if(findedPhoneDublicates[i][5] == "1"){
            phone->setStyleSheet("border-bottom-style: solid; border-bottom-color: red; border-bottom-width: 1px;");
        }
        if(findedPhoneDublicates[i].size() > 6){
            email->setStyleSheet("border-bottom-style: solid; border-bottom-color: red; border-bottom-width: 1px;");
        } else if (findedPhoneDublicates[i][5] == "2"){
            email->setStyleSheet("border-bottom-style: solid; border-bottom-color: red; border-bottom-width: 1px;");
        }

        downH->addLayout(hLay_3); downH->addLayout(hLay_4);
        lay->addLayout(upperH); lay->addLayout(downH);
        frame_phone_2->setLayout(lay);

        vLay_dublicate->addWidget(frame_phone_2);

        ui->verticalLayout_4->addWidget(frame_dublicate);
    }

    ui->widget->show(); ui->widget_2->show();

}

