#include "configwindow.h"
#include "ui_configwindow.h"

ConfigWindow::ConfigWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigWindow)
{
    ui->setupUi(this);
    connect(ui->btnBack, &QPushButton::clicked, this, &ConfigWindow::on_btn_back_clicked);
    }
    void ConfigWindow::on_btn_back_clicked()
    {
            //(QWidget *)parent()->show();
            this->hide();
            //&parentWidget->show()
            //	parent::show();
    }
ConfigWindow::~ConfigWindow()
{
    delete ui;
}
