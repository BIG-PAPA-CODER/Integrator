#ifndef TABLECHARTMLX_H
#define TABLECHARTMLX_H
#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include "table_termo.h"

class tablechartMLX : public QWidget
{
    Q_OBJECT

public:
    explicit tablechartMLX(QWidget *parent = nullptr);
    void updatechartMLX(int columnData[32]);

private:
    QChartView *chartView;
    QLineSeries *series;
    QChart *chart;
    table_termo *m_table_termo;

private slots:
    void updateYAxisMax(int maxY);
};

#endif // TABLECHARTMLX_H
