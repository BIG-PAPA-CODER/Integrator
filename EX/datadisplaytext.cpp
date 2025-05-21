/**
 * @file datadisplaytext.cpp
 * @brief Implementation of the DataDisplayText class for visualizing sensor data with text overlays.
 *
 * This file implements the `DataDisplayText` class, which provides functionality for visualizing
 * sensor data in a grid format. Each cell is color-coded based on its value, with the numerical
 * value displayed as text at the center of the cell.
 */

#include "datadisplaytext.h"
#include <QToolTip>
#include <QMouseEvent>
//#include "comapre.h"
//int _MaxVal_Visualize = 2000;
//int _MinVal_Visualize = 1;
//int sensor = 0;

/**
 * @brief Constructor for the DataDisplay class.
 *
 * Initializes the widget, starts a timer to trigger updates, and sets mouse tracking.
 *
 * @param pParent Pointer to the parent widget. Default is nullptr.
 */

DataDisplayText::DataDisplayText(QWidget *pParent) : QWidget(pParent)//, opacityEffect(new QGraphicsOpacityEffect(this))
{
    connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
    timer.start(1000);
    setMouseTracking(true);
    //setGraphicsEffect(opacityEffect);
}

/**
 * @brief Computes the brightness value based on the cell value and visualization range.
 *
 * This method maps a given cell value within a specified range to a brightness value between 0 and 255.
 *
 * @param CellVal The cell value to map.
 * @param _MinVal_Visualize The minimum value for the visualization range.
 * @param _MaxVal_Visualize The maximum value for the visualization range.
 * @return Brightness value in the range 0–255.
 */

unsigned int DataDisplayText::ComputeBrightness(int CellVal, int _MinVal_Visualize, int _MaxVal_Visualize) const
{
    assert(_MinVal_Visualize < _MaxVal_Visualize);
    if (CellVal < _MinVal_Visualize) CellVal = _MinVal_Visualize;
    else if (_MaxVal_Visualize < CellVal) CellVal = _MaxVal_Visualize;

    return (255*(CellVal - _MinVal_Visualize))/(_MaxVal_Visualize-_MinVal_Visualize);
}

/**
 * @brief Handles mouse movement events to display cell values as tooltips.
 *
 * Detects the grid cell under the mouse cursor based on the sensor type
 * and displays the corresponding value using a tooltip.
 *
 * @param event The mouse movement event.
 */

// Override the mouse move event
void DataDisplayText::mouseMoveEvent(QMouseEvent *event) {
    int cellWidth, cellHeight;
    int index = -1;
    int sensor = getSensor();

    if (sensor == 1 || sensor == 2 || sensor == 4) {
        // 8x8 grid
        cellWidth = width() / 8;
        cellHeight = height() / 8;
        int col = event->pos().x() / cellWidth;
        int row = event->pos().y() / cellHeight;
        index = row * 8 + col;
    } else if (sensor == 3) {
        // 32x24 grid (MLX sensor)
        cellWidth = width() / 32;
        cellHeight = height() / 24;
        int col = event->pos().x() / cellWidth;
        int row = event->pos().y() / cellHeight;
        index = row * 32 + col;
    }

    if (index >= 0 && index < 768) {
        int value = 0;
        if (sensor == 1) {
            value = getVL1(index);
        } else if (sensor == 2) {
            value = getVL2(index);
        } else if (sensor == 3) {
            value = getMLX(index);
        } else if (sensor == 4) {
            value = getAMG(index);
        }

        // Show the tooltip with the pixel value
        QString tooltipText = QString("Value: %1").arg(value);
        QToolTip::showText(event->globalPos(), tooltipText, this);
    } else {
        // Hide the tooltip if the cursor is outside the valid range
        QToolTip::hideText();
    }
}

// void DataDisplayText::setTransparency(int value)
// {
//     if (opacityEffect) {
//         opacityEffect->setOpacity(value / 100.0); // Normalize the value (0-100) to (0.0-1.0)
//     }
// }

/**
 * @brief Paints the widget based on sensor data with text overlays.
 *
 * Draws a grid of rectangles representing sensor data. Each rectangle's color
 * is determined by the corresponding cell value, and the actual value is displayed
 * as text at the center of the cell.
 *
 * @param event The paint event.
 */

