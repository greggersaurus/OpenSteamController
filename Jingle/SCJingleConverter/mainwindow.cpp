#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "composition.h"
#include "scserial.h"

#include <QSerialPortInfo>
#include <QSerialPort>
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    const auto infos = QSerialPortInfo::availablePorts();

    for (const QSerialPortInfo &info : infos)
        ui->serialPortComboBox->addItem(info.portName());

    ui->delJingleToolButton->setIcon(ui->delJingleToolButton->style()->standardIcon(QStyle::SP_TrashIcon));
    ui->mvJingleUpToolButton->setIcon(ui->mvJingleUpToolButton->style()->standardIcon(QStyle::SP_ArrowUp));
    ui->mvJingleDownToolButton->setIcon(ui->mvJingleDownToolButton->style()->standardIcon(QStyle::SP_ArrowDown));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_playJinglePushButton_clicked()
{
    QString serial_port_name = ui->serialPortComboBox->currentText();
    SCSerial serial(serial_port_name);

    if (serial.open()) {
        QMessageBox::information(this, tr("Error"),
            tr("Cannot open %1.")
            .arg(serial_port_name));
        return;
    }

    if (serial.send("jingle play 0\r\n")) {
        QMessageBox::information(this, tr("Error"),
            tr("Failed to send command."));
        return;
    }
}

void MainWindow::on_browsePushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        ("Open musicXML File"),
        QDir::homePath(),
        ("musixXML (*.musicxml)"));

    ui->musicXmlPathLineEdit->setText(fileName);

}

void MainWindow::on_convertPushButton_clicked()
{
    QString filename = ui->musicXmlPathLineEdit->text();

    Composition composition(filename);

    //TODO: Parse musicxml file and create class that stores all notes
    if (composition.parse()) {
        QMessageBox::information(this, tr("Error"),
            tr("Failed to parse %1.")
            .arg(filename));
    }

    std::vector<QString> cmds = composition.getSCCommands();
    for (uint32_t cmds_idx = 0; cmds_idx < cmds.size(); cmds_idx++) {
        qDebug() << cmds[cmds_idx];
    }

    // Create string to identify this Composition
    // Remove path
    QStringList list = filename.split('/');
    filename = list[list.size()-1];
    list = filename.split('\\');
    filename = list[list.size()-1];
    // Remove .musicxml extension
    filename.resize(filename.size() - QString(".musicxml").size());
    ui->jingleListWidget->addItem(filename);

    //TODO: make map that links trimmed fileName to Composition class instance (i.e. for moving up and down list and for deleting from list)
}
