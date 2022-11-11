#include "mainwindow.h"

#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QObject>
#include <QStandardItemModel>
#include <QTableWidget>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <unordered_map>

#include "./ui_mainwindow.h"
#include "aboutwindow.h"
#include "buystocks.h"
#include "info_win.h"
#include "sellstocks.h"
#include "settingswindow.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _model = new QStandardItemModel(this);
    ui->tableView->setModel(_model);

    this->ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // add "About" section
    ui->menuFile->addAction("Load .CSV", this, SLOT(loadCSV()));
    ui->menuFile->addAction("Save .CSV", this, SLOT(saveCSV()));
    ui->menuFile->addAction("Settings", this, SLOT(openSettings()));
    ui->menuFile->addAction("About", this, SLOT(openAbout()));

    // Change the title of the window
    this->setWindowTitle("The Stonks App");

    // depricated due to loadCSV() existance!!
    // Process data into a table
    // createList();

    // depricated due to moving to QModel system
    // Add a Table + Header Labels
    // createTable();

    // Sorting of the table [by name]
    ui->tableView->setSortingEnabled(true);
    ui->tableView->sortByColumn(1, Qt::SortOrder::AscendingOrder);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);  // layout thingie

    // Pie chart
    drawChart();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::openAbout()
{
    AboutWindow abtWin;
    abtWin.setVisible(true);
    abtWin.exec();
}

void MainWindow::openSettings()
{
    SettingsWindow setWin;
    setWin.setPointers(&_settings);
    setWin.setVisible(true);
    setWin.exec();
    for(int i = 0; i < _settings.size(); i++)
    {
        ui->tableView->setColumnHidden(i, !_settings[i]);
    }
}

void MainWindow::saveCSV()
{
    QString filename = QFileDialog::getSaveFileName(this, "Save file", "", ".csv");

    QFile f(filename);
    f.open(QIODevice::WriteOnly);

    // add a header
    f.write("Company;Operation;Quantity;Price;Date\n");

    // add all the entries
    for(int i = 0; i < _vec.size(); i++)
    {
        // get all the data
        string c_name = _vec[i].compName;
        string action = _vec[i].action;
        int stockQty = _vec[i].stockQty;
        float lastPrice = _vec[i].lastPrice;
        string date = _vec[i].date;
        date.erase(std::remove(date.begin(), date.end(), '\n'), date.end());  // remove newline symbol

        std::stringstream ss;
        ss << c_name << ";" << action << ";" << stockQty << ";" << lastPrice << ";" << date << '\n';
        f.write(ss.str().c_str());
    }
    f.close();
}

