#include "widget.h"
#include "ui_widget.h"
#include "snake.h"
#include <QTimer>
#include <QTime>
#include <QKeyEvent>
#include <QPainter>
#include "iostream"
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    resize(400,400);
    setWindowTitle(tr("Qsnake"));
    setStyleSheet("QWidget{background:white}");

    movetimer=new QTimer;
    gamemode=start;
    connect(movetimer,SIGNAL(timeout()),this,SLOT(move()));

}

Widget::~Widget()
{
    delete ui;
}

void Widget::singleini()
{
    if (snake)
        delete snake;
    snake=new Snake;
    snake->setPos(201,201);
    snake->color=Qt::blue;
    foodcount=0;
    if (gamemode==ai)
        pos[20][20].accessable=false;
    addfood(1);
    speed=cspeed;
    movetimer->start(10);
}
void Widget::multiini()
{
    if (snake)
        delete []snake;
    snake=new Snake[2];
    snake[0].setPos(151,201);
    snake[1].setPos(251,201);
    snake[0].color=Qt::blue;
    snake[1].color=Qt::red;
    addfood(2);
    speed=cspeed;
    movetimer->start(cspeed);
}
void Widget::wallini()
{
    if (snake)
        delete snake;
    snake=new Snake;
    snake->setPos(201,201);
    snake->color=Qt::blue;
    walls.push_back(QRectF(20,20,50,10));
    walls.push_back(QRectF(250,190,20,30));
    walls.push_back(QRectF(100,300,10,70));
    walls.push_back(QRectF(360,70,10,30));
    walls.push_back(QRectF(310,280,10,40));
    addfood(1);
    speed=cspeed;
    movetimer->start(cspeed);
}

void Widget::move()
{

    switch (gamemode) {
    case ai:{
        search();
        int a=snake->snake.last().center().x()/10,b=snake->snake.last().center().y()/10;
        if (snake->move(food))
        {
            addfood(1);
        }
        else pos[a][b].accessable=true;
        pos[(int)(snake->snake[0].center().x()/10)][(int)(snake->snake[0].center().y()/10)].accessable=false;
        if (collision())
            gamemode=singleend;
        break;}
    case single:
        if (snake->move(food))
            addfood(1);
        if (collision())
            gamemode=singleend;
        break;
    case multi:
        if (snake[0].move(food)||snake[1].move(food))
            addfood(2);
        switch (collision()) {
        case 0:break;
        case 1:
            winner=2;
            gamemode=multiend;
            break;
        case 2:
            winner=1;
            gamemode=multiend;
            break;
        case 3:
            winner=3;
            gamemode=multiend;
            break;
        }
        break;
    case wall:

        if (snake->move(food))
        {
            addwall();
            addfood(1);
        }
        if (collision())
            gamemode=singleend;
        break;
    case morefood:
        if (bufftime>0)
            bufftime-=speed;
        if (bufftime<=0&&speed!=cspeed)
        {
            speed=cspeed;
            movetimer->stop();
            movetimer->start(speed);
        }
        if (snake->move(food))
        {
            switch (foodtype) {
            case fast:
                movetimer->stop();
                speed=speed/2;
                movetimer->start(speed);
                bufftime=5000;
                break;
            case slow:
                movetimer->stop();
                speed=speed*2;
                movetimer->start(speed);
                bufftime=5000;
                break;
            default:break;
            }
            addfood(1);
        }
        if (collision())
            gamemode=singleend;
        break;
    }
    update();

}



void Widget::addfood(int i)
{
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    if (gamemode==morefood)
    {
        int x=qrand()%10;
        if (x==0)
            foodtype=fast;
        else if (x==1)
            foodtype=slow;
        else foodtype=normal;
    }
    bool flag=false;
    while (!flag){
        food=QRectF(qrand()%(width()/10)*10,qrand()%(height()/10)*10,10,10);
        flag=true;
        for (int j=0;j<i;++j){
            for (int p=0;p<snake[j].snake.length();++p)
                if (food.contains(snake[j].snake[p].topLeft()+QPointF(5,5)))
                {
                    flag=false;
                    break;
                }
            if (!flag)
                break;
        }
        if (flag&&gamemode==wall){
            for (int i=0;i<walls.length();++i)
                if (walls[i].contains(food))
                {
                    flag=false;
                    break;
                }
        }

    }
}

