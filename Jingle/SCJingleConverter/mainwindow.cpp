#include "mainwindow.h"
#include "ui_mainwindow.h"

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
    QSerialPort serial;
    QString serial_port_name = ui->serialPortComboBox->currentText();
    serial.setPortName(serial_port_name);
    if (!serial.open(QIODevice::ReadWrite)) {
        QMessageBox::information(this, tr("Error"),
            tr("Cannot open %1. Error Code %2.")
            .arg(serial_port_name).arg(serial.error()));
        return;
    }

    QString currentRequest = "jingle play 3\r\n";
    const QByteArray requestData = currentRequest.toUtf8();
    serial.write(requestData);
    if (!serial.waitForBytesWritten(1000)) {
        // TODO: error dialog box
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
    QString fileName = ui->musicXmlPathLineEdit->text();
    // Remove path
    QStringList list = fileName.split('/');
    fileName = list[list.size()-1];
    list = fileName.split('\\');
    fileName = list[list.size()-1];
    // Remove .musicxml extension
    fileName.resize(fileName.size() - QString(".musicxml").size());
    ui->jingleListWidget->addItem(fileName);


    //TODO: Parse musicxml file and create class that stores all notes
    //TODO: make map that links trimmed fileName to class?
}
