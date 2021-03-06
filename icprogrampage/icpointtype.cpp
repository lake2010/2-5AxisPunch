#include "icpointtype.h"
#include "ui_icpointtype.h"
#include "icparameterconversion.h"


ICPointType* ICPointType::instance_;


ICPointType::ICPointType(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ICPointType)
{
    ui->setupUi(this);
    Init_();

    boxToType.insert(ui->checkbox_1,OUYY37_ON);
    boxToType.insert(ui->checkbox_2,OUYY37_OFF);
    boxToType.insert(ui->checkbox_3,OUYY40_ON);
    boxToType.insert(ui->checkbox_4,OUYY40_OFF);
    boxToType.insert(ui->checkbox_5,OUYY22_ON);
    boxToType.insert(ui->checkbox_6,OUYY22_OFF);
    boxToType.insert(ui->checkbox_7,OUYY23_ON);
    boxToType.insert(ui->checkbox_8,OUYY23_OFF);
    boxToType.insert(ui->checkbox_13,RESEARVE);
    boxToType.insert(ui->checkbox_14,SMOOTH);
    boxToType.insert(ui->checkbox_15,WAITSAFE);
    boxToType.insert(ui->checkbox_16,PUNCHPERMIT);

   c6Boxs << ui->checkbox_17 << ui->checkbox_18 << ui->checkbox_19 << ui->checkbox_20
          << ui->checkbox_21 << ui->checkbox_22 << ui->checkbox_23 << ui->checkbox_24;


    QIntValidator * validator = new QIntValidator(0, MAX_DEALY, this);
    ui->delayEdit->SetDecimalPlaces(1);
    ui->delayEdit->setValidator(validator);
    ui->delayEdit->SetThisIntToThisText(0);

#ifdef IO_C6
    boxToType.insert(ui->checkbox_9,WAIT_X40);
    boxToType.insert(ui->checkbox_10,WAIT_X41);
    boxToType.insert(ui->checkbox_11,WAIT_X42);
    boxToType.insert(ui->checkbox_12,WAIT_X43);
    boxToType.insert(ui->checkbox_17,OUYY30_ON);
    boxToType.insert(ui->checkbox_20,OUYY30_OFF);
    boxToType.insert(ui->checkbox_18,OUYY31_ON);
    boxToType.insert(ui->checkbox_21,OUYY31_OFF);
    boxToType.insert(ui->checkbox_19,OUYY35_ON);
    boxToType.insert(ui->checkbox_22,OUYY35_OFF);
    boxToType.insert(ui->checkbox_23,OUYY36_ON);
    boxToType.insert(ui->checkbox_24,OUYY36_OFF);

    ui->checkbox_12->hide();
    ui->checkbox_3->hide();
    ui->checkbox_4->hide();
    ui->checkbox_18->hide();
    ui->checkbox_21->hide();
    this->resize(441,320);
#else
    boxToType.insert(ui->checkbox_9,WAIT_X41);
    boxToType.insert(ui->checkbox_10,WAIT_X42);
    boxToType.insert(ui->checkbox_11,WAIT_X43);
    boxToType.insert(ui->checkbox_12,WAIT_X44);
    foreach(QCheckBox * box,c6Boxs){
        box->hide();
    }
    ui->gridLayout->addWidget(ui->checkbox_13,6,0);
    ui->gridLayout->addWidget(ui->checkbox_14,6,1);
    ui->gridLayout->addWidget(ui->checkbox_15,6,2);
    ui->gridLayout->addWidget(ui->checkbox_16,7,0);
    this->resize(441,300);

#endif

    foreach(QCheckBox *box,boxToType.keys()){
        box->setText(propertyToStr.value(boxToType.value(box)));
        connect(box,SIGNAL(stateChanged(int)),
                SLOT(stateChanged(int)));

    }
}

