/**
 * @file mainwindow.cpp
 * @brief Main application window handling UI interactions and hardware integration.
 *
 * This file implements the MainWindow class, which serves as the primary interface
 * between the user and the underlying sensor hardware. It provides functionalities such as:
 * - Managing serial port communication with hardware.
 * - Displaying data from connected sensors in real time.
 * - Switching between different sensor operation modes.
 * - Saving data to a file and managing file paths.
 * - Multilingual support for UI translation.
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_table.h"
#include "dialog.h"
#include "ui_table_termo.h"
#include <QMediaDevices>
#include <QDebug>
#include <camerawindow.h>
// #include "comapre.h"
// #include "datadisplay.h"

#include <QtEndian>

#define BILLION  1000000000L;
//#define CRC16 0x1021
#define CRC16_INIT 0
#define CRC16_POLYNOMIAL 0x8005

float temp[768];
QString path = " ";
int heightD = 20;
int widthD = 20;
char symbol = ' ';
double accum;
int measurements = 0;

struct timespec start, stop;

/**
 * @brief Constructs the MainWindow object.
 *
 * Initializes UI components, serial port communication, and dialogs for data visualization.
 *
 * @param parent Pointer to the parent widget.
 */

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
//    , m_dataDisplay(new DataDisplay())
    , m_table(new table(this)) // Initialize the dialog
    , m_table_termo(new table_termo(this)) // Initialize the dialog
    //m_camera(new CameraWidget())
{
    ui->setupUi(this);
    Port = new QSerialPort();
    Port->setPortName("/dev/ttyACM0");
    Port->setBaudRate(QSerialPort::BaudRate::Baud115200);
    Port->setParity(QSerialPort::Parity::NoParity);
    Port->setDataBits(QSerialPort::DataBits::Data8);
    Port->setStopBits(QSerialPort::StopBits::OneStop);
    Port->setFlowControl(QSerialPort::FlowControl::NoFlowControl);
    Port->open(QIODevice::ReadWrite);

    dialog = new Dialog(this);
    ui->dioda->setPixmap(QPixmap(":/img/diodaOff.png").scaled(widthD, heightD, Qt::KeepAspectRatio));
    ui->polaczenieDioda->setText("Brak połączenia z integratorem");

    // // Create camera window instance
    // cameraWindow = new CameraWindow(this); // Updated to use the new CameraWindow class
    // cameraWindow->hide(); // Ensure the window starts hidden

    // connect(ui->toggleCameraButton, &QPushButton::clicked, this, &MainWindow::toggleCamera);
    // connect(ui->transparencySlider, &QSlider::valueChanged, this, &MainWindow::adjustTransparency);
    ui->transparencySlider->setValue(100);  // Set initial value to fully opaque

    timer = new QTimer(this);
    timer->start(1100);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(get_path()));

    connect(ui->languageComboBox, QOverload<int>::of(&QComboBox::activated), this, &MainWindow::on_languageComboBox_activated);

    ui->toggleCameraButton->setVisible(false);
    ui->transparencySlider->setVisible(false);
}

/**
 * @brief Destructor for MainWindow.
 */

MainWindow::~MainWindow() {
    delete ui;
    //delete cameraWindow; // Clean up camera window on destruction
//    delete m_dataDisplay;
}

/**
 * @brief Handles language selection changes.
 * @param index Selected language index.
 */

void MainWindow::on_languageComboBox_activated(int index) {
    QString langCode;

    // Determine the language based on combo box index
    if (index == 0) {
        langCode = "en"; // English
    } else if (index == 1) {
        langCode = "pl"; // Polish
    }

    // Only switch if the selected language is different
    if (langCode != currentLanguage) {
        switchLanguage(langCode);
    }
}

/**
 * @brief Switches application language.
 * @param langCode Language code (e.g., "en" or "pl").
 */

void MainWindow::switchLanguage(const QString &langCode) {
    QString qmFile = "/home/vboxuser/EX/translations/app_" + langCode + ".qm";

    // Load the translation file
    if (translator.load(qmFile)) {
        qApp->installTranslator(&translator);

        // Retranslate UI
        ui->retranslateUi(this);
        m_table->ui->retranslateUi(m_table);
        m_table_termo->ui->retranslateUi(m_table_termo);

        currentLanguage = langCode; // Update current language
        qDebug() << "Language switched to:" << langCode;
    } else {
        qDebug() << "Failed to load translation file:" << qmFile;
    }
}

