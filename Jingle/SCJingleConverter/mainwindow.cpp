/**
 * \file mainwindow.cpp
 * \brief GUI for customizing Steam Controller Jingles.
 *
 * MIT License
 *
 * Copyright (c) 2019 Gregory Gluszek
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "scserial.h"

#include <QSerialPortInfo>
#include <QSerialPort>
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>

/**
 * @brief MainWindow::MainWindow Constructor.
 *
 * @param parent
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    const auto infos = QSerialPortInfo::availablePorts();

    // Populate combo box with all available serial ports
    for (const QSerialPortInfo &info : infos) {
        ui->serialPortComboBox->addItem(info.portName());
    }

    // Setup icons so user's know what these buttons do
    ui->delJingleToolButton->setIcon(ui->delJingleToolButton->style()->standardIcon(QStyle::SP_TrashIcon));
    ui->mvJingleUpToolButton->setIcon(ui->mvJingleUpToolButton->style()->standardIcon(QStyle::SP_ArrowUp));
    ui->mvJingleDownToolButton->setIcon(ui->mvJingleDownToolButton->style()->standardIcon(QStyle::SP_ArrowDown));
}

/**
 * @brief MainWindow::~MainWindow Destructor.
 */
MainWindow::~MainWindow() {
    delete ui;
}

/**
 * @brief MainWindow::getSelectedComposition Common function for
 *      getting a pointer to the Composition that related to whicih
 *      one has been selected in the GUI.
 *
 * @return A pointer to the selected composition or nullptr if
 *      no valid Composition is selected. In case of nullptr
 *      note that this function will produce a pop-up to
 *      communicate the issue to the user.
 */
Composition* MainWindow::getSelectedComposition() {
    const int comp_idx = ui->jingleListWidget->currentRow();

    if (comp_idx > static_cast<int>(compositions.size()) || comp_idx < 0) {
        QMessageBox::information(this, tr("Error"),
            tr("Invalid Composition selected"));
        return nullptr;
    }

    return &compositions[static_cast<uint32_t>(comp_idx)];
}

/**
 * @brief MainWindow::on_playJinglePushButton_clicked User has pressed button to
 *  demo what a single Jingle will sound like via the Haptics as currently configured.
 *
 * @return None.
 */
void MainWindow::on_playJinglePushButton_clicked() {
    QString serial_port_name = ui->serialPortComboBox->currentText();
    SCSerial serial(serial_port_name);
    if (!compositions.size()) {
        QMessageBox::information(this, tr("Error"),
            tr("No Compositions to Play"));
        return;
    }

    Composition* composition = getSelectedComposition();
    if (!composition) {
        return;
    }

    SCSerial::ErrorCode serial_err_code = serial.open();
    if (serial_err_code != SCSerial::NO_ERROR) {
        QMessageBox::information(this, tr("Error"),
            tr("Cannot open %1.\n\nError: %2")
            .arg(serial_port_name)
            .arg(SCSerial::getErrorString(serial_err_code)));
        return;
    }

    // Make sure there is enough memory to download selected Jingle
    //  as currently configured
    uint32_t num_bytes = Composition::EEPROM_HDR_NUM_BYTES +
            composition->getMemUsage();

    if (num_bytes > Composition::MAX_EEPROM_BYTES) {
        QMessageBox::information(this, tr("Error"),
            tr("Jingle is too large (%1/%2 bytes). Try using configuration "
               "options to reduce size.")
            .arg(num_bytes)
            .arg(Composition::MAX_EEPROM_BYTES));
        return;
    }

    // Since this is a demo mode, we are only concerned with the single
    //  Jingle. Best to start by clearing Steam Controller Jingle Data in
    //  controller RAM
    QString cmd("jingle clear\n");
    QString resp = cmd + "\rJingle data cleared successfully.\n\r";
    if (serial.send(cmd, resp)) {
        QMessageBox::information(this, tr("Error"),
            tr("Failed to clear Jingle Data."));
        return;
    }

    // Since we clear before add Jingle, it will always be at index 0
    Composition::ErrorCode comp_err_code = composition->download(serial, 0);
    if (comp_err_code != Composition::NO_ERROR) {
        QMessageBox::information(this, tr("Error"),
            tr("Cannot download to %1.\n\nError: %2")
            .arg(serial_port_name)
            .arg(Composition::getErrorString(comp_err_code)));
        return;
    }

    // Now instruct the Controller to play the Jingle downloaded to index 0
    cmd = "jingle play 0\n";
    resp = cmd + "\rJingle play started successfully.\n\r";
    if (serial.send(cmd, resp)) {
        QMessageBox::information(this, tr("Error"),
            tr("Failed to send play command."));
        return;
    }
}

