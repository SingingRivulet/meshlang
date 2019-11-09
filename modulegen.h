#ifndef MODULEGEN_H
#define MODULEGEN_H

#include <QDialog>

namespace Ui {
class moduleGen;
}

class moduleGen : public QDialog
{
    Q_OBJECT

public:
    explicit moduleGen(QWidget *parent = 0);
    ~moduleGen();

    std::string moduleName;
    bool create;
    std::vector<std::string> input,output;

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::moduleGen *ui;
};

#endif // MODULEGEN_H
