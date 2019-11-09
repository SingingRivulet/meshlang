#include "funceditor.h"
#include "ui_funceditor.h"

funcEditor::funcEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::funcEditor)
{
    ui->setupUi(this);
    setWindowTitle("编辑函数");

    QRegExp rx = QRegExp("[A-Za-z0-9]{0,11}");
    QRegExpValidator *validator = new QRegExpValidator(rx);
    ui->moduleName->setValidator(validator);
}

funcEditor::~funcEditor()
{
    delete ui;
}

void funcEditor::setTable(){
    removeNode = false;
    ui->table->setColumnCount(2);
    ui->table->setHorizontalHeaderLabels(QStringList()<<"变量名"<<"值");
    ui->table->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->table->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}
void funcEditor::on_removeNode_clicked(){
    removeNode = true;
    accept();
}
void funcEditor::getVar(std::unordered_map<std::string,std::string> & m){
    int l = ui->table->rowCount();
    for(int i=0;i<l;i++){
        m[ui->table->item(i,0)->text().toStdString()] = ui->table->item(i,1)->text().toStdString();
    }
}
void funcEditor::on_saveNode_clicked(){
    accept();
}
void funcEditor::setRows(int r){
    ui->table->setRowCount(r);
}
void funcEditor::setRow(int r,const std::string & name,const std::string & val){
    auto k = new QTableWidgetItem(name.c_str());
    auto v = new QTableWidgetItem(val.c_str());
    k->setFlags(Qt::NoItemFlags);
    ui->table->setItem(r,0,k);
    ui->table->setItem(r,1,v);
}

void funcEditor::setModuleName(const std::string & val){
    ui->moduleName->setText(val.c_str());
}
void funcEditor::getModuleName(std::string & val){
    val = ui->moduleName->text().toStdString();
}