void Widget::addwall()
{
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    bool flag=false;
    while (!flag){
        QRectF wall(qrand()%(width()/10)*10,qrand()%(height()/10)*10,(qrand()%5+1)*10,(qrand()%5+1)*10);
        flag=true;
        if (flag)
            for (int p=0;p<snake[0].snake.length();++p)
                if (wall.contains(snake[0].snake[p].topLeft()+QPointF(5,5)))
                {
                    flag=false;
                    break;
                }
        if (flag)
            for (int i=0;i<walls.length();++i)
                if (walls[i].contains(wall))
                {
                    flag=false;
                    break;
                }
        if (flag)
            walls.push_back(wall);
    }
}

void Widget::keyPressEvent(QKeyEvent *press)
{

    switch (gamemode) {
    case start:
        switch (press->key()) {
        case Qt::Key_Up:if (focus>0) --focus;else focus=5;update();break;
        case Qt::Key_Down:if (focus<5) ++focus;else focus=0;update();break;
        case Qt::Key_Enter:
        case Qt::Key_Return:
            switch (focus) {
            case 0:gamemode=single;singleini();break;
            case 1:gamemode=multi;multiini();break;
            case 2:gamemode=ai;singleini();break;
            case 3:gamemode=morefood;singleini();break;
            case 4:gamemode=wall;wallini();break;
            case 5:gamemode=help;break;
            }
            update();
            break;
        }
        break;
    case single:
        switch (press->key()) {
        case Qt::Key_Up:if (snake[0].movedirect!=snake[0].down) snake[0].movedirect=snake[0].up;break;
        case Qt::Key_Down:if (snake[0].movedirect!=snake[0].up) snake[0].movedirect=snake[0].down;break;
        case Qt::Key_Left:if (snake[0].movedirect!=snake[0].right) snake[0].movedirect=snake[0].left;break;
        case Qt::Key_Right:if (snake[0].movedirect!=snake[0].left) snake[0].movedirect=snake[0].right;break;
        case Qt::Key_Escape:gamemode=singlepause;movetimer->stop();break;
        }
        break;
    case wall:
        switch (press->key()) {
        case Qt::Key_Up:if (snake[0].movedirect!=snake[0].down) snake[0].movedirect=snake[0].up;break;
        case Qt::Key_Down:if (snake[0].movedirect!=snake[0].up) snake[0].movedirect=snake[0].down;break;
        case Qt::Key_Left:if (snake[0].movedirect!=snake[0].right) snake[0].movedirect=snake[0].left;break;
        case Qt::Key_Right:if (snake[0].movedirect!=snake[0].left) snake[0].movedirect=snake[0].right;break;
        case Qt::Key_Escape:gamemode=wallpause;movetimer->stop();break;
        }
        break;
    case morefood:
        switch (press->key()) {
        case Qt::Key_Up:if (snake[0].movedirect!=snake[0].down) snake[0].movedirect=snake[0].up;break;
        case Qt::Key_Down:if (snake[0].movedirect!=snake[0].up) snake[0].movedirect=snake[0].down;break;
        case Qt::Key_Left:if (snake[0].movedirect!=snake[0].right) snake[0].movedirect=snake[0].left;break;
        case Qt::Key_Right:if (snake[0].movedirect!=snake[0].left) snake[0].movedirect=snake[0].right;break;
        case Qt::Key_Escape:gamemode=morefoodpause;movetimer->stop();break;
        }
        break;
    case multi:
        switch (press->key()) {
        case Qt::Key_Up:if (snake[0].movedirect!=snake[0].down) snake[0].movedirect=snake[0].up;break;
        case Qt::Key_Down:if (snake[0].movedirect!=snake[0].up) snake[0].movedirect=snake[0].down;break;
        case Qt::Key_Left:if (snake[0].movedirect!=snake[0].right) snake[0].movedirect=snake[0].left;break;
        case Qt::Key_Right:if (snake[0].movedirect!=snake[0].left) snake[0].movedirect=snake[0].right;break;
        case Qt::Key_W:if (snake[1].movedirect!=snake[1].down) snake[1].movedirect=snake[1].up;break;
        case Qt::Key_S:if (snake[1].movedirect!=snake[1].up) snake[1].movedirect=snake[1].down;break;
        case Qt::Key_A:if (snake[1].movedirect!=snake[1].right) snake[1].movedirect=snake[1].left;break;
        case Qt::Key_D:if (snake[1].movedirect!=snake[1].left) snake[1].movedirect=snake[1].right;break;
        case Qt::Key_Escape:gamemode=multipause;movetimer->stop();break;
        }
        break;
    case help:
        if (press->key()==Qt::Key_Escape) {
            gamemode=start;
            update();
        }
        break;
    case singlepause:
        if (press->key()==Qt::Key_Escape) {
            gamemode=single;
            movetimer->start(speed);
        }
        break;
    case wallpause:
        if (press->key()==Qt::Key_Escape) {
            gamemode=wall;
            movetimer->start(speed);
        }
        break;
    case morefoodpause:
        if (press->key()==Qt::Key_Escape) {
            gamemode=morefood;
            movetimer->start(speed);
        }
        break;
    case multipause:
        if (press->key()==Qt::Key_Escape) {
            gamemode=multi;
            movetimer->start(speed);
        }
        break;
    case aipause:
        if (press->key()==Qt::Key_Escape) {
            gamemode=ai;
            movetimer->start(speed);
        }
        break;
    case singleend:
    case multiend:
        if (press->key()==Qt::Key_Enter||press->key()==Qt::Key_Return)
        {
            gamemode=start;
            update();
        }
        break;
    }
}

