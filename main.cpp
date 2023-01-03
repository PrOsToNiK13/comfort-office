#include "mainwindow.h"
#include <QApplication>
#include <QtSql>
#include "database.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QSettings>

void messageToFile(QtMsgType type,
                   const QMessageLogContext& /* context */,
                   const QString& msg)
{
    QString nameFile = qApp->applicationDirPath() + "/server_log.log";
    QFile file(nameFile);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
        return;
    QString strDateTime =
        QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QTextStream out(&file);
    switch(type)
    {
        case QtDebugMsg:
            out << strDateTime << " " << "Debug: " << msg << "\n";
        break;
        case QtInfoMsg:
            out << strDateTime << " " << "Info: " << msg << "\n";
        break;
        case QtWarningMsg:
            out << strDateTime << " " << "Warning: " << msg << "\n";
        break;
        case QtCriticalMsg:
            out << strDateTime << " " << "Critical: " << msg << "\n";
        break;
        case QtFatalMsg:
            out << strDateTime << " " << "Fatal: " << msg << "\n";
        abort();
    }
}


void SaveSettings(QString path){
    QSettings settings("Comfort-office", "Comfort-CRM");
    settings.beginGroup("Main");
    settings.setValue("DataBasePath", path);
    settings.endGroup();
}

QString LoadSettings(){
    QSettings settings("Comfort-office", "Comfort-CRM");
    settings.beginGroup("Main");
    QString DBPath = settings.value("DataBasePath").toString();
    settings.endGroup();
    return DBPath;
}

int main(int argc, char *argv[])
{
    //qInstallMessageHandler(messageToFile);
    QApplication a(argc, argv);
    QString DBPath;
    if(LoadSettings() == ""){
        QMessageBox::information(nullptr, "", "Выберите ПУТЬ к файлу БД");
        DBPath = QFileDialog::getExistingDirectory(nullptr, "Выберите путь к базе данных", "C:\\") + "/clients.db";
        SaveSettings(DBPath);
    } else {
        DBPath = LoadSettings();
    }
    database::connectToDataBase(DBPath);
    qApp->setStyleSheet("QMessageBox QPushButton{"
                                "color: white;"
                                "background-color:rgb(91,95,199);"
                                "border-radius:3px;"
                                "min-width: 60px;"
                                "min-height: 20px;"
                        "}"
                        "QMessageBox QPushButton:hover{"
                                "color: white;"
                                "background-color:rgb(68,71,145);"
                                "border-radius:3px;"
                        "}"
                        "QMessageBox{"
                                "background-color:white;"
                                "color:black;"
                        "}"
                        "QMessageBox > QLabel{"
                                "color:black"
                        "}");
    MainWindow w;
    w.setWindowState(Qt::WindowMaximized);
    w.display();
    a.exec();
}
