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

private:
    static const uint32_t MAX_NUM_COMPS = 14;

    void updateMemUsage();
    void updateCompositionDisplay();

    Ui::MainWindow *ui;

    std::vector<Composition> compositions;
};

#endif // MAINWINDOW_H
