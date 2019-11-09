#ifndef FUNCEDITOR_H
#define FUNCEDITOR_H

#include <QDialog>
#include <unordered_map>

namespace Ui {
class funcEditor;
}

class funcEditor : public QDialog
{
    Q_OBJECT

public:
    explicit funcEditor(QWidget *parent = 0);
    ~funcEditor();

    bool removeNode;
    void setTable();
    void setRows(int r);
    void setRow(int r,const std::string & name,const std::string & val);
    void getVar(std::unordered_map<std::string,std::string> & m);
    void setModuleName(const std::string & val);
    void getModuleName(std::string & val);

private slots:
    void on_removeNode_clicked();

    void on_saveNode_clicked();

private:
    Ui::funcEditor *ui;
};

#endif // FUNCEDITOR_H
