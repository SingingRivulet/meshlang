#ifndef FUNCTABLE_H
#define FUNCTABLE_H

#include <QDialog>
#include <vector>

namespace Ui {
class funcTable;
}

class funcTable : public QDialog
{
    Q_OBJECT

public:
    explicit funcTable(QWidget *parent = 0);
    ~funcTable();
    void setTable();

    std::string functionName;
    bool createFunction;
    std::vector<std::string> input,output;

    void setRows(int r);
    void setRow(int r , const char * f,const char * i,const char * o,const char * s);


private slots:
    void getSelected();
    void getNewFunc();
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::funcTable *ui;
};

#endif // FUNCTABLE_H
