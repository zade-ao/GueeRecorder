#ifndef STACKEDWIDGETADDCONTENTS_H
#define STACKEDWIDGETADDCONTENTS_H

#include <QStackedWidget>
#include <QToolButton>
#include <QCamera>
#include "InputSource/ScreenLayer.h"

#undef Bool
#undef Status

namespace Ui {
class StackedWidgetAddLayer;
}

class StackedWidgetAddLayer : public QStackedWidget
{
    Q_OBJECT

public:
    QWidget* m_mainWindow = nullptr;
    explicit StackedWidgetAddLayer(QWidget *parent = nullptr);
    ~StackedWidgetAddLayer() override;
    virtual void showEvent(QShowEvent *event) override;
    virtual void hideEvent(QHideEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void keyPressEvent(QKeyEvent* event) override;
    virtual void timerEvent(QTimerEvent* event) override;
private slots:
    void on_toolButtonScreenArea_clicked();
    void on_pushButtonAddCamera_clicked();
    void on_toolButtonAddPicture_clicked();
    void on_comboBoxCameras_currentIndexChanged(int index);
    void on_comboBoxSizeFps_currentIndexChanged(int index);
    void on_camera_statusChanged(QCamera::Status status);
    void on_StackedWidgetAddLayer_currentChanged(int arg1);

private:
    Ui::StackedWidgetAddLayer *ui;
    int32_t m_prevMS = 0;
    void showScreens();
    void showCameras();
    QCamera* m_selectedCamera = nullptr;
    QCameraViewfinderSettings m_camSetting;

    QVector<QToolButton*> m_screenButs;
    QSize rescaleButIcon(QToolButton* but, const QString& path);
    void closeCameraPreview();
signals:
    void selectedScreen(ScreenLayer::Option scrOpt);
    //void selectedCamera()
};

#endif // STACKEDWIDGETADDCONTENTS_H
