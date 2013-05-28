#ifndef VIDEONOTEFACTORY_H
#define VIDEONOTEFACTORY_H

#include "NoteFactory.h"
#include "VideoNote.h"

class VideoNoteFactory : public NoteFactory
{
public:
    VideoNote* buildNewNote();
    VideoNote* buildNote(const QString &path);
    VideoNote* buildNoteCopy(const VideoNote &note);
    QString getFolder();
    QString getExtension();
};

#endif // VIDEONOTEFACTORY_H