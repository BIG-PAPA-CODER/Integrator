/**
 * @file gauss.cpp
 * @brief Implementation of the Gauss class for applying Gaussian smoothing and visualizing sensor data.
 *
 * This file implements the `Gauss` class, which provides a real-time visualization of sensor data.
 * It applies Gaussian smoothing to the input data to enhance its quality and draws the results
 * on the screen. The data is displayed as a grid of cells, each colored based on its intensity.
 */

#include "gauss.h"

/**
 * @brief Constructor for the Gauss class.
 *
 * Initializes the widget and starts a timer to update the visualization periodically.
 *
 * @param pParent Pointer to the parent widget. Default is nullptr.
 */

Gauss::Gauss(QWidget *pParent) : QWidget(pParent)//, opacityEffect(new QGraphicsOpacityEffect(this))
{
    connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
    timer.start(1000);
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

unsigned int Gauss::ComputeBrightness(int CellVal, int _MinVal_Visualize, int _MaxVal_Visualize) const
{
    assert(_MinVal_Visualize < _MaxVal_Visualize);
    if (CellVal < _MinVal_Visualize) CellVal = _MinVal_Visualize;
    else if (_MaxVal_Visualize < CellVal) CellVal = _MaxVal_Visualize;

    return (255*(CellVal - _MinVal_Visualize))/(_MaxVal_Visualize-_MinVal_Visualize);
}

// void Gauss::setTransparency(int value)
// {
//     if (opacityEffect) {
//         opacityEffect->setOpacity(value / 100.0); // Normalize the value (0-100) to (0.0-1.0)
//     }
// }

/**
 * @brief Paints the widget with Gaussian-smoothed sensor data.
 *
 * This method applies a 5x5 Gaussian kernel to the input data, smooths it, and visualizes the results
 * as a grid of cells. Each cell's color intensity is determined by its brightness, which is computed
 * using the smoothed value.
 *
 * @param event The paint event.
 */

void Gauss::paintEvent(QPaintEvent *event){
    QPainter painter(this);
    QBrush brush;
    QPen pen;
    QColor color;

    pen.setWidth(0);
    pen.setStyle(Qt::NoPen);
    brush.setStyle(Qt::SolidPattern);
    painter.setPen(pen);

    int CellVal = 0;
    if (getSensor() == 1) {
        float gaussianKernel[5][5] = {
            {1 / 256.0, 4 / 256.0, 6 / 256.0, 4 / 256.0, 1 / 256.0},
            {4 / 256.0, 16 / 256.0, 24 / 256.0, 16 / 256.0, 4 / 256.0},
            {6 / 256.0, 24 / 256.0, 36 / 256.0, 24 / 256.0, 6 / 256.0},
            {4 / 256.0, 16 / 256.0, 24 / 256.0, 16 / 256.0, 4 / 256.0},
            {1 / 256.0, 4 / 256.0, 6 / 256.0, 4 / 256.0, 1 / 256.0}
        };

        // Temporary array to store smoothed values
        std::array<int, 64> smoothedValues;

        // Apply Gaussian smoothing to the original 8x8 values
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                float sum = 0.0;
                for (int ki = -2; ki <= 2; ++ki) {
                    for (int kj = -2; kj <= 2; ++kj) {
                        int ni = std::clamp(i + ki, 0, 7);
                        int nj = std::clamp(j + kj, 0, 7);
                        int index = ni * 8 + nj;
                        sum += getVL1(index) * gaussianKernel[ki + 2][kj + 2];
                    }
                }
                smoothedValues[i * 8 + j] = static_cast<int>(sum);
            }
        }

        // Scale factor and cell dimensions
        int scaleFactor = 8;
        float cell_width = static_cast<float>(width()) / (8 * scaleFactor);
        float cell_height = static_cast<float>(height()) / (8 * scaleFactor);

        // Draw each cell with floating-point precision
        for (int i = 0; i < 7; ++i) {
            for (int j = 0; j < 7; ++j) {
                int brightnessTL = ComputeBrightness(smoothedValues[i * 8 + j], 20, 1000);
                int brightnessTR = ComputeBrightness(smoothedValues[i * 8 + (j + 1)], 20, 1000);
                int brightnessBL = ComputeBrightness(smoothedValues[(i + 1) * 8 + j], 20, 1000);
                int brightnessBR = ComputeBrightness(smoothedValues[(i + 1) * 8 + (j + 1)], 20, 1000);

                for (int dy = 0; dy < scaleFactor; ++dy) {
                    for (int dx = 0; dx < scaleFactor; ++dx) {
                        float wx = dx / float(scaleFactor);
                        float wy = dy / float(scaleFactor);

                        int brightness = static_cast<int>((1 - wy) * ((1 - wx) * brightnessTL + wx * brightnessTR) +
                                                          wy * ((1 - wx) * brightnessBL + wx * brightnessBR));

                        color.setRgb(255 - brightness, 255 - brightness, 255 - brightness);
                        brush.setColor(color);
                        painter.setBrush(brush);

                        // Calculate precise floating-point position and dimensions for each cell
                        float x = (j * scaleFactor + dx) * cell_width;
                        float y = (i * scaleFactor + dy) * cell_height;
                        painter.drawRect(QRectF(x, y, cell_width, cell_height));
                    }
                }
            }
        }




    // if(getSensor() == 1){

    //     float gaussianKernel[5][5] = {
    //         {1 / 256.0, 4 / 256.0, 6 / 256.0, 4 / 256.0, 1 / 256.0},
    //         {4 / 256.0, 16 / 256.0, 24 / 256.0, 16 / 256.0, 4 / 256.0},
    //         {6 / 256.0, 24 / 256.0, 36 / 256.0, 24 / 256.0, 6 / 256.0},
    //         {4 / 256.0, 16 / 256.0, 24 / 256.0, 16 / 256.0, 4 / 256.0},
    //         {1 / 256.0, 4 / 256.0, 6 / 256.0, 4 / 256.0, 1 / 256.0}
    //     };

    //     // Temporary array to store smoothed values
    //     std::array<int, 64> smoothedValues;

    //     // Apply Gaussian smoothing
    //     for (int i = 0; i < 8; ++i) {
    //         for (int j = 0; j < 8; ++j) {
    //             float sum = 0.0;
    //             for (int ki = -2; ki <= 2; ++ki) {
    //                 for (int kj = -2; kj <= 2; ++kj) {
    //                     // Calculate neighbor indices with edge handling
    //                     int ni = std::clamp(i + ki, 0, 7);
    //                     int nj = std::clamp(j + kj, 0, 7);
    //                     int index = ni * 8 + nj;
    //                     sum += getVL1(index) * gaussianKernel[ki + 2][kj + 2];
    //                 }
    //             }
    //             smoothedValues[i * 8 + j] = static_cast<int>(sum);
    //         }
    //     }

    //     // Render the smoothed image
    //     int cell_width = width() / 8;
    //     int cell_height = height() / 8;
    //     int x_cord = 0;
    //     int y_cord = 0;

    //     for (int i = 0; i < 64; ++i) {
    //         int smoothedVal = smoothedValues[i];
    //         int brightness = ComputeBrightness(smoothedVal, 20, 500);

    //         color.setRgb(255 - brightness, 255 - brightness, 255 - brightness);
    //         brush.setColor(color);
    //         painter.setBrush(brush);
    //         painter.drawRect(x_cord, y_cord, cell_width, cell_height);

    //         x_cord += cell_width;
    //         if ((i + 1) % 8 == 0) {
    //             x_cord = 0;
    //             y_cord += cell_height;
    //         }
    //     }

        // int cell_width = (width())/8;
        // int cell_height = (height())/8;
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

        float gaussianKernel[5][5] = {
            {1 / 256.0, 4 / 256.0, 6 / 256.0, 4 / 256.0, 1 / 256.0},
            {4 / 256.0, 16 / 256.0, 24 / 256.0, 16 / 256.0, 4 / 256.0},
            {6 / 256.0, 24 / 256.0, 36 / 256.0, 24 / 256.0, 6 / 256.0},
            {4 / 256.0, 16 / 256.0, 24 / 256.0, 16 / 256.0, 4 / 256.0},
            {1 / 256.0, 4 / 256.0, 6 / 256.0, 4 / 256.0, 1 / 256.0}
        };

        // Temporary array to store smoothed values
        std::array<int, 64> smoothedValues;

        // Apply Gaussian smoothing
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                float sum = 0.0;
                for (int ki = -2; ki <= 2; ++ki) {
                    for (int kj = -2; kj <= 2; ++kj) {
                        // Calculate neighbor indices with edge handling
                        int ni = std::clamp(i + ki, 0, 7);
                        int nj = std::clamp(j + kj, 0, 7);
                        int index = ni * 8 + nj;
                        sum += getVL2(index) * gaussianKernel[ki + 2][kj + 2];
                    }
                }
                smoothedValues[i * 8 + j] = static_cast<int>(sum);
            }
        }

        // Scale factor and cell dimensions
        int scaleFactor = 8;
        float cell_width = static_cast<float>(width()) / (8 * scaleFactor);
        float cell_height = static_cast<float>(height()) / (8 * scaleFactor);

        // Draw each cell with floating-point precision
        for (int i = 0; i < 7; ++i) {
            for (int j = 0; j < 7; ++j) {
                int brightnessTL = ComputeBrightness(smoothedValues[i * 8 + j], 20, 1000);
                int brightnessTR = ComputeBrightness(smoothedValues[i * 8 + (j + 1)], 20, 1000);
                int brightnessBL = ComputeBrightness(smoothedValues[(i + 1) * 8 + j], 20, 1000);
                int brightnessBR = ComputeBrightness(smoothedValues[(i + 1) * 8 + (j + 1)], 20, 1000);

                for (int dy = 0; dy < scaleFactor; ++dy) {
                    for (int dx = 0; dx < scaleFactor; ++dx) {
                        float wx = dx / float(scaleFactor);
                        float wy = dy / float(scaleFactor);

                        int brightness = static_cast<int>((1 - wy) * ((1 - wx) * brightnessTL + wx * brightnessTR) +
                                                          wy * ((1 - wx) * brightnessBL + wx * brightnessBR));

                        color.setRgb(255 - brightness, 255 - brightness, 255 - brightness);
                        brush.setColor(color);
                        painter.setBrush(brush);

                        // Calculate precise floating-point position and dimensions for each cell
                        float x = (j * scaleFactor + dx) * cell_width;
                        float y = (i * scaleFactor + dy) * cell_height;
                        painter.drawRect(QRectF(x, y, cell_width, cell_height));
                    }
                }
            }
        }

        // int cell_width = (width())/8;
        // int cell_height = (height())/8;
        // for(int i = 0, x_cord = 0, y_cord = 0; i < 64; i++){
        //     CellVal = ComputeBrightness(getVL2(i),20,500);
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
    else if (getSensor() == 3){
        int cell_width = (width())/32;
        int cell_height = (height())/24;
        for(int i = 0, x_cord = 0, y_cord = 0; i < 768; i++){
            CellVal = ComputeBrightness(getMLX(i),0,60);
            color.setRgb(CellVal, 0, 255-CellVal);
            brush.setColor(color);
            painter.setBrush(brush);
            painter.drawRect(x_cord, y_cord, cell_width, cell_height);
            x_cord += cell_width;
            if (i % 32 == 0 && i != 0){
                x_cord = 0;
                y_cord += cell_height;
                color.setRgb(CellVal, 0, 255-CellVal);
                brush.setColor(color);
                painter.setBrush(brush);
                painter.drawRect(x_cord, y_cord, cell_width, cell_height);
                x_cord += cell_width;
            }
        }
    }
    else if (getSensor() == 4){
        int cell_width = (width())/8;
        int cell_height = (height())/8;
        for(int i = 0, x_cord = 0, y_cord = 0; i < 64; i++){
            CellVal = ComputeBrightness(getAMG(i),0,60);
            color.setRgb(CellVal, 0, 255-CellVal);
            brush.setColor(color);
            painter.setBrush(brush);
            painter.drawRect(x_cord, y_cord, cell_width, cell_height);
            x_cord += cell_width;
            if (i % 8 == 0 && i != 0){
                x_cord = 0;
                y_cord += cell_height;
                color.setRgb(CellVal, 0, 255-CellVal);
                brush.setColor(color);
                painter.setBrush(brush);
                painter.drawRect(x_cord, y_cord, cell_width, cell_height);
                x_cord += cell_width;
            }
        }
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
/*for(int row = 0, x_cord = 0, y_cord = 0; row < 8; row++){
        for(int col = 0; col < 8; col++){
            CellVal = ComputeBrightness(dTab.tabVL1[col]);
            color.setRgb(CellVal, CellVal, CellVal);
            brush.setColor(color);
            painter.setBrush(brush);
            painter.drawRect(x_cord, y_cord, cell_width, cell_height);
            x_cord += cell_width;
        }
        x_cord = 0;
        //painter.drawRect(x_cord, y_cord, cell_width, cell_height);

        y_cord += cell_height;
    }*/

