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

signals:
    void error(const QString &s);

private slots:
    void on_playJinglePushButton_clicked();

    void on_browsePushButton_clicked();

    void on_convertPushButton_clicked();

    void on_delJingleToolButton_clicked();

    void on_mvJingleDownToolButton_clicked();

    void on_mvJingleUpToolButton_clicked();

    void on_jingleListWidget_clicked(const QModelIndex &index);

    void on_chanSourceLeftComboBox_activated(int index);

    void on_chanSourceRightComboBox_activated(int index);

    void on_startMeasComboBox_activated(int index);

    void on_endMeasComboBox_activated(int index);

    void on_octaveAdjustLineEdit_editingFinished();

    void on_bpmLineEdit_editingFinished();

    void on_chanChordLeftComboBox_activated(int index);

    void on_chanChordRightComboBox_activated(int index);

private:
    static const uint32_t MAX_NUM_COMPS = 14; // The maximum number of Jingles
        // that can be loaded onto the Steam Controller. This is set based on
        // what Steam expects based on default Jingles.
    static const uint32_t MAX_EEPROM_BITES = 0x400; // The maximum number of
        // bytes that all Jingle Data can take up.

    void updateMemUsage();
    void updateCompositionDisplay();
    void updateChordComboL(Composition& composition);
    void updateChordComboR(Composition& composition);

    Ui::MainWindow *ui;

    std::vector<Composition> compositions;
};

#endif // MAINWINDOW_H
