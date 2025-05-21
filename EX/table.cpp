/**
 * @file table.cpp
 * @brief Implementation of the `table` class for managing UI interactions, sensor data processing, and visualization.
 *
 * This file implements the `table` class, which handles user interactions, sensor data visualization, error calculations,
 * and table updates for two sensors. It also manages measurement series and data saving.
 */

#include "table.h"
#include "ui_table.h"

#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDateTime>

// #include <QtCharts/QChart>
// #include <QtCharts/QChartView>
// #include <QtCharts/QLineSeries>
// #include <QtCharts/QValueAxis>
// #include <QTimer>

/**
 * @class table
 * @brief A dialog-based class for managing tables, camera widgets, and sensor data visualization.
 *
 * The `table` class provides functionality for:
 * - Managing and updating tables for sensor data.
 * - Calculating metrics like Maximum Error and Mean Squared Error (MSE).
 * - Saving measurement series data.
 * - Managing camera widget controls.
 */

/**
 * @brief Constructs a `table` object.
 *
 * Initializes the UI, camera widgets, and sets up necessary signal-slot connections.
 *
 * @param parent Pointer to the parent widget. Default is nullptr.
 */

table::table(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::table),
    cameraWidget(new CameraWidget())
{
    ui->setupUi(this);

    // ui->cameraWidget->stackUnder(ui->mainWidget);
    // ui->cameraWidget->stackUnder(ui->mainWidget_2);
    // ui->cameraWidget->stackUnder(ui->mainWidget_3);
    // ui->cameraWidget->stackUnder(ui->mainWidget_4);
    // ui->cameraWidget->stackUnder(ui->mainWidget_5);
    // ui->cameraWidget->stackUnder(ui->mainWidget_6);

    // Ensure cameraWidget is always visible
    ui->cameraWidget->show();
    ui->cameraWidget_2->show();
    //ui->cameraWidget->lower();

    connect(ui->showImage, &QPushButton::clicked, this, &table::showImageClicked);
    connect(ui->errorMetricComboBox, SIGNAL(currentTextChanged(const QString &)), this, SLOT(setErrorMetric(const QString &)));
    //connect(ui->pushButton_3, &QPushButton::clicked, this, &table::startMeasurementSeries);
    // connect(ui->transparency, &QSlider::valueChanged, ui->mainWidget, &DataDisplay::setTransparency);
    // connect(ui->transparency, &QSlider::valueChanged, ui->mainWidget_2, &DataDisplay::setTransparency);
    // connect(ui->transparency, &QSlider::valueChanged, ui->mainWidget_4, &DataDisplayText::setTransparency);
    // connect(ui->transparency, &QSlider::valueChanged, ui->mainWidget_6, &DataDisplayText::setTransparency);
    // connect(ui->transparency, &QSlider::valueChanged, ui->mainWidget_3, &Gauss::setTransparency);
    // connect(ui->transparency, &QSlider::valueChanged, ui->mainWidget_5, &Gauss::setTransparency);

    connect(ui->zoomInButton, &QPushButton::clicked, ui->cameraWidget, &CameraWidget::zoomIn);
    connect(ui->zoomOutButton, &QPushButton::clicked, ui->cameraWidget, &CameraWidget::zoomOut);
    connect(ui->panLeftButton, &QPushButton::clicked, ui->cameraWidget, &CameraWidget::moveLeft);
    connect(ui->panRightButton, &QPushButton::clicked, ui->cameraWidget, &CameraWidget::moveRight);
    connect(ui->panUpButton, &QPushButton::clicked, ui->cameraWidget, &CameraWidget::moveUp);
    connect(ui->panDownButton, &QPushButton::clicked, ui->cameraWidget, &CameraWidget::moveDown);

    connect(ui->zoomInButton_2, &QPushButton::clicked, ui->cameraWidget_2, &CameraWidget::zoomIn);
    connect(ui->zoomOutButton_2, &QPushButton::clicked, ui->cameraWidget_2, &CameraWidget::zoomOut);
    connect(ui->panLeftButton_2, &QPushButton::clicked, ui->cameraWidget_2, &CameraWidget::moveLeft);
    connect(ui->panRightButton_2, &QPushButton::clicked, ui->cameraWidget_2, &CameraWidget::moveRight);
    connect(ui->panUpButton_2, &QPushButton::clicked, ui->cameraWidget_2, &CameraWidget::moveUp);
    connect(ui->panDownButton_2, &QPushButton::clicked, ui->cameraWidget_2, &CameraWidget::moveDown);

}