void ICPointType::Init_()
{
    typeToStr.insert(Get_Wait,tr("Get_Wait"));
    typeToStr.insert(Get_Wait2,tr("Get_Finish"));
    typeToStr.insert(Get_Wait3,tr("Get_Wait"));
    typeToStr.insert(Get_Up,tr("Get_Up"));
    typeToStr.insert(Get,tr("Get"));
    typeToStr.insert(Put_Wait,tr("Put_Wait"));
    typeToStr.insert(Put_Wait2,tr("Put_Finish"));
    typeToStr.insert(Put_Up,tr("Put_Up"));
    typeToStr.insert(Put,tr("Put"));
    typeToStr.insert(Reserve,tr("Reserve"));
    typeToStr.insert(Wait_Safe,tr("WAIT_SAFE"));


    propertyToStr.insert(OUYY37_ON,tr("OUYY37_ON"));
    propertyToStr.insert(OUYY37_OFF,tr("OUYY37_OFF"));
    propertyToStr.insert(OUYY40_ON,tr("OUYY40_ON"));
    propertyToStr.insert(OUYY40_OFF,tr("OUYY40_OFF"));
    propertyToStr.insert(OUYY22_ON,tr("OUYY22_ON"));
    propertyToStr.insert(OUYY22_OFF,tr("OUYY22_OFF"));
    propertyToStr.insert(OUYY23_ON,tr("OUYY23_ON"));
    propertyToStr.insert(OUYY23_OFF,tr("OUYY23_OFF"));
    propertyToStr.insert(RESEARVE,tr("NULL_Property"));
    propertyToStr.insert(SMOOTH,tr("SMOOTH"));
    propertyToStr.insert(WAITSAFE,tr("WAIT_SAFE"));
    propertyToStr.insert(PUNCHPERMIT,tr("PUNCH_PERMIT"));

#ifdef IO_C6
    propertyToStr.insert(WAIT_X40,tr("WAIT_X40"));
    propertyToStr.insert(WAIT_X41,tr("WAIT_X41"));
    propertyToStr.insert(WAIT_X42,tr("WAIT_X42"));
    propertyToStr.insert(OUYY31_ON,tr("OUYY31_ON"));
    propertyToStr.insert(OUYY31_OFF,tr("OUYY31_OFF"));
    propertyToStr.insert(OUYY30_ON,tr("OUYY30_ON"));
    propertyToStr.insert(OUYY30_OFF,tr("OUYY30_OFF"));
    propertyToStr.insert(OUYY35_ON,tr("OUYY35_ON"));
    propertyToStr.insert(OUYY35_OFF,tr("OUYY35_OFF"));
    propertyToStr.insert(OUYY36_ON,tr("OUYY36_ON"));
    propertyToStr.insert(OUYY36_OFF,tr("OUYY36_OFF"));
#else
    propertyToStr.insert(WAIT_X41,tr("WAIT_X41"));
    propertyToStr.insert(WAIT_X42,tr("WAIT_X42"));
    propertyToStr.insert(WAIT_X43,tr("WAIT_X43"));
    propertyToStr.insert(WAIT_X44,tr("WAIT_X44"));
#endif


}

ICPointType::~ICPointType()
{
    delete ui;
}

QString ICPointType::toString(PointType type)
{
    if(type == NULL_Type)
        return typeToStr.value(Reserve);
    else
        return typeToStr.value(type);
}

QString ICPointType::toString(PointProperty type)
{
    if(type == NULL_Property)
        return propertyToStr.value(NULL_Property);
    else
        return propertyToStr.value(type);
}

QString ICPointType::toPropertyString(PointProperty type,quint32 delay)
{
    if(type == NULL_Property)
        return propertyToStr.value(NULL_Property);
    else
        return propertyToStr.value(type) + QString("%1s").arg(ICParameterConversion::TransThisIntToThisText(delay, 2));
}

ICPointConfig ICPointType::config()
{
    PointType p;
    int delay = 0;
    if(currentPropertyType() == RESEARVE){
        p = Reserve;
    }
    else if(currentPropertyType() == WAITSAFE){
        p = Wait_Safe;
    }
    else{
        p = Point_Property;
    }
    delay = ui->delayEdit->TransThisTextToThisInt();

    ICPointConfig config(p ,
                         currentPropertyType(),
                         delay,0,0,"0");
    return config;
}


PointProperty ICPointType::currentPropertyType()
{
    return boxToType.value(_box,NULL_Property);
}

bool ICPointType::currentPropertyIsPoint()
{
    return ( (boxToType.value(_box,NULL_Property) == RESEARVE ) ||
                  (boxToType.value(_box,NULL_Property) == WAITSAFE ) );

}

void ICPointType::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:{
        ui->retranslateUi(this);
        foreach(QCheckBox *box,boxToType.keys()){
             box->setText(propertyToStr.value(boxToType.value(box)));
        }
    }
        break;
    default:
        break;
    }
}

void ICPointType::stateChanged(int status)
{
    if(status){
        _box = qobject_cast<QCheckBox*> (sender());
    }
//    if(_box == ui->checkbox_13 || _box == ui->checkbox_14
//            || _box == ui->checkbox_15){
//        ui->delayEdit->setEnabled(false);
//    }
//    else{
//        ui->delayEdit->setEnabled(true);
//    }
}
