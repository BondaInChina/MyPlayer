#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QThread>
#include <QImage>

class Player : public QThread
{
    Q_OBJECT
public:
    explicit Player();
signals:
    void sig_GetOneFrame(QImage);
protected:
    void run();
    bool Init();
};

#endif // PLAYER_H
