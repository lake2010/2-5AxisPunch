#include <QFile>
#include <QStringList>
#include <QDebug>
#include "icmold.h"
#include "icinstructparam.h"
#include "icfile.h"
#include "version.h"

struct MoldStepData
{
    MoldStepData():step(-1), begin(-1), end(-1){}
    int step;
    int begin;
    int end;
};

QString ICSubMoldUIItem::ToString() const
{
    QString ret;
    ret += "        " + ICInstructParam::ConvertCommandStr(baseItem_);
    return ret;
}
int ICGroupMoldUIItem::ItemCount() const
{
    int count = 0;
    for(int i = 0; i != topItems_.size(); ++i)
    {
        count += topItems_.at(i).ItemCount();
    }
    return count;
}

void ICGroupMoldUIItem::SetStepNum(int stepNum)
{
    for(int i = 0; i != topItems_.size(); ++i)
    {
        topItems_[i].SetStepNum(stepNum);
    }
}

QList<ICMoldItem> ICGroupMoldUIItem::ToMoldItems() const
{
    QList<ICMoldItem> ret;
    for(int i = 0; i != topItems_.size(); ++i)
    {
        ret.append(topItems_.at(i).ToMoldItems());
    }
    return ret;
}

QStringList ICGroupMoldUIItem::ToStringList() const
{
    QStringList ret;
    for(int i = 0; i != topItems_.size(); ++i)
    {
        ret.append(topItems_.at(i).ToStringList());
    }
    return ret;
}

QList<ICGroupMoldUIItem> ICGroupMoldUIItem::SpliteToTwoGroup(int pos) const
{
    QList<ICGroupMoldUIItem> ret;
    ICGroupMoldUIItem item1;
    ICGroupMoldUIItem item2;
    item1.SetStepNum(StepNum());
    item2.SetStepNum(StepNum() + 1);
    for(int i = 0; i != pos; ++i)
    {
        item1.AddToMoldUIItem(topItems_.at(i));
    }
    for(int i = pos; i != topItems_.size(); ++i)
    {
        item2.AddToMoldUIItem(topItems_.at(i));
    }
    ret<<item1<<item2;
    return ret;
}
void ICGroupMoldUIItem::AddOtherGroup(const ICGroupMoldUIItem &other)
{
    for(int i = 0; i != other.ItemCount(); ++i)
    {
        AddToMoldUIItem(other.at(i));
    }
}

ICMoldItem* ICGroupMoldUIItem::MoldItemAt(int index)
{
    const int topItemCount = TopItemCount();
    for(int i = 0; i != topItemCount; ++i)
    {
        if(index < at(i).ItemCount())
        {
            if(index == 0)
            {
                return at(i).BaseItem();
            }
            else
            {
                return at(i).at(index - 1).BaseItem();
            }
        }
        index -= at(i).ItemCount();
    }
    return NULL;
}

void ICTopMoldUIItem::SetStepNum(int stepNum)
{
    baseItem_.SetNum(stepNum);
    for(int i = 0; i != subItems_.size(); ++i)
    {
        subItems_[i].SetStepNum(stepNum);
    }
}

QStringList ICTopMoldUIItem::ToStringList() const
{
    QStringList ret;
    ret.append(ICInstructParam::ConvertCommandStr(baseItem_));
    for(int i = 0; i != subItems_.size(); ++i)
    {
        ret.append(subItems_.at(i).ToString());
    }
    return ret;
}

bool ICTopMoldUIItem::IsSyncSubItem(int pos) const
{
    if(SubItemCount() == 1)
    {
        return false;
    }
    else if(pos == 0)
    {
        return at(pos + 1).SubNum() == at(pos).SubNum();
    }
    else if(pos == SubItemCount() - 1)
    {
        return at(pos - 1).SubNum() == at(pos).SubNum();
    }
    else
    {
        return (at(pos + 1).SubNum() == at(pos).SubNum()) ||
                at(pos - 1).SubNum() == at(pos).SubNum();
    }
}
//void ICTopMoldUIItem::ReCalSubNum()
//{
//    int currentNum = 0;
//    subItems_[0].SetSubNum(currentNum);
//    for(int i = 1; i != subItems_.size(); ++i)
//    {
//        if(subItems_.at(i).SubNum() == currentNum && subItems_.at(i).IsSyncItem() && subItems_.at(i - 1).IsSyncItem())
//        {
//            subItems_[i].SetSubNum(currentNum);
//        }
//        else
//        {
//            subItems_[i].SetSubNum(++currentNum);
//        }
//    }
//}