/**
 * @brief Destroys the `table` object.
 */

table::~table()
{
    delete ui;
}

/**
 * @brief Clears both tables and resets their dimensions.
 */

void table::clearTables() {
    ui->table_1->clear();
    ui->table_1->setRowCount(8);
    ui->table_1->setColumnCount(8);
    ui->table_1->setVisible(false);

    ui->table_2->clear();
    ui->table_2->setRowCount(8);
    ui->table_2->setColumnCount(8);
    ui->table_2->setVisible(false);

    // measurementCount = 0;
    // memset(measurementsTable1, 0, sizeof(measurementsTable1));
    // memset(measurementsTable2, 0, sizeof(measurementsTable2));
}

/**
 * @brief Updates the first table and calculates the minimum value and column means.
 *
 * @param row The row index to update.
 * @param col The column index to update.
 * @param value The value to set in the table cell.
 */

void table::updateTable_1(int row, int col, int value) {
    if (row >= 0 && row < ui->table_1->rowCount() && col >= 0 && col < ui->table_1->columnCount()) {
        ui->table_1->setItem(row, col, new QTableWidgetItem(QString::number(value)));
        ui->mainWidget->setSensor(1);  // data input from mainwindow
        ui->mainWidget_4->setSensor(1);
        ui->mainWidget_3->setSensor(1); // data input from mainwindow
        //ui->widget->setSensor(1);
        minValueSensor1 = INT_MAX;

        // Find the minimum value in the entire table
        for (int r = 0; r < 8; ++r) {
            for (int c = 0; c < 8; ++c) {
                if (ui->table_1->item(r, c)) {
                    int currentValue = ui->table_1->item(r, c)->text().toInt();
                    minValueSensor1 = qMin(minValueSensor1, currentValue);
                }
            }
        }

        // Update the UI field with the minimum value
        ui->minValueSensor1->setText(QString::number(minValueSensor1));

        int columnMeans[8] = {0};
        int rowCount = ui->table_1->rowCount();

        for (int c = 0; c < 8; ++c) {
            int sum = 0;
            int validCount = 0; // Counter for valid items

            for (int r = 0; r < rowCount; ++r) {
                if (ui->table_1->item(r, c)) {
                    sum += ui->table_1->item(r, c)->text().toInt();
                    ++validCount;
                }
            }

            // Calculate integer mean if there are valid items
            if (validCount > 0) {
                columnMeans[c] = sum / validCount; // Integer division
            }

        }
        // Update the chart with the new column means as integers
        ui->chart->updateChart(columnMeans);
    }
}

/**
 * @brief Updates the second table and calculates the minimum value and column means.
 *
 * @param row The row index to update.
 * @param col The column index to update.
 * @param value The value to set in the table cell.
 */

void table::updateTable_2(int row, int col, int value) {
    if (row >= 0 && row < ui->table_2->rowCount() && col >= 0 && col < ui->table_2->columnCount()) {
        ui->table_2->setItem(row, col, new QTableWidgetItem(QString::number(value)));
        ui->mainWidget_2->setSensor(2); // data input from mainwindow
        ui->mainWidget_6->setSensor(2);
        ui->mainWidget_5->setSensor(2); // data input from mainwindow

        // Reset minValueSensor2 to INT_MAX before scanning the entire table
        minValueSensor2 = INT_MAX;

        // Find the minimum value in the entire table
        for (int r = 0; r < 8; ++r) {
            for (int c = 0; c < 8; ++c) {
                if (ui->table_2->item(r, c)) {
                    int currentValue = ui->table_2->item(r, c)->text().toInt();
                    minValueSensor2 = qMin(minValueSensor2, currentValue);
                }
            }
        }

        // Update the UI field with the minimum value
        ui->minValueSensor2->setText(QString::number(minValueSensor2));

        int columnMeans[8] = {0};
        int rowCount = ui->table_2->rowCount();

        for (int c = 0; c < 8; ++c) {
            int sum = 0;
            int validCount = 0; // Counter for valid items

            for (int r = 0; r < rowCount; ++r) {
                if (ui->table_2->item(r, c)) {
                    sum += ui->table_2->item(r, c)->text().toInt();
                    ++validCount;
                }
            }

            // Calculate integer mean if there are valid items
            if (validCount > 0) {
                columnMeans[c] = sum / validCount; // Integer division
            }
        }

        // Update the chart with the new column means as integers
        ui->chart_2->updateChart(columnMeans);
    }
}

