#include "aboutwindow.h"

#include <QDir>

#include "ui_aboutwindow.h"

AboutWindow::AboutWindow(QWidget *parent) : QDialog(parent), ui(new Ui::AboutWindow)
{
    ui->setupUi(this);

    this->setWindowTitle("About");

    QPixmap image("logo.png");   // The image must be in the project's "working directory"
    ui->logo->setPixmap(image);  // The image is "inserted" as a label in the user interface (ui)
    ui->logo->setScaledContents(true);
}

AboutWindow::~AboutWindow() { delete ui; }