/**
 * @brief MainWindow::on_browsePushButton_clicked Button to bring up a file
 *      brower so that user can find their MusicXML files that contain the
 *      Jingles they want to try and download.
 *
 * @return None.
 */
void MainWindow::on_browsePushButton_clicked()
{
    QString dir = ui->musicXmlPathLineEdit->text();

    // If previously selected directory does not exist,
    //  default to home directory
    if (dir.length() == 0) {
        dir = QDir::homePath();
    }

    QString fileName = QFileDialog::getOpenFileName(this,
        ("Open musicXML File"),
        dir,
        ("MusicXML Files (*.musicxml)"));
        // For when .mxl import support is added (maybe)
        //("MusicXML Files (*.musicxml *.mxl)"));

    // Only update line edit if user selected a file
    if (fileName.length() > 0) {
        ui->musicXmlPathLineEdit->setText(fileName);
    }
}

/**
 * @brief MainWindow::on_convertPushButton_clicked Once file path line edit
 *      has been filled in (either manually or via the Browse button)
 *      the Convert Button instructs us to actually parse the MusicXML file so
 *      that the Composition can be configured in order to download the
 *      desired Notes as a Jingle.
 *
 * @return None.
 */
void MainWindow::on_convertPushButton_clicked()
{
    if (compositions.size() > Composition::MAX_NUM_COMPS) {
        QMessageBox::information(this, tr("Error"),
            tr("Too many Compositions have been added. "
               "Please delete before attemping to add another."));
        return;
    }

    QString full_filename = ui->musicXmlPathLineEdit->text();
    QString reduced_filename = full_filename;

    // Remove .musicxml extension or .mxl extension
    if (full_filename.endsWith(".musicxml")) {
        reduced_filename.resize(reduced_filename.size() - QString(".musicxml").size());
    // Add back in when/if support for .mxl is added
    /*
    } else if (full_filename.endsWith(".mxl")) {
        reduced_filename.resize(reduced_filename.size() - QString(".mxl").size());
    */
    } else {
        QMessageBox::information(this, tr("Error"),
            tr("Bad extension on file '%1'.")
            .arg(full_filename));
        return;
    }

    // Create string to identify this Composition
    // Remove path
    QStringList list = reduced_filename.split('/');
    reduced_filename = list[list.size()-1];
    list = reduced_filename.split('\\');
    reduced_filename = list[list.size()-1];

    compositions.push_back(Composition(full_filename));
    Composition& composition = compositions.back();

    Composition::ErrorCode comp_err_code = composition.parse();
    if (Composition::NO_ERROR != comp_err_code) {
        QMessageBox::information(this, tr("Error"),
            tr("Failed to parse file '%1'.\nError: %2")
            .arg(full_filename)
            .arg(Composition::getErrorString(comp_err_code)));
        compositions.pop_back();
        return;
    }

    // Add identifier string and make sure it is selected
    ui->jingleListWidget->addItem(reduced_filename);
    ui->jingleListWidget->setCurrentItem(ui->jingleListWidget->item(ui->jingleListWidget->count()-1));
    ui->jingleListWidget->repaint();

    // Update GUI to show specs on newly added Composition
    updateCompositionDisplay();

    // Update memory usage display since Composition has been added
    updateMemUsage();
}

/**
 * @brief MainWindow::updateMemUsage The Steam Controller only has MAX_EEPROM_BYTES
 *      of EEPROM to work with in terms of Jingle Data. (This is set by the design of the
 *      official controller firmware). A progress bar and label convey to the user
 *      how much of that data they are currently using with their Compositions
 *      Configured as is. Call this function any time a new Composition is added
 *      or Composition configured is changed.
 *
 * @return None.
 */
