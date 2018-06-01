#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include "player.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
    void paintEvent(QPaintEvent *event);
private:
    QImage mImage;
    Player *mPlayer;
private slots:
    void slotGetOneFrame(QImage img);
    void slotShowErrorMsg(QString msg);
};

#endif // MAINWINDOW_H
