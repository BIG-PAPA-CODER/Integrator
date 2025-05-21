#ifndef CAMERA_H
#define CAMERA_H

#include <QWidget>
#include <QCamera>
#include <QCameraDevice>
#include <QVideoWidget>
#include <QGraphicsOpacityEffect>
#include <QSlider>
#include <QPushButton>
#include <QVBoxLayout>

class camera : public QWidget
{
    Q_OBJECT

public:
    explicit camera(QWidget *parent = nullptr);
    ~camera();

public slots:
    void toggleCamera();
    void adjustCameraTransparency(int value);

private:
    QCamera *camera_m;
    QVideoWidget *videoWidget;
    QGraphicsOpacityEffect *opacityEffect;
};

#endif // CAMERA_H
