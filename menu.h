#ifndef MENU_H
#define MENU_H

#include <QDialog>
#include <QDebug>
#include <QFileDialog>
#include <QInputDialog>

namespace Ui {
class menu;
}

class menu : public QDialog
{
    Q_OBJECT

public:
    explicit menu(QWidget *parent = 0);
    ~menu();

    virtual void importFile(const std::string &)=0;
    virtual void saveFile(const std::string &)=0;
    virtual void addNoteInWindow(const std::string & text)=0;
    virtual void compileProgram()=0;

private slots:
    void on_importButton_clicked();
    void on_saveButton_clicked();

    void on_noteButton_clicked();

    void on_compileButton_clicked();

private:
    Ui::menu *ui;
};

#endif // MENU_H
