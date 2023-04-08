#include "settingswindow.h"

#include "ui_settingswindow.h"

SettingsWindow::SettingsWindow(QWidget *parent) : QDialog(parent), ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Settings");
}

SettingsWindow::~SettingsWindow() { delete ui; }

void SettingsWindow::setPointers(std::vector<bool> *vec)
{
    _vec = vec;

    std::vector<bool> temp = *_vec;
    ui->cbQty->setChecked(temp[1]);
    ui->cbPrice->setChecked(temp[2]);
    ui->cbProfit->setChecked(temp[3]);
    ui->cbTotalSpent->setChecked(temp[4]);
    ui->cbTotalWorth->setChecked(temp[5]);
    ui->cbDate->setChecked(temp[6]);
}

void SettingsWindow::on_done_clicked()
{
    // bool name = true;
    bool qty = ui->cbQty->isChecked();
    bool price = ui->cbPrice->isChecked();
    bool profit = ui->cbProfit->isChecked();
    bool totalSpent = ui->cbTotalSpent->isChecked();
    bool totalWorth = ui->cbTotalWorth->isChecked();
    bool date = ui->cbDate->isChecked();

    *_vec = {true, qty, price, profit, totalSpent, totalWorth, date};

    this->close();
}
