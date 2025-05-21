#ifndef TABLECHART_H
#define TABLECHART_H

#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include "table.h"

//QT_CHARTS_USE_NAMESPACE

class tableChart : public QWidget
{
    Q_OBJECT

public:
    explicit tableChart(QWidget *parent = nullptr);
    void updateChart(int columnData[8]);

private:
    QChartView *chartView;
    QLineSeries *series;
    QChart *chart;
    table *m_table;

private slots:
    void updateYAxisMax(int maxY);
};

#endif // TABLECHART_H
