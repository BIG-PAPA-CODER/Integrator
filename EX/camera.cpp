#include "camera.h"

camera::camera(QWidget *parent)
    : QWidget(parent),
    camera(nullptr),
    videoWidget(new QVideoWidget(this)),
    opacityEffect(new QGraphicsOpacityEffect(this))
{
    // Initialize the camera
    QCameraDevice cameraDevice = QMediaDevices::defaultVideoInput();
    if (!cameraDevice.isNull()) {
        camera = new QCamera(cameraDevice, this);
        camera->setVideoOutput(videoWidget);
        videoWidget->setGraphicsEffect(opacityEffect);
        videoWidget->hide(); // Start hidden
    }
}

camera::~camera()
{
    delete camera;
}

void camera::toggleCamera()
{
    if (camera) {
        if (videoWidget->isVisible()) {
            videoWidget->hide();
            camera->stop();
        } else {
            videoWidget->show();
            camera->start();
        }
    }
}

void camera::adjustCameraTransparency(int value)
{
    if (opacityEffect) {
        opacityEffect->setOpacity(value / 100.0); // Normalize to 0.0 - 1.0
    }
}
