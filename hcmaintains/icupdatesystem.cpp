#include "icupdatesystem.h"
#include "ui_icupdatesystem.h"

#include <QSettings>
#include <QDir>
#include <QMessageBox>
#include <QCloseEvent>
#include <QProcess>
#include <QKeyEvent>

#include <QDebug>
#include "ickeyboard.h"
#include "iccommandprocessor.h"
#include "iccommands.h"
#include "icvirtualhost.h"
#include "icupdatelogodialog.h"
//ICUpdateSystem *icUpdateSystem = NULL;

ICUpdateSystem::ICUpdateSystem(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::ICUpdateSystem),
    updateIniPath_("/mnt/udisk/HCUpdate_5A/"),
    updateHostPath_("/mnt/udisk/HCUpdateHost_5A/"),
    updateSettings_(NULL),
    updateHostSettings_(NULL),
    status_(-1),
    updateDialog_(NULL)
{
    ui->setupUi(this);

    InitInterface();
//    icUpdateSystem = this;
//    hostStatusToStringMap_.insert(-1, "Connect Host Fail");
    connect(&timer_,
            SIGNAL(timeout()),
            SLOT(QueryStatus()));
}

ICUpdateSystem::~ICUpdateSystem()
{
    delete ui;
    if(updateSettings_ != NULL)
    {
        delete updateSettings_;
    }
    if(updateHostSettings_ != NULL)
    {
        delete updateHostSettings_;
    }

}