/**
 * @brief Calculates the maximum error or Mean Squared Error (MSE) for sensor 1.
 *
 * @param row The row index in the table.
 * @param col The column index in the table.
 * @param value The new measurement value.
 * @param useMSE Flag indicating whether to calculate MSE or max error.
 */

void table::calculateMaxError_1(int row, int col, int value, bool useMSE) {
    //static int measurementsTable1[8][8][50] = {{{0}}};
    static int measurementCountTable1[8][8] = {{0}};

    if (row >= 0 && row < 8 && col >= 0 && col < 8 && isMeasurementSeriesActive) {
        int count = measurementCountTable1[row][col];

        if (count < customMeasurementCount) {
            measurementsTable1[row][col][count] = value;
            measurementCountTable1[row][col]++;

            // Update progress bar after each measurement
            ui->progressBar->setValue(ui->progressBar->value() + 1);
        }

        if (measurementCountTable1[row][col] == customMeasurementCount) {
            // Calculate the error (MSE or Maximum Error)
            int errorValue = 0.0;
            if (useMSE) {
                double mean = 0.0;
                for (int i = 0; i < customMeasurementCount; ++i) {
                    mean += measurementsTable1[row][col][i];
                }
                mean /= customMeasurementCount;

                double mse = 0.0;
                for (int i = 0; i < customMeasurementCount; ++i) {
                    double diff = measurementsTable1[row][col][i] - mean;
                    mse += diff * diff;
                }
                mse /= customMeasurementCount;
                errorValue = static_cast<int>(mse); // Cast to int for table display
            } else {
                int minVal = measurementsTable1[row][col][0];
                int maxVal = measurementsTable1[row][col][0];
                for (int i = 1; i < customMeasurementCount; ++i) {
                    minVal = qMin(minVal, measurementsTable1[row][col][i]);
                    maxVal = qMax(maxVal, measurementsTable1[row][col][i]);
                }
                errorValue = maxVal - minVal;
            }

            ui->table_3->setItem(row, col, new QTableWidgetItem(QString::number(errorValue)));
            measurementCountTable1[row][col] = 0;

            currentMeasurementCount1++;
            ui->progressBar->setValue(ui->progressBar->value() + customMeasurementCount); // Update progress

            if (currentMeasurementCount1 >= 64){
                table1Completed = true;
                updateBothTablesAfterMeasurement();
            }
        }
    }
}

/**
 * @brief Calculates the maximum error or Mean Squared Error (MSE) for sensor 2.
 *
 * @param row The row index in the table.
 * @param col The column index in the table.
 * @param value The new measurement value.
 * @param useMSE Flag indicating whether to calculate MSE or max error.
 */

void table::calculateMaxError_2(int row, int col, int value, bool useMSE) {
    //static int measurementsTable2[8][8][50] = {{{0}}};
    static int measurementCountTable2[8][8] = {{0}};

    if (row >= 0 && row < 8 && col >= 0 && col < 8 && isMeasurementSeriesActive) {
        int count = measurementCountTable2[row][col];

        if (count < customMeasurementCount) {
            measurementsTable2[row][col][count] = value;
            measurementCountTable2[row][col]++;

            // Update progress bar after each measurement
            ui->progressBar->setValue(ui->progressBar->value() + 1);
        }

        if (measurementCountTable2[row][col] == customMeasurementCount) {
            // Calculate the error (MSE or Maximum Error)
            int errorValue = 0.0;
            if (useMSE) {
                double mean = 0.0;
                for (int i = 0; i < customMeasurementCount; ++i) {
                    mean += measurementsTable2[row][col][i];
                }
                mean /= customMeasurementCount;

                double mse = 0.0;
                for (int i = 0; i < customMeasurementCount; ++i) {
                    double diff = measurementsTable2[row][col][i] - mean;
                    mse += diff * diff;
                }
                //errorValue = mse / customMeasurementCount;
                errorValue = static_cast<int>(mse / customMeasurementCount); // Cast to int for table display
            } else {
                int minVal = measurementsTable2[row][col][0];
                int maxVal = measurementsTable2[row][col][0];
                for (int i = 1; i < customMeasurementCount; ++i) {
                    minVal = qMin(minVal, measurementsTable2[row][col][i]);
                    maxVal = qMax(maxVal, measurementsTable2[row][col][i]);
                }
                errorValue = maxVal - minVal;
            }

            ui->table_4->setItem(row, col, new QTableWidgetItem(QString::number(errorValue)));
            measurementCountTable2[row][col] = 0;

            currentMeasurementCount2++;
            ui->progressBar->setValue(ui->progressBar->value() + customMeasurementCount); // Update progress

            if (currentMeasurementCount2 >= 64){
                table2Completed = true;
                updateBothTablesAfterMeasurement();
            }
        }
    }
}

