#include "buystocks.h"

#include <QMessageBox>
#include <iostream>
#include <unordered_map>

#include "stock_company.h"
#include "ui_buystocks.h"

buyStocks::buyStocks(QWidget *parent) : QDialog(parent), ui(new Ui::buyStocks) { ui->setupUi(this); }

buyStocks::~buyStocks() { delete ui; }

void buyStocks::setPointers(std::vector<Action> *vec, QStandardItemModel *model, float *totalWorth, float *totalSpent, float *netProfits, int row)
{
    // Change the title
    this->setWindowTitle("Buy Stocks");

    // Set the pointer
    _vec = vec;
    _model = model;
    _totalSpent = totalSpent;
    _totalWorth = totalWorth;
    _netProfits = netProfits;
    QString name = _model->index(row, 0, QModelIndex()).data().toString();
    this->ui->cName->setText(name);
}

// Func to check whether an input is a number
bool numberCheckBuy(string str)
{
    for(int i = 0; i < str.length(); i++)
        if(isdigit(str[i]) == 0)
            return false;
    return true;
}

bool dateCheckBuy(string str)
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
            if(!numberCheckBuy(vec[j]))
                return false;

    for(int j = 0; j < vec.size(); j++)
    {
        if((j == 0 && (stoi(vec[j]) < 0 || stoi(vec[j]) > 31)) || (j == 1 && (stoi(vec[j]) < 0 || stoi(vec[j]) > 12)) || (j == 2 && (stoi(vec[j]) < 1984 || stoi(vec[j]) > 9999)))
            return false;
    }
    return true;
}

// 'Sell' button clicked
void buyStocks::on_pushButton_clicked()
{
    // check if all lines are filled in correctly
    if(ui->cName->text().toStdString() == "" or ui->cPrice->text() == "" or ui->cQty->text() == "" or ui->cDate->text().toStdString() == "" or !numberCheckBuy(ui->cQty->text().toStdString()) or !numberCheckBuy(ui->cPrice->text().toStdString()) or !dateCheckBuy(ui->cDate->text().toStdString()))
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Error");
        msgBox.setText("Check your input once again");
        msgBox.exec();
    }
    else
    {
        Action newAct;
        newAct.action = "buy";

        // fill an Action with set attributes
        newAct.compName = ui->cName->text().toStdString();
        newAct.lastPrice = ui->cPrice->text().toFloat();
        newAct.stockQty = ui->cQty->text().toInt();
        newAct.date = ui->cDate->text().toStdString();
        newAct.stockSpent = newAct.stockQty * newAct.lastPrice;
        newAct.stockWorth = newAct.stockQty * newAct.lastPrice;

        _vec->push_back(newAct);

        // do the overview
        *_totalSpent += newAct.stockQty * newAct.lastPrice;

        // check if the comapny is in the map
        // unordered_map<string, Company>::iterator it = _tableMap->find(newAct.compName);

        // create a checker
        int rowN = 0;
        bool ch = false;
        for(int j = 0; j < _model->rowCount(); j++)
        {
            // cout << _model->index(j, 0, QModelIndex()).data().toString().toStdString() << endl;
            if(_model->index(j, 0, QModelIndex()).data().toString().toStdString() == newAct.compName)
            {
                rowN = j;
                ch = true;
            }
        }

        // if(it != _tableMap->end())
        if(ch == true)
        {
            // adjust the price
            // it->second.lastPrice = newAct.lastPrice;
            QModelIndex priceIndex = _model->index(rowN, 2, QModelIndex());
            _model->setData(priceIndex, newAct.lastPrice);
            float price = priceIndex.data().toFloat();
            // cout << price << endl;

            // change the quantity
            // it->second.stockQty += newAct.stockQty;
            QModelIndex qtyIndex = _model->index(rowN, 1, QModelIndex());
            int qty = qtyIndex.data().toInt();
            _model->setData(qtyIndex, QString::number(qty + newAct.stockQty));
            qty = qtyIndex.data().toInt();

            // change the money spent
            // it->second.stockSpent += newAct.stockQty * newAct.lastPrice;
            QModelIndex ssIndex = _model->index(rowN, 4, QModelIndex());
            float ss = _model->index(rowN, 4, QModelIndex()).data().toFloat();
            _model->setData(ssIndex, ss + newAct.stockQty * newAct.lastPrice);
            float ssNew = _model->index(rowN, 4, QModelIndex()).data().toFloat();

            // change the date
            // it->second.date = newAct.date;
            QModelIndex dateIndex = _model->index(rowN, 6, QModelIndex());
            QString date = QString::fromStdString(newAct.date);
            _model->setData(dateIndex, date);

            // change the stock worth according to the last prcie
            // it->second.stockWorth = it->second.stockQty * it->second.lastPrice;
            QModelIndex swIndex = _model->index(rowN, 5, QModelIndex());
            float sw = qty * price;
            _model->setData(swIndex, qty * price);

            // it->second.cNetProfit = it->second.stockWorth - it->second.stockSpent;
            QModelIndex profitIndex = _model->index(rowN, 3, QModelIndex());
            _model->setData(profitIndex, sw - ssNew);
        }
        else
        {
            if(newAct.compName != "")
            {
                // create a comapny
                Company temp;
                temp.compName = newAct.compName;
                temp.stockQty = newAct.stockQty;
                temp.lastPrice = newAct.lastPrice;
                temp.cNetProfit = 0;
                temp.stockSpent = newAct.stockSpent;
                temp.stockWorth = newAct.stockWorth;
                temp.date = newAct.date;
                // pair<string, Company> a(newAct.compName, temp);
                //_tableMap->insert(_tableMap->end(), a);

                int row = _model->rowCount();

                // Inserting a new row in the table
                _model->insertRows(row, 1);

                QStandardItem *item = 0;

                item = new QStandardItem(QString::fromStdString(newAct.compName));
                _model->setItem(row, 0, item);

                // name
                QModelIndex index = _model->index(row, 0, QModelIndex());
                QString name = QString::fromStdString(newAct.compName);
                _model->setData(index, name);

                // qty
                item = new QStandardItem(QString::number(newAct.stockQty));
                _model->setItem(row, 1, item);

                // price
                item = new QStandardItem(QString::number(newAct.lastPrice));
                _model->setItem(row, 2, item);

                // profit
                QModelIndex profitIndex = _model->index(row, 3, QModelIndex());
                item = new QStandardItem(QString::number(profitIndex.data().toFloat()));
                _model->setItem(row, 3, item);

                // stockSpent
                item = new QStandardItem(QString::number(newAct.stockQty * newAct.lastPrice));
                _model->setItem(row, 4, item);

                // StockWorth
                item = new QStandardItem(QString::number(newAct.stockQty * newAct.lastPrice));
                _model->setItem(row, 5, item);

                // date
                item = new QStandardItem(QString::fromStdString(newAct.date));
                _model->setItem(row, 6, item);
            }
        }

        // update the table and general info
        QStandardItem *item = 0;
        int i = 0;
        *_totalWorth = 0;
        *_netProfits = 0;

        for(int i = 0; i < _model->rowCount(); i++)
        {
            QModelIndex swIndex = _model->index(i, 5, QModelIndex());
            float sw = swIndex.data().toFloat();

            QModelIndex profitIndex = _model->index(i, 3, QModelIndex());
            float profit = profitIndex.data().toFloat();

            QModelIndex nameIndex = _model->index(i, 0, QModelIndex());
            QString name = nameIndex.data().toString();
            // name, qty, price, profit, stockSpent, StockWorth, date
            *_totalWorth += sw;
            *_netProfits += profit;
        }
        this->close();
    }
}