/**
 * @brief Displays the data table for VL sensors.
 */

void MainWindow::showDataTable() {
    m_table->clearTables();

    // Populate the table with VL1 data
    for (int i = 0; i < 64; ++i) {
        int row = i / 8; // Determine the row (0-7)
        int col = i % 8; // Determine the column (0-7)

        int value1 = ui->mainWidget->getVL1(i); // Get the value for the current index
        m_table->updateTable_1(row, col, value1); // Update the table with the row, column, and value

        int value2 = ui->mainWidget->getVL2(i); // Get the value for the current index
        m_table->updateTable_2(row, col, value2); // Update the table with the row, column, and value
    }

    m_table->show();
    //dataTableDialog->table->viewport()->update();
}

/**
 * @brief Displays the data table for termo sensors.
 */

void MainWindow::showDataTableTermo() {
    m_table_termo->clearTables();

    // Populate the table with VL1 data
    for (int i = 0; i < 768; ++i) {
        int row = i / 24; // Determine the row (0-7)
        int col = i % 32; // Determine the column (0-7)

        int value1 = ui->mainWidget->getMLX(i); // Get the value for the current index
        m_table_termo->updateTable_1(row, col, value1); // Update the table with the row, column, and value

        // int value2 = ui->mainWidget->getAMG(i); // Get the value for the current index
        // m_table_termo->updateTable_2(row, col, value2); // Update the table with the row, column, and value
    }

    m_table_termo->show();
    //dataTableDialog->table->viewport()->update();
}

/**
 * @brief Reads data from the serial port.
 */

