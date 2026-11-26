#include "dinastyaplayer.h"

#include <QtMultimedia/QMediaContent>
#include <QtMultimedia/QMediaPlayer>
#include <QFile>
#include <QDir>
#include <QTemporaryFile>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QVBoxLayout>
#include <QApplication>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QCoreApplication>
#include "mycanvas.h"
#include "movecommand.h"

DinastyaPlayer::DinastyaPlayer(QGraphicsItem *parent)
    : QGraphicsProxyWidget(parent), mediaObject(new QMediaPlayer(this)),
      audioFile(new QTemporaryFile(this)), duration(0),
      rightButtonDown(false), rightDragStarted(false)
{
    QWidget *panel = new QWidget;
    panel->setFixedSize(700, 100);
    panel->setStyleSheet("QWidget#dinastyaPanel { background-color: #602719; }"
                         "QLabel { color: #f6eee9; background: transparent; }"
                         "QPushButton { color: #f6eee9; background: transparent; border: 0; }"
                         "QPushButton#playButton { background-color: #ffd23f; color: #602719;"
                         " border-radius: 22px; font-size: 18px; font-weight: bold; }"
                         "QSlider::groove:horizontal { height: 6px; background: #b88c7d;"
                         " border-radius: 3px; }"
                         "QSlider::sub-page:horizontal { background: #f6eee9; border-radius: 3px; }"
                         "QSlider::handle:horizontal { width: 14px; margin: -5px 0;"
                         " background: #ffffff; border-radius: 7px; }");
    panel->setObjectName("dinastyaPanel");
    setWidget(panel);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setZValue(100);

    QLabel *cover = new QLabel(panel);
    cover->setFixedSize(72, 72);
    QPixmap coverImage(":/im/cover-dinastiya.jpg");
    if (!coverImage.isNull())
        cover->setPixmap(coverImage.scaled(72, 72, Qt::KeepAspectRatioByExpanding,
                                           Qt::SmoothTransformation));

    QLabel *title = new QLabel(tr("ДИНАСТИЯ"), panel);
    QFont titleFont = title->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    title->setFont(titleFont);
    QLabel *artist = new QLabel(tr("VILLIAN, madk1d"), panel);
    artist->setStyleSheet("color: #c9a99e;");
    QVBoxLayout *trackLayout = new QVBoxLayout;
    trackLayout->setSpacing(5);
    trackLayout->addWidget(title);
    trackLayout->addWidget(artist);

    playButton = new QPushButton(QString::fromUtf8("▶"), panel);
    playButton->setObjectName("playButton");
    playButton->setFixedSize(44, 44);
    playButton->setToolTip(tr("Play"));
    connect(playButton, SIGNAL(clicked()), this, SLOT(togglePlayback()));

    positionLabel = new QLabel("0:00", panel);
    durationLabel = new QLabel("0:00", panel);
    positionSlider = new QSlider(Qt::Horizontal, panel);
    positionSlider->setRange(0, 0);
    connect(positionSlider, SIGNAL(sliderMoved(int)), this, SLOT(seek(int)));
    connect(mediaObject, SIGNAL(positionChanged(qint64)), this, SLOT(updatePosition(qint64)));
    connect(mediaObject, SIGNAL(durationChanged(qint64)), this, SLOT(updateDuration(qint64)));
    connect(mediaObject, SIGNAL(stateChanged(QMediaPlayer::State)),
            this, SLOT(updatePlaybackState()));
    connect(mediaObject, SIGNAL(error(QMediaPlayer::Error)),
            this, SLOT(handleMediaError()));

    QHBoxLayout *progressLayout = new QHBoxLayout;
    progressLayout->setSpacing(10);
    progressLayout->addWidget(positionLabel);
    progressLayout->addWidget(positionSlider, 1);
    progressLayout->addWidget(durationLabel);
    QVBoxLayout *playbackLayout = new QVBoxLayout;
    playbackLayout->setSpacing(5);
    playbackLayout->addWidget(playButton, 0, Qt::AlignHCenter);
    playbackLayout->addLayout(progressLayout);

    muteButton = new QPushButton(QString::fromUtf8("♫"), panel);
    muteButton->setFixedWidth(28);
    muteButton->setToolTip(tr("Mute"));
    volumeSlider = new QSlider(Qt::Horizontal, panel);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(80);
    volumeSlider->setFixedWidth(90);
    mediaObject->setVolume(80);
    connect(volumeSlider, SIGNAL(valueChanged(int)), this, SLOT(setVolume(int)));
    connect(muteButton, SIGNAL(clicked()), this, SLOT(toggleMute()));
    QHBoxLayout *volumeLayout = new QHBoxLayout;
    volumeLayout->setSpacing(6);
    volumeLayout->addWidget(muteButton);
    volumeLayout->addWidget(volumeSlider);

    QHBoxLayout *layout = new QHBoxLayout(panel);
    layout->setContentsMargins(14, 10, 14, 10);
    layout->setSpacing(18);
    layout->addWidget(cover);
    layout->addLayout(trackLayout, 0);
    layout->addStretch(1);
    layout->addLayout(playbackLayout, 4);
    layout->addLayout(volumeLayout, 0);

    QFile resource(":/im/audio-dinastiya.mp3");
    audioFile->setFileTemplate(QDir::tempPath() + "/netemul-dinastya-XXXXXX.mp3");
    if (resource.open(QIODevice::ReadOnly) && audioFile->open()) {
        const QByteArray audioData = resource.readAll();
        if (audioFile->write(audioData) == audioData.size() && audioFile->flush()) {
            const QString audioPath = audioFile->fileName();
            audioFile->close();
            mediaObject->setMedia(QMediaContent(QUrl::fromLocalFile(audioPath)));
        } else {
            playButton->setEnabled(false);
            playButton->setToolTip(tr("Unable to prepare the audio file"));
            qWarning() << "Unable to write the Dinastya audio resource:" << audioFile->errorString();
        }
    } else {
        playButton->setEnabled(false);
        playButton->setToolTip(tr("Audio file is unavailable"));
        qWarning() << "Unable to open the Dinastya audio resource or temporary file:"
                   << resource.errorString() << audioFile->errorString();
    }
}

