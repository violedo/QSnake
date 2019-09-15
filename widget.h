#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTimer>
#include <QTime>
#include <QKeyEvent>
#include <QPainter>
#include "snake.h"
namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();
protected slots:
    void move();
    void addfood(int i);
    void addwall();
protected:
    void keyPressEvent(QKeyEvent *press);
    void paintEvent(QPaintEvent *paint);
private:
    int collision();
    void singleini();
    void multiini();
    void wallini();
    void search();
    int dfs(int i,int j);
private:
    Ui::Widget *ui;
    QTimer *movetimer;
    int speed=100;
    const int cspeed=100;
    QRectF food;
    QList<QRectF> walls;
    Snake* snake=nullptr;
    enum Gamemode{start,single,multi,ai,singlepause,multipause,aipause,singleend,multiend,help,morefood,morefoodpause,wall,wallpause} gamemode;
    int focus=0;
    int winner;
    int foodcount=0;
    enum Foodtype{normal,fast,slow} foodtype=normal;
    int bufftime;

    struct Pos{
        bool used=false;
        bool accessable=true;
        int last;
    };
    Pos pos[40][40];
    QList<int> quex;
    QList<int> quey;
};

#endif // WIDGET_H