void MainWindow::loadCSV()
{
    QString filename = QFileDialog::getOpenFileName(this, "Load file", "");
    QFile inputFile(filename);
    inputFile.open(QIODevice::ReadOnly);

    // make the name pretty
    string winName = filename.toStdString();
    int pos = 0;

    for(int i = 0; i < winName.size(); i++)
        if(winName[i] == '/')
            pos = i;

    winName = winName.substr(pos + 1, winName.size());
    QString qstr = QString::fromStdString("The Stonks App [" + winName + "]");
    this->setWindowTitle(qstr);

    // clearing
    _vec.clear();  // clear the Actions _vector
    _model->clear();
    _tableMap.clear();

    _totalSpent = 0;
    _totalWorth = 0;
    _netProfits = 0;

    // reading the file
    std::string line, token;

    inputFile.readLine();  // skip the first (header) line
    while(!inputFile.atEnd())
    {
        string line = inputFile.readLine().toStdString();
        string token;
        std::istringstream ss(line);
        std::vector<std::string> lineAsVector;

        while(std::getline(ss, token, ';'))
        {
            lineAsVector.push_back(token);  // insert all words of this line in a _vector
        }

        // create an action with given attributes
        Action act;
        act.compName = lineAsVector[0];
        act.action = lineAsVector[1];
        act.stockQty = stoi(lineAsVector[2]);
        act.lastPrice = stof(lineAsVector[3]);
        act.date = lineAsVector[4];

        _vec.push_back(act);
    }

    inputFile.close();

    // process into unqiue companies:
    for(int i = 0; i < _vec.size(); i++)
    {
        // create a checker
        int rowN = 0;
        bool ch = false;
        for(int j = 0; j < _model->rowCount(); j++)
        {
            // cout << _model->index(j, 0, QModelIndex()).data().toString().toStdString() << endl;
            if(_model->index(j, 0, QModelIndex()).data().toString().toStdString() == _vec[i].compName)
            {
                rowN = j;
                ch = true;
            }
        }

        // check if the company is already in _tableMap
        if(ch == true)
        {
            // adjust the price
            float old_price = _model->index(rowN, 2, QModelIndex()).data().toFloat();

            //_tableMap[_vec[i].compName].lastPrice = _vec[i].lastPrice;
            QModelIndex priceIndex = _model->index(rowN, 2, QModelIndex());
            _model->setData(priceIndex, _vec[i].lastPrice);

            // change the quantity
            if(_vec[i].action == "buy")
            {
                //_tableMap[_vec[i].compName].stockQty += _vec[i].stockQty;
                QModelIndex qtyIndex = _model->index(rowN, 1, QModelIndex());
                int qty = _model->index(rowN, 1, QModelIndex()).data().toInt();
                _model->setData(qtyIndex, qty + _vec[i].stockQty);

                // change the money spent
                QModelIndex ssIndex = _model->index(rowN, 4, QModelIndex());
                float ss = _model->index(rowN, 4, QModelIndex()).data().toFloat();

                //_tableMap[_vec[i].compName].stockSpent += _vec[i].stockQty * _vec[i].lastPrice;
                _model->setData(ssIndex, ss + _vec[i].stockQty * _vec[i].lastPrice);

                _totalSpent += _vec[i].stockQty * _vec[i].lastPrice;
            }
            else if(_vec[i].action == "sell")
            {
                QModelIndex qtyIndex = _model->index(rowN, 1, QModelIndex());
                int qty = _model->index(rowN, 1, QModelIndex()).data().toInt();
                _model->setData(qtyIndex, qty - _vec[i].stockQty);

                QModelIndex profitIndex = _model->index(rowN, 3, QModelIndex());
                float profit = _model->index(rowN, 3, QModelIndex()).data().toFloat();
                _model->setData(profitIndex, profit + _vec[i].stockQty * (_vec[i].lastPrice - old_price));

                //_tableMap[_vec[i].compName].lastPrice = _vec[i].lastPrice;
                // 20 lines above it was done
            }

            // change the date
            QModelIndex dateIndex = _model->index(rowN, 6, QModelIndex());
            QString date = QString::fromStdString(_vec[i].date);
            _model->setData(dateIndex, date);

            // change the stock worth according to the last prcie
            QModelIndex swIndex = _model->index(rowN, 5, QModelIndex());
            QModelIndex qtyIndex = _model->index(rowN, 1, QModelIndex());
            int qty = qtyIndex.data().toInt();
            float price = priceIndex.data().toFloat();
            _model->setData(swIndex, qty * price);

            // name, qty, price, profit, stockSpent, StockWorth, date
        }
        else
        {
            int row = _model->rowCount();

            // Inserting a new row in the table
            _model->insertRows(row, 1);

            QStandardItem* item = 0;

            item = new QStandardItem(QString::fromStdString(_vec[i].compName));
            _model->setItem(row, 0, item);

            // name
            QModelIndex index = _model->index(row, 0, QModelIndex());
            QString name = QString::fromStdString(_vec[i].compName);
            _model->setData(index, name);

            // qty
            item = new QStandardItem(QString::number(_vec[i].stockQty));
            _model->setItem(row, 1, item);

            // price
            item = new QStandardItem(QString::number(_vec[i].lastPrice));
            _model->setItem(row, 2, item);

            // profit
            QModelIndex profitIndex = _model->index(row, 3, QModelIndex());
            item = new QStandardItem(profitIndex.data().toFloat());
            _model->setItem(row, 3, item);

            // stockSpent
            item = new QStandardItem(QString::number(_vec[i].stockQty * _vec[i].lastPrice));
            _model->setItem(row, 4, item);

            // StockWorth
            item = new QStandardItem(QString::number(_vec[i].stockQty * _vec[i].lastPrice));
            _model->setItem(row, 5, item);

            // date
            item = new QStandardItem(QString::fromStdString(_vec[i].date));
            _model->setItem(row, 6, item);

            // name, qty, price, profit, stockSpent, StockWorth, date

            _totalSpent += _vec[i].stockQty * _vec[i].lastPrice;
        }
    }

    // calculate total worth according to last price
    // for(unordered_map<string, Company>::iterator it = _tableMap.begin(); it != _tableMap.end(); it++)
    //{
    //    _totalWorth += it->second.stockQty * it->second.lastPrice;
    //    _netProfits += it->second.cNetProfit;
    //}

    for(int k = 0; k < _model->rowCount(); k++)
    {
        int qty = _model->index(k, 1, QModelIndex()).data().toInt();
        float price = _model->index(k, 2, QModelIndex()).data().toFloat();
        float profit = _model->index(k, 3, QModelIndex()).data().toFloat();
        float spent = _model->index(k, 4, QModelIndex()).data().toFloat();

        _totalSpent += spent;
        _totalWorth += qty * price;
        _netProfits += profit;
    }

    // show ''Overview'' section
    ui->TotalSpent->setText(QString::number(_totalSpent));
    ui->TotalWorth->setText(QString::number(_totalWorth));
    ui->NetProfits->setText(QString::number(_netProfits));

    // display the table
    // createTable();

    QStringList labels;
    labels << "Name"
           << "Qty"
           << "Price"
           << "Profit"
           << "Total Spent"
           << "Total Worth"
           << "Date";
    _model->setHorizontalHeaderLabels(labels);
    //------------------------------------------------------------------------------------------------------------
    for(int i = 0; i < _settings.size(); i++)
    {
        ui->tableView->setColumnHidden(i, !_settings[i]);
    }

    // show the chart
    drawChart();
}

