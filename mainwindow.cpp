#include "mainwindow.h"
#include "NoteFactory.h"
#include "NoteTypeSignalAction.h"
#include "NotesManager.h"
#include "Editor.h"
#include "NotesException.h"
#include "Note.h"
#include "ExportStrategy.h"
#include "htmlViewer.h"
#include "TexViewer.h"
#include "Document.h"
#include "TextViewer.h"
#include "TreeModel.h"
#include "trashDialog.h"
#include <typeinfo>
#include <QMap>
#include <QScrollArea>
#include "ui_mainwindow.h"
#include <QSettings>
#include <QStandardPaths>
#include <QCoreApplication>
#include <assert.h>
#include "TreeItem.h"
#include <QCheckBox>
#include "TagManager.h"
#include <QStandardItem>
#include "Filter.h"
#include "Binary.h"
#include <QDebug>
#include "ListWidgetItemCheckTag.h"

MainWindow* MainWindow::instance = 0;

MainWindow* MainWindow::getInstance(){
    if(!instance){
        instance = new MainWindow();
    }
    return instance;
}

void MainWindow::freeInstance()
{
    if(instance){
        delete instance;
        instance = 0;
    }
}

void MainWindow::addOpenedFiles(const QString & path)
{
    openedFiles.insert(path);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), hv(0), tv(0), nm(0), textv(0), tm(0), sideBarModel(0)
{

    ui->setupUi(this);
    editorWidget = new QWidget;
    ui->editorScroll->setWidget(editorWidget);
    qDebug() << "hello";
    QToolBar *toolBar = addToolBar("General");
    QAction *actionQuit = new QAction("&Quit", this);
    QAction *actionOpen = new QAction("&Open...", this);

    actionQuit->setIcon(QIcon(":images/quit"));

    QAction *actionTrashBin = new QAction("&Trash", this);

    // subclassed QAction, this emets also the NoteType. So that we don't need different handling slot
    NoteTypeSignalAction *actionNewArticle = new NoteTypeSignalAction(article, "&Article", this);
    NoteTypeSignalAction *actionNewAudioNote = new NoteTypeSignalAction(audioNote, "&AudioNote", this);
    NoteTypeSignalAction *actionNewVideoNote = new NoteTypeSignalAction(videoNote, "&VideoNote", this);
    NoteTypeSignalAction *actionNewDocument = new NoteTypeSignalAction(document, "&Document", this);
    NoteTypeSignalAction *actionNewImageNote = new NoteTypeSignalAction(imageNote, "&ImageNote", this);
    actionNewAudioNote->setIcon(QIcon(":images/music"));
    actionNewArticle->setIcon(QIcon(":images/article"));
    actionNewDocument->setIcon(QIcon(":images/document"));
    actionNewImageNote->setIcon(QIcon(":images/image"));
    actionNewVideoNote->setIcon(QIcon(":images/video"));
    QMenu *menuNew = new QMenu("&New...");
    menuNew->addAction(actionNewArticle);
    menuNew->addAction(actionNewImageNote);
    menuNew->addAction(actionNewVideoNote);
    menuNew->addAction(actionNewAudioNote);
    menuNew->addAction(actionNewDocument);

    // subclassed QAction, this emets also the ExportType
    ExportTypeSignalAction *actionExportHTML = new ExportTypeSignalAction(html, "&HTML", this);
    ExportTypeSignalAction *actionExportTeX = new ExportTypeSignalAction(tex, "&TeX", this);
    ExportTypeSignalAction *actionExportText = new ExportTypeSignalAction(text, "&Text", this);
    qDebug() << "hello";
    QMenu *menuExport = new QMenu("&Export");
    menuExport->addAction(actionExportHTML);
    menuExport->addAction(actionExportTeX);
    menuExport->addAction(actionExportText);
    actionExportHTML->setIcon(QIcon(":images/html"));
    actionExportTeX->setIcon(QIcon(":images/tex"));
    actionExportText->setIcon(QIcon(":images/text"));
    menuFichier = menuBar()->addMenu("&File");
    menuEdition = menuBar()->addMenu("&Edit");

    menuFichier->addMenu(menuNew);
    menuFichier->addMenu(menuExport);

    menuFichier->addAction(actionQuit);

    toolBar->addAction(actionNewArticle);
    toolBar->addAction(actionNewImageNote);
    toolBar->addAction(actionNewVideoNote);
    toolBar->addAction(actionNewAudioNote);
    toolBar->addSeparator();
    toolBar->addAction(actionNewDocument);
    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    toolBar->addWidget(spacer);
    toolBar->addAction(actionTrashBin);
    toolBar->addAction(actionQuit);

    tab = new QTabWidget();
    EditorPage = new QWidget();
    htmlViewerPage = new QWidget();
    texViewerPage = new QWidget();
    textViewerPage = new QWidget();

    // Creat a new article, with generated file path and empty title&text
    nm = &NotesManager::getInstance();
    tm = &TagManager::getInstance();

    tab->addTab(EditorPage, "Editor");
    tab->addTab(htmlViewerPage, "HTML");
    tab->addTab(texViewerPage, "TeX");
    tab->addTab(textViewerPage, "Text");
    tab->setTabIcon(0,QIcon(":images/edit"));
    tab->setTabIcon(1,QIcon(":images/html"));
    tab->setTabIcon(2,QIcon(":images/tex"));
    tab->setTabIcon(3,QIcon(":images/text"));

    layout = new QVBoxLayout();
    layout->addWidget(tab);

    editorWidget->setLayout(layout);


    QObject::connect(actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));
    QObject::connect(actionOpen, SIGNAL(triggered()), this, SLOT(UI_OPEN_FILE()));
    QObject::connect(actionNewArticle, SIGNAL(triggeredWithId(const int)), this, SLOT(UI_NEW_NOTE_EDITOR(const int)));
    QObject::connect(actionNewDocument, SIGNAL(triggeredWithId(const int)), this, SLOT(UI_NEW_NOTE_EDITOR(const int)));
    QObject::connect(actionNewImageNote, SIGNAL(triggeredWithId(const int)), this, SLOT(UI_NEW_NOTE_EDITOR(const int)));
    QObject::connect(actionNewAudioNote, SIGNAL(triggeredWithId(const int)), this, SLOT(UI_NEW_NOTE_EDITOR(const int)));
    QObject::connect(actionNewVideoNote, SIGNAL(triggeredWithId(const int)), this, SLOT(UI_NEW_NOTE_EDITOR(const int)));

    QObject::connect(actionExportHTML, SIGNAL(triggeredWithId(const int)), this, SLOT(UI_EXPOR_TO_FILE(const int)));
    QObject::connect(actionExportTeX, SIGNAL(triggeredWithId(const int)), this, SLOT(UI_EXPOR_TO_FILE(const int)));
    QObject::connect(actionExportText, SIGNAL(triggeredWithId(const int)), this, SLOT(UI_EXPOR_TO_FILE(const int)));
    QObject::connect(ui->addTag, SIGNAL(clicked()), this, SLOT(ADD_TAG()));
    QObject::connect(ui->removeTag, SIGNAL(clicked()), this, SLOT(REMOVE_TAG()));
    QObject::connect(ui->tagList, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(CHANGE_NAME_TAG(QListWidgetItem*)));
    QObject::connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(BACKEND_CLOSING()));

    QObject::connect(ui->noteBookTree, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(UI_LOAD_FROM_SIDE_BAR(const QModelIndex&)));
    QObject::connect(ui->tagList, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(STOCK_DISABLED_TAGS(QListWidgetItem*)));
    // Tab change handling
    QObject::connect(tab, SIGNAL(currentChanged(int)), this, SLOT(UI_TAB_CHANGE_HANDLER(int)));

    QObject::connect(ui->searchLineEdit, SIGNAL(textChanged(QString)), this, SLOT(updateSideBarWithNewSearchFilter(QString)));
    QObject::connect(actionTrashBin, SIGNAL(triggered()), this, SLOT(FIRE_UP_TRASH_BIN_DIALOG()));

    updateSideBar();
    createTagList();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::UI_INFORM_NOT_IMPLEMENTED(){
    QMessageBox::information(this, "New fonctionality", "To be implemented...");
}


