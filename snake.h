#ifndef SNAKE_H
#define SNAKE_H

#include <QPainter>
#include <QKeyEvent>

class Snake
{
private:
    void deletetail();
    void addleft();
    void addright();
    void addup();
    void adddown();
public:

    QList<QRectF> snake;
    enum Movedirect{left,right,up,down} movedirect=left;
    QColor color;

    Snake();
    ~Snake();
    void setColor(QColor Color);
    QColor getcolor();
    void paint(QPainter painter);
    bool move(QRectF food);
    void setPos(int x,int y);
};

#endif // SNAKE_H