QList<ICMoldItem> ICTopMoldUIItem::ToMoldItems() const
{
    QList<ICMoldItem> ret;
    ret.append(baseItem_);
    for(int i = 0; i != subItems_.size(); ++i)
    {
        ret.append(subItems_.at(i).ToMoldItem());
    }
    return ret;
}

ICMold* ICMold::currentMold_ = NULL;
ICMold::ICMold(QObject *parent) :
    QObject(parent)
{
    ICInstructParam::Instance();
    //    axisActions_.append(GX);
    //    axisActions_.append();
}

bool ICMold::ReadMoldFile(const QString &fileName, bool isLoadParams)
{
    QFile file(fileName);
    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        return false;
    }
//    moldName_ = fileName;
    QString content = QString::fromUtf8(file.readAll());
    file.close();
    //    content = content.remove('\r');

    if(content.isNull())
    {
        qDebug("mold null");
        return false;
    }
    QStringList records = content.split("\n", QString::SkipEmptyParts);
    if(records.size() < 1)
    {
        qDebug("mold less than 4");
        return false;
    }
    QStringList items;
    ICMoldItem moldItem;
    qDebug("before read");
    qDebug()<<"size"<<records.size();
    QList<ICMoldItem> tempmoldContent;
    for(int i = 0; i != records.size(); ++i)
    {
        qDebug()<<"in"<<i;
        items = records.at(i).split(' ', QString::SkipEmptyParts);
        if(items.size() != 10 &&
                items.size() != 11)
        {
            qDebug()<<i<<"th line size wrong";
            return false;
        }
        moldItem.SetValue(items.at(0).toUInt(),
                          items.at(1).toUInt(),
                          items.at(2).toUInt(),
                          items.at(3).toUInt(),
                          items.at(4).toUInt(),
                          items.at(5).toUInt(),
                          items.at(6).toUInt(),
                          items.at(7).toUInt(),
                          items.at(8).toUInt(),
                          items.at(9).toUInt());
        if(items.size() == 11)
        {
            moldItem.SetComment(items.at(10));
        }
        tempmoldContent.append(moldItem);
    }
    qDebug("read ok");
    bool ret = true;
    if(isLoadParams)
    {
        QString moldParamFileName = fileName;
        moldParamFileName.chop(3);
        moldParamFileName += "fnc";
        ret = ReadMoldParamsFile(moldParamFileName);
        emit MoldNumberParamChanged();
    }
    if(ret == true)
    {
        moldContent_ = tempmoldContent;
        moldName_ = fileName;
    }
    return ret;
}

bool ICMold::ReadConfigFile(const QString &fileName)
{
    QFile file(fileName);

    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        return false;
    }
    moldNativeParams_.clear();
    QString content = QString::fromUtf8(file.readAll());
    file.close();
    //    content = content.remove('\r');

    if(content.isNull())
    {
        qDebug("mold configure null");
        return false;
    }

    QStringList records = content.split("\n", QString::SkipEmptyParts);

//    if(records.size() < MoldNativeParamCount){
//        return false;
//    }
    for(int i = 0; i != records.size(); ++i)
    {
        moldNativeParams_.append(records.at(i).toInt());
    }
    if(records.size() < MoldNativeParamCount){
        for(int i=0;i< MoldNativeParamCount - records.size();i++){
            moldNativeParams_.append(0);
        }
    }

    moldConfigName_ = fileName;

    return true;
}

bool ICMold::ReadPointConfigFile(const QString &fileName)
{
    QFile file(fileName);

    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        return false;
    }
    pointConfigs.clear();
    QString content = QString::fromUtf8(file.readAll());
    file.close();

    if(content.isNull())
    {
        qDebug("mold point configure is  null");
        return false;
    }

    QStringList ql = content.split("\n", QString::SkipEmptyParts);
    foreach(QString line,ql){
        QStringList items = line.split(" ", QString::SkipEmptyParts);
        if(!VersionCompatiblePT(items)){
            return false;
        }
        ICPointConfig config(items.at(0).toInt(),items.at(1).toInt(),items.at(2).toInt(),
                             items.at(3).toInt(),items.at(4).toInt(),items.at(5));
        pointConfigs.append(config);

    }
    moldPointName_ = fileName;
    return true;
}

