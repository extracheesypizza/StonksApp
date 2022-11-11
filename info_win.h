#ifndef INFO_WIN_H
#define INFO_WIN_H

#include <stock_company.h>

#include <QDialog>
#include <QStandardItemModel>
#include <QStringListModel>

namespace Ui
{
class info_win;
}

class info_win : public QDialog
{
    Q_OBJECT

   public:
    explicit info_win(QWidget *parent = nullptr);
    ~info_win();
    void getData(QStandardItemModel *mod, int row);

   private:
    Ui::info_win *ui;
};

#endif  // INFO_WIN_H
