#include "mainwindow.h"
#include <QHBoxLayout>
#include <QPaintEvent>
#include <QPainter>
#include <QMessageBox>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    resize(QSize(800, 600));
    setWindowTitle("BondaPlayer");
    mPlayer = new Player;
    connect(&mPlayer->video,SIGNAL(sigGetOneFrame(QImage)),this,SLOT(slotGetOneFrame(QImage)));
    connect(mPlayer,SIGNAL(sigSendErrorMsg(QString)),this,SLOT(slotShowErrorMsg(QString)));
//    QString strPath = QFileDialog::getOpenFileName(this, tr("Open file dialog"), tr("/home"), tr("Videos (*.mp4 *.avi *.mkv)"));
//    if(strPath == NULL)
//    {
//        slotShowErrorMsg("Open file failed!");
//    }
//    std::string str = strPath.toStdString();

    mPlayer->Init("E:\\test.mp4"); // 此处应该会出问题
    mPlayer->start();
}

MainWindow::~MainWindow()
{
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setBrush(Qt::black);
    painter.drawRect(0, 0, this->width(), this->height()); //先画成黑色

    if (mImage.size().width() <= 0) return;

    ///将图像按比例缩放成和窗口一样大小
    QImage img = mImage.scaled(this->size(),Qt::KeepAspectRatio);

    int x = this->width() - img.width();
    int y = this->height() - img.height();

    x /= 2;
    y /= 2;

    painter.drawImage(QPoint(x,y),img); //画出图像

}

void MainWindow::slotGetOneFrame(QImage img)
{
    mImage = img;
    update(); //调用update将执行 paintEvent函数
}

void MainWindow::slotShowErrorMsg(QString msg)
{
    QMessageBox::critical(this, "Error", msg, QMessageBox::Ok);
}
