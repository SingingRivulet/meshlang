#include "functable.h"
#include "ui_functable.h"

funcTable::funcTable(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::funcTable)
{
    ui->setupUi(this);
    setWindowTitle("添加函数");

    ui->nName->setValidator(new QRegExpValidator(QRegExp("[A-Za-z0-9]{0,20}")));
    ui->nInput->setValidator(new QRegExpValidator(QRegExp("[A-Za-z0-9/,]*")));
    ui->nOutput->setValidator(new QRegExpValidator(QRegExp("[A-Za-z0-9/,]*")));
}
void funcTable::setTable(){
    ui->table->setColumnCount(4);
    ui->table->setHorizontalHeaderLabels(QStringList()<<"函数名"<<"输入"<<"输出"<<"大小");
    ui->table->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->table->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    createFunction = false;
    functionName.clear();
}
void funcTable::setRows(int r){
    ui->table->setRowCount(r);
}
void funcTable::setRow(int r , const char * f,const char * i,const char * o,const char * s){
    ui->table->setItem(r,0,new QTableWidgetItem(f));
    ui->table->setItem(r,1,new QTableWidgetItem(i));
    ui->table->setItem(r,2,new QTableWidgetItem(o));
    ui->table->setItem(r,3,new QTableWidgetItem(s));
}
void funcTable::getSelected(){
    QList<QTableWidgetItem*> items = ui->table->selectedItems();
    int count = items.count();
    if(count>0){
        int row = ui->table->row(items.at(0));
        functionName = ui->table->model()->index(row,0).data().toString().toStdString();
    }
}
funcTable::~funcTable(){
    delete ui;
}

void funcTable::getNewFunc(){
    auto name       = ui->nName->text().simplified();
    auto inputOri   = ui->nInput->text().simplified();
    auto outputOri  = ui->nOutput->text().simplified();

    input.clear();
    output.clear();
    createFunction = true;

    if(name.isEmpty() || inputOri.isEmpty() || outputOri.isEmpty())
        return;

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

    functionName = name.toStdString();
}

void funcTable::on_buttonBox_accepted(){
    createFunction = false;
    functionName.clear();
    if(ui->useNew->isChecked()){
        getNewFunc();
    }else{
        getSelected();
    }
}

void funcTable::on_buttonBox_rejected(){
    createFunction = false;
    functionName.clear();
}
