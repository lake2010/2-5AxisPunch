#include "icprogramguidepage.h"
#include "ui_icprogramguidepage.h"
#include "icvirtualhost.h"

ICProgramGuidePage::ICProgramGuidePage(QWidget *parent) :
    ICInstructionEditorBase(parent),
    ui(new Ui::ICProgramGuidePage),
    pageIndex_(0)
{
    ui->setupUi(this);
    axisWidgets_.append(QList<QWidget*>()<<ui->gx1Label<<ui->x1Edit<<ui->x1Box);
    axisWidgets_.append(QList<QWidget*>()<<ui->gy1Label<<ui->y1Edit<<ui->y1Box);
    axisWidgets_.append(QList<QWidget*>()<<ui->gzLabel<<ui->zEdit<<ui->zBox);
    axisWidgets_.append(QList<QWidget*>()<<ui->gx2Label<<ui->x2Edit<<ui->x2Box);
    axisWidgets_.append(QList<QWidget*>()<<ui->gy2Label<<ui->y2Edit<<ui->y2Box);
    axisWidgets_.append(QList<QWidget*>()<<ui->gaLabel<<ui->aEdit<<ui->aBox);
    axisWidgets_.append(QList<QWidget*>()<<ui->gbLabel<<ui->bEdit<<ui->bBox);
    axisWidgets_.append(QList<QWidget*>()<<ui->gcLabel<<ui->cEdit<<ui->cBox);
    axisWidgets_.append(QList<QWidget*>()<<ui->releaseOutletLabel<<ui->outletBox);
    posEdits_<<ui->x1Edit<<ui->y1Edit<<ui->zEdit<<ui->x2Edit<<ui->y2Edit
            <<ui->aEdit<<ui->bEdit<<ui->cEdit;
    limitEdits_<<ui->x1Box<<ui->y1Box<<ui->zBox<<ui->x2Box<<ui->y2Box
              <<ui->aBox<<ui->bBox<<ui->cBox;

    for(int i = 0; i != 8; ++i)
    {
        posValidators_[i].setBottom(0);
        axis_[i].mode = 0;
        axis_[i].standbyPos = 0;
        axis_[i].getPos = 0;
        axis_[i].releasePos = 0;
    }
    for(int i = 0; i != posEdits_.size(); ++i)
    {
        posEdits_[i]->SetDecimalPlaces(1);
        posEdits_[i]->setValidator(&posValidators_[i]);
    }
    QList<int> temp;
    temp<<ICMold::ACTMAINFORWARD<<ICMold::ACTMAINBACKWARD;
    limitActionMap_.insert(axis_ + 0, temp);
    temp.clear();
    temp<<ICMold::ACTMAINUP<<ICMold::ACTMAINDOWN;
    limitActionMap_.insert(axis_ + 1, temp);
    temp.clear();
    temp<<ICMold::ACTCOMEIN<<ICMold::ACTGOOUT;
    limitActionMap_.insert(axis_ + 2, temp);
    temp.clear();
    temp<<ICMold::ACTVICEFORWARD<<ICMold::ACTVICEBACKWARD;
    limitActionMap_.insert(axis_ + 3, temp);
    temp.clear();
    temp<<ICMold::ACTVICEUP<<ICMold::ACTVICEDOWN;
    limitActionMap_.insert(axis_ + 4, temp);
    temp.clear();
    temp<<ICMold::ACT_PoseHori2<<ICMold::ACT_PoseVert2;
    limitActionMap_.insert(axis_ + 5, temp);
    temp.clear();
    temp<<ICMold::ACTPOSEHORI<<ICMold::ACTPOSEVERT;
    limitActionMap_.insert(axis_ + 6, temp);
    temp.clear();
    temp<<ICMold::ACTPOSEHORI<<ICMold::ACTPOSEVERT;
    limitActionMap_.insert(axis_ + 7, temp);
    temp.clear();

    fixtureOnAction_<<ICMold::ACTCLIP1ON<<ICMold::ACTCLIP2ON<<ICMold::ACTCLIP3ON
                      <<ICMold::ACTCLIP4ON<<ICMold::ACTCLIP5ON<<ICMold::ACTCLIP6ON;
    fixtureOffAction_<<ICMold::ACTCLIP1OFF<<ICMold::ACTCLIP2OFF<<ICMold::ACTCLIP3OFF
                   <<ICMold::ACTCLIP4OFF<<ICMold::ACTCLIP5OFF<<ICMold::ACTCLIP6OFF;

    on_usedMainArmBox_toggled(ui->usedMainArmBox->isChecked());
    on_usedSubArmBox_toggled(ui->usedSubArmBox->isChecked());


#ifdef Q_WS_X11
    UpdateAxisDefine_();
#endif
}