bool ICMold::ReadMoldParamsFile(const QString &fileName)
{
//    moldParamName_ = fileName;
    QFile file(fileName);
    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        return false;
    }
    QString fileContent = file.readAll();
    file.close();
    //    fileContent = fileContent.remove('\r');


    QStringList items = fileContent.split('\n', QString::SkipEmptyParts);

   if(!VersionCompatibleFNC(items,versoin_)){
       return false;
   }

    if(items.size() != MoldParamCount + StackParamCount * 4 + 1)
    {
        return false;
    }
    moldParams_.clear();
    stackParams_.clear();
    for(int i = 0; i != MoldParamCount; ++i)
    {
        moldParams_.append(items.at(i).toInt());
    }
    //    Q_ASSERT_X(items.size() == 58, "ICMold::ReadMoldParamFile", "fnc file is not correct!");

    QList<int> stackParam;
    int base;
    int count;
    for(int i = 0; i != 4; ++i)
    {
        base = MoldParamCount + StackParamCount * i;
        count = base + StackParamCount;
        stackParam.clear();
        for(int j = base; j != count; ++j)
        {
            stackParam.append(items.at(j).toUInt());
        }
        stackParams_.append(stackParam);
    }
//    moldParams_[CheckClip5] = 0;
//    moldParams_[CheckClip6] = 0;
    checkSum_ = items.last().toUInt();
    if(versoin_ != VERSION_lATEST - 1){
        //同步校验和
        UpdateSyncSum();
    }
    moldParamName_ = fileName;
    return true;
}

bool ICMold::SaveMoldFile(bool isSaveParams)
{
    bool ret = false;
    MoldReSum();
    QByteArray toWrite;
    if(moldContent_.size() < 1)
    {
        qDebug("mold content less than 4 when save");
        return false;
    }
    for(int i = 0; i != moldContent_.size(); ++i)
    {
        toWrite += moldContent_.at(i).ToString() + "\n";
    }
    ICFile file(moldName_);
    ret = file.ICWrite(toWrite);
//    QFile file(moldName_);
//    if(!file.open(QFile::ReadWrite | QFile::Text))
//    {
//        return false;
//    }
//    if(file.readAll() != toWrite)
//    {
//        QFile::copy(moldName_, moldName_ + "~");
//        file.resize(0);
//        file.write(toWrite);
//        file.close();
//        //    QDir dir(file.parent())
//        //    system(QString("rm %1~").arg(moldName_).toAscii());
//        QFile::remove(moldName_ + "~");
//        ret = true;
//    }
    if(isSaveParams)
    {
        SaveMoldParamsFile();
    }
    return ret;
}

bool ICMold::SaveMoldParamsFile()
{
    bool ret = false;
    UpdateSyncSum();
    QByteArray toWrite;
    QList<int> allParams = AllParams();
    for(int i = 0; i != allParams.size(); ++i)
    {
        toWrite += QByteArray::number(allParams.at(i)) + "\n";
    }
    ICFile file(moldParamName_);
    ret = file.ICWrite(toWrite);
//    QFile file(moldParamName_);
//    if(!file.open(QFile::ReadWrite | QFile::Text))
//    {
//        return false;
//    }
//    QByteArray fc = file.readAll();
//    if(fc != toWrite)
//    {
//        QFile::copy(moldParamName_, moldParamName_ + "~");
//        file.resize(0);
//        file.write(toWrite);
//        file.close();
//        //    system(QString("rm %1~").arg(moldParamName_).toAscii());
//        QFile::remove(moldParamName_ + "~");
//        ret = true;
//    }
    return ret;
}

bool ICMold::SaveMoldConfigFile()
{
    bool ret = false;
    QByteArray toWrite;
    for(int i = 0; i != moldNativeParams_.size(); ++i)
    {
        toWrite += QByteArray::number(moldNativeParams_.at(i)) + "\n";
    }
    ICFile file(moldConfigName_);
    ret = file.ICWrite(toWrite);

    return ret;
}

bool ICMold::SaveMoldPointFile()
{
    bool ret = false;
    QByteArray toWrite;
    for(int i = 0; i != pointConfigs.size(); ++i)
    {
        toWrite += pointConfigs.at(i).ToString() + "\n";
    }
    ICFile file(moldPointName_);
    ret = file.ICWrite(toWrite);

    return ret;
}