void MainWindow::read_Data()
{
    if (Port->isOpen())
    {
        //qDebug() << "Port otwarty\n";
        while (Port->bytesAvailable())
        {
            Data_From_Port += Port->readAll();
            if (Data_From_Port.at(Data_From_Port.length() -1) == char(10))
            {
                Is_Data_Received = true;
            }
        }
        if (Is_Data_Received == true){
            list = Data_From_Port.split(" ");
            //dataTableDialog->updateTable(row, col, value);
            //int it = 0;

            bool ok;
            unsigned short int receivedCrc = list.at(list.size() - 2).toUInt(&ok, 16);

            // QByteArray data;
            // for (const QString &part : list) {
            //     data.append(part.toUtf8());
            // }

            QByteArray data;
            for (int i = 2; i < list.size() - 2; ++i) {
                uint16_t value = list[i].toUInt(); // Assuming values are 16-bit integers
                data.append(reinterpret_cast<const char*>(&value), sizeof(value));
            }

            // QByteArray rawData = Port->readAll();
            // rawData.chop(1); // Remove trailing newline character
            // unsigned short int calculatedCrc = ComputeCRC16(rawData.data(), rawData.size(), CRC16_POLYNOMIAL, CRC16_INIT);

            // Build QByteArray for numeric data
            // QByteArray data;
            // for (int i = 2; i < list.size() - 2; ++i) { // Skip the last two elements
            //     uint16_t value = list[i].toUInt(&ok);
            //     if (ok) {
            //         value = qToLittleEndian(value); // Convert to little-endian
            //         data.append(reinterpret_cast<const char*>(&value), sizeof(value));
            //     }
            // }


            // Compute CRC
            //unsigned short int calculatedCrc = ComputeCRC16(data.data(), data.size(), 0x1021, 0xFFFF);


            unsigned short int calculatedCrc = ComputeCRC16(data.data(), data.size(), CRC16_POLYNOMIAL, CRC16_INIT);

            // Validate CRC
            if (calculatedCrc == receivedCrc) {
                qDebug() << "CRC Match: Data is valid. Comp = " << calculatedCrc << ", Reci = " << receivedCrc;
                // Process data here
            } else {
                qDebug() << "CRC Mismatch: Data is corrupted. Comp = " << calculatedCrc << ", Reci = " << receivedCrc;
                qDebug() << "Received CRC String:" << receivedCrc;
                //qDebug() << "crc: " << list.at(66);
            }

            if( list.at(0) == 'X'){
                // uint16_t receivedCrc = list.last().toUInt(nullptr, 15); // Assuming CRC is the last element
                // list.removeLast(); // Remove CRC from the list to process data

                // // Compute CRC for received data
                // uint16_t computedCrc = calculateCRC16(reinterpret_cast<uint16_t *>(list.data()), list.size());

                // if (computedCrc == receivedCrc) {
                //     qDebug() << "CRC match. Processing data...";
                //     // Process the data
                // } else {
                //     qDebug() << "CRC mismatch. Data corrupted.";
                // }

                bool useMSE = (m_table->ui->errorMetricComboBox->currentText() == "Mean Squared Error");

                for (int i = 0; i < 64; i++){
                    ui->mainWidget->setVL1(63-i,list[i+2].toInt());
                    //qDebug() << "ZAPISANA: " << ui->mainWidget->getVL1(63-i);

                    m_table->ui->mainWidget->setVL1(63-i,list[i+2].toInt()); //data flow to datadisplay widget
                    m_table->ui->mainWidget_4->setVL1(63-i,list[i+2].toInt()); //data flow to datadispalytext widget
                    m_table->ui->mainWidget_3->setVL1(63-i,list[i+2].toInt()); //data flow to gauss widget
                    int row = i / 8; // Determine the row (0-7)
                    int col = i % 8; // Determine the column (0-7)
                    int value = ui->mainWidget->getVL1(i); // Get the value for the current index
                    m_table->updateTable_1(row, col, value); // Update the table with the row, column, and value
                    //m_table->calculateMaxError(row, col, value);

                    //if (m_table->isMeasurementSeriesActive) {
                    m_table->calculateMaxError_1(row, col, value, useMSE);
                    //}
                    //qDebug() << "ODCZYTANA: " << list[i+2].toInt();
                    //qDebug() << "test:" << dTab.tabVL1[63-i];

                }
                // // Increment the global measurement count
                // if (m_table->isMeasurementSeriesActive) {
                //     m_table->currentMeasurementCount++;

                //     // If 5 measurements have been completed, end the series
                //     if (m_table->currentMeasurementCount == 5) {
                //         m_table->isMeasurementSeriesActive = false;
                //         m_table->currentMeasurementCount = 0;
                //         qDebug() << "Measurement series completed. Table_3 updated.";
                //     }
                // }

                //Data_From_Port="";
                /*if(list.at(18) == crc_result){
                    qDebug() << "CRC zgadza się\n";
                    qDebug() << "Ramka odebrana\n";
                    Data_From_Port="";
                }*/

                Data_From_Port="";
            }
            else if( list.at(0) == 'Z'){
                bool useMSE = (m_table->ui->errorMetricComboBox->currentText() == "Mean Squared Error");

                for (int i = 0; i < 64; i++){
                    ui->mainWidget->setVL2(63-i,list[i+2].toInt());
                    //qDebug() << "ZAPISANA: " << ui->mainWidget->getVL2(63-i);

                    m_table->ui->mainWidget_2->setVL2(63-i,list[i+2].toInt());
                    m_table->ui->mainWidget_6->setVL2(63-i,list[i+2].toInt()); //data flow to datadispalytext widget
                    m_table->ui->mainWidget_5->setVL2(63-i,list[i+2].toInt()); //data flow to gauss widget
                    int row = i / 8; // Determine the row (0-7)
                    int col = i % 8; // Determine the column (0-7)
                    int value = ui->mainWidget->getVL2(i); // Get the value for the current index
                    m_table->updateTable_2(row, col, value); // Update the table with the row, column, and value
                    //qDebug() << "ODCZYTANA: " << list[i+2].toInt();
                    m_table->calculateMaxError_2(row, col, value, useMSE);
                }
                //Data_From_Port="";
                /*qDebug() << list[18];
                if(list.at(18) == crc_result){
                    Data_From_Port="";
                }*/
                Data_From_Port="";
            }
            else if( list.at(0) == 'P'){
                bool useMSE = (m_table_termo->ui->errorMetricComboBox->currentText() == "Mean Squared Error");

                for (int i = 0; i < 64; i++){
                    float meas = list[i+2].toFloat();
                    ui->mainWidget->setAMG(63-i,meas);

                    m_table_termo->ui->mainWidget_2->setAMG(63-i,meas); //data flow to datadisplay widget
                    m_table_termo->ui->mainWidget_6->setAMG(63-i,meas); //data flow to datadispalytext widget
                    int row = i / 8; // Determine the row (0-7)
                    int col = i % 8; // Determine the column (0-7)
                    int value = ui->mainWidget->getAMG(i); // Get the value for the current index
                    m_table_termo->updateTable_2(row, col, value); // Update the table with the row, column, and value
                    //m_table->calculateMaxError(row, col, value);

                    //if (m_table->isMeasurementSeriesActive) {
                    m_table_termo->calculateMaxError_2(row, col, value, useMSE);
                    // qDebug() << "ZAPISANA: " << ui->mainWidget->getAMG(63-i);
                    // qDebug() << "ODCZYTANA: " << list[i+2].toFloat();
                }
                //Data_From_Port="";
                /*if(list.at(66) == crc_result){
                    Data_From_Port="";
                }*/
                Data_From_Port="";
            }
            else if( list.at(0) == 'L'){
                bool useMSE = (m_table_termo->ui->errorMetricComboBox->currentText() == "Mean Squared Error");

                for (int i = 0; i < 768; ++i){
                    ui->mainWidget->setMLX(767-i,list[i+2].toFloat());

                    m_table_termo->ui->mainWidget->setMLX(767-i,list[i+2].toFloat()); //data flow to datadisplay widget
                    m_table_termo->ui->mainWidget_4->setMLX(767-i,list[i+2].toFloat()); //data flow to datadispalytext widget
                    int row = i / 32; // Determine the row (0-7)
                    int col = i % 32; // Determine the column (0-7)
                    int value = ui->mainWidget->getMLX(i); // Get the value for the current index
                    m_table_termo->updateTable_1(row, col, value); // Update the table with the row, column, and value
                    //m_table->calculateMaxError(row, col, value);

                    //if (m_table->isMeasurementSeriesActive) {
                    m_table_termo->calculateMaxError_1(row, col, value, useMSE);
                    // qDebug() << "ZAPISANA: " << ui->mainWidget->getMLX(767-i);
                    // qDebug() << "ODCZYTANA: " << list[i+2].toFloat();
                }

                Data_From_Port="";
                /*if(list.at(770) == crc_result){
                    Data_From_Port="";
                }*/
                //Data_From_Port="";
            }
            else
            {
                //qDebug() << "Niepoprawna ramka danych!\n";
                Data_From_Port="";
            }
            Is_Data_Received = false;
        }
    } else {
        qDebug() << "Port nie został otwarty\n";
    }
}