void MainWindow::drawChart()
{
    // create an invisible pixmap
    QPixmap pixmap(1000, 500);
    pixmap.fill(QColor("transparent"));

    // set painter's 'scene' to the pixmap
    QPainter painter(&pixmap);

    // setting drawing up
    QPen pen;
    QRectF size = QRectF(50, 75, this->height() / 2, this->height() / 2);
    pen.setColor(Qt::white);
    pen.setWidth(0);
    painter.setPen(pen);
    // size = QRectF(50, 75, this->height() / 2, this->height() / 2);

    double startAngle = 0.0;
    double angle = 0.0;
    double endAngle, percent;

    // for(unordered_map<string, Company>::iterator it = _tableMap.begin(); it != _tableMap.end(); it++)
    for(int i = 0; i < _model->rowCount(); i++)
    {
        QModelIndex nameIndex = _model->index(i, 0, QModelIndex());
        string name = nameIndex.data().toString().toStdString();

        // drawing pie slices according to stock worth of all companies
        QModelIndex swIndex = _model->index(i, 5, QModelIndex());
        float sw = swIndex.data().toFloat();

        percent = sw / _totalWorth;
        angle = percent * 360.0;
        endAngle = startAngle + angle;
        painter.setBrush(QBrush(Qt::black));
        painter.drawPie(size, startAngle * 16, angle * 16);
        startAngle = endAngle;
    }
    // showing the pie chart
    ui->pieLabel->setPixmap(pixmap);
}

int MainWindow::selectedCompany()
{
    // get the index of the selected row
    int compNumber = this->ui->tableView->currentIndex().row();

    return compNumber;
}

