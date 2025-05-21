// #include "camerawidget.h"
// #include "qmediacapturesession.h"
// #include <QPainter>
// #include <QMediaDevices>
// #include <QCamera>
// #include <QGraphicsOpacityEffect>

// #include <QResizeEvent>
// #include <QTransform>

// CameraWidget::CameraWidget(QWidget *parent)
//     : QWidget(parent),
//     camera(nullptr),
//     captureSession(new QMediaCaptureSession(this)),
//     videoWidget(new QVideoWidget(this)),
//     zoomFactor(1.0f),
//     panX(0.0f),
//     panY(0.0f)
// {
//     // Setup video widget
//     videoWidget->setGeometry(0, 0, width(), height());

//     // Select the default camera
//     QCameraDevice cameraDevice = QMediaDevices::defaultVideoInput();
//     if (!cameraDevice.isNull()) {
//         qDebug() << "Camera detected: " << cameraDevice.description();
//         camera = new QCamera(cameraDevice, this);

//         // Configure the media capture session
//         captureSession->setCamera(camera);
//         captureSession->setVideoOutput(videoWidget);

//         camera->start();
//     } else {
//         qCritical() << "No camera detected!";
//     }
// }

// CameraWidget::~CameraWidget()
// {
//     if (camera) {
//         camera->stop();
//         delete camera;
//     }
//     delete captureSession;
//     delete videoWidget;
// }

// void CameraWidget::resizeEvent(QResizeEvent *event)
// {
//     QWidget::resizeEvent(event);
//     if (videoWidget) {
//         videoWidget->setGeometry(0, 0, width(), height());
//     }
// }

// void CameraWidget::paintEvent(QPaintEvent *event)
// {
//     Q_UNUSED(event);

//     // Use QPainter to draw the transformed video
//     if (!videoWidget) {
//         return;
//     }

//     QPainter painter(this);
//     painter.setRenderHint(QPainter::SmoothPixmapTransform);

//     // Apply the transformation matrix
//     painter.setTransform(transformMatrix);

//     // Draw the video widget
//     videoWidget->render(&painter);
// }

// void CameraWidget::updateTransform()
// {
//     // Reset the transformation matrix
//     transformMatrix.reset();

//     // Apply zoom
//     transformMatrix.scale(zoomFactor, zoomFactor);

//     // Apply pan
//     transformMatrix.translate(panX, panY);

//     // Request a repaint
//     update();
// }

// void CameraWidget::zoomIn()
// {
//     qDebug() << "Zooming in...";
//     zoomFactor = qMin(zoomFactor * 1.1f, 5.0f); // Max zoom = 5x
//     updateTransform();
// }

// void CameraWidget::zoomOut()
// {
//     qDebug() << "Zooming out...";
//     zoomFactor = qMax(zoomFactor * 0.9f, 0.5f); // Min zoom = 0.5x
//     updateTransform();
// }

// void CameraWidget::moveUp()
// {
//     qDebug() << "Moving up...";
//     panY -= 10 / zoomFactor; // Adjust pan by zoom level
//     updateTransform();
// }

// void CameraWidget::moveDown()
// {
//     qDebug() << "Moving down...";
//     panY += 10 / zoomFactor; // Adjust pan by zoom level
//     updateTransform();
// }

// void CameraWidget::moveLeft()
// {
//     qDebug() << "Moving left...";
//     panX -= 10 / zoomFactor; // Adjust pan by zoom level
//     updateTransform();
// }

// void CameraWidget::moveRight()
// {
//     qDebug() << "Moving right...";
//     panX += 10 / zoomFactor; // Adjust pan by zoom level
//     updateTransform();
// }

/**
 * @file camerawidget.cpp
 * @brief Implementation of the CameraWidget class for real-time camera feed display with zoom, pan, and transparency controls.
 *
 * The CameraWidget class enables capturing video frames from the connected camera, displaying them,
 * and allowing interactive zoom, pan, and transparency adjustments. It uses the Qt Multimedia framework for
 * camera operations and the QPainter class for rendering the video frames onto the widget.
 */

#include "camerawidget.h"
#include <QPainter>
#include <QMediaDevices>
#include <QVideoSink>
#include <QDebug>

/**
 * @brief Constructor for the CameraWidget class.
 *
 * Initializes the camera and sets up the video capture session. Also initializes default values for zoom,
 * pan, and transparency settings.
 *
 * @param parent Pointer to the parent widget. Default is nullptr.
 */