void MainWindow::updateMemUsage() {
    const int PROG_BAR_MAX = 100;

    uint32_t num_bytes = Composition::EEPROM_HDR_NUM_BYTES;

    for (uint32_t comp_idx = 0; comp_idx < compositions.size(); comp_idx++) {
        num_bytes += compositions[comp_idx].getMemUsage();
    }

    if (num_bytes >= Composition::MAX_EEPROM_BYTES) {
        ui->memUsageProgressBar->setValue(PROG_BAR_MAX);
    } else {
        ui->memUsageProgressBar->setValue(PROG_BAR_MAX * num_bytes
            / Composition::MAX_EEPROM_BYTES);
    }
    ui->memUsageProgressBar->update();
    ui->memUsageProgressBar->repaint();

    QString bytes_used_str = QString::number(num_bytes) +
            "/" + QString::number(Composition::MAX_EEPROM_BYTES) +
            " bytes used";
    ui->memUsageCurrBytesLabel->setText(bytes_used_str);
    ui->memUsageCurrBytesLabel->update();
    ui->memUsageCurrBytesLabel->repaint();
}

/**
 * @brief MainWindow::updateCompositionDisplay This updates the GUI elements that are
 *      specific to the selected Composition. When a Composition is selected (or new
 *      one added) these fields need to be updated based on the configuration for
 *      that Composition.
 *
 * @return None.
 */
void MainWindow::updateCompositionDisplay() {
    Composition* composition = getSelectedComposition();
    if (!composition) {
        return;
    }

    ui->startMeasComboBox->clear();
    ui->endMeasComboBox->clear();
    for (uint32_t meas_idx = 0; meas_idx < composition->getNumMeasures(); meas_idx++) {
        ui->startMeasComboBox->addItem(QString::number(meas_idx));
        ui->endMeasComboBox->addItem(QString::number(meas_idx));
    }
    ui->startMeasComboBox->setCurrentIndex(static_cast<int>(composition->getMeasStartIdx()));
    ui->endMeasComboBox->setCurrentIndex(static_cast<int>(composition->getMeasEndIdx()));
    ui->startMeasComboBox->update();
    ui->startMeasComboBox->repaint();
    ui->endMeasComboBox->update();
    ui->endMeasComboBox->repaint();

    ui->bpmLineEdit->setText(QString::number(composition->getBpm()));
    ui->bpmLineEdit->update();
    ui->bpmLineEdit->repaint();
    ui->noteIntensityLineEdit->setText(QString::number(composition->getNoteIntensity()));
    ui->noteIntensityLineEdit->update();
    ui->noteIntensityLineEdit->repaint();
    ui->octaveAdjustLineEdit->setText(QString().setNum(composition->getOctaveAdjust(), 'f', 2));
    ui->octaveAdjustLineEdit->update();
    ui->octaveAdjustLineEdit->repaint();

    std::vector<QString> voice_strs = composition->getVoiceStrs();
    ui->chanSourceLeftComboBox->clear();
    ui->chanSourceRightComboBox->clear();
    ui->chanSourceLeftComboBox->addItem(Composition::getNoVoiceStr());
    ui->chanSourceRightComboBox->addItem(Composition::getNoVoiceStr());
    ui->chanSourceLeftComboBox->setCurrentIndex(0);
    ui->chanSourceRightComboBox->setCurrentIndex(0);
    for (uint32_t voice_idx = 0; voice_idx < voice_strs.size(); voice_idx++) {
        ui->chanSourceLeftComboBox->addItem(voice_strs[voice_idx]);
        if (voice_strs[voice_idx] == composition->getVoice(Composition::LEFT)) {
            ui->chanSourceLeftComboBox->setCurrentIndex(static_cast<int>(voice_idx+1));
        }
        ui->chanSourceRightComboBox->addItem(voice_strs[voice_idx]);
        if (voice_strs[voice_idx] == composition->getVoice(Composition::RIGHT)) {
            ui->chanSourceRightComboBox->setCurrentIndex(static_cast<int>(voice_idx+1));
        }
    }
    ui->chanSourceLeftComboBox->update();
    ui->chanSourceLeftComboBox->repaint();
    ui->chanSourceRightComboBox->update();
    ui->chanSourceRightComboBox->repaint();

    updateChordComboBox(Composition::LEFT);
    updateChordComboBox(Composition::RIGHT);
}