/**
 * @brief Saves data to a file.
 */

void MainWindow::save_file(){
    QString filename=dialog->filePath;
    QFile file( filename );
    if ( file.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        QTextStream stream( &file );
        if (symbol == 'A'){
            //stream << "\r\n";
            for(int i = 0; i < 64; i++){
                if(i%8 == 0 && i != 0){
                    stream << "\r\n";
                } if(i==0){
                    stream << "\r\n";
                }
                stream << ui->mainWidget->getVL1(i) << " ";
            }
            stream << "\r\n";
            for(int i = 0; i < 64; i++){
                if(i%8 == 0 && i != 0){
                    stream << "\r\n";
                } if(i==0){
                    stream << "\r\n";
                }
                stream << ui->mainWidget->getVL2(i) << " ";
            }
            stream << "\r\n";
            for(int i = 0; i < 64; i++){
                if(i%8 == 0 && i != 0){
                    stream << "\r\n";
                } if(i==0){
                    stream << "\r\n";
                }
                stream << ui->mainWidget->getAMG(i) << " ";
            }
            stream << "\r\n";
            for(int i = 0; i < 768; i++){
                if(i%32 == 0 && i != 0){
                    stream << "\r\n";
                } if(i==0){
                    stream << "\r\n";
                }
                stream << ui->mainWidget->getMLX(i) << " ";
            }
            stream << "\r\n";
        }
        else if (symbol == 'B'){
            /*clock_gettime(CLOCK_REALTIME, &stop);
        int seconds = ((int)stop.tv_sec - (int)start.tv_sec);
        double nseconds = (stop.tv_nsec - start.tv_nsec) / (double)BILLION;
        stream << "POMIAR " << seconds << " " << nseconds << "\r\n";*/
            //stream << "\r\n";
            for(int i = 0; i < 64; i++){
                if(i%8 == 0 && i != 0){
                    stream << "\r\n";
                } if(i==0){
                    stream << "\r\n";
                }
                stream << ui->mainWidget->getVL1(i) << " ";
            }
            stream << "\r\n\n";
        }
        else if (symbol == 'C'){
            for(int i = 0; i < 64; i++){
                if(i%8 == 0 && i != 0){
                    stream << "\r\n";
                } if(i==0){
                    stream << "\r\n";
                }
                stream << ui->mainWidget->getVL2(i) << " ";
            }
            stream << "\r\n";
        }
        else if (symbol == 'D'){
            for(int i = 0; i < 768; i++){
                if(i%32 == 0 && i != 0){
                    stream << "\r\n";
                } if(i==0){
                    stream << "\r\n";
                }
                stream << ui->mainWidget->getMLX(i) << " ";
            }
            stream << "\r\n";
        }
        else if (symbol == 'E'){
            for(int i = 0; i < 64; i++){
                if(i%8 == 0 && i != 0){
                    stream << "\r\n";
                } if(i==0){
                    stream << "\r\n";
                }
                stream << ui->mainWidget->getAMG(i) << " ";
            }
            stream << "\r\n";
        }
        else if (symbol == 'F'){
            for(int i = 0; i < 64; i++){
                if(i%8 == 0 && i != 0){
                    stream << "\r\n";
                } if(i==0){
                    stream << "\r\n";
                }
                stream << ui->mainWidget->getVL1(i) << " ";
            }
            stream << "\r\n";
            for(int i = 0; i < 64; i++){
                if(i%8 == 0 && i != 0){
                    stream << "\r\n";
                } if(i==0){
                    stream << "\r\n";
                }
                stream << ui->mainWidget->getVL2(i) << " ";
            }
            stream << "\r\n";
            for(int i = 0; i < 64; i++){
                if(i%8 == 0 && i != 0){
                    stream << "\r\n";
                } if(i==0){
                    stream << "\r\n";
                }
                stream << ui->mainWidget->getAMG(i) << " ";
            }
            stream << "\r\n";
        }
        else if (symbol == 'G'){
            for(int i = 0; i < 64; i++){
                if(i%8 == 0 && i != 0){
                    stream << "\r\n";
                } if(i==0){
                    stream << "\r\n";
                }
                stream << ui->mainWidget->getVL1(i) << " ";
            }
            stream << "\r\n";
            for(int i = 0; i < 64; i++){
                if(i%8 == 0 && i != 0){
                    stream << "\r\n";
                } if(i==0){
                    stream << "\r\n";
                }
                stream << ui->mainWidget->getVL2(i) << " ";
            }
            stream << "\r\n";
            for(int i = 0; i < 768; i++){
                if(i%32 == 0 && i != 0){
                    stream << "\r\n";
                } if(i==0){
                    stream << "\r\n";
                }
                stream << ui->mainWidget->getMLX(i) << " ";
            }
            stream << "\r\n";
        }
    }
}

