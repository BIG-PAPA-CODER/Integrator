// #ifndef CAMERAWIDGET_H
// #define CAMERAWIDGET_H

// #include <QWidget>
// #include <QCamera>
// #include <QMediaCaptureSession>
// #include <QVideoWidget>
// #include <QTransform>
// #include <QPaintEvent>

// class CameraWidget : public QWidget
// {
//     Q_OBJECT

// public:
//     explicit CameraWidget(QWidget *parent = nullptr);
//     ~CameraWidget();

//     // Transformation controls
//     void zoomIn();
//     void zoomOut();
//     void moveUp();
//     void moveDown();
//     void moveLeft();
//     void moveRight();

// protected:
//     void resizeEvent(QResizeEvent *event) override;
//     void paintEvent(QPaintEvent *event) override;

// private:
//     void updateTransform();  // Updates the transformation matrix

//     QCamera *camera;                            // Camera instance
//     QMediaCaptureSession *captureSession;       // Media capture session
//     QVideoWidget *videoWidget;                  // Video display widget

//     QTransform transformMatrix; // Transformation matrix for zoom and pan
//     float zoomFactor;           // Current zoom factor
//     float panX;                 // Horizontal pan offset
//     float panY;                 // Vertical pan offset
// };

// #endif // CAMERAWIDGET_H

#ifndef CAMERAWIDGET_H
#define CAMERAWIDGET_H

#include <QWidget>
#include <QCamera>
#include <QMediaCaptureSession>
#include <QImage>
#include <QTransform>
#include <QVideoFrame>
#include <QSlider>

namespace Ui {
class CameraWidget;  // Forward declaration for the UI class
}

class CameraWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CameraWidget(QWidget *parent = nullptr);
    ~CameraWidget();

    // Transformation controls
    void zoomIn();
    void zoomOut();
    void moveUp();
    void moveDown();
    void moveLeft();
    void moveRight();

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onNewVideoFrame(const QVideoFrame &frame);
    void onTransparencyChanged(int value);

private:
    void updateTransform();          // Updates the transformation matrix
    void enforceImageLimits();       // Ensures panning does not exceed image boundaries

    Ui::CameraWidget *ui;            // Pointer to the UI
    QCamera *camera;                 // Camera instance
    QMediaCaptureSession *captureSession;   // Media capture session
    QImage currentFrame;             // Current frame as QImage
    QTransform transformMatrix;      // Transformation matrix for zoom and pan
    float zoomFactor;                // Current zoom factor
    float panX;                      // Horizontal pan offset
    float panY;                      // Vertical pan offset
    float transparency;              // Transparency level of the image (0.0 - 1.0)
};

#endif // CAMERAWIDGET_H
