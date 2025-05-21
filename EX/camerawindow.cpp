#include "camerawindow.h"
#include <QDebug>

TransparentWindow::TransparentWindow(QWidget *parent) : QWidget(parent) {
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
}

void TransparentWindow::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setOpacity(opacity);
    painter.fillRect(rect(), QColor(0, 0, 0, 128)); // Semi-transparent background
}

void TransparentWindow::setOpacity(float value) {
    opacity = value;
    update(); // Trigger a repaint
    setWindowOpacity(opacity); // Set the overall window opacity (0.0 to 1.0)
}

// Implement mouse events to enable moving the window
void TransparentWindow::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        dragging = true;
        dragStartPosition = event->globalPos() - frameGeometry().topLeft();
    }
}

void TransparentWindow::mouseMoveEvent(QMouseEvent *event) {
    if (dragging) {
        move(event->globalPos() - dragStartPosition);
    }
}

void TransparentWindow::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        dragging = false;
    }
}

CameraWindow::CameraWindow(QWidget *parent) : QWidget(parent) {
    transparentWindow = new TransparentWindow;
    transparentWindow->setGeometry(9, 9, 706, 514);
    transparentWindow->setWindowTitle("Camera Video");

    videoWidget = new QVideoWidget;
    videoWidget->setMinimumSize(640, 480);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(videoWidget);
    transparentWindow->setLayout(layout);
}

CameraWindow::~CameraWindow() {
    if (camera) {
        camera->stop();
        delete camera;
    }
    delete captureSession;
    delete videoWidget;
    delete transparentWindow;
}

void CameraWindow::setCamera(QCamera *cam) {
    if (camera) {
        camera->stop(); // Stop the current camera if it's already set
        delete camera;  // Delete the old camera to free resources

        // Clear the video output and hide the window
        transparentWindow->hide(); // Hide the window when stopping the camera
        captureSession->setCamera(nullptr); // Disconnect the current camera
    }

    camera = cam;
    if (camera) {
        captureSession = new QMediaCaptureSession;
        captureSession->setCamera(camera);
        captureSession->setVideoOutput(videoWidget);

        transparentWindow->show(); // Show the window after setting the camera
        camera->start(); // Start the camera
    }
}


void CameraWindow::setOpacity(int value) {
    float opacityValue = value / 100.0; // Convert value from slider (0-100) to (0.0-1.0)
    transparentWindow->setOpacity(opacityValue); // Set opacity between 0 and 1
}

