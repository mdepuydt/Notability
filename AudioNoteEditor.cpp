#include "AudioNoteEditor.h"
#include "AudioNote.h"
#include <QWidget>
#include <QFileDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <QUrl>
#include <QProgressBar>

AudioNoteEditor::AudioNoteEditor(AudioNote* a, QWidget *parent)
    :BinaryEditor(a, parent), ressource(a)
{
    btnAddAudio = new QPushButton("Choose a Song");

    player = new QMediaPlayer;
    playlist = new QMediaPlaylist(player);

    if(!ressource->getMediaPath().isNull())
    {
        playlist->addMedia(QUrl::fromLocalFile(ressource->getMediaPath()));
        player->setMedia(QUrl::fromLocalFile(ressource->getMediaPath()));
        playlist->setCurrentIndex(0);
        player->setPlaylist(playlist);
        play = false;
        btnPlayPause = new QPushButton("PLay/Pause");
        btnStop = new QPushButton("Stop");
        btnStop->resize(32,32);
        btnStop->setIcon(QIcon(":images/stop"));
        progression = new QProgressBar;
        progression->setMinimum(0);
        progression->setMaximum(100);
        btnPlayPause->setIcon(QIcon(":images/play"));
        QObject::connect(btnPlayPause, SIGNAL(clicked()), this, SLOT(PLAY_PAUSE_SONG()));
        QObject::connect(btnStop, SIGNAL(clicked()), this, SLOT(STOP_SONG()));
        QObject::connect(player, SIGNAL(positionChanged(qint64)), this, SLOT(SET_VALUE(qint64)));
    }

    btnLine = new QHBoxLayout;
    contentLayout->addWidget(progression);
    contentLayout->addLayout(btnLine);
    btnLine->addWidget(btnPlayPause);
    btnLine->addWidget(btnStop);


    contentLayout->addWidget(new QLabel("Description:"));

    contentLayout->addWidget(getDescriptionWidget());
    buttonsLayout->addWidget(btnAddAudio);

    QObject::connect(btnAddAudio, SIGNAL(clicked()), this, SLOT(LOAD_AUDIO()));
    QObject::connect(this, SIGNAL(destroyed()), this, SLOT(CLOSING()));
}

AudioNoteEditor::~AudioNoteEditor(){
    player->stop();
}

void AudioNoteEditor::CLOSING()
{
    player->stop();
}

void AudioNoteEditor::BACKEND_SET_CONTENT()
{
    ressource->setDescription(getDescriptionWidget()->text());
    ressource->setMediaPath(ressource->getMediaPath());
}

void AudioNoteEditor::LOAD_AUDIO(){
    ressource->setMediaPath(QFileDialog::getOpenFileName(this, "Ouvrir un fichier", QString(), "Audio (*.mp3 *.ogg)"));
    qDebug()<<ressource->getMediaPath();

    player->setMedia(QMediaContent(QUrl::fromLocalFile(ressource->getMediaPath())));

    player->setMedia(QUrl::fromLocalFile(ressource->getMediaPath()));
    play = false;

}

void AudioNoteEditor::PLAY_PAUSE_SONG()
{
    if(ressource->getMediaPath().isEmpty())
        return;

    if(play)
    {
        player->pause();
        btnPlayPause->setIcon(QIcon(":images/play"));
        play = false;
    }
    else
    {
        player->play();
        btnPlayPause->setIcon(QIcon(":images/pause"));
        play = true;
    }
}

void AudioNoteEditor::STOP_SONG()
{
    player->stop();
}

void AudioNoteEditor::SET_VALUE(qint64 val)
{
    qDebug()<<val << "/"<<player->duration();
    qDebug()<< "=" << val/player->duration();
    progression->setValue(val*100/player->duration());
}

