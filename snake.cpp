#include "snake.h"
#include <QPainter>
#include <QKeyEvent>

Snake::Snake(){}
Snake::~Snake(){
    snake.clear();
}
void Snake::setPos(int x,int y)
{
     snake.insert(0,QRectF(x,y,8,8));
}
void Snake::paint(QPainter painter){
    painter.setPen(color);
    painter.setBrush(color);
    for (int i=0;i<snake.length();++i)
        painter.drawRect(snake[i]);
}
bool Snake::move(QRectF food)
{

    switch (movedirect) {
        case left:addleft();break;
        case right:addright();break;
        case up:addup();break;
        case down:adddown();break;
        default:break;
    }
    if (food.contains(snake[0].topLeft()+QPointF(5,5)))
        return true;
    else {
        deletetail();
        return false;
    }
}

void Snake::deletetail(){
    snake.pop_back();
}
void Snake::addleft()
{
    /*if (snake[0].x()<10)
        snake.insert(0,QRectF(width()-10,snake[0].y(),10,10));
    else*/ snake.insert(0,QRectF(snake[0].x()-10,snake[0].y(),8,8));
}
void Snake::addright()
{
    /*if (width()-snake[0].x()<20)
        snake.insert(0,QRectF(0,snake[0].y(),10,10));
    else*/ snake.insert(0,QRectF(snake[0].x()+10,snake[0].y(),8,8));
}
void Snake::addup()
{
    /*if (snake[0].y()<10)
        snake.insert(0,QRectF(snake[0].x(),height()-10,10,10));
    else*/ snake.insert(0,QRectF(snake[0].x(),snake[0].y()-10,8,8));
}
void Snake::adddown()
{
    /*if (height()-snake[0].y()<20)
        snake.insert(0,QRectF(snake[0].x(),0,10,10));
    else*/ snake.insert(0,QRectF(snake[0].x(),snake[0].y()+10,8,8));
}