void MainWindow::on_buy_stocks_clicked()
{
    buyStocks buyWin;

    // get the name of the company on the selected line
    // string c_name = MainWindow::selectedCompany();
    int compNumber = MainWindow::selectedCompany();

    // set the pointers for the window
    buyWin.setPointers(&_vec, _model, &_totalWorth, &_totalSpent, &_netProfits, compNumber);

    buyWin.setModal(true);
    buyWin.exec();

    // update the table
    // createTable();

    // update the overview
    ui->TotalWorth->setText(QString::number(_totalWorth));
    ui->TotalSpent->setText(QString::number(_totalSpent));
    // _netProfits = _totalWorth - _totalSpent;
    ui->NetProfits->setText(QString::number(_netProfits));

    // update the chart
    drawChart();
    for(int i = 0; i < _settings.size(); i++)
    {
        ui->tableView->setColumnHidden(i, !_settings[i]);
    }
}

void MainWindow::on_Information_clicked()
{
    // check if there are companies to check
    if(_vec.size() == 0)
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Error");
        msgBox.setText("Your Portfolio is Empty!");
        msgBox.exec();
    }
    else
    {
        // get the name of the company on the selected line
        // string c_name = MainWindow::selectedCompany();
        // if(c_name == "")
        //    c_name = _vec[0].compName;

        int row = this->ui->tableView->currentIndex().row();
        QModelIndex index = _model->index(row, 0, QModelIndex());
        // cout << index.data().toString().toStdString();
        info_win infoWin;
        // infoWin.getData(_tableMap[c_name]);

        infoWin.getData(_model, row);
        infoWin.setModal(true);
        infoWin.exec();
    }
}

void MainWindow::on_sell_stocks_clicked()
{
    // check if there are actually companies to check
    if(_vec.size() == 0)
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Error");
        msgBox.setText("Your Portfolio is Empty!");
        msgBox.exec();
    }
    else
    {
        // get the selected company's struct item
        //        string c_name = MainWindow::selectedCompany();
        //        if(c_name == "")
        //            c_name = _vec[0].compName;
        sellstocks sellWin;

        // set needed pointers for the window
        sellWin.setPointers(_model, &_totalWorth, &_totalSpent, &_netProfits, &_vec);
        int rowNumber = MainWindow::selectedCompany();
        sellWin.getData(rowNumber);

        sellWin.setModal(true);
        sellWin.exec();

        // update the table
        // createTable();

        // update the overview
        ui->TotalWorth->setText(QString::number(_totalWorth));
        ui->TotalSpent->setText(QString::number(_totalSpent));
        ui->NetProfits->setText(QString::number(_netProfits));

        // update the chart
        drawChart();
    }
}

