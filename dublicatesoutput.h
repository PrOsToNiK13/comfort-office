#ifndef DUBLICATESOUTPUT_H
#define DUBLICATESOUTPUT_H

#include <QDialog>
#include "QObject"

namespace Ui {
class DublicatesOutput;
}

class DublicatesOutput : public QDialog
{
    Q_OBJECT

public:
    explicit DublicatesOutput(QWidget *parent = nullptr);
    ~DublicatesOutput();

private slots:
    void on_pushButton_cancel_clicked();
    void on_pushButton_save_clicked();
    void ClearAllWidgets();

public slots:
    void updateInformation(QList<QList<QString>> findedPhoneDublicates, QList<QList<QString>> phonesWithDublicates);

private:
    Ui::DublicatesOutput *ui;

};

#endif // DUBLICATESOUTPUT_H