void DinastyaPlayer::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        rightButtonDown = true;
        rightDragStarted = false;
        rightDragStartPos = pos();
        rightDragOffset = event->scenePos() - pos();
        if (scene()) {
            myCanvas *canvas = static_cast<myCanvas*>(scene());
            canvas->turnToMove();
            scene()->clearSelection();
        }
        setSelected(true);
        event->accept();
        return;
    }
    QGraphicsProxyWidget::mousePressEvent(event);
}

void DinastyaPlayer::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (rightButtonDown && (event->buttons() & Qt::RightButton)) {
        if (!rightDragStarted &&
                QLineF(event->scenePos(), event->buttonDownScenePos(Qt::RightButton)).length()
                >= QApplication::startDragDistance())
            rightDragStarted = true;
        if (rightDragStarted)
            setPos(event->scenePos() - rightDragOffset);
        event->accept();
        return;
    }
    QGraphicsProxyWidget::mouseMoveEvent(event);
}

void DinastyaPlayer::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::RightButton && rightButtonDown) {
        rightButtonDown = false;
        if (rightDragStarted && scene()) {
            myCanvas *canvas = static_cast<myCanvas*>(scene());
            canvas->recordPlayerMove(this, rightDragStartPos, pos());
        } else if (!rightDragStarted) {
            QMenu menu;
            QAction *deleteAction = menu.addAction(QCoreApplication::translate("MainWindow", "Delete"));
            if (menu.exec(event->screenPos()) == deleteAction && scene()) {
                myCanvas *canvas = static_cast<myCanvas*>(scene());
                canvas->deletePlayer(this);
            }
        }
        rightDragStarted = false;
        event->accept();
        return;
    }
    QGraphicsProxyWidget::mouseReleaseEvent(event);
}

void DinastyaPlayer::stopPlayback()
{
    if (mediaObject->state() != QMediaPlayer::StoppedState)
        mediaObject->stop();
}

void DinastyaPlayer::togglePlayback()
{
    if (mediaObject->state() == QMediaPlayer::PlayingState) {
        mediaObject->pause();
        return;
    }
    if (mediaObject->state() == QMediaPlayer::StoppedState && duration > 0)
        mediaObject->setPosition(0);
    mediaObject->play();
}

void DinastyaPlayer::seek(int position)
{
    mediaObject->setPosition(position);
}

void DinastyaPlayer::updatePosition(qint64 position)
{
    positionSlider->blockSignals(true);
    positionSlider->setValue(static_cast<int>(position));
    positionSlider->blockSignals(false);
    positionLabel->setText(formatTime(position));
}

void DinastyaPlayer::updateDuration(qint64 milliseconds)
{
    duration = milliseconds;
    positionSlider->setRange(0, static_cast<int>(milliseconds));
    durationLabel->setText(formatTime(milliseconds));
}

void DinastyaPlayer::updatePlaybackState()
{
    const bool playing = mediaObject->state() == QMediaPlayer::PlayingState;
    playButton->setText(playing ? QString::fromUtf8("Ⅱ") : QString::fromUtf8("▶"));
    playButton->setToolTip(playing ? tr("Pause") : tr("Play"));
}

void DinastyaPlayer::handleMediaError()
{
    const QString error = mediaObject->errorString();
    playButton->setEnabled(false);
    playButton->setToolTip(error.isEmpty() ? tr("Unable to play audio") : error);
    qWarning() << "Dinastya audio playback failed:" << error;
}

void DinastyaPlayer::toggleMute()
{
    const bool muted = mediaObject->isMuted();
    mediaObject->setMuted(!muted);
    muteButton->setText(muted ? QString::fromUtf8("♫") : QString::fromUtf8("×"));
    muteButton->setToolTip(muted ? tr("Mute") : tr("Unmute"));
}

void DinastyaPlayer::setVolume(int volume)
{
    mediaObject->setVolume(volume);
}

QString DinastyaPlayer::formatTime(qint64 milliseconds) const
{
    const int seconds = static_cast<int>(milliseconds / 1000);
    return QString::number(seconds / 60) + ":" +
            (seconds % 60 < 10 ? "0" : "") + QString::number(seconds % 60);
}
