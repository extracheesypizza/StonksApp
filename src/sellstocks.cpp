#include "sellstocks.h"

#include <QMessageBox>
#include <iostream>
#include <unordered_map>

#include "stock_company.h"
#include "ui_sellstocks.h"

sellstocks::sellstocks(QWidget *parent) : QDialog(parent), ui(new Ui::sellstocks) { ui->setupUi(this); }

sellstocks::~sellstocks() { delete ui; }

void sellstocks::setPointers(QStandardItemModel *model, float *totalWorth, float *totalSpent, float *netProfits, std::vector<Action> *vec)
{
    _vec = vec;
    _model = model;
    _totalSpent = totalSpent;
    _totalWorth = totalWorth;
    _netProfits = netProfits;
}

void sellstocks::getData(int row)
{
    // Change the title of the window
    this->setWindowTitle("Sell Stocks");

    // Set the name of the company we are about to sell
    QModelIndex nameIndex = _model->index(row, 0, QModelIndex());
    string name = nameIndex.data().toString().toStdString();

    ui->cName->setText(QString::fromStdString(name));
}

// Func to check whether an input is a number
bool numberCheckSell(string str)
{
    for(int i = 0; i < str.length(); i++)
        if(isdigit(str[i]) == 0)
            return false;
    return true;
}

bool dateCheckSell(string str)
{
    string temp = "";
    vector<string> vec;
    for(int i = 0; i < str.size(); i++)
    {
        if(str[i] != '.')
        {
            temp += str[i];
            if(i == str.size() - 1)
                vec.push_back(temp);
        }
        else
        {
            vec.push_back(temp);
            temp = "";
        }
    }

    if(vec.size() < 3)
        return false;

    else
        for(int j = 0; j < vec.size(); j++)
            if(!numberCheckSell(vec[j]))
                return false;

    for(int j = 0; j < vec.size(); j++)
    {
        if((j == 0 && (stoi(vec[j]) < 0 || stoi(vec[j]) > 31)) || (j == 1 && (stoi(vec[j]) < 0 || stoi(vec[j]) > 12)) || (j == 2 && (stoi(vec[j]) < 1984 || stoi(vec[j]) > 9999)))
            return false;
    }
    return true;
}

void sellstocks::on_pushButton_clicked()
{
    // check if all lines are filled in correctly
    if(ui->cDate->text().toStdString() == "" or !numberCheckSell(ui->cPrice->text().toStdString()) or !numberCheckSell(ui->cQty->text().toStdString()) or !dateCheckSell(ui->cDate->text().toStdString()))
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Error");
        msgBox.setText("Check your input once again");
        msgBox.exec();
    }
    else
    {
        Action newAct;
        newAct.action = "sell";

        // find that comany
        // unordered_map<string, Company>::iterator it = tableMap->find(ui->cName->text().toStdString());

        int rowN = 0;
        for(int j = 0; j < _model->rowCount(); j++)
            if(_model->index(j, 0, QModelIndex()).data().toString().toStdString() == ui->cName->text().toStdString())
                rowN = j;

        // we cannot sell more than we own
        QModelIndex qtyIndex = _model->index(rowN, 1, QModelIndex());
        int qty = qtyIndex.data().toInt();
        int sellQty = min(ui->cQty->text().toInt(), qty);

        // set the last price
        float sellPrice = ui->cPrice->text().toFloat();

        // set the date
        string sellDate = ui->cDate->text().toStdString();

        // fill the Action with set attributes
        QModelIndex nameIndex = _model->index(rowN, 0, QModelIndex());
        string name = nameIndex.data().toString().toStdString();

        newAct.compName = name;
        newAct.lastPrice = sellPrice;
        newAct.stockQty = sellQty;
        newAct.date = ui->cDate->text().toStdString();
        newAct.stockSpent = newAct.stockQty * newAct.lastPrice;
        newAct.stockWorth = newAct.stockQty * newAct.lastPrice;

        _vec->push_back(newAct);

        // update company info
        // name, qty, price, profit, stockSpent, StockWorth, date
        // it->second.cNetProfit = sellQty * (sellPrice - it->second.lastPrice);
        QModelIndex profitIndex = _model->index(rowN, 3, QModelIndex());
        QModelIndex priceIndex = _model->index(rowN, 2, QModelIndex());
        float lastPrice = priceIndex.data().toFloat();
        _model->setData(profitIndex, QString::number(sellQty * (sellPrice - lastPrice)));

        // it->second.lastPrice = sellPrice;
        _model->setData(priceIndex, QString::number(sellPrice));

        // it->second.stockQty -= sellQty;
        qty = qtyIndex.data().toInt();
        _model->setData(qtyIndex, QString::number(qty - sellQty));

        // it->second.stockWorth = it->second.stockQty * it->second.lastPrice;
        QModelIndex swIndex = _model->index(rowN, 5, QModelIndex());
        qty = qtyIndex.data().toInt();
        lastPrice = priceIndex.data().toFloat();
        _model->setData(swIndex, QString::number(qty * lastPrice));

        // it->second.date = sellDate;
        QModelIndex dateIndex = _model->index(rowN, 6, QModelIndex());
        QString date = QString::fromStdString(nameIndex.data().toString().toStdString());
        _model->setData(dateIndex, date);

        // update the table
        QStandardItem *item = 0;
        int i = 0;
        *_totalWorth = 0;
        *_totalSpent = 0;
        *_netProfits = 0;

        // update general info
        for(int j = 0; j < _model->rowCount(); j++)
        {
            QModelIndex swIndex = _model->index(j, 5, QModelIndex());
            float sw = swIndex.data().toFloat();

            QModelIndex ssIndex = _model->index(j, 4, QModelIndex());
            float ss = ssIndex.data().toFloat();

            QModelIndex profitsIndex = _model->index(j, 3, QModelIndex());
            float profits = profitsIndex.data().toFloat();

            *_totalWorth += sw;
            *_totalSpent += ss;
            *_netProfits += profits;
        }
        this->close();
    }
}