/**
 * @brief Updates both tables after completing a measurement series.
 *
 * This method calculates the sum of errors for both sensors and updates the UI elements
 * accordingly. It also resets flags and counters for the next measurement series.
 */

void table::updateBothTablesAfterMeasurement() {
    if (table1Completed && table2Completed) {
        //qDebug() << "Measurement series completed. Updating both tables.";

        // Reset flags and counters
        // table1Completed = false;
        // table2Completed = false;
        isMeasurementSeriesActive = false;
        ui->pushButton_3->setText("Start Series");

        ui->saveMeasurementSeriesButton->setEnabled(true);
        //qDebug() << "Measurement series stopped after one complete set of" << customMeasurementCount << "measurements.";

        // Calculate the sum of errors for Sensor 1
        int sumMaxError1 = 0, sumMSE1 = 0;
        int sumMaxError2 = 0, sumMSE2 = 0;

        // Iterate over each cell in the table and sum the errors
        for (int row = 0; row < 8; ++row) {
            for (int col = 0; col < 8; ++col) {
                // Sensor 1
                int errorValue1 = ui->table_3->item(row, col)->text().toInt();
                sumMaxError1 += errorValue1;

                // Sensor 2
                int errorValue2 = ui->table_4->item(row, col)->text().toInt();
                sumMaxError2 += errorValue2;
            }
        }

        // Check if MSE is chosen and update the sum fields
        if (selectedErrorMetric == "Mean Squared Error") {
            sumMSE1 = sumMaxError1; // In this context, sumMaxError1 holds the MSE sum when MSE is chosen
            sumMSE2 = sumMaxError2;
            ui->sumErrorSensor1->setText(QString::number(sumMSE1));
            ui->sumErrorSensor2->setText(QString::number(sumMSE2));
        } else {
            ui->sumErrorSensor1->setText(QString::number(sumMaxError1));
            ui->sumErrorSensor2->setText(QString::number(sumMaxError2));
        }

        // Reset the progress bar after completion
        ui->progressBar->setValue(0);


        // int minValueSensor1 = INT_MAX;
        // int minValueSensor2 = INT_MAX;

        // // Iterate through measurements for Sensor 1 (table_1)
        // for (int measurementIndex = 0; measurementIndex < customMeasurementCount; ++measurementIndex) {
        //     for (int row = 0; row < 8; ++row) {
        //         for (int col = 0; col < 8; ++col) {
        //             int value = measurementsTable1[row][col][measurementIndex];
        //             minValueSensor1 = qMin(minValueSensor1, value);
        //         }
        //     }
        // }

        // // Iterate through measurements for Sensor 2 (table_2)
        // for (int measurementIndex = 0; measurementIndex < customMeasurementCount; ++measurementIndex) {
        //     for (int row = 0; row < 8; ++row) {
        //         for (int col = 0; col < 8; ++col) {
        //             int value = measurementsTable2[row][col][measurementIndex];
        //             minValueSensor2 = qMin(minValueSensor2, value);
        //         }
        //     }
        // }

        // // Update the UI fields with the minimum values
        // ui->minValueSensor1->setText(QString::number(minValueSensor1));
        // ui->minValueSensor2->setText(QString::number(minValueSensor2));
    }
}

