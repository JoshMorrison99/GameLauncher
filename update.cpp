#include "update.h"
#include "ui_update.h"

Update::Update(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Update)
{
    ui->setupUi(this);
    this->setFixedSize(QSize(400, 300));
}

Update::~Update()
{
    delete ui;
}

void Update::on_buttonBox_accepted()
{
    //qDebug() << "OK";
    emit OK();
}

void Update::on_buttonBox_rejected()
{
    //qDebug() << "CANCELLED";
    emit UserCancelUpdate();
}
