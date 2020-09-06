#include "DialogSetting.h"
#include "ui_DialogSetting.h"
#include "./InputSource/ScreenSource.h"
#include <QFileDialog>
#include <QFontMetrics>
#include <QStandardPaths>

DialogSetting::DialogSetting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSetting),
    m_video(VideoSynthesizer::instance())
{
    ui->setupUi(this);
    m_verticalScrollBar = ui->scrollArea->verticalScrollBar();
    connect(m_verticalScrollBar, &QScrollBar::valueChanged, this, &DialogSetting::on_scrollArea_valueChanged);
    m_timResolution = new QTimer(this);
    m_timResolution->setInterval(100);
    m_timResolution->setSingleShot(true);
    connect(m_timResolution, &QTimer::timeout, this, &DialogSetting::on_resolution_changed);

    QRegExp regx("[^//\\\\:*?\"<>|]+$");
    ui->lineEdit_Filename->setValidator(new QRegExpValidator(regx, this));

    setParamsToUi();
}

DialogSetting::~DialogSetting()
{
    delete ui;
}

bool DialogSetting::loadProfile()
{
    QString optDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);


    if (!optDir.isEmpty())
    {
        if (!QFile::exists(optDir))
        {
            if (!QDir().mkpath(optDir))
            {
                optDir.clear();
            }
        }
    }
    if (optDir.isEmpty())
    {
        optDir = QApplication::applicationDirPath();
    }
    userSetting().profile = optDir + "/setting.ini";
    QSettings ini(userSetting().profile, QSettings::IniFormat);
    ini.beginGroup("FileSave");
    userSetting().videoDir = ini.value("dir").toString();
    if (userSetting().videoDir.isEmpty())
    {
        userSetting().videoDir = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation);
        ini.setValue("dir", userSetting().videoDir);
    }
    userSetting().fileName = ini.value("name").toString();
    if (userSetting().fileName.isEmpty())
    {
        userSetting().fileName = QApplication::applicationName();
        ini.setValue("name", userSetting().fileName);
    }
    userSetting().fileType = ini.value("type").toString();
    if (userSetting().fileType.isEmpty())
    {
        userSetting().fileType = "mp4";
        ini.setValue("type", userSetting().fileType);
    }
    ini.endGroup();

    VideoSynthesizer& vid = VideoSynthesizer::instance();
    ini.beginGroup("Video");
    vid.setSize(ini.value("width").toInt(), ini.value("height").toInt());
    vid.setFrameRate(ini.value("fps").toFloat());
    vid.setBitrate(ini.value("bps").toInt());
    vid.setBitrateMode(EVideoRateMode(ini.value("bps-mode").toInt()));
    vid.setPreset(EVideoPreset_x264(ini.value("preset").toInt()));
    vid.setGopMax(ini.value("gop-max").toInt());
    ScreenSource::setRecordCursor(ini.value("recCursor").toBool());
    ini.endGroup();
    return true;
}

bool DialogSetting::saveProfile()
{
    QSettings ini(userSetting().profile, QSettings::IniFormat);
    ini.beginGroup("FileSave");

    if (userSetting().videoDir.isEmpty())
        userSetting().videoDir = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation);
    ini.setValue("dir", userSetting().videoDir);

    if (userSetting().fileName.isEmpty())
        userSetting().fileName = QApplication::applicationName();
    ini.setValue("name", userSetting().fileName);

    if (userSetting().fileType.isEmpty())
        userSetting().fileType = "mp4";
    ini.setValue("type", userSetting().fileType);

    ini.endGroup();

    VideoSynthesizer& vid = VideoSynthesizer::instance();
    ini.beginGroup("Video");
    ini.setValue("width", vid.width());
    ini.setValue("height", vid.height());

    ini.setValue("fps", vid.frameRate());
    ini.setValue("bps", vid.bitrate());
    ini.setValue("bps-mode", vid.bitrateMode());
    ini.setValue("preset", vid.preset());
    ini.setValue("gop-max", vid.gopMax());
    ini.setValue("recCursor", ScreenSource::isRecordCursor());

    ini.endGroup();
    return true;
}

void DialogSetting::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    startTimer(200);
}

void DialogSetting::timerEvent(QTimerEvent *event)
{
    killTimer(event->timerId());
    QFontMetrics fm(ui->label_Folder->font(), ui->label_Folder);
    ui->label_Folder->setText(fm.elidedText(userSetting().videoDir, Qt::ElideMiddle, ui->label_Folder->width()));
}

void DialogSetting::on_pushButton_Video_clicked(bool checked)
{
    Q_UNUSED(checked)
    m_verticalScrollBar->setValue(ui->groupBox_Video->geometry().top());
}