void ICUpdateSystem::changeEvent(QEvent *e)
{
    QFrame::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void ICUpdateSystem::showEvent(QShowEvent *e)
{
//    timer_.start(1000);
    ICVirtualHost::GlobalVirtualHost()->StopRefreshStatus();
    QFrame::showEvent(e);
}

void ICUpdateSystem::hideEvent(QHideEvent *e)
{
    timer_.stop();
    ui->updateToolButton->setEnabled(false);
    ui->updateHostButton->setEnabled(false);
    ui->connectHostButton->setEnabled(false);
    ui->writeHostButton->setEnabled(false);
    ui->rebootButton->setEnabled(false);
    ICVirtualHost::GlobalVirtualHost()->RestartRefreshStatus();
    QFrame::hideEvent(e);
}

void ICUpdateSystem::on_refreshToolButton_clicked()
{
    RefreshUSBIniInfo();
}

void ICUpdateSystem::on_updateToolButton_clicked()
{
    SystemUpdateStart();
}

void ICUpdateSystem::SystemUpdateStart()
{
    QDir dir("/proc/scsi/usb-storage");
    if(!dir.exists())
    {
        QMessageBox::warning(this,tr("tips"),tr("USB no exists...")) ;
        ui->hmiVersionShowLabel->setText(tr("No available HMI version"));
        ui->hostVersionShowLabel->setText(tr("No available Host version"));
        ui->connectHostButton->setEnabled(false);
        ui->updateToolButton->setEnabled(false);
        return;
    }
    if(updateSettings_ == NULL)
    {
        return;
    }
    QStringList updateFileList = updateSettings_->childGroups();
    if(updateFileList.count() > 0)
    {
        ui->copyFilesProgressBar->setRange(0, updateFileList.count());
        ui->copyFilesProgressBar->setValue(0);
    }

    if(updateFileList.isEmpty())
    {
        return;
    }

    QDir targetFileDir;
    for(int i = 0; i < updateFileList.count(); ++i)
    {
        updateSettings_->beginGroup(updateFileList.at(i));
        targetFileDir.setPath(updateSettings_->value("targetPath").toString());

        if(!targetFileDir.absolutePath().isEmpty())
        {
            targetFileDir.mkpath(targetFileDir.absolutePath());
            QFile updateFile(targetFileDir.absolutePath() + '/' + updateFileList.at(i));
            if(updateFile.exists())
            {
                updateFile.remove();
            }

            QDir currentFileDir(updateIniPath_);
            currentFileDir.cd(updateSettings_->value("currentPath").toString());

            QFile::copy(currentFileDir.absolutePath() + '/' + updateFileList.at(i),
                        targetFileDir.absolutePath() + '/' + updateFileList.at(i));
            ui->copyFilesProgressBar->setValue(i + 1);
        }
        updateSettings_->endGroup();
    }
    //    if(QFile::exists("/opt/Qt/bin/custom_step.sh"))
    //    {
    //        system("cd /opt/Qt/bin && chmod +x custom_step.sh && ./custom_step.sh && rm /opt/Qt/bin/custom_step.sh");
    //    }
    int ret = QMessageBox::information(this, tr("Congratulations"),
                                       tr("Update finish\n"
                                          "You must restart this program\n"
                                          "Would you want to restart now?"),
                                       QMessageBox::Yes | QMessageBox::Cancel,
                                       QMessageBox::Yes);
    switch(ret)
    {
    case QMessageBox::Yes:
        RestartAndUpdateTheProgram();
        break;
    default:
        break;
    }
}

void ICUpdateSystem::RefreshUSBIniInfo()
{
    if(updateSettings_ != NULL)
    {
        delete updateSettings_;
        updateSettings_ = NULL;
    }

    if(updateHostSettings_ != NULL)
    {
        delete updateHostSettings_;
        updateHostSettings_ = NULL;
    }

    updateSettings_ = new QSettings(updateIniPath_ + "HCUpdate", QSettings::IniFormat);
    updateHostSettings_ = new QSettings(updateHostPath_ + "HCUpdateHost", QSettings::IniFormat);
    ui->hmiVersionShowLabel->setText(updateSettings_->value("version", tr("No available HMI version")).toString());
    ui->hostVersionShowLabel->setText(updateHostSettings_->value("version", tr("No available Host version")).toString());
    if(!updateSettings_->value("version","").toString().isEmpty())
    {
        ui->updateToolButton->setEnabled(true);
    }
    if(!updateHostSettings_->value("version", "").toString().isEmpty())
    {
        ui->connectHostButton->setEnabled(true);
    }
    //    QStringList updateFileList = updateSettings_->childGroups();

    //    if(updateFileList.count() > 0)
    //    {
    //        ui->copyFilesProgressBar->setRange(0, updateFileList.count());
    //        ui->copyFilesProgressBar->setValue(0);
    //    }
}

void ICUpdateSystem::RestartAndUpdateTheProgram()
{
    //    qApp->notify(qApp, new QCloseEvent());
    if(QFile::exists("/opt/Qt/bin/Multi-axisManipulatorSystem.bfe"))
    {
        system("cd /opt/Qt/bin \
               && mv Multi-axisManipulatorSystem Multi-axisManipulatorSystemOld \
               && decrypt.sh Multi-axisManipulatorSystem.bfe \
               && rm Multi-axisManipulatorSystemOld");
    }
    if(QFile::exists("/opt/Qt/bin/custom_step.sh"))
    {
        system("cd /opt/Qt/bin && chmod +x custom_step.sh && ./custom_step.sh && rm /opt/Qt/bin/custom_step.sh");
    }
    system("reboot");
}

void ICUpdateSystem::InitInterface()
{
    ui->copyFilesProgressBar->setValue(0);

//    ui->versionLabel->setEnabled(false);
//    ui->versionShowLabel->setEnabled(false);
//    ui->copyFilesLabel->setEnabled(false);
//    ui->copyFilesShowLabel->setEnabled(false);
//    ui->copyFilesProgressBar->setEnabled(false);
//    ui->rebootLabel->setEnabled(false);
//    ui->rebootShowLabel->setEnabled(false);

//    ui->deviceShowLabel->setStyleSheet("border: 1px solid rgb(192,192,192);");
//    ui->versionShowLabel->setStyleSheet("border: 1px solid rgb(192,192,192);");
//    ui->copyFilesShowLabel->setStyleSheet("border: 1px solid rgb(192,192,192);");
//    ui->rebootShowLabel->setStyleSheet("border: 1px solid rgb(192,192,192);");
}

void ICUpdateSystem::keyPressEvent(QKeyEvent *e)
{
    switch(e->key())
    {
    case ICKeyboard::FB_Up:
    {
        RefreshUSBIniInfo();
    }
    break;
    case ICKeyboard::FB_Down:
    {
        SystemUpdateStart();
    }
    break;
    }
    QFrame::keyPressEvent(e);
}

void ICUpdateSystem::on_updateHostButton_clicked()
{
    if(updateHostSettings_ == NULL)
    {
        return;
    }
    QStringList updateFileList = updateHostSettings_->childGroups();
    if(updateFileList.isEmpty())
    {
        return;
    }
    ui->copyFilesProgressBar->setValue(0);
    QFile file(updateHostPath_ + updateFileList.at(0));
    if(file.open(QFile::ReadOnly))
    {
        ICCommandProcessor* processor = ICCommandProcessor::Instance();
        ICUpdateHostRequestCommand reqCommand;
        if(processor->ExecuteCommand(reqCommand).toBool())
        {
            qDebug("req successful");
            ICUpdateHostTransferCommand tranCommand;
            QByteArray readySend(file.readAll());
            file.close();
            bool isTranSuccessful = true;
            ui->copyFilesProgressBar->setRange(0, readySend.size() / 32);
            for(int addr = 0; addr != readySend.size() / 32; ++addr)
            {
                tranCommand.SetDataBuffer(readySend.mid(addr << 5, 32));
                tranCommand.SetStartAddr(addr);
                isTranSuccessful = isTranSuccessful && processor->ExecuteCommand(tranCommand).toBool();
                if(isTranSuccessful)
                {
                    ui->copyFilesProgressBar->setValue(addr + 1);
                }
                else
                {
                    QMessageBox::warning(this, tr("Warning"), tr("Update Host fail!"));
                    return ;
                }
            }
            qDebug("tran successful");
            QMessageBox::information(this, tr("Congratulations"),
                                     tr("Send to  Host finished!"));
            ui->writeHostButton->setEnabled(true);
            return;
//            ICUpdateHostFinishCommand finishCommand;
//            if(processor->ExecuteCommand(finishCommand).toBool())
//            {
//                QMessageBox::information(this, tr("Congratulations"),
//                                         tr("Update Host finished!"));

//                return;
//            }

        }
    }
    QMessageBox::warning(this, tr("Warning"), tr("Update Host fail!"));
}

void ICUpdateSystem::QueryStatus()
{
    ICUpdateHostQueryCommand command;
    status_ = ICCommandProcessor::Instance()->ExecuteCommand(command).toInt();
    if(status_ >= 0)
    {
        ui->updateHostButton->setEnabled(true);
        ui->rebootButton->setEnabled(true);
    }
    else
    {
        ui->updateHostButton->setEnabled(false);
        ui->rebootButton->setEnabled(false);
        timer_.stop();
    }
    ui->statusLabel->setText(QString::number(status_));
}

void ICUpdateSystem::on_rebootButton_clicked()
{
    ICUpdateHostRestartCommand rebootCommand;
    if(ICCommandProcessor::Instance()->ExecuteCommand(rebootCommand).toBool())
    {
        QMessageBox::information(this, tr("tips"), tr("Reboot Successful!"));
    }
}

void ICUpdateSystem::on_connectHostButton_clicked()
{
    QDir dir("/proc/scsi/usb-storage");
    if(!dir.exists())
    {
        QMessageBox::warning(this,tr("tips"),tr("USB no exists...")) ;
        ui->hmiVersionShowLabel->setText(tr("No available HMI version"));
        ui->hostVersionShowLabel->setText(tr("No available Host version"));
        ui->connectHostButton->setEnabled(false);
        ui->updateToolButton->setEnabled(false);
        return;
    }
    ICUpdateHostStart linkCmd;
    ICCommandProcessor* processor = ICCommandProcessor::Instance();
    linkCmd.SetSlave(processor->SlaveID());
    processor->ExecuteCommand(linkCmd);
    if(!timer_.isActive())
    {
        timer_.start(1000);
    }
}

void ICUpdateSystem::on_writeHostButton_clicked()
{
    ICUpdateHostFinishCommand finishCommand;
    if(ICCommandProcessor::Instance()->ExecuteCommand(finishCommand).toBool())
    {
        if(!timer_.isActive())
        {
            timer_.start(100);
        }
        QMessageBox::information(this, tr("Congratulations"),
                                 tr("Update Host finished!"));

        return;
    }
}

void ICUpdateSystem::on_updateLogoButton_clicked()
{
    if(updateDialog_ == NULL)
    {
        updateDialog_ = new ICUpdateLogoDialog(this);
    }
    updateDialog_->exec();
}
