#include "vehicle_toolbutton.h"
#include <qmath.h>
#include <QDebug>
#define PI 3.1415926


VehicleToolButton::VehicleToolButton(QWidget *parent) : QToolButton(parent){
    //设置按钮大小
    this->setFixedSize(50, 50);
    //设置图标大小
    this->setIconSize(QSize(50,50));

    this->setAutoRaise(true);//设置按钮自动凸起
    this->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    QPixmap icoPix(":/Image/train.png");
    //QPixmap icoPix = QPixmap::fromImage(l_image);
    icoPix.scaled(QSize(50,50), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    m_image = icoPix.toImage();
    m_pixSize = QSize(50,50);
    this->setIcon(QIcon(icoPix));
}

VehicleToolButton::VehicleToolButton(QString imagePath, QSize size, QWidget *parent)
:QToolButton(parent),  m_pixSize(size)
{

    m_image.load(imagePath);

    //设置按钮大小
    this->setFixedSize(size.width(), size.height());
    //设置图标大小
    this->setIconSize(size);

    this->setAutoRaise(true);//设置按钮自动凸起
    this->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    QPixmap icoPix = QPixmap::fromImage(m_image);
    icoPix.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    m_image = icoPix.toImage();

    this->setIcon(QIcon(icoPix));

}

VehicleToolButton::~VehicleToolButton()
{

}

void VehicleToolButton::updatedisplay()
{
    int x=0, y=0;
    getCurrentPos(m_refreshTime*0.001, x, y);//得到现在的坐标

    this->move(x, y);//移动
    if(m_curposindex == m_pointVector.size() -1){//只剩最后一个坐标时，停止计时器（停车）
        //emit stopVehicleMove();
        m_timer.stop();
    }
}

void VehicleToolButton::startTimer(int _msec)
{
    m_refreshTime = _msec;
    //关联计时器
    //connect(&m_timer, &QTimer::timeout, this, &VehicleToolButton::updatedisplay);

    //关联计时器
    connect(&m_timer, &QTimer::timeout, this, &VehicleToolButton::updatedisplay, Qt::UniqueConnection);
    m_timer.start(_msec);
}

qreal VehicleToolButton::getSpeed()
{
    return m_speed;
}
//根据车速和运动轨迹计算time时间之后位置，timer事件调用move()函数移动到该位置
void VehicleToolButton::getCurrentPos(qreal time, int &x, int &y)
{

    if(m_linedistance <=0 )//所有点之间的距离为0
        return;

    //time  = 刷新时间（单位为s）
    //time * m_speed：得到图形中点的距离长度
    //m_linedistance/m_distance：图形中的距离和设置距离的比列（图形中的距离1像素对应设置的实际距离？米）
    qreal move_line = (time * m_speed)*m_linedistance/m_distance;

    if(m_curposindex == m_pointVector.size()-1)//当前点在终点
    {

    }
    else
    {
        if(m_linedistanceVector[m_curposindex] <= m_curlinedistance + move_line)//超越该点
        {

            //下一个点移动的距离 - 上一点移动的还未移动完的距离：为了使驾驶的时间正确
            m_curlinedistance = m_curlinedistance + move_line - m_linedistanceVector[m_curposindex];
            m_curposindex++;
            if(m_curposindex == m_pointVector.size() - 1)//当前点在终点
            {
                m_curlinedistance = 0;
                m_curposx = m_pointVector.back().x();
                m_curposy = m_pointVector.back().y();
            }
            else
            {
                QPoint pt = getPoswithLinedistance(m_curlinedistance, m_pointVector[m_curposindex], m_pointVector[m_curposindex+1]);
                m_curposx = pt.x();
                m_curposy = pt.y();
                //旋转图标位置
                setImageRote(m_pointVector[m_curposindex].x(), m_pointVector[m_curposindex].y(), m_pointVector[m_curposindex+1].x(), m_pointVector[m_curposindex+1].y());
            }
        }
        else
        {
            m_curlinedistance += move_line;
            QPoint pt = getPoswithLinedistance(m_curlinedistance, m_pointVector[m_curposindex], m_pointVector[m_curposindex + 1]);
            m_curposx = pt.x();
            m_curposy = pt.y();
        }
        m_curlinetotledistance += move_line;
    }
    x = m_curposx;
    y = m_curposy;



}

void VehicleToolButton::setCurCoordinate(int x, int y)
{
    m_curposx = x;
    m_curposy = y;
}

void VehicleToolButton::setCurrentPosition(int x, int y)
{
    this->move(x,y);
}

void VehicleToolButton::setSpeed(qreal _speed)
{
    m_speed = _speed;
}
//设置行进参数
void VehicleToolButton::setData(QVector<QPoint> _v_point, qreal _distance, qreal _speed)
{
    m_pointVector.clear();
    m_linedistanceVector.clear();
    m_pointVector = _v_point;
    m_distance = _distance;
    m_speed = _speed;
    m_linedistance = 0;//距离总长度
    m_curposindex = -1;
    m_curposx = -1;
    m_curposy = -1;
    m_curlinetotledistance = 0;

    //计算两点间的距离长度
    for(int i = 1;i< m_pointVector.size();i++)
    {
        m_linedistanceVector.push_back(qSqrt((m_pointVector[i].x() - m_pointVector[i-1].x())*(m_pointVector[i].x() - m_pointVector[i-1].x())+
                                       (m_pointVector[i].y() - m_pointVector[i-1].y())*(m_pointVector[i].y() - m_pointVector[i-1].y())));

        m_linedistance += m_linedistanceVector.back();
    }

    //位置坐标大于2个，设置最开始的坐标为起始位置，并旋转图片
    if(m_pointVector.size() > 0 )
    {
        m_curposindex = 0;
        m_curlinedistance = 0;
        m_curposx = m_pointVector[0].x();
        m_curposy = m_pointVector[0].y();
        if(m_pointVector.size() >1)
            setImageRote(m_pointVector[0].x(), m_pointVector[0].y(), m_pointVector[1].x(), m_pointVector[1].y());
    }
}

void VehicleToolButton::setRefreshTime(int refreshTime)
{
    m_refreshTime = refreshTime;
}

void VehicleToolButton::setImage(QImage image)
{
    m_image = image;
}

void VehicleToolButton::setSize(QSize size)
{
    m_pixSize = size;
}
//计算从起点到终点方向距离distance的坐标点
QPoint VehicleToolButton::getPoswithLinedistance(qreal distance, QPoint start, QPoint end)
{
    QPoint p;
    qreal t = qSqrt((end.x() - start.x())*(end.x() - start.x()) + (end.y() - start.y())*(end.y() - start.y()));
    p.setX((end.x() - start.x())*distance/t + start.x());
    p.setY((end.y() - start.y())*distance/t + start.y());
    return p;
}
//按水平轴或者垂直线作镜像翻转，bIsHorizon为true按垂直方向，false按水平轴
QImage VehicleToolButton::filp(const QImage &image, bool bIsHorizon)
{
    return image.mirrored(bIsHorizon,!bIsHorizon);
}

//根据弧度值（角度值）起点（x1,y1)和终点(x2,y2)确定图片旋转的角度
void VehicleToolButton::setImageRote(int x1, int y1, int x2, int y2)
{
    setImageRote(qreal(x2-x1),qreal(y2-y1));
}
//将图片按顺时针方向旋转一定的角度，fAngle为角度值
QImage VehicleToolButton::rotateImage(const QImage &image, qreal fAngle)
{
    // QMatrix matrix;
    // matrix.rotate(fAngle);
    //  return image.transformed(matrix, Qt::SmoothTransformation);

    QTransform transform;
    transform.rotate(fAngle);

    return image.transformed(transform, Qt::SmoothTransformation);

}
//根据弧度值（角度值）r_x,r_y确定图片旋转的角度（核心算法）
void VehicleToolButton::setImageRote(qreal r_x, qreal r_y)
{
    //坐标和理论坐标轴不同,X轴正向为水平右，Y轴正向为垂直向下
    QImage tmp;
    if(r_x == 0 &&  r_y == 0)
        return;
    if(r_x == 0 )				//Y轴
    {
        if(r_y > 0)
        {
            tmp = rotateImage(m_image,90.0);
        }
        else
        {
            tmp = rotateImage(m_image,270.0);
        }
    }
    else if(r_y == 0)			//X轴
    {
        if(r_x >0)
        {
            tmp = m_image;
        }
        else
        {
            tmp = filp(m_image,true);
        }
    }
    else if(r_x > 0 && r_y > 0) //第一象限
    {

        qreal k = qAtan(r_y/r_x)*180/PI;
        tmp = rotateImage(m_image,k);

        qDebug()<<"第一象限:"<<k;
    }
    else if(r_x < 0 && r_y > 0) //第二象限
    {

        QImage _filp = filp(m_image,true);
        qreal k = qAtan(r_y/r_x)*180/PI;

        qDebug()<<"第二象限："<<k;

        tmp = rotateImage(_filp,360.0 + k);//顺时针旋转
        //tmp = rotateImage(_filp, k);//逆时针旋转
    }else if(r_x < 0 && r_y < 0) //第三象限
    {

        QImage _filp = filp(m_image,true);
        qreal k = qAtan(r_y/r_x)*180/PI;
        tmp = rotateImage(_filp,k);

        qDebug()<<"第三象限:"<<k;
    }
    else if(r_x > 0 && r_y < 0) //第四象限
    {
        qreal k = qAtan(r_y/r_x)*180/PI;

        tmp = rotateImage(m_image,360.0+k);
        //tmp = rotateImage(m_image, k);

         qDebug()<<"第四象限:"<<k;
    }

    tmp.scaled(this->width(), this->height());

    QPixmap icoPix = QPixmap::fromImage(tmp);
    icoPix.scaled(this->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    this->setIcon(QIcon(icoPix));
}