void DataDisplayText::paintEvent(QPaintEvent *event){
    QPainter painter(this);
    QBrush brush;
    QPen pen;
    QColor color;

    pen.setWidth(0);
    pen.setStyle(Qt::NoPen);
    brush.setStyle(Qt::SolidPattern);
    painter.setPen(pen);

    int CellVal = 0;

    if(getSensor() == 1){
        int cell_width = (width())/8;
        int cell_height = (height())/8;
        QFont font = painter.font();
        font.setPointSize(10); // Increase font size for better visibility
        painter.setFont(font);
        painter.setPen(Qt::red); // Set text color to black

        for (int i = 0, x_cord = 0, y_cord = 0; i < 64; i++) {
            int CellVal = ComputeBrightness(getVL1(i), 20, 1000);
            color.setRgb(255 - CellVal, 255 - CellVal, 255 - CellVal);
            brush.setColor(color);
            painter.setBrush(brush);
            painter.drawRect(x_cord, y_cord, cell_width, cell_height);

            // Prepare the text to display
            QString cellText = QString::number(getVL1(i)); // Display the actual value
            QRect textRect(x_cord, y_cord, cell_width, cell_height);

            // Draw the text centered within the cell
            painter.drawText(textRect, Qt::AlignCenter, cellText);

            // Update coordinates
            x_cord += cell_width;
            if ((i + 1) % 8 == 0) {
                x_cord = 0;
                y_cord += cell_height;
            }
        }
        // for(int i = 0, x_cord = 0, y_cord = 0; i < 64; i++){
        //     CellVal = ComputeBrightness(getVL1(i),20,500);
        //     color.setRgb(255-CellVal, 255-CellVal, 255-CellVal);
        //     brush.setColor(color);
        //     painter.setBrush(brush);
        //     painter.drawRect(x_cord, y_cord, cell_width, cell_height);
        //     x_cord += cell_width;
        //     if (i % 8 == 0 && i != 0){
        //         x_cord = 0;
        //         y_cord += cell_height;
        //         color.setRgb(255-CellVal, 255-CellVal, 255-CellVal);
        //         brush.setColor(color);
        //         painter.setBrush(brush);
        //         painter.drawRect(x_cord, y_cord, cell_width, cell_height);
        //         x_cord += cell_width;
        //     }
        // }
    }
    else if (getSensor() == 2){
        int cell_width = (width())/8;
        int cell_height = (height())/8;
        QFont font = painter.font();
        font.setPointSize(10); // Increase font size for better visibility
        painter.setFont(font);
        painter.setPen(Qt::red); // Set text color to black

        for (int i = 0, x_cord = 0, y_cord = 0; i < 64; i++) {
            int CellVal = ComputeBrightness(getVL2(i), 20, 1000);
            color.setRgb(255 - CellVal, 255 - CellVal, 255 - CellVal);
            brush.setColor(color);
            painter.setBrush(brush);
            painter.drawRect(x_cord, y_cord, cell_width, cell_height);

            // Prepare the text to display
            QString cellText = QString::number(getVL2(i)); // Display the actual value
            QRect textRect(x_cord, y_cord, cell_width, cell_height);

            // Draw the text centered within the cell
            painter.drawText(textRect, Qt::AlignCenter, cellText);

            // Update coordinates
            x_cord += cell_width;
            if ((i + 1) % 8 == 0) {
                x_cord = 0;
                y_cord += cell_height;
            }
        }
    }
    else if (getSensor() == 3){
        int cell_width = width() / 32;  // Divide the widget width by the number of columns
        int cell_height = height() / 24;  // Divide the widget height by the number of rows
        QFont font = painter.font();
        font.setPointSize(6);  // Adjust font size for readability
        painter.setFont(font);
        painter.setPen(Qt::white);

        // Iterate over all 768 elements (32 columns x 24 rows)
        for (int i = 0; i < 768; i++) {
            int row = i / 32;  // Calculate the row (integer division)
            int col = i % 32;  // Calculate the column (remainder)

            int x_cord = col * cell_width;  // Calculate x-coordinate
            int y_cord = row * cell_height;  // Calculate y-coordinate

            // Fetch the value and compute its brightness
            int value = static_cast<int>(getMLX(i));
            CellVal = ComputeBrightness(value, 0, 60);  // Adjust brightness range as needed

            // Set the cell color
            color.setRgb(CellVal, 0, 255 - CellVal);  // Example: red to blue gradient
            brush.setColor(color);
            painter.setBrush(brush);

            // Draw the rectangle for the cell
            painter.drawRect(x_cord, y_cord, cell_width, cell_height);

            // Display the value in the center of the cell
            QString cellText = QString::number(value);
            QRect textRect(x_cord, y_cord, cell_width, cell_height);
            painter.drawText(textRect, Qt::AlignCenter, cellText);
        }
        // int cell_width = (width())/32;
        // int cell_height = (height())/24;
        // QFont font = painter.font();
        // font.setPointSize(6);
        // painter.setFont(font);
        // painter.setPen(Qt::white);

        // for(int i = 0, x_cord = 0, y_cord = 0; i < 768; i++){
        //     CellVal = ComputeBrightness(getMLX(i),0,60);
        //     color.setRgb(CellVal, 0, 255-CellVal);
        //     brush.setColor(color);
        //     painter.setBrush(brush);
        //     painter.drawRect(x_cord, y_cord, cell_width, cell_height);

        //     int value = static_cast<int>(getMLX(i));
        //     QString cellText = QString::number(value);
        //     QRect textRect(x_cord, y_cord, cell_width, cell_height);
        //     painter.drawText(textRect, Qt::AlignCenter, cellText);

        //     x_cord += cell_width;
        //     if (i % 32 == 0 && i != 0){
        //         x_cord = 0;
        //         y_cord += cell_height;
        //         color.setRgb(CellVal, 0, 255-CellVal);
        //         brush.setColor(color);
        //         painter.setBrush(brush);
        //         painter.drawRect(x_cord, y_cord, cell_width, cell_height);
        //         x_cord += cell_width;
        //     }
        // }
    }
    else if (getSensor() == 4){
        int cell_width = width() / 8;  // Divide the widget width by the number of columns
        int cell_height = height() / 8;  // Divide the widget height by the number of rows
        QFont font = painter.font();
        font.setPointSize(10);  // Adjust font size for readability
        painter.setFont(font);
        painter.setPen(Qt::white);

        // Iterate over all 64 elements
        for (int i = 0; i < 64; i++) {
            int row = i / 8;  // Calculate the row (integer division)
            int col = i % 8;  // Calculate the column (remainder)

            int x_cord = col * cell_width;  // Calculate x-coordinate
            int y_cord = row * cell_height;  // Calculate y-coordinate

            // Fetch the value and compute its brightness
            int value = static_cast<int>(getAMG(i));
            CellVal = ComputeBrightness(value, 0, 60);

            // Set the cell color
            color.setRgb(CellVal, 0, 255 - CellVal);
            brush.setColor(color);
            painter.setBrush(brush);

            // Draw the rectangle for the cell
            painter.drawRect(x_cord, y_cord, cell_width, cell_height);

            // Display the value in the center of the cell
            QString cellText = QString::number(value);
            QRect textRect(x_cord, y_cord, cell_width, cell_height);
            painter.drawText(textRect, Qt::AlignCenter, cellText);
        }
        // int cell_width = (width())/8;
        // int cell_height = (height())/8;
        // QFont font = painter.font();
        // font.setPointSize(10);
        // painter.setFont(font);
        // painter.setPen(Qt::white);

        // for(int i = 0, x_cord = 0, y_cord = 0; i < 64; i++){
        //     CellVal = ComputeBrightness(getAMG(i),0,60);
        //     color.setRgb(CellVal, 0, 255-CellVal);
        //     brush.setColor(color);
        //     painter.setBrush(brush);
        //     painter.drawRect(x_cord, y_cord, cell_width, cell_height);

        //     QString cellText = QString::number(getAMG(i));
        //     QRect textRect(x_cord, y_cord, cell_width, cell_height);
        //     painter.drawText(textRect, Qt::AlignCenter, cellText);

        //     x_cord += cell_width;
        //     if (i % 8 == 0 ){//&& i != 0){
        //         x_cord = 0;
        //         y_cord += cell_height;
        //         color.setRgb(CellVal, 0, 255-CellVal);
        //         brush.setColor(color);
        //         painter.setBrush(brush);
        //         painter.drawRect(x_cord, y_cord, cell_width, cell_height);
        //         x_cord += cell_width;
        //     }
        // }
    }

    else if (getSensor() == 5){
        /*int cell_width = (width())/18;
        int cell_height = (height())/18;
        for(int i = 0, x_cord = 0, y_cord = 0; i < 64; ++i){
            CellVal = ComputeBrightness(getVL1(i),1,2000);
            color.setRgb(CellVal, CellVal, CellVal);
            brush.setColor(color);
            painter.setBrush(brush);
            painter.drawRect(x_cord, y_cord, cell_width, cell_height);
            x_cord += cell_width;
            if (i % 8 == 0 && i != 0){
                x_cord = 0;
                y_cord += cell_height;
                CellVal = ComputeBrightness(getVL1(i),1,2000);
                color.setRgb(CellVal, CellVal, CellVal);
                brush.setColor(color);
                painter.setBrush(brush);
                painter.drawRect(x_cord, y_cord, cell_width, cell_height);
                x_cord += cell_width;
            }
        }
        for(int i = 0, x_cord = cell_width*10, y_cord = 0; i < 64; ++i){
            CellVal = ComputeBrightness(getVL2(i),1,2000);
            color.setRgb(CellVal, CellVal, CellVal);
            brush.setColor(color);
            painter.setBrush(brush);
            painter.drawRect(x_cord, y_cord, cell_width, cell_height);
            x_cord += cell_width;
            if (i % 8 == 0 && i != 0){
                x_cord = cell_width*10;
                y_cord += cell_height;
                color.setRgb(CellVal, CellVal, CellVal);
                brush.setColor(color);
                painter.setBrush(brush);
                painter.drawRect(x_cord, y_cord, cell_width, cell_height);
                x_cord += cell_width;
            }
        }*/
    }
}