/**
 * @brief Starts or stops a measurement series.
 *
 * This method toggles the measurement series activity state, resets counters,
 * and updates the UI elements such as buttons and progress bar.
 */

void table::startMeasurementSeries() {
    if (!isMeasurementSeriesActive) {
        isMeasurementSeriesActive = true;
        currentMeasurementCount1 = 0;
        currentMeasurementCount2 = 0;
        table1Completed = false;
        table2Completed = false;
        //qDebug() << "Starting measurement series. Collecting" << customMeasurementCount << "measurements.";
        ui->pushButton_3->setText("Stop Series");
        ui->saveMeasurementSeriesButton->setEnabled(false);
    } else {
        isMeasurementSeriesActive = false;
        //qDebug() << "Measurement series stopped by user.";
        ui->pushButton_3->setText("Start Series");
    }
}

/**
 * @brief Sets the custom measurement count for the series.
 *
 * Updates the internal counter and adjusts the progress bar range accordingly.
 * @param count The number of measurements in the series.
 */

void table::setCustomMeasurementCount(int count) {
    if (count >= 1) {
        customMeasurementCount = count;
        //qDebug() << "Custom measurement count set to:" << customMeasurementCount;
        // progress bar implementation
        ui->progressBar->setValue(0);
        ui->progressBar->setRange(0, customMeasurementCount * 64 * 2); // Total measurements for both tables
        ui->progressBar->setValue(0); // Reset progress to 0
    }
}

/**
 * @brief Sets the error metric for the measurement calculations (MSE or Max error).
 *
 * @param metric The selected error metric as a string.
 */

void table::setErrorMetric(const QString &metric) {
    selectedErrorMetric = metric;
    //qDebug() << "Error metric set to:" << selectedErrorMetric;
}

// void table::saveMeasurementSeries() {
//     if (!table1Completed || !table2Completed) {
//         qDebug() << "Measurement series not completed. Cannot save.";
//         return;
//     }

//     // Prompt user to select the save location and filename
//     QString fileName = QFileDialog::getSaveFileName(this, "Save Measurement Series", "", "Text Files (*.txt)");
//     if (fileName.isEmpty()) {
//         return;
//     }

//     QFile file(fileName);
//     if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
//         qDebug() << "Failed to open file for writing.";
//         return;
//     }

//     QTextStream out(&file);

//     // Write measurement series for sensor 1
//     out << "Sensor 1 Measurements:\n";
//     for (int measurementIndex = 0; measurementIndex < customMeasurementCount; ++measurementIndex) {
//         out << "Measurement_" << (measurementIndex + 1) << ":\n";
//         for (int row = 0; row < 8; ++row) {
//             for (int col = 0; col < 8; ++col) {
//                 out << measurementsTable1[row][col][measurementIndex] << "\t";
//             }
//             out << "\n";
//         }
//         out << "\n";
//     }

//     // Write measurement series for sensor 2
//     out << "Sensor 2 Measurements:\n";
//     for (int measurementIndex = 0; measurementIndex < customMeasurementCount; ++measurementIndex) {
//         out << "Measurement_" << (measurementIndex + 1) << ":\n";
//         for (int row = 0; row < 8; ++row) {
//             for (int col = 0; col < 8; ++col) {
//                 out << measurementsTable2[row][col][measurementIndex] << "\t";
//             }
//             out << "\n";
//         }
//         out << "\n";
//     }

//     // Write the error table for sensor 1 (from table_3)
//     out << "Sensor 1 Error Table (Max Error or MSE):\n";
//     int sensor1ErrorSum = 0;
//     for (int row = 0; row < 8; ++row) {
//         for (int col = 0; col < 8; ++col) {
//             int errorValue = ui->table_3->item(row, col)->text().toInt();
//             out << errorValue << "\t";
//             sensor1ErrorSum += errorValue;
//         }
//         out << "\n";
//     }
//     out << "Sum of Errors for Sensor 1: " << sensor1ErrorSum << "\n\n";

