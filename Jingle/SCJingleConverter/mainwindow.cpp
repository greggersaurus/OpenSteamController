#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "scserial.h"

#include <QSerialPortInfo>
#include <QSerialPort>
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
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
    if (!compositions.size()) {
        QMessageBox::information(this, tr("Error"),
            tr("No Compositions to Play"));
        return;
    }

    const int comp_idx = ui->jingleListWidget->currentRow();
    if (comp_idx > static_cast<int>(compositions.size()) || comp_idx < 0) {
        QMessageBox::information(this, tr("Error"),
            tr("Invalid Composition selected"));
        return;
    }

    Composition& composition = compositions.at(static_cast<uint32_t>(comp_idx));

    SCSerial::ErrorCode serial_err_code = serial.open();
    if (serial_err_code != SCSerial::NO_ERROR) {
        QMessageBox::information(this, tr("Error"),
            tr("Cannot open %1.\n\nError: %2")
            .arg(serial_port_name)
            .arg(SCSerial::getErrorString(serial_err_code)));
        return;
    }

//TODO: make sure there is enough memory to download Jingle...

    QString cmd("jingle clear\n");
    QString resp = cmd + "\rJingle data cleared successfully.\n\r";
    if (serial.send(cmd, resp)) {
        QMessageBox::information(this, tr("Error"),
            tr("Failed to clear Jingle Data."));
        return;
    }

    Composition::ErrorCode comp_err_code = composition.download(serial, 0);
    if (comp_err_code != Composition::NO_ERROR) {
        QMessageBox::information(this, tr("Error"),
            tr("Cannot download to %1.\n\nError: %2")
            .arg(serial_port_name)
            .arg(Composition::getErrorString(comp_err_code)));
        return;
    }

    cmd = "jingle play 0\n";
    resp = cmd + "\rJingle play started successfully.\n\r";
    if (serial.send(cmd, resp)) {
        QMessageBox::information(this, tr("Error"),
            tr("Failed to send play command."));
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
    if (compositions.size() > MAX_NUM_COMPS) {
        QMessageBox::information(this, tr("Error"),
            tr("Too many Compositions have been added. "
               "Please delete before attemping to add another."));
        return;
    }

    QString filename = ui->musicXmlPathLineEdit->text();

    compositions.push_back(Composition(filename));
    Composition& composition = compositions.back();

    Composition::ErrorCode comp_err_code = composition.parse();
    if (Composition::NO_ERROR != comp_err_code) {
        QMessageBox::information(this, tr("Error"),
            tr("Failed to parse file '%1'.\nError: %2")
            .arg(filename)
            .arg(Composition::getErrorString(comp_err_code)));
        compositions.pop_back();
        return;
    }

    // Create string to identify this Composition
    // Remove path
    QStringList list = filename.split('/');
    filename = list[list.size()-1];
    list = filename.split('\\');
    filename = list[list.size()-1];
    // Remove .musicxml extension
    filename.resize(filename.size() - QString(".musicxml").size());
    // Add identifier string and make sure it is selected
    ui->jingleListWidget->addItem(filename);
    ui->jingleListWidget->setCurrentItem(ui->jingleListWidget->item(ui->jingleListWidget->count()-1));
    ui->jingleListWidget->repaint();

    // Fill out GUI with details from this newly added Composition
    updateCompositionDisplay();

    // Update memory usage display since Composition has been added
    updateMemUsage();
}

void MainWindow::updateMemUsage() {
    const int num_comps = ui->jingleListWidget->count();
//TODO: do this right
    ui->progressBar->setValue(num_comps * 10);
}

void MainWindow::updateCompositionDisplay() {
    /*
    ui->startMeasComboBox->clear();
    ui->startMeasComboBox->addItem("test");
    ui->startMeasComboBox->addItem("test2");
    ui->startMeasComboBox->setCurrentIndex(1);
    */
}

void MainWindow::on_delJingleToolButton_clicked()
{
    const int comp_idx = ui->jingleListWidget->currentRow();
    if (comp_idx > static_cast<int>(compositions.size()) || comp_idx < 0) {
        QMessageBox::information(this, tr("Error"),
            tr("Invalid Composition selected"));
        return;
    }

    compositions.erase(compositions.begin() + comp_idx);
    ui->jingleListWidget->takeItem(comp_idx);
    ui->jingleListWidget->setCurrentItem(NULL);
    ui->jingleListWidget->update();
    ui->jingleListWidget->repaint();
}

void MainWindow::on_mvJingleDownToolButton_clicked()
{
    const int comp_idx = ui->jingleListWidget->currentRow();
    if (comp_idx > static_cast<int>(compositions.size()) || comp_idx < 0) {
        QMessageBox::information(this, tr("Error"),
            tr("Invalid Composition selected"));
        return;
    }

    if (comp_idx + 1 >= static_cast<int>(compositions.size()) ) {
        return;
    }

    QListWidgetItem* item = ui->jingleListWidget->takeItem(comp_idx);
    ui->jingleListWidget->insertItem(comp_idx+1, item);
    ui->jingleListWidget->setCurrentItem(item);
    ui->jingleListWidget->update();
    ui->jingleListWidget->repaint();

    Composition comp = compositions.at(static_cast<uint32_t>(comp_idx));
    compositions[static_cast<uint32_t>(comp_idx)] = compositions[static_cast<uint32_t>(comp_idx)+1];
    compositions[static_cast<uint32_t>(comp_idx)+1] = comp;
}

void MainWindow::on_mvJingleUpToolButton_clicked()
{
    const int comp_idx = ui->jingleListWidget->currentRow();
    if (comp_idx > static_cast<int>(compositions.size()) || comp_idx < 0) {
        QMessageBox::information(this, tr("Error"),
            tr("Invalid Composition selected"));
        return;
    }

    if (comp_idx == 0) {
        return;
    }

    QListWidgetItem* item = ui->jingleListWidget->takeItem(comp_idx);
    ui->jingleListWidget->insertItem(comp_idx-1, item);
    ui->jingleListWidget->setCurrentItem(item);
    ui->jingleListWidget->update();
    ui->jingleListWidget->repaint();

    Composition comp = compositions.at(static_cast<uint32_t>(comp_idx));
    compositions[static_cast<uint32_t>(comp_idx)] = compositions[static_cast<uint32_t>(comp_idx)-1];
    compositions[static_cast<uint32_t>(comp_idx)-1] = comp;
}
