#ifndef TABLE_TERMO_H
#define TABLE_TERMO_H

#include "camerawidget.h"
#include <QMainWindow>

namespace Ui {
class table_termo;
}

class table_termo : public QMainWindow
{
    Q_OBJECT
    int sensor = 0;

public:
    explicit table_termo(QWidget *parent = nullptr);
    ~table_termo();

//private:
    void clearTables(); // Clears both tables
    void updateTable_1(int row, int col, int value); // Updates a cell in table_1
    void updateTable_2(int row, int col, int value); // Updates a cell in table_2
    //void on_trybWybor_activated(int index);
    void updateChart();
    void calculateMaxError_1(int row, int col, int value, bool useMSE);
    void calculateMaxError_2(int row, int col, int value, bool useMSE);
    void updateBothTablesAfterMeasurement();



    void setSensor(int i){sensor = i;}
    int getSensor(){return sensor;}

    //private:

    Ui::table_termo *ui;
    CameraWidget *cameraWidget;

    bool isMeasurementSeriesActive = false; // To track if a measurement series is active
    int currentMeasurementCount1 = 0;        // To track the current count of measurements
    int currentMeasurementCount2 = 0;        // To track the current count of measurements
    int customMeasurementCount = 5;
    QString selectedErrorMetric = "Maximum Error"; // Default error metric

    bool table1Completed = false;    // Flag for table_1 completion
    bool table2Completed = false;    // Flag for table_2 completion
    int measurementsTable1[24][32][50] = {{{0}}};
    int measurementsTable2[8][8][50] = {{{0}}};

    int minValueSensor1 = INT_MAX;
    int minValueSensor2 = INT_MAX;


public slots:
    void showImageClicked();
    //void GaussClicked();
    void ValuesClicked();
    void startMeasurementSeries();
    void setCustomMeasurementCount(int count);
    void setErrorMetric(const QString &metric);
    void saveMeasurementSeries();
    void cameraClicked();
};

#endif // TABLE_TERMO_H
