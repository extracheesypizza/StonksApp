#ifndef BUYSTOCKS_H
#define BUYSTOCKS_H

#include <QDialog>
#include <QStandardItemModel>
#include <unordered_map>

#include "stock_company.h"

namespace Ui
{
class buyStocks;
}

class buyStocks : public QDialog
{
    Q_OBJECT

   public:
    explicit buyStocks(QWidget *parent = nullptr);
    ~buyStocks();
    void setPointers(std::vector<Action> *vec, QStandardItemModel *model, float *totalWorth, float *totalSpent, float *netProfits, int row);

   private slots:
    void on_pushButton_clicked();

   private:
    Ui::buyStocks *ui;
    Action *_act;
    QStandardItemModel *_model;
    unordered_map<string, Company> *_tableMap;
    std::vector<Action> *_vec;
    float *_totalWorth;
    float *_totalSpent;
    float *_netProfits;
    Company *_comp;
};

#endif  // BUYSTOCKS_H