void DialogSetting::on_pushButton_Encode_clicked(bool checked)
{
    Q_UNUSED(checked)
    m_verticalScrollBar->setValue(ui->groupBox_Encode->geometry().top());
}

void DialogSetting::on_pushButton_Audio_clicked(bool checked)
{
    Q_UNUSED(checked)
    m_verticalScrollBar->setValue(ui->groupBox_Audio->geometry().top());
}

void DialogSetting::on_pushButton_Save_clicked(bool checked)
{
    Q_UNUSED(checked)
    m_verticalScrollBar->setValue(ui->groupBox_Save->geometry().top());
}

void DialogSetting::on_pushButton_Readme_clicked(bool checked)
{
    Q_UNUSED(checked)
    m_verticalScrollBar->setValue(ui->groupBox_Readme->geometry().top());
}


void DialogSetting::on_pushButton_Resolution_16_9_clicked(bool checked)
{
    Q_UNUSED(checked)
    QSize si = pixelCountToSize(ui->spinBox_Width->value() * ui->spinBox_Height->value(), 16, 9);
    setResparamToUi(si.width(), si.height(), 0);
    m_timResolution->start();
}

void DialogSetting::on_pushButton_Resolution_9_16_clicked(bool checked)
{
    Q_UNUSED(checked)
    QSize si = pixelCountToSize(ui->spinBox_Width->value() * ui->spinBox_Height->value(), 9, 16);
    setResparamToUi(si.width(), si.height(), 1);
    m_timResolution->start();
}

void DialogSetting::on_pushButton_Resolution_4_3_clicked(bool checked)
{
    Q_UNUSED(checked)
    QSize si = pixelCountToSize(ui->spinBox_Width->value() * ui->spinBox_Height->value(), 4, 3);
    setResparamToUi(si.width(), si.height(), 2);
    m_timResolution->start();
}

void DialogSetting::on_pushButton_Resolution_3_4_clicked(bool checked)
{
    Q_UNUSED(checked)
    QSize si = pixelCountToSize(ui->spinBox_Width->value() * ui->spinBox_Height->value(), 3, 4);
    setResparamToUi(si.width(), si.height(), 3);
    m_timResolution->start();
}

void DialogSetting::on_pushButton_Resolution_Userdef_clicked(bool checked)
{
    Q_UNUSED(checked)
    ui->horizontalSlider_Resolution->setDisabled(true);
    ui->horizontalSlider_Resolution->setValue(ui->horizontalSlider_Resolution->minimum());
}

void DialogSetting::on_horizontalSlider_Resolution_valueChanged(int value)
{
    if(ui->horizontalSlider_Resolution->isEnabled())
    {
        m_resolutionChanging = true;
        int w = ui->spinBox_Width->minimum() + (ui->spinBox_Width->maximum() - ui->spinBox_Width->minimum()) * value / 1000;
        int h = ui->spinBox_Height->minimum() + (ui->spinBox_Height->maximum() - ui->spinBox_Height->minimum()) * value / 1000;
        ui->spinBox_Width->setValue(w);
        ui->spinBox_Height->setValue(h);
        m_resolutionChanging = false;
        m_timResolution->start();
    }
}

void DialogSetting::on_spinBox_Width_valueChanged(int arg1)
{
    if(!m_resolutionChanging && ui->horizontalSlider_Resolution->isEnabled())
    {
        m_resolutionChanging = true;
        int h = (arg1 * ui->spinBox_Height->minimum() * 10 / ui->spinBox_Width->minimum() + 5) / 10;
        ui->spinBox_Height->setValue(h);
        m_resolutionChanging = false;
        m_timResolution->start();
    }
}

void DialogSetting::on_spinBox_Height_valueChanged(int arg1)
{
    if(!m_resolutionChanging && ui->horizontalSlider_Resolution->isEnabled())
    {
        m_resolutionChanging = true;
        int w = (arg1 * ui->spinBox_Width->minimum() * 10 / ui->spinBox_Height->minimum() + 5) / 10;
        ui->spinBox_Width->setValue(w);
        m_resolutionChanging = false;
        m_timResolution->start();
    }
}

void DialogSetting::on_horizontalSlider_Fps_valueChanged(int value)
{
    if (ui->horizontalSlider_Fps->isEnabled())
        ui->doubleSpinBox_Fps->setValue(value);
}