uint ICMold::SyncAct() const
{
    uint ret = 0;
    for(int i = 0; i != moldContent_.size(); ++i)
    {
        if(moldContent_.at(i).Action() != ACTCOMMENT)
            ret += moldContent_.at(i).GMVal();
    }
    return ret;
}

uint ICMold::SyncSum() const
{
    uint ret = 0;
    for(int i = 0; i != moldContent_.size(); ++i)
    {
        if(moldContent_.at(i).Action() != ACTCOMMENT)
            ret += moldContent_.at(i).Sum();
    }
    return ret;
}


void ICMold::Insert(const QList<ICMoldItem> &items, QList<ICMoldItem> &sourceItems)
{
    Q_UNUSED(items)
    Q_UNUSED(sourceItems)
}

void ICMold::Modify(const QList<ICMoldItem> &items, QList<ICMoldItem> &sourceItems)
{
    Q_UNUSED(items)
    Q_UNUSED(sourceItems)
}

void ICMold::Delete(int step, QList<ICMoldItem> &sourceItems)
{
    Q_UNUSED(step)
    Q_UNUSED(sourceItems)
}

void ICMold::MoldReSum(QList<ICMoldItem> &items)
{
    int seq = 0;
    for(int i = 0; i != items.size(); ++i)
    {
        items[i].SetSeq(seq);
        items[i].ReSum();
        if(items.at(i).Action() != ACTCOMMENT)
            ++seq;
    }
}

void ICMold::UpdateSyncSum()
{
    int sum = 0;
    for(int i = 0; i != moldParams_.size(); ++i)
    {
        sum += moldParams_.at(i);
    }
    for(int i = 0; i != stackParams_.size(); ++i)
    {
        for(int j = 0; j != stackParams_.at(i).size(); ++j)
        {
            sum += stackParams_.at(i).at(j);
        }
    }
    //    sum += checkSum_;
    checkSum_ = (-sum) & 0xFFFF;
}

QList<ICMoldItem> ICMold::UIItemToMoldItem(const QList<ICGroupMoldUIItem> &items)
{
    QList<ICMoldItem> ret;
    for(int i = 0; i != items.size(); ++i)
    {
        ret.append(items.at(i).ToMoldItems());
    }
    return ret;
}

QList<ICGroupMoldUIItem> ICMold::MoldItemToUIItem(const QList<ICMoldItem> &items)
{
    QList<ICTopMoldUIItem> ret;
    QList<ICGroupMoldUIItem> groupRet;
    ICTopMoldUIItem topItem;
//    ICSubMoldUIItem subItem;
    ICMoldItem moldItem;
    for(int i = 0; i != items.size(); ++i)
    {
        moldItem = items.at(i);
//        if(moldItem.SubNum() == 255)
//        {
            topItem.SetBaseItem(moldItem);
            ret.append(topItem);
//        }
//        else
//        {
//            if(moldItem.SubNum() == items.at(i + 1).SubNum())
//            {
//                subItem.SetSyncItem(true);
//            }
//            subItem.SetBaseItem(moldItem);
//            if(ret.isEmpty())
//            {
//                return groupRet;
//            }
//            ret.last().AddSubMoldUIItem(subItem);
//        }
    }
    ICGroupMoldUIItem groupItem;
    if(ret.isEmpty())
    {
        return groupRet;
    }
    groupItem.AddToMoldUIItem(ret.first());
    groupRet.append(groupItem);
    for(int i = 1; i != ret.size(); ++i)
    {
        topItem = ret.at(i);
        if(topItem.StepNum() == groupRet.last().StepNum())
        {
            groupRet.last().AddToMoldUIItem(topItem);
        }
        else
        {
            ICGroupMoldUIItem newItem;
            newItem.AddToMoldUIItem(topItem);
            groupRet.append(newItem);
        }
    }
    return groupRet;
}

QStringList ICMold::UIItemsToStringList(const QList<ICGroupMoldUIItem> &items)
{
    QStringList ret;
    ICGroupMoldUIItem item;
    for(int i = 0; i != items.size(); ++i)
    {
        item = items.at(i);
        ret.append(item.ToStringList());
    }
    return ret;
}

int ICGroupMoldUIItem::RunableTopItemCount()
{
    int ret = 0;
    for(int i = 0 ; i != topItems_.size(); ++i)
    {
        if(topItems_[i].BaseItem()->Action() != ICMold::ACTCOMMENT)
        {
            ++ret;
        }
    }
    return ret;
}
