#include "info_win.h"

#include <QStandardItemModel>
#include <iostream>
#include <unordered_map>

#include "ui_info_win.h"

info_win::info_win(QWidget* parent) : QDialog(parent), ui(new Ui::info_win) { ui->setupUi(this); }

info_win::~info_win() { delete ui; }

void info_win::getData(QStandardItemModel* mod, int row)
{
    this->setWindowTitle("Check Stocks");

    // set the ui up to display info abt selected company
    string name = mod->index(row, 0, QModelIndex()).data().toString().toStdString();
    int qty = mod->index(row, 1, QModelIndex()).data().toInt();
    float price = mod->index(row, 2, QModelIndex()).data().toFloat();
    float profit = mod->index(row, 3, QModelIndex()).data().toFloat();
    float stockSpent = mod->index(row, 4, QModelIndex()).data().toFloat();
    float stockWorth = mod->index(row, 5, QModelIndex()).data().toFloat();
    string date = mod->index(row, 6, QModelIndex()).data().toString().toStdString();

    ui->label->setText(QString::fromStdString(name));
    ui->cPrice->setText(QString::number(price));
    ui->cTotalSpent->setText(QString::number(stockSpent));
    ui->cTotalWorth->setText(QString::number(stockWorth));
    ui->cNetProfits->setText(QString::number(profit));
    ui->cQty->setText(QString::number(qty));
    ui->cLastAction->setText((QString::fromStdString(date)));
}
