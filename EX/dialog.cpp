#include "dialog.h"
#include "ui_dialog.h"


Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    QString sPath = "/home/grzegorz/Dokumenty";
    dirmodel = new QFileSystemModel(this);
    dirmodel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files);
    dirmodel->setRootPath(sPath);
    ui->treeView->setModel(dirmodel);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_treeView_clicked(const QModelIndex &index)
{
    filePath = dirmodel->fileInfo(index).absoluteFilePath();
    fileName = dirmodel->fileInfo(index).fileName();
}


void Dialog::on_zamknij_clicked()
{
    filePath = " ";
    fileName = " ";
    close();
}


void Dialog::on_wybierz_clicked()
{
    close();
}

