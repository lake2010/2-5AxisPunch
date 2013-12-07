#include "axismodifydialog.h"
#include "ui_axismodifydialog.h"
#include "icvirtualhost.h"

AxisModifyDialog::AxisModifyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AxisModifyDialog)
{
    ui->setupUi(this);
    ui->xPos->SetDecimalPlaces(1);
    ui->yPos->SetDecimalPlaces(1);
//    ui->zPos->SetDecimalPlaces(1);
    xValidator_ = new QIntValidator(-32760, 32760, this);
    yValidator_ = new QIntValidator(-32760, 32760, this);
    zValidator_ = new QIntValidator(-32760, 32760, this);
    ui->xPos->setValidator(xValidator_);
    ui->yPos->setValidator(yValidator_);
//    ui->zPos->setValidator(zValidator_);
}

AxisModifyDialog::~AxisModifyDialog()
{
    delete ui;
}

void AxisModifyDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void AxisModifyDialog::on_okButton_clicked()
{
    ICMold::CurrentMold()->SetMoldParam(static_cast<ICMold::ICMoldParam>(point_.pointID * 3), ui->xPos->TransThisTextToThisInt());
    ICMold::CurrentMold()->SetMoldParam(static_cast<ICMold::ICMoldParam>(point_.pointID * 3 + 1), ui->yPos->TransThisTextToThisInt());
//    ICMold::CurrentMold()->SetMoldParam(static_cast<ICMold::ICMoldParam>(point_.pointID * 3 + 2), ui->zPos->TransThisTextToThisInt());
    this->accept();
}

void AxisModifyDialog::on_cancelButton_clicked()
{
    this->reject();
}

void AxisModifyDialog::StartModify(ICPoint point)
{
    point_ = point;
    ICVirtualHost* host = ICVirtualHost::GlobalVirtualHost();
    xValidator_->setRange(host->SystemParameter(ICVirtualHost::SYS_X_Maxium).toInt(),
                          host->SystemParameter(ICVirtualHost::SYS_X_Length).toInt());
    yValidator_->setRange(host->SystemParameter(ICVirtualHost::SYS_Y_Maxium).toInt(),
                          host->SystemParameter(ICVirtualHost::SYS_Y_Length).toInt());
    zValidator_->setRange(host->SystemParameter(ICVirtualHost::SYS_Z_Maxium).toInt(),
                          host->SystemParameter(ICVirtualHost::SYS_Z_Length).toInt());
    ui->xPos->SetThisIntToThisText(point.x);
    ui->yPos->SetThisIntToThisText(point.y);
//    ui->zPos->SetThisIntToThisText(point.z);
    this->show();
}

void AxisModifyDialog::on_setButton_clicked()
{
    if(point_.pointID < 0)
    {
        return;
    }
    ICVirtualHost* host = ICVirtualHost::GlobalVirtualHost();
    ui->xPos->SetThisIntToThisText(host->HostStatus(ICVirtualHost::XPos).toInt());
    ui->yPos->SetThisIntToThisText(host->HostStatus(ICVirtualHost::YPos).toInt());
//    ui->zPos->SetThisIntToThisText(host->HostStatus(ICVirtualHost::ZPos).toInt());
}
