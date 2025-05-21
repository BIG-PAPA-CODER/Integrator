#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QTimer>
#include <QString>
#include <QDebug>
#include <QFile>
#include <QPainter>
#include "camerawindow.h"
#include "dialog.h"
#include <time.h>
#include <QCamera>
#include <QMediaCaptureSession>
#include <QVideoWidget>
#include <QCameraDevice>  // For the camera device class
#include "datatabledialog.h"
#include "table.h"
#include "table_termo.h"
#include <QTranslator>

// class comapre;
// class DataDisplay;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    int sensor = 0;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    int flag = 1;

    void setSensor(int i){sensor = i;}
    int getSensor(){return sensor;}
    // void toggleCamera();
    // void adjustTransparency(int value);
    //uint16_t calculateCRC16(uint16_t *data, int length);



private slots:
    void read_Data();
    void save_file();
    void on_trybWybor_activated(int index);
    void get_path();
    void on_actionPo_cz_triggered();
    void on_actionRoz_cz_triggered();

    void on_zamnkij_clicked();

    void on_zapisPlik_clicked();

    void on_rozpocznijZapis_clicked();
    void showDataTable();
    void showDataTableTermo();

    void switchLanguage(const QString &langCode);
    void on_languageComboBox_activated(int index);

    //void on_compareButton_clicked();

    //void on_compare_clicked();

private:
    Ui::MainWindow *ui;
    QSerialPort* Port;
    QString Data_From_Port;
    QStringList list;
    bool Is_Data_Received = false;
    QTimer *timer;
    Dialog *dialog;
    //QCamera *camera;
    QMediaCaptureSession *captureSession;
    QVideoWidget *videoWidget;
    //CameraWindow *cameraWindow;
    // DataDisplay* m_dataDisplay;
    //DataTableDialog *dataTableDialog;
    table *m_table;
    table_termo *m_table_termo;
    //CameraWidget *m_camera;
    QTranslator translator;
    QString currentLanguage = "en";

    unsigned short int ComputeCRC16(const char* pData, int Length, unsigned int Poly, unsigned short int InitVal)
    {
        short int i;
        unsigned short int ResCRC = InitVal;

        while (--Length >= 0) {
            ResCRC ^= *pData++ << 8;
            for (short int i=0; i < 8; ++i)
                ResCRC = ResCRC & 0x8000 ? (ResCRC << 1) ^ Poly : ResCRC << 1;
        }
        return ResCRC & 0xFFFF;
    }
};
#endif // MAINWINDOW_H
