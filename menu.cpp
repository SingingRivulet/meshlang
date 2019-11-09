#include "menu.h"
#include "ui_menu.h"

menu::menu(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::menu)
{
    ui->setupUi(this);
    setWindowTitle("操作");
}

menu::~menu()
{
    delete ui;
}

void menu::on_importButton_clicked(){
    QString file_name = QFileDialog::getOpenFileName(this,"导入文件",".","*.mla");
    if(!file_name.isEmpty())
        importFile(file_name.toStdString());
    accept();
}

void menu::on_saveButton_clicked(){
    QString file_name=QFileDialog::getSaveFileName(this,"保存文件",".","*.mla");
    if(!file_name.isEmpty())
        saveFile(file_name.toStdString());
    accept();
}

void menu::on_noteButton_clicked(){
    bool isOK;
    QString text = QInputDialog::getText(NULL, "注释",
        "添加注释",
        QLineEdit::Normal,
        "",
        &isOK);
    if(isOK && !text.isEmpty()){
        addNoteInWindow(text.toStdString());
        accept();
    }
}

void menu::on_compileButton_clicked(){
    compileProgram();
    accept();
}
