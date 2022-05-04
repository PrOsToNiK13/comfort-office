#include "mainwindow.h"
#include "auth.h"
#include <QApplication>
#include <QtSql>
#include "database.h"

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

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //qInstallMessageHandler(messageToFile);
    database::connectToDataBase();
    auth authWindow;
    authWindow.show();
    a.exec();
}