void Widget::paintEvent(QPaintEvent *paint)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    switch (gamemode) {
    case start:
    {
        painter.setPen(Qt::blue);
        painter.setBrush(Qt::blue);
        painter.drawRect(QRectF(60,100,280,40));
        painter.drawRect(QRectF(60,150,280,40));
        painter.drawRect(QRectF(60,200,280,40));
        painter.drawRect(QRectF(60,250,280,40));
        painter.drawRect(QRectF(60,300,280,40));
        painter.drawRect(QRectF(60,350,280,40));
        QPointF points[3]={QPoint(10,100+50*focus),QPoint(10,150+50*focus),QPoint(43,125+50*focus)};
        painter.drawPolygon(points,3);
        QFont font1("黑体",30);
        painter.setFont(font1);
        painter.drawText(100,50,"Qsnake");
        painter.setPen(Qt::white);
        QFont font2("黑体",20);
        painter.setFont(font2);
        painter.drawText(70,130,"经典");
        painter.drawText(70,180,"双人");
        painter.drawText(70,230,"自动");
        painter.drawText(70,280,"特殊食物");
        painter.drawText(70,330,"地形");
        painter.drawText(70,380,"帮助");
        break;
    }
    case help:{

        QFont font3("黑体",10);
        painter.setFont(font3);
        painter.drawText(50,50,"单人");
        painter.drawText(50,70,"用↑↓←→键改变方向");
        painter.drawText(50,90,"双人");
        painter.drawText(50,110,"玩家1用↑↓←→键改变方向");
        painter.drawText(50,130,"玩家2用wasd键改变方向");
        painter.drawText(50,150,"特殊食物");
        painter.drawText(50,170,"绿色食物有加速buff");
        painter.drawText(50,190,"黄色食物有减速buff");

        painter.drawText(50,300,"按esc回到上一界面");
        painter.drawText(50,350,"游戏中按esc来暂停或继续");
        break;
    }
    case single:
    case ai:
    {
        painter.setPen(snake[0].color);
        painter.setBrush(snake[0].color);
        for (int i=0;i<snake[0].snake.length();++i)
            painter.drawRect(snake[0].snake[i]);

        painter.setPen(Qt::red);
        painter.setBrush(Qt::red);
        painter.drawRect(food);
        break;
    }
    case multi:
    {
        painter.setPen(snake[0].color);
        painter.setBrush(snake[0].color);
        for (int i=0;i<snake[0].snake.length();++i)
            painter.drawRect(snake[0].snake[i]);
        painter.setPen(snake[1].color);
        painter.setBrush(snake[1].color);
        for (int i=0;i<snake[1].snake.length();++i)
            painter.drawRect(snake[1].snake[i]);

        painter.setPen(Qt::red);
        painter.setBrush(Qt::red);
        painter.drawRect(food);
        break;
    }
    case singleend:
    {
        delete snake;
        snake=nullptr;
        for (int i=0;i<width()/10;++i)
            for (int j=0;j<height()/10;++j)
            {
                pos[i][j].accessable=true;
                pos[i][j].used=false;
            }
        walls.clear();
        foodtype=normal;
        speed=cspeed;
        QFont font("黑体",10);
        painter.setFont(font);
        painter.drawText(50,50,"game over,press Enter to restart");
        break;
    }
    case multiend:
    {
        delete []snake;
        snake=nullptr;
        walls.clear();
        QFont font("黑体",10);
        painter.setFont(font);
        painter.drawText(100,50,"game over");
        if (winner==1)
            painter.drawText(80,100,"the winner is Player 1");
        else painter.drawText(80,100,"the winner is Player 2");
        break;
    }
    case wall:
        painter.setPen(Qt::black);
        painter.setBrush(Qt::black);
        for (int i=0;i<walls.length();++i)
            painter.drawRect(walls[i]);
        painter.setPen(snake[0].color);
        painter.setBrush(snake[0].color);
        for (int i=0;i<snake[0].snake.length();++i)
            painter.drawRect(snake[0].snake[i]);

        painter.setPen(Qt::red);
        painter.setBrush(Qt::red);
        painter.drawRect(food);
        break;
    case morefood:
    {
        painter.setPen(snake[0].color);
        painter.setBrush(snake[0].color);
        for (int i=0;i<snake[0].snake.length();++i)
            painter.drawRect(snake[0].snake[i]);
        switch (foodtype){
        case normal:
            painter.setPen(Qt::red);
            painter.setBrush(Qt::red);
            painter.drawRect(food);
            break;
        case fast:
            painter.setPen(Qt::green);
            painter.setBrush(Qt::green);
            painter.drawRect(food);
            break;
        case slow:
            painter.setPen(Qt::yellow);
            painter.setBrush(Qt::yellow);
            painter.drawRect(food);
            break;
        }
        break;
    }

    }

}


