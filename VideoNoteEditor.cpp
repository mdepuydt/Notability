#include "VideoNoteEditor.h"
#include "VideoNote.h"
#include <QWidget>


VideoNoteEditor::VideoNoteEditor(VideoNote *v, QWidget* parent)
    :BinaryEditor(v, parent)
{
}

void VideoNoteEditor::BACKEND_SET_CONTENT()
{
}
