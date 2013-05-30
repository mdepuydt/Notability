#include <QWidget>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDebug>
#include <QMessageBox>

#include "Note.h"
#include "Editor.h"
#include "NotesException.h"
#include "NotesManager.h"
#include "htmlViewer.h"

Editor::Editor(Note *n, QWidget *parent) :
    QWidget(parent), ressource(n)
{
    btnSave = new QPushButton("Save");
    btnClose= new QPushButton("Close");
    btnMove = new QPushButton("Move");
    btnTag = new QPushButton("Tag");

    titleWidget = new QLineEdit(ressource->getTitle());
    contentWidget = new QWidget();
    buttonsWidget = new QWidget();

    editorBaseLayout = new QVBoxLayout();
    contentLayout = new QVBoxLayout();
    buttonsLayout = new QHBoxLayout();

    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    editorBaseLayout->addWidget(new QLabel("Title:"));
    editorBaseLayout->addWidget(titleWidget);
    editorBaseLayout->addWidget(contentWidget);
    editorBaseLayout->addWidget(buttonsWidget);
    editorBaseLayout->addWidget(line);

    this->setLayout(editorBaseLayout);
    contentWidget->setLayout(contentLayout);
    buttonsWidget->setLayout(buttonsLayout);

    buttonsLayout->addWidget(btnSave);
    buttonsLayout->addWidget(btnClose);
    buttonsLayout->addWidget(btnMove);
    buttonsLayout->addWidget(btnTag);
    btnSave->setEnabled(false);


    QObject::connect(titleWidget, SIGNAL(textChanged(QString)), this, SLOT(UI_ENABLE_SAVE_BUTTON()));
    QObject::connect(btnSave, SIGNAL(clicked()),this, SLOT(BACKEND_SAVE()));
}

void Editor::UI_ENABLE_SAVE_BUTTON()
{
    this->btnSave->setEnabled(true);
}

void Editor::BACKEND_SET_TITLE()
{
    QString str = titleWidget->text();
    this->ressource->setTitle(str);
}

void Editor::BACKEND_SET()
{
    BACKEND_SET_TITLE();
    BACKEND_SET_CONTENT();
}

void Editor::BACKEND_SAVE()
{
    BACKEND_SET();
    try{
        NotesManager *nm = &NotesManager::getInstance();
//        ressource->save();
        nm->saveNote(*ressource);
        UI_INFORM_USER_OF_SAVE();
    }
    catch(NotesException e){
//        qDebug() << e.getInfo();
        QMessageBox::critical(this, "Error", "Your modifications have not been saved! Error: "+e.getInfo());
    }
}

void Editor::UI_INFORM_USER_OF_SAVE(){
    QMessageBox::information(this, "Article Saved", "Your modifications have been saved!");
    this->btnSave->setEnabled(false);
}

QLineEdit *Editor::getTitleWidget() const
{
    return titleWidget;
}

void Editor::setTitleWidget(QLineEdit *value)
{
    titleWidget = value;
}

Note *Editor::getRessource() const
{
    return ressource;
}

void Editor::setRessource(Note *value)
{
    ressource = value;
}

bool Editor::operator ==(const Editor &rhs)
{
    return this->getRessource()->getFilePath() == rhs.getRessource()->getFilePath();
}