void MainWindow::UI_NEW_NOTE_EDITOR(const int type){
    NoteType nt;
    QLayout *parentLayout;
    try{
        nt = static_cast<NoteType>(type);
    }catch(std::bad_cast& bc){
        QMessageBox::critical(this, "Error", "something serious happenned during creation of new editor..."+QString(bc.what()));
    }
    if(nt!=document){
        for(QList<Note*>::iterator it = ressources.begin(); it != ressources.end(); ++it){
            if((*it)->getEditor()){
                (*it)->getEditor()->BACKEND_SET();
                delete (*it)->getEditor();
                (*it)->setEditor(0);
            }
        }
        parentLayout = EditorPage->layout() ? EditorPage->layout() : new QVBoxLayout();
        try{
            Note* temp = &nm->getNewNote(nt);
            ressources.clear();
            openedFiles.clear();

            Editor* noteEditor = temp->createAndAttachEditor();
            parentLayout->addWidget(noteEditor);
            if(!EditorPage->layout())
                EditorPage->setLayout(parentLayout);
        }
        catch(NotesException e){
            QMessageBox::critical(this, "Error", e.getInfo());
        }
    }
    else {
        &nm->getNewNote(nt);
    }
    updateSideBar();
}

void MainWindow::UI_LOAD_FROM_SIDE_BAR(const QModelIndex& index){
    TreeItem * temp = sideBarModel->getItem(index);

    bool first = false;
    for(QList<Note*>::iterator it = ressources.begin(); it != ressources.end(); ++it){
        if(first) break;
        if((*it)->getEditor()){
            qDebug()<<(*it)->getTitle();
            (*it)->getEditor()->BACKEND_SET();
            delete (*it)->getEditor();
            (*it)->setEditor(0);
            first = true;
        }
    }

    ressources.clear();
    openedFiles.clear();

    Editor *editor = temp->getItemId()->createAndAttachEditor();
    // TODO Deep
    editor->updateDocBtnWithRessource(temp);

    QLayout * layout = EditorPage->layout() ? EditorPage->layout() : new QVBoxLayout();
    layout->addWidget(editor);
    if(!EditorPage->layout())
        EditorPage->setLayout(layout);
}