/**
 * @brief Gets the file path selected by the user.
 */

void MainWindow::get_path(){
    ui->wybranyPlik->clear();
    ui->wybranyPlik->append(dialog->fileName);
    path = dialog->filePath;
}

/**
 * @brief Handles the selection of sensor operation modes.
 * @param index The selected mode index from the dropdown.
 */
void MainWindow::on_trybWybor_activated(int index)
{

    if (index == 0){
        qDebug() << "Wybrano tryb 1";       //Tryb pracy wszystkich czujników
        Port->write("A");
        ui->mainWidget->setSensor(5);
        // Start the camera
        //camera->start();
        symbol = 'A';
        statusBar()->showMessage("Tryb pracy wszystkich czujników");
    } else if (index == 1) {
        qDebug() << "Wybrano tryb 2";       //Tryb pracy pierwszego czujnika VL53L5CX
        Port->write("B");
        ui->mainWidget->setSensor(1);
        //m_table->ui->mainWidget->setSensor(1);
        //ui->mainWidget_2->setSensor(1);
        symbol = 'B';
        statusBar()->showMessage("Tryb pracy pierwszego czujnika VL53L5CX");
    } else if (index == 2) {
        qDebug() << "Wybrano tryb 3";       //Tryb pracy drugiego czujnika VL53L5CX
        Port->write("C");
        ui->mainWidget->setSensor(2);
        symbol = 'C';
        statusBar()->showMessage("Tryb pracy drugiego czujnika VL53L5CX");
    } else if (index == 3) {
        qDebug() << "Wybrano tryb 4";       //Tryb pracy czujnika MLX
        Port->write("D");
        ui->mainWidget->setSensor(3);
        symbol = 'D';
        statusBar()->showMessage("Tryb pracy czujnika MLX90640");
    } else if (index == 4) {
        qDebug() << "Wybrano tryb 5";       //Tryb pracy czujnika AMG
        Port->write("E");
        ui->mainWidget->setSensor(4);
        symbol = 'E';
        statusBar()->showMessage("Tryb pracy czujnika AMG8833");
    } else if (index == 5) {
        qDebug() << "Wybrano tryb 6";       //Tryb pracy czujników odległości i czujnika AMG
        Port->write("F");
        ui->mainWidget->setSensor(4);
        symbol = 'F';
        statusBar()->showMessage("Tryb pracy czujników odległości i czujnika AMG8833");
    } else if (index == 6) {
        qDebug() << "Wybrano tryb 7";       //Tryb pracy czujników odległości i czujnika MLX
        Port->write("G");
        ui->mainWidget->setSensor(3);
        symbol = 'G';
        statusBar()->showMessage("Tryb pracy czujników odległości i czujnika MLX90640");
        /*ITS A NEW SECTION*/
    } else if (index == 7) {
        qDebug() << "Wybrano tryb 8";       //Tryb pracy czujników odległości i czujnika M
        Port->write("H");
        //ui->mainWidget->setSensor(3);
        symbol = 'H';
        statusBar()->showMessage("Tryb pracy czujnikow VL");
    } else if (index == 8) {
        qDebug() << "Wybrano tryb 9";       //Tryb pracy czujników odległości i czujnika MLX
        Port->write("I");
        //ui->mainWidget->setSensor(3);
        symbol = 'I';
        statusBar()->showMessage("Tryb pracy MLX90640 i AMG8833");
    }
}