int Widget::collision()
{
    switch (gamemode) {
    case single:
    case ai:
    case morefood:
        for (int i=4;i<snake[0].snake.length();++i)
            if (snake[0].snake[i]==snake[0].snake[0])
                return 1;
        if (snake[0].snake[0].x()<0||snake[0].snake[0].x()>=width()||snake[0].snake[0].y()<0||snake[0].snake[0].y()>=height())
            return 1;

        break;
    case multi:

        for (int j=0;j<2;++j)
        {
            for (int i=4;i<snake[j].snake.length();++i)
                if (snake[j].snake[i]==snake[j].snake[0])
                    return j+1;
            if (snake[j].snake[0].x()<0||snake[j].snake[0].x()>=width()||snake[j].snake[0].y()<0||snake[j].snake[0].y()>=height())
                return j+1;
        }
        for (int i=0;i<snake[0].snake.length();++i)
            if (snake[0].snake[i]==snake[1].snake[0])
            {
                if (i==0)
                    return 3;
                else return 2;
            }
        for (int i=0;i<snake[1].snake.length();++i)
            if (snake[1].snake[i]==snake[0].snake[0])
                return 1;
        break;
    case wall:
        for (int i=4;i<snake[0].snake.length();++i)
            if (snake[0].snake[i]==snake[0].snake[0])
                return 1;
        if (snake[0].snake[0].x()<0||snake[0].snake[0].x()>=width()||snake[0].snake[0].y()<0||snake[0].snake[0].y()>=height())
            return 1;
        for (int i=0;i<walls.length();++i)
            if (walls[i].contains(snake->snake[0]))
                return 1;

        break;
    }
    return 0;
}