void MainWindow::UI_OPEN_FILE(){
    QString fichier = QFileDialog::getOpenFileName(this, "Ouvrir un fichier", QString(), "Notes (*.art *.img *.vid *.aud *.doc)");

    QLayout *parentLayout;
    if(!fichier.isNull()){
        // Test if already opened
        if(openedFiles.contains(fichier))
            return;

        parentLayout = EditorPage->layout() ? EditorPage->layout() : new QVBoxLayout();
        try{
            Note* temp = &nm->getNote(fichier);
            ressources << temp;

            Editor* noteEditor = temp->createAndAttachEditor();
            parentLayout->addWidget(noteEditor);
            if(!EditorPage->layout())
                EditorPage->setLayout(parentLayout);
            openedFiles << temp->getFilePath();
        }
        catch(NotesException e){
            QMessageBox::critical(this, "Error", e.getInfo());
        }
    }
    updateSideBar();
}

void MainWindow::LoadExportToViewerPage(ExportType type, QList<Note*>& list, QWidget* viewerPage, Viewer* viewer){
    if(list.isEmpty()){
        QMessageBox::warning(0, "No opened file", "You need to open some notes in order to export.");
        return;
    }
    ExportStrategy *es = NotesManager::getInstance().strategies->value(type);
    QString content("");

    for(QList<Note*>::iterator it = list.begin(); it != list.end(); ++it){
        (*it)->getEditor()->BACKEND_SET();
        //        content+=(*it)->exportNote(es);
    }
    QList<Note*>::iterator it = list.begin();
    content = (*it)->exportNote(es);

    if(viewerPage){
        // export to viewer.
        QLayout* parentLayout = viewerPage->layout();

        delete viewer;
        if(parentLayout)
            delete parentLayout;

        // add viewer into tab
        parentLayout = new QVBoxLayout();

        switch(type){
        case html:
            viewer = new HtmlViewer(content);
            //            qDebug()<<content;
            break;
        case tex:
            viewer = new TexViewer(content);
            break;
        case text:
            viewer = new TextViewer(content);
            break;
        default:
            throw NotesException("Error... should not happen. Handling tab action");
        }
        parentLayout->addWidget(viewer);
        viewerPage->setLayout(parentLayout);
    } else {
        // Used to export to file
        // now that content is prepared, we need a file path to save.
        QString defaultFilename = (*it)->getTitle();
        switch(type){
        case html:
            defaultFilename+=".html";
            break;
        case tex:
            defaultFilename+=".tex";
            break;
        case text:
            defaultFilename+=".txt";
            break;
        default:
            throw NotesException("Should not happen.");
        }

        qDebug()<<QStandardPaths::locate(QStandardPaths::DocumentsLocation, "", QStandardPaths::LocateDirectory)+defaultFilename;

        QString filename = QFileDialog::getSaveFileName(this, "Export to...", QStandardPaths::locate(QStandardPaths::DocumentsLocation, "", QStandardPaths::LocateDirectory)+defaultFilename);
        if(!filename.isEmpty()){

            QFile f(filename);
            if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
                throw NotesException("Failed to save your note, please check if I have the permission to write on your harddisk and stop hacking the software!");

            QTextStream flux(&f);
            flux<<content;
            f.close();
        }
    }
}

