#ifndef DINASTYAPLAYER_H
#define DINASTYAPLAYER_H

#include <QtGui/QGraphicsProxyWidget>

namespace Phonon {
class MediaObject;
class AudioOutput;
}

class QLabel;
class QPushButton;
class QSlider;
class QTemporaryFile;

class DinastyaPlayer : public QGraphicsProxyWidget
{
    Q_OBJECT
public:
    enum { Type = QGraphicsItem::UserType + 4 };
    explicit DinastyaPlayer(QGraphicsItem *parent = 0);
    int type() const { return Type; }

private slots:
    void togglePlayback();
    void seek(int position);
    void updatePosition(qint64 position);
    void updateDuration(qint64 duration);
    void updatePlaybackState();
    void toggleMute();
    void setVolume(int volume);

private:
    QString formatTime(qint64 milliseconds) const;

    Phonon::MediaObject *mediaObject;
    Phonon::AudioOutput *audioOutput;
    QTemporaryFile *audioFile;
    QPushButton *playButton;
    QPushButton *muteButton;
    QSlider *positionSlider;
    QSlider *volumeSlider;
    QLabel *positionLabel;
    QLabel *durationLabel;
    qint64 duration;
};

#endif // DINASTYAPLAYER_H
