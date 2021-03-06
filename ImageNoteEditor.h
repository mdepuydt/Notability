#ifndef IMAGENOTEEDITOR_H
#define IMAGENOTEEDITOR_H

#include "BinaryEditor.h"
//#include "ImageNote.h"

class ImageNote;

/*!
 * \brief The ImageNoteEditor class Un editeur multimedia qui permet d'afficher et modifie des images.
 */
class ImageNoteEditor : public BinaryEditor
{
    Q_OBJECT
public:
    explicit ImageNoteEditor(ImageNote *img, QWidget *parent = 0);

signals:

public slots:
    /*!
     * \brief BACKEND_SET_CONTENT Methode virtuelle. On enreigistre ici le chemin vers l'image et la description.
     */
    void BACKEND_SET_CONTENT();
    void LOAD_IMAGE();
    void CLOSING(){}

private:
    QPushButton *btnAddImage;
    QPixmap image;
    QLabel *imageWidget;
    ImageNote *ressource;
    QString currentImgPath;
};

#endif // IMAGENOTEEDITOR_H
