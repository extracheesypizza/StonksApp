#ifndef SELLSTOCKS_H
#define SELLSTOCKS_H

#include <QDialog>
#include <QStandardItemModel>
#include <string>
#include <unordered_map>

#include "stock_company.h"

using namespace std;

namespace Ui
{
class sellstocks;
}

class sellstocks : public QDialog
{
    Q_OBJECT

   public:
    explicit sellstocks(QWidget *parent = nullptr);
    ~sellstocks();
    void getData(int row);
    void setPointers(QStandardItemModel *model, float *totalWorth, float *totalSpent, float *netProfits, std::vector<Action> *vec);

   private slots:
    void on_pushButton_clicked();

   private:
    Ui::sellstocks *ui;
    QStandardItemModel *_model;
    Action *_act;
    std::vector<Action> *_vec;

    float *_totalWorth;
    float *_totalSpent;
    float *_netProfits;
};

#endif  // SELLSTOCKS_H
