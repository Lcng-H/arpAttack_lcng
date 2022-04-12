#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <getdevinfo.h>
#include "sendrequest.h"
#include "scanner.h"
#include "arpattack.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    char *Dev;

signals:
    void startRequest(QString Dev);
    void startGetReply();

private slots:
    void on_action_getDev_triggered();
    void on_pBtn_startScan_clicked();
    void on_pBtn_endScan_clicked();
    void on_actionARP_triggered();

private:
    Ui::MainWindow *ui;
    QThread scan_Thread;
    QThread send_Thread;
    Scanner sc;
    SendRequest sr;
    getDevInfo *gdInfo = new getDevInfo(this);
    arpAttack *aa = new arpAttack(this);
};
#endif // MAINWINDOW_H