void DialogSetting::on_doubleSpinBox_Fps_valueChanged(double arg1)
{
    if (qRound(arg1) != ui->horizontalSlider_Fps->value())
    {
        ui->horizontalSlider_Fps->setDisabled(true);
        ui->horizontalSlider_Fps->setValue(qRound(arg1));
        ui->horizontalSlider_Fps->setEnabled(true);
    }
    m_video.setFrameRate(static_cast<float>(arg1));
    m_video.setGopMax(int(ui->doubleSpinBox_Keyframe->value() * qreal(m_video.frameRate())));
}

void DialogSetting::on_checkBox_RecCursor_clicked(bool checked)
{
    if (!ScreenSource::setRecordCursor(checked))
    {
        if (checked) ui->checkBox_RecCursor->setChecked(false);
    }
}

void DialogSetting::on_horizontalSlider_Bps_valueChanged(int value)
{
    if (ui->horizontalSlider_Bps->isEnabled())
        ui->spinBox_Bps->setValue(value);
}

void DialogSetting::on_spinBox_Bps_valueChanged(int arg1)
{
    ui->horizontalSlider_Bps->setDisabled(true);
    ui->horizontalSlider_Bps->setValue(arg1);
    ui->horizontalSlider_Bps->setEnabled(true);
    m_video.setBitrate(arg1);
}

void DialogSetting::on_comboBox_Bps_currentIndexChanged(int index)
{
    m_video.setBitrateMode(EVideoRateMode(index));
}

void DialogSetting::on_horizontalSlider_Preset_valueChanged(int value)
{
    m_video.setPreset(EVideoPreset_x264(value));
    ui->label_VidPreset->setText(QString("[%1]").arg(video_preset_x264_names[value]));
}

void DialogSetting::on_doubleSpinBox_Keyframe_valueChanged(double arg1)
{
    m_video.setGopMax(int(arg1 * double(m_video.frameRate())));
}

void DialogSetting::on_pushButton_Folder_clicked()
{
    QFileDialog folder(this);
    folder.setFileMode(QFileDialog::Directory);
    folder.setWindowTitle("选择保存视频的文件夹");
    folder.setAcceptMode(QFileDialog::AcceptOpen);
    folder.setOption(QFileDialog::ShowDirsOnly);
    if (folder.exec()==QDialog::Accepted)
    {
        QStringList dirs = folder.selectedFiles();
        if (dirs.isEmpty())
            return;
        userSetting().videoDir = dirs[0];
        ui->label_Folder->setToolTip(userSetting().videoDir);
        QFontMetrics fm(ui->label_Folder->font(), ui->label_Folder);
        ui->label_Folder->setText(fm.elidedText(userSetting().videoDir, Qt::ElideMiddle, ui->label_Folder->width()));
    }
}

void DialogSetting::on_scrollArea_valueChanged(int value)
{
    int vid = ui->groupBox_Video->geometry().bottom();
    int enc = ui->groupBox_Encode->geometry().bottom();
    int aud = ui->groupBox_Audio->geometry().bottom();
    int sav = ui->groupBox_Save->geometry().bottom();
    int rea = ui->groupBox_Readme->geometry().bottom();

    if (value < ui->groupBox_Video->geometry().top() + 8)
    {
        qDebug() << value << "pushButton_Video";
        ui->pushButton_Video->setChecked(true);
    }
    else if ((value >= vid) && (value < enc))
    {
        qDebug() << value << "pushButton_Encode";
        ui->pushButton_Encode->setChecked(true);
    }
    else if ((value >= enc) && (value < aud))
    {
        qDebug() << value << "pushButton_Audio";
        ui->pushButton_Audio->setChecked(true);
    }
    else if ((value >= aud) && (value < sav))
    {
        qDebug() << value << "pushButton_Save";
        ui->pushButton_Save->setChecked(true);
    }
    else if ((value >= sav - 20) && (value < rea))
    {
        qDebug() << value << "pushButton_Readme";
        ui->pushButton_Readme->setChecked(true);
    }
}

void DialogSetting::on_resolution_changed()
{
    m_video.setSize(ui->spinBox_Width->value(), ui->spinBox_Height->value());
}

int DialogSetting::checkSizeRatio(int32_t w, int32_t h)
{
    float sw = h * 16.0f / 9.0f;
    float sh = w * 9.0f / 16.0f;
    if ( qAbs(sw - w) <= 1.0f || qAbs(sh - h) <= 1.0f )
        return 0;

    sw = h * 9.0f / 16.0f;
    sh = w * 16.0f / 9.0f;
    if ( qAbs(sw - w) <= 1.0f || qAbs(sh - h) <= 1.0f )
        return 1;

    sw = h * 4.0f / 3.0f;
    sh = w * 3.0f / 4.0f;
    if ( qAbs(sw - w) <= 1.0f || qAbs(sh - h) <= 1.0f )
         return 2;

    sw = h * 3.0f / 4.0f;
    sh = w * 4.0f / 3.0f;
    if ( qAbs(sw - w) <= 1.0f || qAbs(sh - h) <= 1.0f )
         return 3;

    return -1;
}

