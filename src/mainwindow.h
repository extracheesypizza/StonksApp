#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QStandardItemModel>
#include <QWidget>
#include <map>
#include <unordered_map>

#include "stock_company.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

   public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void createList();
    // void createTable();
    void drawChart();
    int selectedCompany();

   private slots:
    void on_Information_clicked();
    void on_buy_stocks_clicked();
    void on_sell_stocks_clicked();
    void openAbout();
    void saveCSV();
    void loadCSV();
    void openSettings();

   private:
    Ui::MainWindow *ui;
    std::vector<Action> _vec;
    QStandardItemModel *_model;
    unordered_map<string, Company> _tableMap;

    vector<bool> _settings = {true, false, false, false, false, false, false};  // bitset <---

    float _totalWorth = 0;  // double <---
    float _totalSpent = 0;
    float _netProfits = 0;
};

#endif  // MAINWINDOW_H
