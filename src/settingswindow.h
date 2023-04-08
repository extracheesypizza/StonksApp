#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QDialog>

namespace Ui
{
class SettingsWindow;
}

class SettingsWindow : public QDialog
{
    Q_OBJECT

   public:
    explicit SettingsWindow(QWidget *parent = nullptr);
    ~SettingsWindow();
    void setPointers(std::vector<bool> *vec);

   private slots:
    void on_done_clicked();

   private:
    Ui::SettingsWindow *ui;
    std::vector<bool> *_vec;
};

#endif  // SETTINGSWINDOW_H
