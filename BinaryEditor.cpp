#include "BinaryEditor.h"
#include "Binary.h"
/***
 * BinaryEditor
 */

//Binary *BinaryEditor::getRessource() const
//{
//    return ressource;
//}

//void BinaryEditor::setRessource(Binary *value)
//{
//    ressource = value;
//}

QLineEdit *BinaryEditor::getDescriptionWidget() const
{
    return descriptionWidget;
}

void BinaryEditor::setDescriptionWidget(QLineEdit *value)
{
    descriptionWidget = value;
}

BinaryEditor::BinaryEditor(Binary *b, MainWindow *mw, QWidget *parent)
    :Editor(b, mw, parent), ressource(b)
{
    descriptionWidget = new QLineEdit(ressource->getDescription());
}
