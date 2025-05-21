#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QFileSystemModel>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();
    QString filePath = " ";
    QString fileName = " ";

private slots:
    void on_treeView_clicked(const QModelIndex &index);

    void on_zamknij_clicked();

    void on_wybierz_clicked();

private:
    Ui::Dialog *ui;
    QFileSystemModel *dirmodel;
};

#endif // DIALOG_H
