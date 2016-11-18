#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QDesktopWidget>
#include <QLabel>
#include "taskwidget.h"
#include "taskdialog.h"
#include "project.h"

static MainWindow *s_pMainWindow = NULL;

MainWindow *MainWindow::getInstance()
{
    return s_pMainWindow;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    s_pMainWindow = this;
    ui->setupUi(this);
    this->centerAndResize();
    newProjectButton = ui->newProjectButton;
    deleteProjectButton= ui->deleteProjectButton;

    if (ProjectUtils::Instance()->get_projects().empty()){
        ProjectUtils::Instance()->add_project(Project());
    }
    ProjectUtils::Instance()->open_project(ProjectUtils::Instance()->get_projects()[0].get_name());
    m_project = ProjectUtils::Instance()->get_open_project();

    projectListView = ui->projectsList;
    projectModel = new ProjectListModel(ProjectUtils::Instance()->get_projects());
    projectListView->setModel(projectModel);
    newTaskButton = ui->newTaskButton;

    toDoLayout = ui->Todo_List;
    doingLayout = ui->Doing_List;
    testingLayout = ui->Testing_List;
    doneLayout = ui->Done_List;

    projectNameLabel = ui->projectNameLabel;

    //Connections
    connect(newProjectButton, SIGNAL (released()), this, SLOT(onNewProjectButtonClick()));
    connect(deleteProjectButton, SIGNAL(released()), this, SLOT(onDeleteProjectButtonClick()));
    connect(newTaskButton, SIGNAL(released()), this, SLOT(onNewTaskButtonClicked()));
    connect(projectListView, SIGNAL(pressed(QModelIndex)), this, SLOT(item_selected_in_list()));

    projectNameLabel->setText(QString::fromStdString(m_project.get_name()));

}

void MainWindow::centerAndResize()
{
    // get the dimension available on this screen
    QSize availableSize = qApp->desktop()->availableGeometry().size();
    int width = availableSize.width();
    int height = availableSize.height();
    qDebug() << "Available dimensions " << width << "x" << height;
    width *= 0.85; // 90% of the screen size
    height *= 0.85; // 90% of the screen size
    qDebug() << "Computed dimensions " << width << "x" << height;
    QSize newSize( width, height );

    setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            newSize,
            qApp->desktop()->availableGeometry()
        )
    );
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onNewProjectButtonClick()
{
    qDebug() << "Main Window New Project Button";
    Project newProject("New Project", "");
    projectModel->addProject(newProject, Qt::EditRole);
}

void MainWindow::onDeleteProjectButtonClick()
{
    qDebug() << "Main Window Delete Project Button";
    if(projectListView->selectionModel()->selectedIndexes().isEmpty())
        return;
    projectModel->deleteProject(projectListView->selectionModel()->selectedIndexes().first(), Qt::EditRole);
}

void MainWindow::onNewTaskButtonClicked()
{
    qDebug() << " Main Window Add New Task Button";
    if(projectListView->selectionModel()->selectedIndexes().isEmpty())
        return;

    taskDialog = new TaskDialog();
    taskDialog->show();
}

void MainWindow::onTaskDialogAccepted()
{
    TaskWidget *task = new TaskWidget;
    Task current = ProjectUtils::Instance()->get_open_task();
    if(current.get_status() == "To Do")
        ui->Todo_List->addWidget(task);
    else if(current.get_status() == "Doing")
        ui->Doing_List->addWidget(task);
    else if(current.get_status() == "Testing")
        ui->Testing_List->addWidget(task);
    else if(current.get_status() == "Done")
        ui->Done_List->addWidget(task);
    qDebug() << "Main Window TASK ACCEPTED!!!!!";
    qDebug() << QString::fromStdString(to_string(ProjectUtils::Instance()->get_open_project().get_tasks().size()));
}

void MainWindow::mousePressEvent(QMouseEvent *event){
    qDebug() << "Main Window Mouse";

}

void MainWindow::item_selected_in_list(){
    qDebug() << "Item Selected in QListView";
    if(projectListView->selectionModel()->selectedIndexes().isEmpty())
        return;
    ProjectUtils::Instance()->open_project(ProjectUtils::Instance()->get_projects().at(projectListView->selectionModel()->selectedIndexes().first().row()));
    qDebug() << QString::fromStdString(ProjectUtils::Instance()->get_open_project().get_name());
}
