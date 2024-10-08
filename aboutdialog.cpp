#include "aboutdialog.h"
#include "ui_aboutdialog.h"

#include <QDesktopServices>
#include <QDate>

AboutDialog::AboutDialog(std::shared_ptr<VoukoderPro::IClient> vkdrPro, QWidget *parent) : QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);

    ui->label->setText(qApp->applicationName());
    ui->label_2->setText("Version " + qApp->applicationVersion());
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