//     // Write the error table for sensor 2 (from table_4)
//     out << "Sensor 2 Error Table (Max Error or MSE):\n";
//     int sensor2ErrorSum = 0;
//     for (int row = 0; row < 8; ++row) {
//         for (int col = 0; col < 8; ++col) {
//             int errorValue = ui->table_4->item(row, col)->text().toInt();
//             out << errorValue << "\t";
//             sensor2ErrorSum += errorValue;
//         }
//         out << "\n";
//     }
//     out << "Sum of Errors for Sensor 2: " << sensor2ErrorSum << "\n";

//     file.close();
//     qDebug() << "Measurement series saved to:" << fileName;
// }

// void table::saveMeasurementSeries() {
//     if (!table1Completed || !table2Completed) {
//         qDebug() << "Measurement series not completed. Cannot save.";
//         return;
//     }

//     QString fileName = QFileDialog::getSaveFileName(this, "Save Measurement Series", "", "Text Files (*.txt)");
//     if (fileName.isEmpty()) {
//         return;
//     }

//     QFile file(fileName);
//     if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
//         qDebug() << "Failed to open file for writing.";
//         return;
//     }

//     QTextStream out(&file);

//     // Function to calculate Maximum Error
//     auto calculateMaxError = [&](int sensor, int row, int col) {
//         int minVal = (sensor == 1) ? measurementsTable1[row][col][0] : measurementsTable2[row][col][0];
//         int maxVal = (sensor == 1) ? measurementsTable1[row][col][0] : measurementsTable2[row][col][0];
//         for (int i = 1; i < customMeasurementCount; ++i) {
//             if (sensor == 1) {
//                 minVal = qMin(minVal, measurementsTable1[row][col][i]);
//                 maxVal = qMax(maxVal, measurementsTable1[row][col][i]);
//             } else {
//                 minVal = qMin(minVal, measurementsTable2[row][col][i]);
//                 maxVal = qMax(maxVal, measurementsTable2[row][col][i]);
//             }
//         }
//         return maxVal - minVal;
//     };

//     // Function to calculate MSE
//     auto calculateMSE = [&](int sensor, int row, int col) {
//         double mean = 0.0;
//         for (int i = 0; i < customMeasurementCount; ++i) {
//             if (sensor == 1) {
//                 mean += measurementsTable1[row][col][i];
//             } else {
//                 mean += measurementsTable2[row][col][i];
//             }
//         }
//         mean /= customMeasurementCount;

//         double mse = 0.0;
//         for (int i = 0; i < customMeasurementCount; ++i) {
//             double diff = (sensor == 1) ? measurementsTable1[row][col][i] - mean : measurementsTable2[row][col][i] - mean;
//             mse += diff * diff;
//         }
//         return static_cast<int>(mse / customMeasurementCount);
//     };

//     int sumMaxError1 = 0, sumMSE1 = 0;
//     int sumMaxError2 = 0, sumMSE2 = 0;

//     // Write measurements for Sensor 1
//     out << "Sensor 1 Measurements:\n";
//     for (int measurementIndex = 0; measurementIndex < customMeasurementCount; ++measurementIndex) {
//         out << "Measurement_" << (measurementIndex + 1) << ":\n";
//         for (int row = 0; row < 8; ++row) {
//             for (int col = 0; col < 8; ++col) {
//                 out << measurementsTable1[row][col][measurementIndex] << "\t";
//             }
//             out << "\n";
//         }
//         out << "\n";
//     }

//     // Write Maximum Error and MSE for Sensor 1
//     out << "Sensor 1 Maximum Error Table:\n";
//     for (int row = 0; row < 8; ++row) {
//         for (int col = 0; col < 8; ++col) {
//             int maxError = calculateMaxError(1, row, col);
//             out << maxError << "\t";
//             sumMaxError1 += maxError;
//         }
//         out << "\n";
//     }
//     out << "\n";

//     out << "Sensor 1 MSE Table:\n";
//     for (int row = 0; row < 8; ++row) {
//         for (int col = 0; col < 8; ++col) {
//             int mse = calculateMSE(1, row, col);
//             out << mse << "\t";
//             sumMSE1 += mse;
//         }
//         out << "\n";
//     }
//     out << "\n";

//     out << "Sensor 1 Sum of Maximum Errors: " << sumMaxError1 << "\n";
//     out << "Sensor 1 Sum of MSE: " << sumMSE1 << "\n\n";