/**
 * @brief Wizualisation of established connection with the hardware.
 */

void MainWindow::on_actionPo_cz_triggered()
{
    if (Port->isOpen()){
        statusBar()->showMessage("Połączenie z integratorem zainicjowane", 5000);
        ui->dioda->setPixmap(QPixmap(":/img/diodaOn.png").scaled(widthD, heightD, Qt::KeepAspectRatio));
        ui->polaczenieDioda->setText("Połączono z integratorem");
    } else {
        qDebug() << Port->error();
        statusBar()->showMessage("Połączenie z integratorem nie powiodło się", 5000);
    }
    connect(Port, SIGNAL(readyRead()),this,SLOT(read_Data()));
}

/**
 * @brief Wizualisation of disconnecting from the hardware.
 */

void MainWindow::on_actionRoz_cz_triggered()
{
    disconnect(Port, SIGNAL(readyRead()),this,SLOT(read_Data()));
    ui->dioda->setPixmap(QPixmap(":/img/diodaOff.png").scaled(widthD, heightD, Qt::KeepAspectRatio));
    ui->polaczenieDioda->setText("Brak połączenia z integratorem");
    statusBar()->showMessage("Port zamknięty. Połączenie przerwane", 5000);
}

/**
 * @brief Closes the application.
 */

void MainWindow::on_zamnkij_clicked()
{
    close();
}

/**
 * @brief Opens the file selection dialog for saving data.
 */

void MainWindow::on_zapisPlik_clicked()
{
    dialog ->show();
}

/**
 * @brief Starts or stops saving data based on the flag state.
 */

void MainWindow::on_rozpocznijZapis_clicked()
{
    if(flag == 1){
        if(path != " "){
            ui->rozpocznijZapis->setText("Zakończ zapisywanie");
            connect(timer, SIGNAL(timeout()), this, SLOT(save_file()));
            flag = 0;
            clock_gettime(CLOCK_REALTIME, &start);
        } else if(path == " "){
            statusBar()->showMessage("Nie wybrano pliku do zapisu!", 4000);
        }

    } else if(flag == 0){
        ui->rozpocznijZapis->setText("Rozpocznij zapisywanie");
        disconnect(timer, SIGNAL(timeout()), this, SLOT(save_file()));
        flag = 1;
    }
}

