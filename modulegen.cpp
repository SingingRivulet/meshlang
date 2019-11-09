#include "modulegen.h"
#include "ui_modulegen.h"

moduleGen::moduleGen(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::moduleGen)
{
    ui->setupUi(this);
    ui->nName->setValidator(new QRegExpValidator(QRegExp("[A-Za-z0-9]{0,20}")));
    ui->nInput->setValidator(new QRegExpValidator(QRegExp("[A-Za-z0-9/,]*")));
    ui->nOutput->setValidator(new QRegExpValidator(QRegExp("[A-Za-z0-9/,]*")));
    setWindowTitle("创建模块");
}

moduleGen::~moduleGen(){
    delete ui;
}

void moduleGen::on_buttonBox_accepted(){
    auto name       = ui->nName->text().simplified();
    auto inputOri   = ui->nInput->text().simplified();
    auto outputOri  = ui->nOutput->text().simplified();

    input.clear();
    output.clear();
    create = false;

    if(name.isEmpty() || inputOri.isEmpty() || outputOri.isEmpty())
        return;

    create = true;

    auto inputls = inputOri.split(",");
    for(auto ito:inputls){
        auto inputs = ito.split(" ");
        for(auto it:inputs){
            if(!it.isEmpty()){
                input.push_back(it.toStdString());
            }
        }
    }
    auto outputls = outputOri.split(",");
    for(auto ito:outputls){
        auto outputs = ito.split(" ");
        for(auto it:outputs){
            if(!it.isEmpty()){
                output.push_back(it.toStdString());
            }
        }
    }

    moduleName = name.toStdString();
    accept();
}

void moduleGen::on_buttonBox_rejected()
{
    create = false;
    reject();
}