QSize DialogSetting::pixelCountToSize(int32_t c, int32_t w, int32_t h)
{
    qreal s = qSqrt(static_cast<qreal>(c) / (w*h));
    w = qRound(s * w);
    h = qRound(s * h);
    return QSize(w, h);
}

void DialogSetting::setResparamToUi(int32_t w, int32_t h, int32_t r)
{
    if (w <= 0 || h <= 0) return;
    int minW = 240, minH, maxW, maxH;
    int ratio = r < 0 ? checkSizeRatio(w, h) : r;

    switch(ratio)
    {
    case 0:
        minH = minW * 9 / 16;
        if (r < 0 ) ui->pushButton_Resolution_16_9->setChecked(true);
        break;
    case 1:
        minH = minW; minW = minW * 9 / 16;
        if (r < 0 ) ui->pushButton_Resolution_9_16->setChecked(true);
        break;
    case 2:
        minH = minW * 3 / 4;
        if (r < 0 ) ui->pushButton_Resolution_4_3->setChecked(true);
        break;
    case 3:
        minH = minW; minW = minW * 3 / 4;
        if (r < 0 ) ui->pushButton_Resolution_3_4->setChecked(true);
        break;
    default:
        minH = minW;
        if (r < 0 ) ui->pushButton_Resolution_Userdef->setChecked(true);
        ui->horizontalSlider_Resolution->setDisabled(true);
        ui->horizontalSlider_Resolution->setValue(ui->horizontalSlider_Resolution->minimum());
    }
    maxW = minW * 16; maxH = minH * 16;
    ui->spinBox_Width->setDisabled(true);
    ui->spinBox_Height->setDisabled(true);
    if (ratio >= 0) ui->horizontalSlider_Resolution->setDisabled(true);

    ui->spinBox_Width->setRange(minW, maxW);
    ui->spinBox_Height->setRange(minH, maxH);
    w = qMin( qMax(w, minW), maxW );
    h = qMin( qMax(h, minH), maxH );
    ui->spinBox_Width->setValue(w);
    ui->spinBox_Height->setValue(h);
    if (ratio >= 0)
        ui->horizontalSlider_Resolution->setValue((w - minW) * 1000 / (maxW - minW));

    ui->spinBox_Width->setDisabled(false);
    ui->spinBox_Height->setDisabled(false);
    if (ratio >= 0) ui->horizontalSlider_Resolution->setDisabled(false);
}

void DialogSetting::setParamsToUi()
{
    setResparamToUi(m_video.width(), m_video.height());
    ui->doubleSpinBox_Keyframe->setValue(qreal(m_video.gopMax() / m_video.frameRate()));
    ui->doubleSpinBox_Fps->setValue(double(m_video.frameRate()));
    ui->checkBox_RecCursor->setChecked(ScreenSource::isRecordCursor());
    ui->spinBox_Bps->setValue(m_video.bitrate());
    ui->comboBox_Bps->setCurrentIndex(m_video.bitrateMode());
    ui->horizontalSlider_Preset->setValue(m_video.preset());
    ui->label_VidPreset->setText(QString("[%1]").arg(video_preset_x264_names[m_video.preset()]));

    ui->label_Folder->setToolTip(userSetting().videoDir);
    QFontMetrics fm(ui->label_Folder->font(), ui->label_Folder);
    ui->label_Folder->setText(fm.elidedText(userSetting().videoDir, Qt::ElideMiddle, ui->label_Folder->width()));
    ui->lineEdit_Filename->setText(userSetting().fileName);
    if (userSetting().fileType.compare("flv", Qt::CaseInsensitive) == 0)
        ui->radioButton_FileFlv->setChecked(true);
    else if (userSetting().fileType.compare("mp4", Qt::CaseInsensitive) == 0)
        ui->radioButton_FileMp4->setChecked(true);

}

void DialogSetting::on_radioButton_FileMp4_clicked(bool checked)
{
    userSetting().fileType = "mp4";
}

void DialogSetting::on_radioButton_FileFlv_clicked(bool checked)
{
    userSetting().fileType = "flv";
}

void DialogSetting::on_lineEdit_Filename_editingFinished()
{
    userSetting().fileName = ui->lineEdit_Filename->text();
    if (userSetting().fileName.isEmpty())
    {
        userSetting().fileName = QApplication::applicationName();
        ui->lineEdit_Filename->setText(userSetting().fileName);
    }
}
