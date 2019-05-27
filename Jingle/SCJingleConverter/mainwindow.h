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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <vector>

#include "composition.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_playJinglePushButton_clicked();

    void on_browsePushButton_clicked();

    void on_convertPushButton_clicked();

    void on_delJingleToolButton_clicked();

    void on_mvJingleDownToolButton_clicked();

    void on_mvJingleUpToolButton_clicked();

    void on_jingleListWidget_clicked(const QModelIndex &index);

    void on_startMeasComboBox_activated(int index);

    void on_endMeasComboBox_activated(int index);

    void on_octaveAdjustLineEdit_editingFinished();

    void on_bpmLineEdit_editingFinished();

    void on_chanChordLeftComboBox_activated(int index);

    void on_chanChordRightComboBox_activated(int index);

    void on_chanSourceLeftComboBox_activated(const QString &arg1);

    void on_chanSourceRightComboBox_activated(const QString &arg1);

    void on_clearJinglesPushButton_clicked();

    void on_saveJinglesPushButton_clicked();

    void on_noteIntensityLineEdit_editingFinished();

private:
    Composition* getSelectedComposition();

    void updateMemUsage();
    void updateCompositionDisplay();
    void updateChordComboBox(Composition::Channel chan);

    Ui::MainWindow *ui;

    std::vector<Composition> compositions; // The compositions that
        // have been loaded from file, parsed and configured. This
        // is the data we read from when writing Jingle Data to
        // the Steam Controller EEPROM.
};

#endif // MAINWINDOW_H
