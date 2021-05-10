#ifndef UPDATE_H
#define UPDATE_H

#include <QDialog>

namespace Ui {
class Update;
}

class Update : public QDialog
{
    Q_OBJECT

public:
    explicit Update(QWidget *parent = nullptr);
    ~Update();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

signals:
    void UserCancelUpdate();
    void OK();

private:
    Ui::Update *ui;

};

#endif // UPDATE_H