ICProgramGuidePage::~ICProgramGuidePage()
{
    delete ui;
}

void ICProgramGuidePage::changeEvent(QEvent *e)
{
    ICInstructionEditorBase::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void ICProgramGuidePage::showEvent(QShowEvent *e)
{
    pageIndex_ = 0;
    ui->stackedWidget->setCurrentIndex(0);
    UpdateAxisDefine_();
    ICVirtualHost* host = ICVirtualHost::GlobalVirtualHost();
    posMaxs_[0] = host->SystemParameter(ICVirtualHost::SYS_X_Maxium).toInt();
    posMaxs_[1] = host->SystemParameter(ICVirtualHost::SYS_Y_Maxium).toInt();
    posMaxs_[2] = host->SystemParameter(ICVirtualHost::SYS_Z_Maxium).toInt();
    posMaxs_[3] = host->SystemParameter(ICVirtualHost::SYS_P_Maxium).toInt();
    posMaxs_[4] = host->SystemParameter(ICVirtualHost::SYS_Q_Maxium).toInt();
    posMaxs_[5] = host->SystemParameter(ICVirtualHost::SYS_A_Maxium).toInt();
    posMaxs_[6] = host->SystemParameter(ICVirtualHost::SYS_B_Maxium).toInt();
    posMaxs_[7] = host->SystemParameter(ICVirtualHost::SYS_C_Maxium).toInt();
    for(int i = 0; i != 8; ++i)
    {
        posValidators_[i].setTop(posMaxs_[i]);
    }
    ICInstructionEditorBase::showEvent(e);
}

void ICProgramGuidePage::SyncStatusImpl(const QList<ICMoldItem> &items)
{
    Q_UNUSED(items)

}

QList<ICMoldItem> ICProgramGuidePage::CreateCommandImpl() const
{
    ICMoldItem item;
    QList<ICMoldItem> ret;
    int stepNum = 0;
    item.SetSVal(80);
    item.SetDVal(0);
    item.SetNum(stepNum++);
    item.SetSubNum(255);
    for(int i = 0; i != 8; ++i )//standby step
    {
        if(SetAxisICMoldItem_(&item, axis_ + i, STANDBY_SETTING))
        {
            ret.append(item);
        }
    }
    /*add wait for mold-open step*/
    item.SetNum(stepNum++);
    item.SetSVal(1);
    item.SetDVal(0);
    item.SetPos(0);
    item.SetAction(ICMold::ACT_WaitMoldOpened);
    ret.append(item);

    /*donw to get product*/
    item.SetSVal(80);
    item.SetDVal(0);
    int tempStep;
    bool isMainArmUsed = ui->usedMainArmBox->isChecked();
    bool isSubArmUsed = ui->usedSubArmBox->isChecked();
    if(axis_[Z_AXIS].mode == AXIS_SERVO)
    {
        item.SetNum(stepNum++);
        if(axis_[Z_AXIS].standbyPos != axis_[Z_AXIS].getPos)
        {
            item.SetAction(axis_[Z_AXIS].action);
            ret.append(item);
        }
    }
    if(isMainArmUsed)
    {
        tempStep = stepNum;
        item.SetNum(stepNum++);
        if(SetAxisICMoldItem_(&item, axis_ + Y1_AXIS, GET_PRODUCT_SETTING))
        {
            ret.append(item);
        }
    }
    if(isSubArmUsed)
    {
        item.SetNum(tempStep);
        if(SetAxisICMoldItem_(&item, axis_ + Y2_AXIS, GET_PRODUCT_SETTING))
        {
            ret.append(item);
        }
    }

    /*Ejector on*/
    item.SetNum(stepNum++);
    item.SetClip(ICMold::ACTEJECTON);
    item.SetDVal(10);
    ret.append(item);

    /*x axis forward to get product*/
    item.SetDVal(0);
    item.SetSVal(80);
    if(isMainArmUsed)
    {
        tempStep = stepNum;
        item.SetNum(stepNum++);
        if(SetAxisICMoldItem_(&item, axis_ + X1_AXIS, GET_PRODUCT_SETTING))
        {
            ret.append(item);
        }
    }
    if(isSubArmUsed)
    {
        item.SetNum(tempStep);
        if(SetAxisICMoldItem_(&item, axis_ + X2_AXIS, GET_PRODUCT_SETTING))
        {
            ret.append(item);
        }
    }

    /*clip product*/
    item.SetDVal(10);
    if(isMainArmUsed)
    {
        tempStep = stepNum;
        item.SetNum(stepNum++);
        item.SetClip(fixtureOnAction_.at(ui->productFixtureBox->currentIndex()));
        ret.append(item);
    }
    if(isSubArmUsed)
    {
        item.SetNum(tempStep);
        item.SetClip(fixtureOnAction_.at(ui->outletFixtureBox->currentIndex()));
        ret.append(item);
    }


    /*X axis backward*/
    item.SetDVal(0);
    item.SetSVal(80);
    if(isMainArmUsed)
    {
        tempStep = stepNum;
        item.SetNum(stepNum++);
        if(SetAxisICMoldItem_(&item, axis_ + X1_AXIS, STANDBY_SETTING))
        {
            ret.append(item);
        }
    }
    if(isSubArmUsed)
    {
        item.SetNum(tempStep);
        if(SetAxisICMoldItem_(&item, axis_ + X2_AXIS, STANDBY_SETTING))
        {
            ret.append(item);
        }
    }

    /*fixture check*/
    item.SetAction(ICMold::ACT_Cut);
    item.SetIFVal(1);
    if(isMainArmUsed && ui->productCheck->isChecked())
    {
        tempStep = stepNum;
        item.SetNum(stepNum++);
        item.SetSVal(ui->productFixtureBox->currentIndex());
        ret.append(item);
    }
    if(isSubArmUsed && ui->outletCheck->isChecked())
    {
        item.SetNum(tempStep);
        item.SetSVal(ui->outletFixtureBox->currentIndex());
        ret.append(item);
    }

    /*go up*/
    item.SetSVal(80);
    item.SetDVal(0);
    if(isMainArmUsed)
    {
        tempStep = stepNum;
        item.SetNum(stepNum++);
        if(SetAxisICMoldItem_(&item, axis_ + Y1_AXIS, STANDBY_SETTING))
        {
            ret.append(item);
        }
    }
    if(isSubArmUsed)
    {
        item.SetNum(tempStep);
        if(SetAxisICMoldItem_(&item, axis_ + Y2_AXIS, STANDBY_SETTING))
        {
            ret.append(item);
        }
    }

    /*go out*/
    item.SetNum(stepNum++);
    if(ui->outHorizontalBox->isChecked())
    {
        tempStep = stepNum;
        item.SetAction(ICMold::ACTPOSEHORI);
    }
    else if(ui->outVerticalBox->isChecked())
    {
        tempStep = stepNum;
        item.SetAction(ICMold::ACTPOSEVERT);
    }
    else if(ui->outRunningHorizonBox->isChecked())
    {
        tempStep = item.Num();
        item.SetAction(ICMold::ACTPOSEHORI);
    }
    ret.append(item);

//    int pos = qMin(axis_[Z_AXIS].)

    return ret;

}

void ICProgramGuidePage::UpdateAxisDefine_()
{
    ICVirtualHost* host = ICVirtualHost::GlobalVirtualHost();
    int currentAxis = host->SystemParameter(ICVirtualHost::SYS_Config_Arm).toInt();
    if(axisDefine_ != currentAxis)
    {
        axisDefine_ = currentAxis;
        for(int i = 0 ; i != axisWidgets_.size(); ++i)
        {
            HideWidgets_(axisWidgets_[i]);
        }

        if(host->AxisDefine(ICVirtualHost::ICAxis_AxisX1) == ICVirtualHost::ICAxisDefine_None)
        {
            HideWidgets_(axisWidgets_[0]);
            axis_[0].mode = AXIS_NONE;
        }
        else if(host->AxisDefine(ICVirtualHost::ICAxis_AxisX1) == ICVirtualHost::ICAxisDefine_Pneumatic)
        {
            ShowWidgets_(axisWidgets_[0]);
            ui->x1Edit->hide();
            axis_[0].mode = AXIS_PNEUMATIC;
            //            ui->x1pLabel->hide();
        }
        else
        {
            ShowWidgets_(axisWidgets_[0]);
            ui->x1Box->hide();
            axis_[0].mode = AXIS_SERVO;
            axis_[0].action = ICMold::GX;
        }

        if(host->AxisDefine(ICVirtualHost::ICAxis_AxisY1) == ICVirtualHost::ICAxisDefine_None)
        {
            HideWidgets_(axisWidgets_[1]);
            axis_[1].mode = AXIS_NONE;
        }
        else if(host->AxisDefine(ICVirtualHost::ICAxis_AxisY1) == ICVirtualHost::ICAxisDefine_Pneumatic)
        {
            ShowWidgets_(axisWidgets_[1]);
            ui->y1Edit->hide();
            axis_[1].mode = AXIS_PNEUMATIC;
        }
        else
        {
            ShowWidgets_(axisWidgets_[1]);
            ui->y1Box->hide();
            axis_[1].mode = AXIS_SERVO;
            axis_[1].action = ICMold::GY;
        }

        if(host->AxisDefine(ICVirtualHost::ICAxis_AxisZ) == ICVirtualHost::ICAxisDefine_None)
        {
            HideWidgets_(axisWidgets_[2]);
            axis_[2].mode = AXIS_NONE;
        }
        else if(host->AxisDefine(ICVirtualHost::ICAxis_AxisZ) == ICVirtualHost::ICAxisDefine_Pneumatic)
        {
            ShowWidgets_(axisWidgets_[2]);
            ui->zEdit->hide();
            axis_[2].mode = AXIS_PNEUMATIC;
        }
        else
        {
            ShowWidgets_(axisWidgets_[2]);
            ui->zBox->hide();
            axis_[2].mode = AXIS_SERVO;
            axis_[2].action = ICMold::GZ;
        }

        if(host->AxisDefine(ICVirtualHost::ICAxis_AxisX2) == ICVirtualHost::ICAxisDefine_None)
        {
            HideWidgets_(axisWidgets_[3]);
            axis_[3].mode = AXIS_NONE;
        }
        else if(host->AxisDefine(ICVirtualHost::ICAxis_AxisX2) == ICVirtualHost::ICAxisDefine_Pneumatic)
        {
            ShowWidgets_(axisWidgets_[3]);
            ui->x2Edit->hide();
            axis_[3].mode = AXIS_PNEUMATIC;
        }
        else
        {
            ShowWidgets_(axisWidgets_[3]);
            ui->x2Box->hide();
            axis_[3].mode = AXIS_SERVO;
            axis_[3].action = ICMold::GP;
        }

        if(host->AxisDefine(ICVirtualHost::ICAxis_AxisY2) == ICVirtualHost::ICAxisDefine_None)
        {
            HideWidgets_(axisWidgets_[4]);
            axis_[4].mode = AXIS_NONE;
        }
        else if(host->AxisDefine(ICVirtualHost::ICAxis_AxisY2) == ICVirtualHost::ICAxisDefine_Pneumatic)
        {
            ShowWidgets_(axisWidgets_[4]);
            ui->y2Edit->hide();
            axis_[4].mode = AXIS_PNEUMATIC;
        }
        else
        {
            ShowWidgets_(axisWidgets_[4]);
            ui->y2Box->hide();
            axis_[4].mode = AXIS_SERVO;
            axis_[4].action = ICMold::GQ;
        }

        if(host->AxisDefine(ICVirtualHost::ICAxis_AxisA) == ICVirtualHost::ICAxisDefine_None)
        {
            HideWidgets_(axisWidgets_[5]);
            axis_[5].mode = AXIS_NONE;
        }
        else if(host->AxisDefine(ICVirtualHost::ICAxis_AxisA) == ICVirtualHost::ICAxisDefine_Pneumatic)
        {
            ShowWidgets_(axisWidgets_[5]);
            ui->aEdit->hide();
            axis_[5].mode = AXIS_PNEUMATIC;
        }
        else
        {
            ShowWidgets_(axisWidgets_[5]);
            ui->aBox->hide();
            axis_[5].mode = AXIS_SERVO;
            axis_[5].action = ICMold::GA;
        }

        if(host->AxisDefine(ICVirtualHost::ICAxis_AxisB) == ICVirtualHost::ICAxisDefine_None)
        {
            HideWidgets_(axisWidgets_[6]);
            axis_[6].mode = AXIS_NONE;
        }
        else if(host->AxisDefine(ICVirtualHost::ICAxis_AxisB) == ICVirtualHost::ICAxisDefine_Pneumatic)
        {
            ShowWidgets_(axisWidgets_[6]);
            ui->bEdit->hide();
            axis_[6].mode = AXIS_PNEUMATIC;
        }
        else
        {
            ShowWidgets_(axisWidgets_[6]);
            ui->bBox->hide();
            axis_[6].mode = AXIS_SERVO;
            axis_[6].action = ICMold::GB;
        }

        if(host->AxisDefine(ICVirtualHost::ICAxis_AxisC) == ICVirtualHost::ICAxisDefine_None)
        {
            HideWidgets_(axisWidgets_[7]);
            axis_[7].mode = AXIS_NONE;
        }
        else if(host->AxisDefine(ICVirtualHost::ICAxis_AxisC) == ICVirtualHost::ICAxisDefine_Pneumatic)
        {
            ShowWidgets_(axisWidgets_[7]);
            ui->cEdit->hide();
            axis_[7].mode = AXIS_PNEUMATIC;
        }
        else
        {
            ShowWidgets_(axisWidgets_[7]);
            ui->cBox->hide();
            axis_[7].mode = AXIS_SERVO;
            axis_[7].action = ICMold::GC;
        }
    }
}

void ICProgramGuidePage::ShowWidgets_(QList<QWidget *> &widgets)
{
    for(int i = 0; i != widgets.size(); ++i)
    {
        widgets[i]->show();;
    }
}

void ICProgramGuidePage::HideWidgets_(QList<QWidget *> &widgets)
{
    for(int i = 0; i != widgets.size(); ++i)
    {
        widgets[i]->hide();;
    }
}

void ICProgramGuidePage::on_nextButton_clicked()
{
    ++pageIndex_;
    ui->productLabel->show();
    ui->outletLabel->show();
    if(pageIndex_ == 1) //show for standby settings
    {
        ui->stackedWidget->setCurrentIndex(1);
        ShowForStandby_();
        UpdateAxisShow(STANDBY_SETTING);
    }
    else if(pageIndex_ == 2)
    {
        ui->stackedWidget->setCurrentIndex(1);
        ui->descrLabel->setText(tr("Get Position Settings"));
        HideWidgets_(axisWidgets_[8]);
        SaveAxis_(STANDBY_SETTING);
        UpdateAxisShow(GET_PRODUCT_SETTING);
    }
    else if(pageIndex_ == 3)
    {
        ui->stackedWidget->setCurrentIndex(1);
        ui->descrLabel->setText(tr("Release Position Settings"));
        ShowWidgets_(axisWidgets_[8]);
        SaveAxis_(GET_PRODUCT_SETTING);
        UpdateAxisShow(RELEASE_PRODUCT_SETTING);
    }
    else if(pageIndex_ == 4)
    {
        ui->stackedWidget->setCurrentIndex(2);
        SaveAxis_(RELEASE_PRODUCT_SETTING);
    }
    UpdatePageButton_();
}

void ICProgramGuidePage::on_preButton_clicked()
{
    --pageIndex_;
    ui->productLabel->show();
    ui->outletLabel->show();
    if(pageIndex_ == 1) //show for standby settings
    {
        ui->stackedWidget->setCurrentIndex(1);
        ShowForStandby_();
        SaveAxis_(GET_PRODUCT_SETTING);
        UpdateAxisShow(STANDBY_SETTING);
    }
    else if(pageIndex_ == 2)
    {
        ui->stackedWidget->setCurrentIndex(1);
        ui->descrLabel->setText(tr("Get Position Settings"));
        HideWidgets_(axisWidgets_[8]);
        SaveAxis_(REPORT_SLAVE_ID_LENGTH);
        UpdateAxisShow(GET_PRODUCT_SETTING);
    }
    else if(pageIndex_ == 3)
    {
        ui->stackedWidget->setCurrentIndex(1);
        ui->descrLabel->setText(tr("Release Position Settings"));
        ShowWidgets_(axisWidgets_[8]);
        UpdateAxisShow(RELEASE_PRODUCT_SETTING);
    }
    else if(pageIndex_ == 0)
    {
        ui->stackedWidget->setCurrentIndex(0);
        SaveAxis_(STANDBY_SETTING);
    }
    UpdatePageButton_();
}

void ICProgramGuidePage::UpdatePageButton_()
{
    if(pageIndex_ != 0)
    {
        ui->preButton->setEnabled(true);
        ui->nextButton->setEnabled(true);
    }
    else
    {
        ui->preButton->setEnabled(false);
    }
    if(ui->stackedWidget->currentIndex() == 2)
    {
        ui->nextButton->setEnabled(false);
    }
}

void ICProgramGuidePage::ShowForStandby_()
{
    ui->descrLabel->setText(tr("Stanby Settings"));
    HideWidgets_(axisWidgets_[8]);
    ICVirtualHost* host = ICVirtualHost::GlobalVirtualHost();
    if(host->AxisDefine(ICVirtualHost::ICAxis_AxisX1) == ICVirtualHost::ICAxisDefine_Pneumatic)
    {
        HideWidgets_(axisWidgets_[0]);
        ui->x1Box->setCurrentIndex(1);
    }
    if(host->AxisDefine(ICVirtualHost::ICAxis_AxisY1) == ICVirtualHost::ICAxisDefine_Pneumatic)
    {
        HideWidgets_(axisWidgets_[1]);
        ui->y1Box->setCurrentIndex(0);
    }
    if(host->AxisDefine(ICVirtualHost::ICAxis_AxisZ) == ICVirtualHost::ICAxisDefine_Pneumatic)
    {
        HideWidgets_(axisWidgets_[2]);
        ui->zBox->setCurrentIndex(0);
    }
    if(host->AxisDefine(ICVirtualHost::ICAxis_AxisX2) == ICVirtualHost::ICAxisDefine_Pneumatic)
    {
        HideWidgets_(axisWidgets_[3]);
        ui->x2Box->setCurrentIndex(1);
    }
    if(host->AxisDefine(ICVirtualHost::ICAxis_AxisY2) == ICVirtualHost::ICAxisDefine_Pneumatic)
    {
        HideWidgets_(axisWidgets_[4]);
        ui->y2Box->setCurrentIndex(0);
    }
    if(host->AxisDefine(ICVirtualHost::ICAxis_AxisA) == ICVirtualHost::ICAxisDefine_Servo)
    {
        HideWidgets_(axisWidgets_[5]);
        ui->aEdit->SetThisIntToThisText(0);
    }
    if(host->AxisDefine(ICVirtualHost::ICAxis_AxisB) == ICVirtualHost::ICAxisDefine_Servo)
    {
        HideWidgets_(axisWidgets_[6]);
        ui->bEdit->SetThisIntToThisText(0);
    }
    if(host->AxisDefine(ICVirtualHost::ICAxis_AxisC) == ICVirtualHost::ICAxisDefine_Servo)
    {
        HideWidgets_(axisWidgets_[7]);
        ui->cEdit->SetThisIntToThisText(0);
    }
    ui->productLabel->hide();
    ui->outletLabel->hide();
}

void ICProgramGuidePage::SetAxis_(_ICAxis_ *axis, int pos, int setting)
{
    if(setting == STANDBY_SETTING)
    {
        axis->standbyPos = pos;
    }
    else if(setting == GET_PRODUCT_SETTING)
    {
        axis->getPos = pos;
    }
    else if(setting == RELEASE_PRODUCT_SETTING)
    {
        axis->releasePos = pos;
    }
}

void ICProgramGuidePage::SaveAxis_(int setting)
{
    for(int i = 0; i != posEdits_.size(); ++i)
    {
        if(axis_[i].mode == AXIS_SERVO)
        {
            SetAxis_(axis_ + i, posEdits_.at(i)->TransThisTextToThisInt(), setting);
        }
        else if(axis_[i].mode == AXIS_PNEUMATIC)
        {
            SetAxis_(axis_ + i, limitEdits_.at(i)->currentIndex(), setting);
        }
    }
}

void ICProgramGuidePage::SetAxisPosEdit_(ICLineEditWithVirtualNumericKeypad *edit, _ICAxis_ *axis, int setting)
{
    if(setting == STANDBY_SETTING)
    {
        edit->SetThisIntToThisText(axis->standbyPos);
    }
    else if(setting == GET_PRODUCT_SETTING)
    {
        edit->SetThisIntToThisText(axis->getPos);
    }
    else if(setting == RELEASE_PRODUCT_SETTING)
    {
        edit->SetThisIntToThisText(axis->releasePos);
    }
}

void ICProgramGuidePage::SetAxisLimitEdit_(QComboBox *edit, _ICAxis_ *axis, int setting)
{
    if(setting == STANDBY_SETTING)
    {
        edit->setCurrentIndex(axis->standbyLimit);
    }
    else if(setting == GET_PRODUCT_SETTING)
    {
        edit->setCurrentIndex(axis->getLimit);
    }
    else if(setting == RELEASE_PRODUCT_SETTING)
    {
        edit->setCurrentIndex(axis->releaseLimit);
    }
}

void ICProgramGuidePage::UpdateAxisShow(int setting)
{
    for(int i = 0; i != posEdits_.size(); ++i)
    {
        if(axis_[i].mode == AXIS_SERVO)
        {
            SetAxisPosEdit_(posEdits_[i], axis_ + i, setting);
        }
        else if(axis_[i].mode == AXIS_PNEUMATIC)
        {
            SetAxisLimitEdit_(limitEdits_[i], axis_ + i, setting);
        }
    }
}

bool ICProgramGuidePage::SetAxisICMoldItem_(ICMoldItem *item, const _ICAxis_ *axis, int setting) const
{
    if(axis->mode == AXIS_SERVO)
    {
        item->SetAction(axis->action);
        if(setting == STANDBY_SETTING)
        {
            item->SetPos(axis->standbyPos);
        }
        else if(setting == GET_PRODUCT_SETTING)
        {
            item->SetPos(axis->getPos);
        }
        else if(setting == RELEASE_PRODUCT_SETTING)
        {
            item->SetPos(axis->releasePos);
        }
        return true;
    }
    else if(axis->mode == AXIS_PNEUMATIC)
    {
        item->SetAction(limitActionMap_.value(axis).at(axis->standbyLimit));
        item->SetPos(0);
        return true;
    }
    return false;
}

void ICProgramGuidePage::on_finishButton_clicked()
{
    if(ui->stackedWidget->currentIndex() == 1)
    {
        SaveAxis_(pageIndex_);
    }
    emit GuideFinished();
}

void ICProgramGuidePage::on_usedMainArmBox_toggled(bool checked)
{
    ui->productLabel->setEnabled(checked);
    ui->x1Box->setEnabled(checked);
    ui->x1Edit->setEnabled(checked);
    ui->y1Box->setEnabled(checked);
    ui->y1Edit->setEnabled(checked);
    ui->productFixtureGroup->setEnabled(checked);
}

void ICProgramGuidePage::on_usedSubArmBox_toggled(bool checked)
{
    ui->outletLabel->setEnabled(checked);
    ui->x2Box->setEnabled(checked);
    ui->x2Edit->setEnabled(checked);
    ui->y2Box->setEnabled(checked);
    ui->y2Edit->setEnabled(checked);
    ui->outletFixtureGroup->setEnabled(checked);
}