CameraWidget::CameraWidget(QWidget *parent)
    : QWidget(parent),
    camera(nullptr),
    captureSession(new QMediaCaptureSession(this)),
    zoomFactor(0.22f),
    panX(0.0f),
    panY(0.0f),
    transparency(1.0f)
{
    //ui->setupUi(this);

    // Setup camera
    QCameraDevice cameraDevice = QMediaDevices::defaultVideoInput();
    if (!cameraDevice.isNull()) {
        qDebug() << "Camera detected: " << cameraDevice.description();
        camera = new QCamera(cameraDevice, this);

        // Configure the media capture session
        captureSession->setCamera(camera);

        // Set up a QVideoSink for capturing video frames
        QVideoSink *videoSink = new QVideoSink(this);
        captureSession->setVideoOutput(videoSink);

        connect(videoSink, &QVideoSink::videoFrameChanged, this, &CameraWidget::onNewVideoFrame);

        camera->start();
    } else {
        qCritical() << "No camera detected!";
    }
}

/**
 * @brief Destructor for the CameraWidget class.
 *
 * Stops and cleans up the camera and capture session resources.
 */

CameraWidget::~CameraWidget()
{
    if (camera) {
        camera->stop();
        delete camera;
    }
    delete captureSession;
}

/**
 * @brief Handles new video frames from the camera.
 *
 * Converts the video frame to a QImage and stores a copy for rendering.
 *
 * @param frame The new video frame.
 */

void CameraWidget::onNewVideoFrame(const QVideoFrame &frame)
{
    QVideoFrame copyFrame = frame;

    if (!copyFrame.isValid()) {
        qWarning() << "Invalid video frame.";
        return;
    }

    if (!copyFrame.map(QVideoFrame::ReadOnly)) {
        qWarning() << "Failed to map video frame.";
        return;
    }

    QImage image = copyFrame.toImage();
    if (!image.isNull()) {
        currentFrame = image.copy();  // Create a copy to avoid issues with the frame's lifecycle
    }

    copyFrame.unmap();
    update();  // Trigger a repaint
}

/**
 * @brief Paints the camera feed onto the widget.
 *
 * Applies zoom, pan, and transparency settings before rendering the video frame.
 *
 * @param event The paint event.
 */

void CameraWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    if (!currentFrame.isNull()) {
        // Set transparency
        painter.setOpacity(transparency);

        // Apply the transformation matrix
        painter.setTransform(transformMatrix);

        // Draw the current frame
        QRect targetRect(0, 0, currentFrame.width(), currentFrame.height());
        painter.drawImage(targetRect, currentFrame);
    }
}

/**
 * @brief Sets the transparency level for the video display.
 *
 * @param value Transparency value as an integer (0–100).
 */

void CameraWidget::onTransparencyChanged(int value)
{
    transparency = value / 100.0f;
    update();
}

/**
 * @brief Updates the transformation matrix for zoom and pan.
 *
 * Ensures that pan values are within acceptable bounds and applies the transformations.
 */

void CameraWidget::updateTransform()
{
    enforceImageLimits();  // Ensure pan is within bounds

    // Reset the transformation matrix
    transformMatrix.reset();

    // Apply zoom
    transformMatrix.scale(zoomFactor, zoomFactor);

    // Apply pan
    transformMatrix.translate(panX, panY);

    // Request a repaint
    update();
}

/**
 * @brief Ensures that pan values are within the limits of the image boundaries.
 */

void CameraWidget::enforceImageLimits()
{
    if (currentFrame.isNull()) return;

    // Calculate image boundaries based on zoom
    float imageWidth = currentFrame.width() * zoomFactor;
    float imageHeight = currentFrame.height() * zoomFactor;

    // Widget size
    float widgetWidth = width();
    float widgetHeight = height();

    // Calculate limits
    float maxPanX = qMax(0.0f, (imageWidth - widgetWidth) / (2 * zoomFactor));
    float maxPanY = qMax(0.0f, (imageHeight - widgetHeight) / (2 * zoomFactor));

    // Clamp panX and panY within limits
    panX = qBound(-maxPanX, panX, maxPanX);
    panY = qBound(-maxPanY, panY, maxPanY);
}

/**
 * @brief Zooms in on the video display.
 */

void CameraWidget::zoomIn()
{
    zoomFactor = qMin(zoomFactor * 1.1f, 5.0f); // Max zoom = 5x
    updateTransform();
}

/**
 * @brief Zooms out of the video display.
 */

void CameraWidget::zoomOut()
{
    zoomFactor = qMax(zoomFactor * 0.9f, 0.21f); // Min zoom = 0.5x
    updateTransform();
}

/**
 * @brief Moves the view upwards.
 */

void CameraWidget::moveUp()
{
    panY += 10 / zoomFactor; // Adjust pan by zoom level
    updateTransform();
}

/**
 * @brief Moves the view downwards.
 */

void CameraWidget::moveDown()
{
    panY -= 10 / zoomFactor; // Adjust pan by zoom level
    updateTransform();
}

/**
 * @brief Moves the view to the left.
 */

void CameraWidget::moveLeft()
{
    panX += 10 / zoomFactor; // Adjust pan by zoom level
    updateTransform();
}

/**
 * @brief Moves the view to the right.
 */

void CameraWidget::moveRight()
{
    panX -= 10 / zoomFactor; // Adjust pan by zoom level
    updateTransform();
}