void Widget::search(){

    quex.push_front((int)(food.center().x()/10));
    quey.push_front((int)(food.center().y()/10));
    bool foodfindtail=false,findfood=false;

    while ((!foodfindtail||!findfood)&&!quex.empty())
    {
        int a[4]={0,1,2,3};
        std::random_shuffle(a,a+4);
        int dx[4]={-1,0,1,0};
        int dy[4]={0,-1,0,1};
        for (int i=0;i<4;++i)
        {
            if (quex[0]+dx[a[i]]>=0&&quex[0]+dx[a[i]]<width()/10&&quey[0]+dy[a[i]]>=0&&quey[0]+dy[a[i]]<height()/10)
            {
                if (snake->snake[0].contains(QPoint((quex[0]+dx[a[i]])*10+5,(quey[0]+dy[a[i]])*10+5))&&!pos[quex[0]+dx[a[i]]][quey[0]+dy[a[i]]].used)
                {
                    findfood=true;
                    pos[quex[0]+dx[a[i]]][quey[0]+dy[a[i]]].used=true;
                    pos[quex[0]+dx[a[i]]][quey[0]+dy[a[i]]].last=a[i];
                }
                if (snake->snake.last().contains(QPoint((quex[0]+dx[a[i]])*10+5,(quey[0]+dy[a[i]])*10+5)))
                {
                    foodfindtail=true;
                    pos[quex[0]+dx[a[i]]][quey[0]+dy[a[i]]].used=true;
                    pos[quex[0]+dx[a[i]]][quey[0]+dy[a[i]]].last=a[i];
                }
                else if (pos[quex[0]+dx[a[i]]][quey[0]+dy[a[i]]].accessable&&!pos[quex[0]+dx[a[i]]][quey[0]+dy[a[i]]].used)
                {
                    pos[quex[0]+dx[a[i]]][quey[0]+dy[a[i]]].used=true;
                    pos[quex[0]+dx[a[i]]][quey[0]+dy[a[i]]].last=a[i];
                    quex.push_back(quex[0]+dx[a[i]]);
                    quey.push_back(quey[0]+dy[a[i]]);
                }
            }
        }
        quex.pop_front();quey.pop_front();


    }
    quex.clear();
    quey.clear();
    for (int i=0;i<width()/10;++i)
        for (int j=0;j<height()/10;++j)
            pos[i][j].used=0;
    if (findfood&&foodfindtail)
    {
        std::cout<<"case1"<<std::endl;

        switch (pos[(int)(snake->snake[0].center().x()/10)][(int)(snake->snake[0].center().y()/10)].last)
        {
        case 0:snake->movedirect=snake->right;break;
        case 1:snake->movedirect=snake->down;break;
        case 2:snake->movedirect=snake->left;break;
        case 3:snake->movedirect=snake->up;break;
        }
    }

    else {
        bool headfindtail=false;
        quex.push_front((int)(snake->snake.last().center().x()/10));
        quey.push_front((int)(snake->snake.last().center().y()/10));
        while (!headfindtail&&!quex.empty())
        {
            int dx[4]={-1,0,1,0};
            int dy[4]={0,-1,0,1};
            for (int i=0;i<4;++i)
            {
                if (quex[0]+dx[i]>=0&&quex[0]+dx[i]<width()/10&&quey[0]+dy[i]>=0&&quey[0]+dy[i]<height()/10)
                {

                    if (snake->snake[0].contains(QPoint((quex[0]+dx[i])*10+5,(quey[0]+dy[i])*10+5))&&!snake->snake.last().contains(QPoint((quex[0])*10+5,(quey[0])*10+5))&&!pos[quex[0]+dx[i]][quey[0]+dy[i]].used)
                    {
                        headfindtail=true;
                        pos[quex[0]+dx[i]][quey[0]+dy[i]].used=true;
                        pos[quex[0]+dx[i]][quey[0]+dy[i]].last=i;
                    }
                    else if (pos[quex[0]+dx[i]][quey[0]+dy[i]].accessable&&!pos[quex[0]+dx[i]][quey[0]+dy[i]].used)
                    {
                        pos[quex[0]+dx[i]][quey[0]+dy[i]].used=true;
                        pos[quex[0]+dx[i]][quey[0]+dy[i]].last=i;
                        quex.push_back(quex[0]+dx[i]);
                        quey.push_back(quey[0]+dy[i]);
                    }
                }
            }
            quex.pop_front();quey.pop_front();
        }
        quex.clear();
        quey.clear();
        for (int i=0;i<width()/10;++i)
            for (int j=0;j<height()/10;++j)
                pos[i][j].used=0;

        if (headfindtail)
        {
            std::cout<<"case2"<<std::endl;
            switch (pos[(int)(snake->snake[0].center().x()/10)][(int)(snake->snake[0].center().y()/10)].last)
            {
            case 0:snake->movedirect=snake->right;break;
            case 1:snake->movedirect=snake->down;break;
            case 2:snake->movedirect=snake->left;break;
            case 3:snake->movedirect=snake->up;break;
            }
        }

        else {
            std::cout<<"case3"<<std::endl;
            dfs(snake->snake[0].center().x()/10,snake->snake[0].center().y()/10);
            for (int i=0;i<width()/10;++i)
                for (int j=0;j<height()/10;++j)
                    pos[i][j].used=0;
            switch (pos[(int)(snake->snake[0].center().x()/10)][(int)(snake->snake[0].center().y()/10)].last)
            {
            case 0:snake->movedirect=snake->down;break;
            case 1:snake->movedirect=snake->up;break;
            case 2:snake->movedirect=snake->left;break;
            case 3:snake->movedirect=snake->right;break;
            }
        }
    }

}
int Widget::dfs(int i,int j){
    if (pos[i][j].used||!pos[i][j].accessable)
        return 0;
    int up=0,down=0,left=0,right=0;
    pos[i][j].used=true;
    if (j>0)
        up=dfs(i,j-1);
    if (i>0)
        left=dfs(i-1,j);
    if (j<height()/10)
        down=dfs(i,j+1);
    if (i<width()/10)
        right=dfs(i+1,j);
    if (up>=left&&up>=right&&up>=down)
    {
        pos[i][j].last=1;
        return up+1;
    }
    else if (left>=up&&left>=right&&left>=down)
    {
        pos[i][j].last=2;
        return left+1;
    }
    else if (right>=up&&right>=left&&right>=down)
    {
        pos[i][j].last=3;
        return right+1;
    }
    else {
        pos[i][j].last=0;
        return down+1;
    }
}