/**
 * @brief MainWindow::updateChordComboBox Chord combo boxes allow a user to
 *      select which Frequency in a chord is to be played. However, what selections
 *      are available can change based on a variety of actions (i.e. changing
 *      the start and/or stop measure).
 *
 * @param chan Defines which Channel/Haptic we are referring to.
 *
 * @return None.
 */
void MainWindow::updateChordComboBox(Composition::Channel chan) {
    Composition* composition = getSelectedComposition();
    if (!composition) {
        return;
    }

    QComboBox* combo_box = nullptr;

    switch (chan) {
    case Composition::LEFT:
        combo_box = ui->chanChordLeftComboBox;
        break;

    case Composition::RIGHT:
        combo_box = ui->chanChordRightComboBox;
        break;
    }

    combo_box->clear();

    const QString voice_str = composition->getVoice(chan);
    if (voice_str == Composition::getNoVoiceStr()) {
        combo_box->update();
        combo_box->repaint();
        return;
    }

    const uint32_t meas_start_idx = composition->getMeasStartIdx();
    const uint32_t meas_end_idx = composition->getMeasEndIdx();
    const uint32_t num_chords = composition->getNumChords(voice_str, meas_start_idx, meas_end_idx);

    for (uint32_t chord_idx = 0; chord_idx < num_chords; chord_idx++) {
        combo_box->addItem(QString::number(chord_idx));
    }
    combo_box->setCurrentIndex(static_cast<int>(composition->getChordIdx(chan)));
    combo_box->update();
    combo_box->repaint();
}

/**
 * @brief MainWindow::on_delJingleToolButton_clicked User has pressed button
 *      to remove a Jingle that was previously added.
 *
 * @return None.
 */
void MainWindow::on_delJingleToolButton_clicked()
{
    Composition* composition = getSelectedComposition();
    if (!composition) {
        return;
    }

    const int comp_idx = ui->jingleListWidget->currentRow();

    compositions.erase(compositions.begin() + comp_idx);
    ui->jingleListWidget->takeItem(comp_idx);
    ui->jingleListWidget->setCurrentItem(nullptr);
    ui->jingleListWidget->update();
    ui->jingleListWidget->repaint();

    // Clear out all ui elements related to Composition
    ui->startMeasComboBox->clear();
    ui->startMeasComboBox->update();
    ui->startMeasComboBox->repaint();
    ui->endMeasComboBox->clear();
    ui->endMeasComboBox->update();
    ui->endMeasComboBox->repaint();

    ui->bpmLineEdit->clear();
    ui->bpmLineEdit->update();
    ui->bpmLineEdit->repaint();
    ui->noteIntensityLineEdit->clear();
    ui->noteIntensityLineEdit->update();
    ui->noteIntensityLineEdit->repaint();
    ui->octaveAdjustLineEdit->clear();
    ui->octaveAdjustLineEdit->update();
    ui->octaveAdjustLineEdit->repaint();

    ui->chanSourceLeftComboBox->clear();
    ui->chanSourceLeftComboBox->update();
    ui->chanSourceLeftComboBox->repaint();
    ui->chanSourceRightComboBox->clear();
    ui->chanSourceRightComboBox->update();
    ui->chanSourceRightComboBox->repaint();

    ui->chanChordRightComboBox->clear();
    ui->chanChordRightComboBox->update();
    ui->chanChordRightComboBox->repaint();
    ui->chanChordLeftComboBox->clear();
    ui->chanChordLeftComboBox->update();
    ui->chanChordLeftComboBox->repaint();

    updateMemUsage();
}

/**
 * @brief MainWindow::on_mvJingleDownToolButton_clicked For organizing
 *      the order of the Jingles. This moves a Jingle down on the list.
 *
 * @return None.
 */