//     // Write measurements for Sensor 2
//     out << "Sensor 2 Measurements:\n";
//     for (int measurementIndex = 0; measurementIndex < customMeasurementCount; ++measurementIndex) {
//         out << "Measurement_" << (measurementIndex + 1) << ":\n";
//         for (int row = 0; row < 8; ++row) {
//             for (int col = 0; col < 8; ++col) {
//                 out << measurementsTable2[row][col][measurementIndex] << "\t";
//             }
//             out << "\n";
//         }
//         out << "\n";
//     }

//     // Write Maximum Error and MSE for Sensor 2
//     out << "Sensor 2 Maximum Error Table:\n";
//     for (int row = 0; row < 8; ++row) {
//         for (int col = 0; col < 8; ++col) {
//             int maxError = calculateMaxError(2, row, col);
//             out << maxError << "\t";
//             sumMaxError2 += maxError;
//         }
//         out << "\n";
//     }
//     out << "\n";

//     out << "Sensor 2 MSE Table:\n";
//     for (int row = 0; row < 8; ++row) {
//         for (int col = 0; col < 8; ++col) {
//             int mse = calculateMSE(2, row, col);
//             out << mse << "\t";
//             sumMSE2 += mse;
//         }
//         out << "\n";
//     }
//     out << "\n";

//     out << "Sensor 2 Sum of Maximum Errors: " << sumMaxError2 << "\n";
//     out << "Sensor 2 Sum of MSE: " << sumMSE2 << "\n";

//     file.close();
//     qDebug() << "Measurement series with Max Error and MSE saved to:" << fileName;
// }

/**
 * @brief Saves the measurement series data to files.
 */

