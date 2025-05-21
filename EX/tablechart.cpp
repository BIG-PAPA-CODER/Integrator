/**
 * @file tablechart.cpp
 * @brief Implementation of the tableChart class for visualizing data using a line chart.
 */

#include "tablechart.h"
#include "qboxlayout.h"
#include <QValueAxis>

/**
 * @class tableChart
 * @brief A QWidget-based class that displays a line chart with customizable X and Y axes.
 *
 * This class uses QChart and QLineSeries from the Qt Charts module to create
 * a customizable chart for visualizing data.
 */

/**
 * @brief Constructs a tableChart object.
 *
 * Initializes the chart, sets up the layout, and configures the X and Y axes.
 *
 * @param parent Pointer to the parent widget. Default is nullptr.
 */
tableChart::tableChart(QWidget *parent) : QWidget(parent)
{
    series = new QLineSeries();
    chart = new QChart();
    chart->addSeries(series);
    chart->createDefaultAxes();

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setFixedSize(421, 281); // Set fixed size to match the frame

    // Create a custom font for the numerical labels
    QFont labelFont;
    labelFont.setPointSize(8); // Set the desired font size for numerical labels

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(chartView);
    setLayout(layout);

    // Configure the Y-axis to show only integers with fixed tick interval
    QValueAxis *axisY = new QValueAxis();
    axisY->setLabelFormat("%d");
    axisY->setTitleBrush(QBrush(Qt::white));
    axisY->setLabelsBrush(QBrush(Qt::darkGray));
    axisY->setGridLineColor(Qt::darkGray);
    axisY->setRange(0, 4000);
    axisY->setTickCount(9);
    axisY->setMinorTickCount(0);
    axisY->setLabelsFont(labelFont);
    chart->setAxisY(axisY, series);

    // Configure the X-axis to show only integers with fixed tick interval
    QValueAxis *axisX = new QValueAxis();
    axisX->setLabelFormat("%d");
    axisX->setRange(1, 8);
    axisX->setTickCount(8);
    axisX->setTitleBrush(QBrush(Qt::white));
    axisX->setLabelsBrush(QBrush(Qt::darkGray));
    axisX->setGridLineColor(Qt::darkGray);
    axisX->setMinorTickCount(0);
    axisX->setLabelsFont(labelFont);
    chart->setAxisX(axisX, series);

    // Enable point markers for the series
    series->setPointsVisible(true);
    series->setColor(Qt::red);

    // Make the chart background transparent
    chart->setBackgroundBrush(Qt::NoBrush);
    chart->setPlotAreaBackgroundBrush(Qt::NoBrush);
    chart->setPlotAreaBackgroundVisible(false);

    // Remove the legend
    chart->legend()->hide();

    // Disable automatic margins and set fixed plot area
    chart->setMargins(QMargins(10, 10, 10, 10)); // Set fixed margins
    QRectF plotArea(30, 0, 351, 241); // Define a fixed plot area size
    chart->setPlotArea(plotArea);
}

/**
 * @brief Updates the chart with new data.
 *
 * Clears the existing series data and appends new data points.
 *
 * @param columnData Array of integers representing the Y-axis values for the chart.
 *                   The X-axis values range from 1 to 8.
 */
void tableChart::updateChart(int columnData[8])
{
    series->clear();
    for (int i = 0; i < 8; ++i) {
        series->append(i + 1, columnData[i]);
    }

    chart->axisX()->setRange(1, 8);
}

/**
 * @brief Updates the maximum value of the Y-axis.
 *
 * Adjusts the Y-axis range to accommodate a new maximum value.
 *
 * @param maxY The new maximum value for the Y-axis.
 */
void tableChart::updateYAxisMax(int maxY)
{
    chart->axisY()->setRange(0, maxY);
}