void MainWindow::on_mvJingleDownToolButton_clicked()
{
    Composition* composition = getSelectedComposition();
    if (!composition) {
        return;
    }

    const int comp_idx = ui->jingleListWidget->currentRow();

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

/**
 * @brief MainWindow::on_mvJingleUpToolButton_clicked Similar to
 *      on_mvJingleDownToolButton_clicked except moving Composition in other
 *      direction on list.
 *
 * @return None.
 */
void MainWindow::on_mvJingleUpToolButton_clicked()
{
    Composition* composition = getSelectedComposition();
    if (!composition) {
        return;
    }

    const int comp_idx = ui->jingleListWidget->currentRow();

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

/**
 * @brief MainWindow::on_jingleListWidget_clicked User has selected a
 *      Composition to configure or test play.
 *
 * @param index Unused.
 *
 * @return None.
 */
void MainWindow::on_jingleListWidget_clicked(const QModelIndex &index)
{
    Q_UNUSED(index);

    if (ui->jingleListWidget->currentRow() < 0)
        return;

    updateCompositionDisplay();
}

/**
 * @brief MainWindow::on_startMeasComboBox_activated User has changed Start
 *      Measure configuration for currently selected Composition.
 *
 * @param index Defines Start Measure index that has been selected.
 *
 * @return None.
 */
void MainWindow::on_startMeasComboBox_activated(int index)
{
    Composition* composition = getSelectedComposition();
    if (!composition) {
        return;
    }

    if (index < 0) {
        QMessageBox::information(this, tr("Error"),
            tr("Invalid Start Measure selected"));
        return;
    }

    Composition::ErrorCode comp_err_code =  composition->setMeasStartIdx(static_cast<uint32_t>(index));
    if (comp_err_code != Composition::NO_ERROR) {
        QMessageBox::information(this, tr("Error"),
            tr("Error setting Start Measure Index.\n\nError: %1")
            .arg(Composition::getErrorString(comp_err_code)));
        return;
    }

    updateChordComboBox(Composition::LEFT);
    updateChordComboBox(Composition::RIGHT);

    updateMemUsage();
}

/**
 * @brief MainWindow::on_endMeasComboBox_activated User has changed End
 *      Measure configuration for currently selected Composition.
 *
 * @param index Defines End Measure index that has been selected.
 *
 * @return None.
 */
void MainWindow::on_endMeasComboBox_activated(int index)
{
    Composition* composition = getSelectedComposition();
    if (!composition) {
        return;
    }

    if (index < 0) {
        QMessageBox::information(this, tr("Error"),
            tr("Invalid Start Measure selected"));
        return;
    }

    Composition::ErrorCode comp_err_code =  composition->setMeasEndIdx(static_cast<uint32_t>(index));
    if (comp_err_code != Composition::NO_ERROR) {
        QMessageBox::information(this, tr("Error"),
            tr("Error setting End Measure Index.\n\nError: %1")
            .arg(Composition::getErrorString(comp_err_code)));
        return;
    }

    updateChordComboBox(Composition::LEFT);
    updateChordComboBox(Composition::RIGHT);

    updateMemUsage();
}

/**
 * @brief MainWindow::on_octaveAdjustLineEdit_editingFinished Allows user to
 *      specificy multiplication factor for adjusting all Note frequencies.
 *
 * @return None.
 */
void MainWindow::on_octaveAdjustLineEdit_editingFinished()
{
    Composition* composition = getSelectedComposition();
    if (!composition) {
        return;
    }

    float octave_adjust = ui->octaveAdjustLineEdit->text().toFloat();

    qDebug() << "Adjusting Octave scaling factor to " << octave_adjust;

    composition->setOctaveAdjust(octave_adjust);
}

/**
 * @brief MainWindow::on_bpmLineEdit_editingFinished Allows user to adjust
 *      playback speed.
 *
 * @return None.
 */
void MainWindow::on_bpmLineEdit_editingFinished()
{
    Composition* composition = getSelectedComposition();
    if (!composition) {
        return;
    }

    uint32_t bpm = ui->bpmLineEdit->text().toUInt();

    qDebug() << "Adjusting BPM to " << bpm;

    composition->setBpm(bpm);
}

/**
 * @brief MainWindow::on_chanChordLeftComboBox_activated Allows user to specify
 *      which Frequency in Chord Notes is to be played.
 *
 * @param index Defines which Chord Index selection was made.
 *
 * @return None.
 */
void MainWindow::on_chanChordLeftComboBox_activated(int index)
{
    Composition* composition = getSelectedComposition();
    if (!composition) {
        return;
    }

    if (index < 0) {
        QMessageBox::information(this, tr("Error"),
            tr("Invalid Left Chord Index selected"));
        return;
    }

    Composition::ErrorCode comp_err_code = composition->setChordIdx(Composition::LEFT, static_cast<uint32_t>(index));
    if (comp_err_code != Composition::NO_ERROR) {
        QMessageBox::information(this, tr("Error"),
            tr("Error setting Left Chord Index.\n\nError: %1")
            .arg(Composition::getErrorString(comp_err_code)));
        return;
    }
}

/**
 * @brief MainWindow::on_chanChordRightComboBox_activated Allows user to specify
 *      which Frequency in Chord Notes is to be played.
 *
 * @param index Defines which Chord Index selection was made.
 *
 * @return None.
 */
void MainWindow::on_chanChordRightComboBox_activated(int index)
{
    Composition* composition = getSelectedComposition();
    if (!composition) {
        return;
    }

    if (index < 0) {
        QMessageBox::information(this, tr("Error"),
            tr("Invalid Right Chord Index selected"));
        return;
    }

    Composition::ErrorCode comp_err_code = composition->setChordIdx(Composition::RIGHT, static_cast<uint32_t>(index));
    if (comp_err_code != Composition::NO_ERROR) {
        QMessageBox::information(this, tr("Error"),
            tr("Error setting Right Chord Index.\n\nError: %1")
            .arg(Composition::getErrorString(comp_err_code)));
        return;
    }
}

/**
 * @brief MainWindow::on_chanSourceLeftComboBox_activated User selects which
 *      Voice of Composition will be played on Left Haptic.
 *
 * @param voiceStr Key used to refer to Notes in a particular Voice.
 *
 * @return None.
 */
void MainWindow::on_chanSourceLeftComboBox_activated(const QString& voiceStr)
{
    Composition* composition = getSelectedComposition();
    if (!composition) {
        return;
    }

    Composition::ErrorCode comp_err_code = composition->setVoice(Composition::LEFT, voiceStr);
    if (comp_err_code != Composition::NO_ERROR) {
        QMessageBox::information(this, tr("Error"),
            tr("Error setting Voice.\n\nError: %1")
            .arg(Composition::getErrorString(comp_err_code)));
        return;
    }

    updateChordComboBox(Composition::LEFT);

    updateMemUsage();
}

/**
 * @brief MainWindow::on_chanSourceRightComboBox_activated User selects which
 *      Voice of Composition will be played on Right Haptic.
 *
 * @param voiceStr Key used to refer to Notes in a particular Voice.
 *
 * @return None.
 */
void MainWindow::on_chanSourceRightComboBox_activated(const QString& voiceStr)
{
    Composition* composition = getSelectedComposition();
    if (!composition) {
        return;
    }

    Composition::ErrorCode comp_err_code = composition->setVoice(Composition::RIGHT, voiceStr);
    if (comp_err_code != Composition::NO_ERROR) {
        QMessageBox::information(this, tr("Error"),
            tr("Error setting Voice.\n\nError: %1")
            .arg(Composition::getErrorString(comp_err_code)));
        return;
    }

    updateChordComboBox(Composition::RIGHT);

    updateMemUsage();
}

/**
 * @brief MainWindow::on_clearJinglesPushButton_clicked User is instructing us to clear EEPROM so
 *      that Steam Controller uses defaults baked into official Firmware.
 *
 * @return None.
 */
void MainWindow::on_clearJinglesPushButton_clicked()
{
    QString serial_port_name = ui->serialPortComboBox->currentText();
    SCSerial serial(serial_port_name);

    SCSerial::ErrorCode serial_err_code = serial.open();
    if (serial_err_code != SCSerial::NO_ERROR) {
        QMessageBox::information(this, tr("Error"),
            tr("Cannot open %1.\n\nError: %2")
            .arg(serial_port_name)
            .arg(SCSerial::getErrorString(serial_err_code)));
        return;
    }

    QString cmd("jingle eeprom clear\n");
    QString resp = cmd + "\rClear complete\n\r";
    if (serial.send(cmd, resp)) {
        QMessageBox::information(this, tr("Error"),
            tr("Failed to clear Jingle Data."));
        return;
    }

    QMessageBox::information(this, tr("Success!"),
        tr("EEPROM cleared of custom Jingle Data."));
}

/**
 * @brief MainWindow::on_saveJinglesPushButton_clicked User is instructing us to save all
 *      converted Compositions to Jingle Data to EEPROM so that when official firmware is
 *      loaded the controller will still use the customized Jingles.
 *
 * @return None.
 */
void MainWindow::on_saveJinglesPushButton_clicked()
{
    QString serial_port_name = ui->serialPortComboBox->currentText();
    SCSerial serial(serial_port_name);
    if (!compositions.size()) {
        QMessageBox::information(this, tr("Error"),
            tr("No Compositions to Save to EEPROM"));
        return;
    }

    SCSerial::ErrorCode serial_err_code = serial.open();
    if (serial_err_code != SCSerial::NO_ERROR) {
        QMessageBox::information(this, tr("Error"),
            tr("Cannot open %1.\n\nError: %2")
            .arg(serial_port_name)
            .arg(SCSerial::getErrorString(serial_err_code)));
        return;
    }

    // Make sure there is enough memory to download all Jingle Data...
    uint32_t num_bytes = Composition::EEPROM_HDR_NUM_BYTES;
    for (uint32_t comp_idx = 0; comp_idx < compositions.size(); comp_idx++) {
        num_bytes += compositions[comp_idx].getMemUsage();
    }

    if (num_bytes > Composition::MAX_EEPROM_BYTES) {
        QMessageBox::information(this, tr("Error"),
            tr("Total Jingle Data is too large (%1/%2 bytes). Try using configuration "
               "options to reduce size.")
            .arg(num_bytes)
            .arg(Composition::MAX_EEPROM_BYTES));
        return;
    }

    // Start fresh with Jingle Data in RAM
    QString cmd("jingle clear\n");
    QString resp = cmd + "\rJingle data cleared successfully.\n\r";
    if (serial.send(cmd, resp)) {
        QMessageBox::information(this, tr("Error"),
            tr("Failed to clear Jingle Data."));
        return;
    }

    // Attempt to write each Jingle to RAM
    for (uint32_t comp_idx = 0; comp_idx < compositions.size(); comp_idx++) {
        Composition::ErrorCode comp_err_code = compositions[comp_idx].download(serial, comp_idx);
        if (comp_err_code != Composition::NO_ERROR) {
            QMessageBox::information(this, tr("Error"),
                tr("Failed downloading Jingle %1 via %2.\n\nError: %3")
                .arg(comp_idx)
                .arg(serial_port_name)
                .arg(Composition::getErrorString(comp_err_code)));
            return;
        }
    }

    // Save Jingle data in controller RAM to EEPROM
    cmd = "jingle eeprom save\n";
    resp = cmd + "\rSave complete\n\r";
    if (serial.send(cmd, resp, 100)) {
        QMessageBox::information(this, tr("Error"),
            tr("Failed to save Jingle Data."));
        return;
    }

    QMessageBox::information(this, tr("Success!"),
        tr("Jingle Data Saved to EEPROM."));
}

/**
 * @brief MainWindow::on_noteIntensityLineEdit_editingFinished This allows user to
 *      adjust how loud/intense each Note of a Jingle is.
 *
 * @return None.
 */
void MainWindow::on_noteIntensityLineEdit_editingFinished()
{
    Composition* composition = getSelectedComposition();
    if (!composition) {
        return;
    }

    uint32_t note_intensity = ui->noteIntensityLineEdit->text().toUInt();

    if (note_intensity > 255) {
        note_intensity = 255;
    }

    qDebug() << "Adjusting Note Intensity to " << note_intensity;

    composition->setNoteIntensity(note_intensity);
}
