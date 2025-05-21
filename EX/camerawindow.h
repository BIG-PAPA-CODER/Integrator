#ifndef CAMERAWINDOW_H
#define CAMERAWINDOW_H

#include <QWidget>
#include <QCamera>
#include <QMediaCaptureSession>
#include <QVideoWidget>
#include <QVBoxLayout>
#include <QPainter>
#include <QMouseEvent>

class TransparentWindow : public QWidget {
    Q_OBJECT

public:
    explicit TransparentWindow(QWidget *parent = nullptr);
    void setOpacity(float value);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override; // For moving the window
    void mouseMoveEvent(QMouseEvent *event) override; // For moving the window
    void mouseReleaseEvent(QMouseEvent *event) override; // For moving the window

private:
    float opacity = 1.0; // Default opacity
    bool dragging = false;
    QPoint dragStartPosition;
};

class CameraWindow : public QWidget {
    Q_OBJECT

public:
    explicit CameraWindow(QWidget *parent = nullptr);
    ~CameraWindow();

    void setCamera(QCamera *camera);
    void setOpacity(int value);

private:
    TransparentWindow *transparentWindow;
    QVideoWidget *videoWidget;
    QMediaCaptureSession *captureSession = nullptr;
    QCamera *camera = nullptr;
};

#endif // CAMERAWINDOW_H