void MainWindow::UI_TAB_CHANGE_HANDLER(int n){
    if(!ressources.empty()){
        switch(n){
        case -1:{
            return;
        case 1:{
                qDebug()<<"HTML";
                LoadExportToViewerPage(html, ressources, htmlViewerPage, hv);
                break;
            }
            case 2:{
                qDebug()<<"TeX";
                LoadExportToViewerPage(tex, ressources, texViewerPage, tv);
                break;
            }
            case 3:{
                qDebug()<<"Text";
                LoadExportToViewerPage(text, ressources, textViewerPage, textv);
                break;
            }
            default:
                return;
            }
        }
    }
}

void MainWindow::BACKEND_CLOSING()
{
    if(nm->getRootDocument()->isModified()){
        try{
            nm->saveNote(*nm->getRootDocument());
            QSettings settings;
            settings.setValue("rootDocument", nm->getRootDocument()->getFilePath());
            qDebug() << nm->getRootDocument()->getFilePath();
        }
        catch (NotesException e){
            QMessageBox::warning(this, "Saving error", e.getInfo());
        }
    }
}

void MainWindow::updateSideBar()
{
    TreeModel *old = 0;
    if(sideBarModel)
        old = sideBarModel;
    sideBarModel = new TreeModel();
    ui->noteBookTree->setModel(sideBarModel);
    delete old;
    old = 0;
}
void MainWindow::createTagList()
{
    tm = &TagManager::getInstance();

    for(tagSetIt it = tm->begin(); it != tm->end(); it++)
    {
        qDebug() << (*it)->getName();
        ListWidgetItemCheckTag* item = new ListWidgetItemCheckTag((*it)->getName(), (*it), ui->tagList);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsEditable); // set checkable flag
        item->setCheckState(Qt::Checked);
    }
}

void MainWindow::STOCK_DISABLED_TAGS(QListWidgetItem* item){

    TagFilter *f = new TagFilter();
    Tag* t;
    if(item->checkState()==Qt::Checked)
    {
        t=static_cast<ListWidgetItemCheckTag *>(item)->getTag();
        if(f->contains(t))
            f->removeDisabledTag(t);
    }
    else f->addDisabledTag(t);

    FilterKit* kit = FilterKit::getInstance();
    kit->setFilter(tag, f);
    updateSideBar();
}


void MainWindow::updateSideBarWithNewSearchFilter(QString str)
{
    FilterKit* kit = FilterKit::getInstance();
    SearchFilter* filter = new SearchFilter(str);
    kit->setFilter(search, filter);
    updateSideBar();
}


void MainWindow::UI_EXPOR_TO_FILE(const int type)
{
    ExportType et = static_cast<ExportType>(type);
    LoadExportToViewerPage(et, ressources);
}

void MainWindow::addRessources(Note* n)
{
    ressources.append(n);

}

void MainWindow::FIRE_UP_TRASH_BIN_DIALOG()
{
    new TrashDialog();
}

void MainWindow::ADD_TAG()
{
    TagManager* tm=&TagManager::getInstance();
    if(ui->tagList->findItems("New Tag", Qt::MatchExactly).count()==0)
    {
        qDebug() << "hello";
        ListWidgetItemCheckTag* item = new ListWidgetItemCheckTag("New Tag", tm->getTag("New Tag"), ui->tagList);
        qDebug() << "hello2";
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsEditable); // set checkable flag
        qDebug() << "hello3";
        item->setCheckState(Qt::Checked);
    }
    else QMessageBox::information(this, "Erreur", "Nouveau Tag déjà existant...");
    qDebug() << "hello";
}

void MainWindow::REMOVE_TAG()
{
    qDebug() << "Got current item to delete: " << ui->tagList->currentItem()->data(0).toString();
    if(ui->tagList->currentItem())
    {
        ListWidgetItemCheckTag* item = static_cast<ListWidgetItemCheckTag*>(ui->tagList->currentItem());
        tm->removeTag(item->getTag());
        delete item;
    }

}

void MainWindow::CHANGE_NAME_TAG(QListWidgetItem* item)
{
    ListWidgetItemCheckTag* itemTag = static_cast<ListWidgetItemCheckTag*>(item);
    itemTag->getTag()->setName(item->data(0).toString());
}