void table::saveMeasurementSeries() {
    if (!table1Completed || !table2Completed) {
        qDebug() << "Measurement series not completed. Cannot save.";
        return;
    }

    // Create a directory with the current date and time
    QString dirName = "data-" + QDateTime::currentDateTime().toString("yyyy-MM-dd_hh_mm");
    QDir().mkdir(dirName);

    // Lambda to calculate Maximum Error
    auto calculateMaxError = [&](int sensor, int row, int col) {
        int minVal = (sensor == 1) ? measurementsTable1[row][col][0] : measurementsTable2[row][col][0];
        int maxVal = (sensor == 1) ? measurementsTable1[row][col][0] : measurementsTable2[row][col][0];
        for (int i = 1; i < customMeasurementCount; ++i) {
            if (sensor == 1) {
                minVal = qMin(minVal, measurementsTable1[row][col][i]);
                maxVal = qMax(maxVal, measurementsTable1[row][col][i]);
            } else {
                minVal = qMin(minVal, measurementsTable2[row][col][i]);
                maxVal = qMax(maxVal, measurementsTable2[row][col][i]);
            }
        }
        return maxVal - minVal;
    };

    // Lambda to calculate MSE
    auto calculateMSE = [&](int sensor, int row, int col) {
        double mean = 0.0;
        for (int i = 0; i < customMeasurementCount; ++i) {
            if (sensor == 1) {
                mean += measurementsTable1[row][col][i];
            } else {
                mean += measurementsTable2[row][col][i];
            }
        }
        mean /= customMeasurementCount;

        double mse = 0.0;
        for (int i = 0; i < customMeasurementCount; ++i) {
            double diff = (sensor == 1) ? measurementsTable1[row][col][i] - mean : measurementsTable2[row][col][i] - mean;
            mse += diff * diff;
        }
        return static_cast<int>(mse / customMeasurementCount);
    };

    // Save measurements and errors for Sensor 1
    QString sensor1FileName = dirName + "/vl53l5cx-1.dat";
    QFile sensor1File(sensor1FileName);
    if (sensor1File.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&sensor1File);

        out << "M1:S1 Sensor 1 Measurements:\n";
        for (int measurementIndex = 0; measurementIndex < customMeasurementCount; ++measurementIndex) {
            out << "M1:I" << (measurementIndex + 1) << " Measurement_" << (measurementIndex + 1) << ":\n";
            for (int row = 0; row < 8; ++row) {
                out << "M1:I" << (measurementIndex + 1) << ":R" << (row + 1) << " ";
                for (int col = 0; col < 8; ++col) {
                    out << measurementsTable1[row][col][measurementIndex] << "\t";
                }
                out << "\n";
            }
            out << "\n";
        }

        // Write Maximum Error table
        out << "M1:E1 Max Error Table:\n";
        for (int row = 0; row < 8; ++row) {
            out << "M1:E1:R" << (row + 1) << " ";
            for (int col = 0; col < 8; ++col) {
                out << calculateMaxError(1, row, col) << "\t";
            }
            out << "\n";
        }
        out << "\n";

        // Write MSE table
        out << "M1:MSE1 MSE Table:\n";
        for (int row = 0; row < 8; ++row) {
            out << "M1:MSE1:R" << (row + 1) << " ";
            for (int col = 0; col < 8; ++col) {
                out << calculateMSE(1, row, col) << "\t";
            }
            out << "\n";
        }
        out << "\n";

        sensor1File.close();
        qDebug() << "Sensor 1 data saved to:" << sensor1FileName;
    } else {
        qDebug() << "Failed to open file for Sensor 1.";
    }

    // Save measurements and errors for Sensor 2
    QString sensor2FileName = dirName + "/vl53l5cx-2.dat";
    QFile sensor2File(sensor2FileName);
    if (sensor2File.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&sensor2File);

        out << "M1:S2 Sensor 2 Measurements:\n";
        for (int measurementIndex = 0; measurementIndex < customMeasurementCount; ++measurementIndex) {
            out << "M1:I" << (measurementIndex + 1) << " Measurement_" << (measurementIndex + 1) << ":\n";
            for (int row = 0; row < 8; ++row) {
                out << "M1:I" << (measurementIndex + 1) << ":R" << (row + 1) << " ";
                for (int col = 0; col < 8; ++col) {
                    out << measurementsTable2[row][col][measurementIndex] << "\t";
                }
                out << "\n";
            }
            out << "\n";
        }

        // Write Maximum Error table
        out << "M1:E2 Max Error Table:\n";
        for (int row = 0; row < 8; ++row) {
            out << "M1:E2:R" << (row + 1) << " ";
            for (int col = 0; col < 8; ++col) {
                out << calculateMaxError(2, row, col) << "\t";
            }
            out << "\n";
        }
        out << "\n";

        // Write MSE table
        out << "M1:MSE2 MSE Table:\n";
        for (int row = 0; row < 8; ++row) {
            out << "M1:MSE2:R" << (row + 1) << " ";
            for (int col = 0; col < 8; ++col) {
                out << calculateMSE(2, row, col) << "\t";
            }
            out << "\n";
        }
        out << "\n";

        sensor2File.close();
        qDebug() << "Sensor 2 data saved to:" << sensor2FileName;
    } else {
        qDebug() << "Failed to open file for Sensor 2.";
    }
}



void table::showImageClicked() {
    // bool isVisible = ui->mainWidget->isVisible();  // Check current visibility
    // ui->mainWidget->setVisible(!isVisible);        // Toggle visibility
    // ui->mainWidget_2->setVisible(!isVisible);      // Toggle visibility

    // bool isVisible = ui->cameraWidget->isVisible();  // Check current visibility
    // ui->cameraWidget->setVisible(!isVisible);        // Toggle visibility
}

/**
 * @brief Toggles the visibility of the main camera widget.
 */

void table::cameraClicked()
{
    bool isVisible = ui->cameraWidget->isVisible();  // Check current visibility
    ui->cameraWidget->setVisible(!isVisible);        // Toggle visibility
    ui->cameraWidget_2->setVisible(!isVisible);      // Toggle visibility
}

/**
 * @brief Toggles the visibility of Gaussian widgets.
 */

void table::GaussClicked() {
    bool isVisible = ui->mainWidget_3->isVisible();  // Check current visibility
    ui->mainWidget_3->setVisible(!isVisible);        // Toggle visibility
    ui->mainWidget_5->setVisible(!isVisible);      // Toggle visibility
}

/**
 * @brief Toggles the visibility of value display widgets.
 */

void table::ValuesClicked(){
    bool isVisible = ui->mainWidget_4->isVisible();  // Check current visibility
    ui->mainWidget_4->setVisible(!isVisible);        // Toggle visibility
    ui->mainWidget_6->setVisible(!isVisible);      // Toggle visibility
}