// depricated due to loadCSV() existance!!
/*
 * void MainWindow::createList()
{
    std::ifstream inputFile("DATAPROJECT.csv");
    std::string line, token;
    std::getline(inputFile, line);  // skip first line

while(std::getline(inputFile, line))  // reading line-by-line
{
    std::istringstream ss(line);
    std::_vector<std::string> lineAs_vector;
    while(std::getline(ss, token, ';')) lineAs_vector.push_back(token);  // insert all words of this line in a _vector

    Action act;

    act.compName = lineAs_vector[0];
    act.action = lineAs_vector[1];
    act.stockQty = stoi(lineAs_vector[2]);
    act.lastPrice = stof(lineAs_vector[3]);
    act.date = lineAs_vector[4];
    this->_vec.push_back(act);
}
inputFile.close();

// process into unqiue companies:
for(int i = 0; i < _vec.size(); i++)
{
    // check if the company is already in _tableMap
    if(_tableMap.find(_vec[i].compName) != _tableMap.end())
    {
        // adjust the price
        float old_price = _tableMap[_vec[i].compName].lastPrice;
        _tableMap[_vec[i].compName].lastPrice = _vec[i].lastPrice;

        // change the quantity
        if(_vec[i].action == "buy")
        {
            _tableMap[_vec[i].compName].stockQty += _vec[i].stockQty;
            // change the money spent
            _tableMap[_vec[i].compName].stockSpent += _vec[i].stockQty * _vec[i].lastPrice;
            _totalSpent += _vec[i].stockQty * _vec[i].lastPrice;
        }
        else if(_vec[i].action == "sell")
        {
            _tableMap[_vec[i].compName].stockQty -= _vec[i].stockQty;
            _tableMap[_vec[i].compName].lastPrice = _vec[i].lastPrice;
        }

        // change the date
        _tableMap[_vec[i].compName].date = _vec[i].date;

        // change the stock worth according to the last prcie
        _tableMap[_vec[i].compName].stockWorth = _tableMap[_vec[i].compName].stockQty * _tableMap[_vec[i].compName].lastPrice;
        _tableMap[_vec[i].compName].cNetProfit = _tableMap[_vec[i].compName].stockWorth - _tableMap[_vec[i].compName].stockSpent;
        _tableMap[_vec[i].compName].entries += 1;
    }
    else
    {
        // create a company if it is not in the table
        _tableMap[_vec[i].compName].compName = _vec[i].compName;
        _tableMap[_vec[i].compName].entries = 1;
        _tableMap[_vec[i].compName].date = _vec[i].date;
        _tableMap[_vec[i].compName].lastPrice = _vec[i].lastPrice;
        _tableMap[_vec[i].compName].stockQty = _vec[i].stockQty;
        _tableMap[_vec[i].compName].stockSpent = _vec[i].stockQty * _vec[i].lastPrice;
        _tableMap[_vec[i].compName].stockWorth = _vec[i].stockQty * _vec[i].lastPrice;
        _tableMap[_vec[i].compName].cNetProfit = 0;
        _totalSpent += _vec[i].stockQty * _vec[i].lastPrice;
    }
}

// calculate total worth according to last price
for(unordered_map<string, Company>::iterator it = _tableMap.begin(); it != _tableMap.end(); it++)
{
    _totalWorth += it->second.stockQty * it->second.lastPrice;
    _netProfits += it->second.cNetProfit;
}

// show ''Overview'' section
ui->_totalSpent->setText(QString::number(_totalSpent));
ui->_totalWorth->setText(QString::number(_totalWorth));
ui->_netProfits->setText(QString::number(_netProfits));
} */

/*
void MainWindow::createTable()
{
    // creating a model and adding a "name" label

    QStringList labels;
    labels << "Name";

    _model->setHorizontalHeaderLabels(labels);

    // add elements
    QStandardItem* item = 0;
    int i = 0;
    for(unordered_map<string, Company>::iterator it = _tableMap.begin(); it != _tableMap.end(); it++)
    {
        if(it->first != "")
        {
            item = new QStandardItem(QString::fromStdString(it->second.compName));
            _model->setItem(i, 0, item);
            item = new QStandardItem(QString::number(it->second.stockQty));
            _model->setItem(i, 1, item);
            item = new QStandardItem(QString::number(it->second.lastPrice));
            _model->setItem(i, 2, item);
            item = new QStandardItem(QString::number(it->second.cNetProfit));
            _model->setItem(i, 3, item);
            item = new QStandardItem(QString::number(it->second.stockSpent));
            _model->setItem(i, 4, item);
            item = new QStandardItem(QString::number(it->second.stockWorth));
            _model->setItem(i, 5, item);
            item = new QStandardItem(QString::fromStdString(it->second.date));
            _model->setItem(i, 6, item);

            i++;
        }
    }

    ui->tableView->setModel(_model);
    this->ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    //    ui->tableView->setColumnHidden(1, true);
    //    ui->tableView->setColumnHidden(2, true);
    //    ui->tableView->setColumnHidden(3, true);
    //    ui->tableView->setColumnHidden(4, true);
    //    ui->tableView->setColumnHidden(5, true);
    //    ui->tableView->setColumnHidden(6, true);
}
*/

/*
 * string MainWindow::selectedCompany()
{
    // get the index of the selected row
    int compNumber = this->ui->tableView->currentIndex().row();

// get the name of the company in that row
string c_name = _model->data(ui->tableView->model()->index(compNumber, 0)).toString().toStdString();
return c_name;
}
*/
