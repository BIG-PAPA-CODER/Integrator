#ifndef GAUSS_H
#define GAUSS_H

#include <QWidget>
#include <QPainter>
#include <QTimer>

#include <QGraphicsOpacityEffect>

#include "mainwindow.h"
#include "table.h"

class Gauss: public QWidget
{
    int16_t tabVL1[64];
    int16_t tabVL2[64];
    float tabAMG[64];
    float tabMLX[768];
    int sensor = 0;
public:
    Gauss(QWidget *pParent = nullptr);

    virtual void paintEvent(QPaintEvent *event) override;
    unsigned int ComputeBrightness(int CellVal, int _MinVal_Visualize, int _MaxVal_Visualize) const;

    void setVL1(int i, int meas) {tabVL1[i] = meas;}
    int getVL1(int i){return tabVL1[i];}

    void setVL2(int i, int meas) {tabVL2[i] = meas;}
    int getVL2(int i){return tabVL2[i];}

    void setAMG(int i, float meas) {tabAMG[i] = meas;}
    float getAMG(int i){return tabAMG[i];}

    void setMLX(int i, float meas) {tabMLX[i] = meas;}
    float getMLX(int i){return tabMLX[i];}

    void setSensor(int i){sensor = i;}
    int getSensor(){return sensor;}

    //void setTransparency(int value);
private:
    QTimer timer;
    MainWindow *mw;
    table *m_table;
    //QGraphicsOpacityEffect *opacityEffect; // For transparency effect
};

#endif // GAUSS_H
