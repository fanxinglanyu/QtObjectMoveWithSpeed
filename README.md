[toc]



# 0 效果
实现后的效果如下
![在这里插入图片描述](https://img-blog.csdnimg.cn/direct/1accc206560d4bcea741e165beb5ba09.gif#pic_center)


![在这里插入图片描述](https://img-blog.csdnimg.cn/direct/b25192ba7fe1455c805a57e9d5f3a09b.gif#pic_center)
![在这里插入图片描述](https://img-blog.csdnimg.cn/direct/c3f79916d07f47a69497ee9611925a19.gif#pic_center)

可以显示属性（继承自`QToolButton`）:
![在这里插入图片描述](https://img-blog.csdnimg.cn/direct/827783def6f348f2819b8e2de81b42e5.png)
鼠标悬浮显示文字
![在这里插入图片描述](https://img-blog.csdnimg.cn/direct/b93e8f12d8b74def8b89f0ed97cea7e7.png)
按钮显示文字

# 1 原理
类继承自`QToolButton`，默认朝右行驶为正方向，设置图片为按钮的图标：

## 1.1 图片旋转



* 1，如果水平移动，朝左移动（纵坐标差值为0，横坐标差值为负【终点-起点，后面计算方式相同】），则对图片进行镜像处理`image.mirrored`；


```cpp
QImage rotateImage(const QImage &image, qreal fAngle)
{

    QTransform transform;
    transform.rotate(fAngle);

    return image.transformed(transform, Qt::SmoothTransformation);

}

	QImage tmp;
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
```

![在这里插入图片描述](https://img-blog.csdnimg.cn/direct/2fcaceddb71540b3b6946b396913d3d6.png)
朝右
![在这里插入图片描述](https://img-blog.csdnimg.cn/direct/f48e81e2e4a5460184dd157a45c58d84.png)
朝左


* 2，如果是垂直移动，朝下移动（水平坐标差值为0，纵坐标差值大于0），把图片顺时针旋转90度；朝上移动（水平坐标差值为0，纵坐标差值小于0），图片顺指针旋转270度（对图片使用镜像`image.transformed`处理）；

```cpp
QImage filp(const QImage &image, bool bIsHorizon)
{
    return image.mirrored(bIsHorizon,!bIsHorizon);
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
```

![在这里插入图片描述](https://img-blog.csdnimg.cn/direct/0c7b20fbf2ca46c19c8b4022f950d95d.png)
朝下
![在这里插入图片描述](https://img-blog.csdnimg.cn/direct/898ff777afee4976a03fa9491ac4c12d.png)
朝上

* 3，如果是倾斜移动，计算弧度（纵坐标的差和横坐标的差的比值（tan值）取反正切值（`qAtan(r_y/r_x)`）；），把弧度转为角度(`qAtan(r_y/r_x)*180/PI;`)【二、三象限的旋转角度为负数】；如果方向位于第2,3象限（因为x轴正向，为水平朝右，y轴正向，为垂直向下），则把车图片做镜像处理（`image.transformed`处理，把车头调为朝左）；

```cpp
//核心
        QImage _filp = filp(m_image,true);
        qreal k = qAtan(r_y/r_x)*180/PI;
        tmp = rotateImage(_filp,k);
```

下面为四个象限的情况：
```cpp
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
```


![在这里插入图片描述](https://img-blog.csdnimg.cn/direct/fd538b04749d472bb706cf866218453b.png)
四个象限

![在这里插入图片描述](https://img-blog.csdnimg.cn/direct/927d6c1254224ae9b7e489103455e5cc.png)
左上（位于第三象限）
![在这里插入图片描述](https://img-blog.csdnimg.cn/direct/5db3f1ece22342cf9b2abeab2ca49a9c.png)
右上（位于第四象限）


![在这里插入图片描述](https://img-blog.csdnimg.cn/direct/9fca0faa79c84e3aa8bdd4694a4ef804.png)
右下（位于第一象限）

![在这里插入图片描述](https://img-blog.csdnimg.cn/direct/2ebdb01023ea40b6adb1ab558ec0b662.png)
左下（位于第二象限）

## 1.2 物体带速度移动
> 前提：
> * 1，使用`QTimer`计时器，进行刷新，来更新物体位置；
> * 2，传入每个线段路径的起点和终点到向量（Vector）构成总路线；
> * 3， 使用`move`方法来进行物体移动；

算法：
* 1，根据总路线，依次计算出两点间的距离（两点间距离公式），累加后构成图形中的总距离`m_linedistance`；

```cpp
    for(int i = 1;i< m_pointVector.size();i++)
    {
        m_linedistanceVector.push_back(qSqrt((m_pointVector[i].x() - m_pointVector[i-1].x())*(m_pointVector[i].x() - m_pointVector[i-1].x())+
                                       (m_pointVector[i].y() - m_pointVector[i-1].y())*(m_pointVector[i].y() - m_pointVector[i-1].y())));

        m_linedistance += m_linedistanceVector.back();
    }
```

* 2，计算出图形中单次刷新的实际移动的距离（像素）：
	 +  使用图形中的总距离`m_linedistance`(单位：像素)***除以***输入的距离`m_distance`（单位：m），得到图形中1像素代表实际距离中多少米；
	 + 使用速度`m_speed`（单位:m/s）***乘以***时间`time` （单位：s，为`QTimer`的刷新时间）得到目前移动的像素；

```cpp
qreal move_line = (time * m_speed)*m_linedistance/m_distance;
```

* 3，计算出坐标的变化：
	+ 计算方法：用上一步算出的实际移动的距离**除以**起点和终点两点间的距离得到移动的比列，起点的横、纵坐标分别加上比列***乘以***横、纵坐标的起终点之差，得到下一次移动后的坐标位置；
```cpp
 QPoint p;
    qreal t = qSqrt((end.x() - start.x())*(end.x() - start.x()) + (end.y() - start.y())*(end.y() - start.y()));
    p.setX((end.x() - start.x())*distance/t + start.x());
    p.setY((end.y() - start.y())*distance/t + start.y());
```
![在这里插入图片描述](https://img-blog.csdnimg.cn/direct/7d5f9bb3fd434d518c55a347dc182636.png)

*
	+ 更换线路：如果当前点移动的距离和超过起点和终点间的距离差（也就是换到下一段线路上，且不是终点时），下一段线路的应该移动的距离***减去***当前点移动后超过的距离值（为了使得最终用时无误）为实际移动距离；

```cpp
if(m_linedistanceVector[m_curposindex] <= m_curlinedistance + move_line)//超越该点
        {
            qDebug()<<"换点的距离："<<"m_curlinedistance:"<<m_curlinedistance<<" m_linedistanceVector[m_curposindex]: "<<m_linedistanceVector[m_curposindex];
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
```
* 4，终止：如果该点为最后一个点，则车辆停止（停止定时器刷新）；

```cpp
class xx{
   //计时器
    QTimer m_timer;
};

    if(m_curposindex == m_pointVector.size() -1){//只剩最后一个坐标时，停止计时器（停车）
        m_timer.stop();
    }
```

# 2 完整实现
## 2.1 将车辆按钮封装为一个类：

```cpp
#ifndef VEHICLE_TOOLBUTTON_H
#define VEHICLE_TOOLBUTTON_H
#include <QToolButton>
#include <QTimer>

class VehicleToolButton : public QToolButton
{
    Q_OBJECT

public:
    /**
     * @brief 使用默认图片
     * @param parent
     */
    VehicleToolButton(QWidget *parent = nullptr);
    /**
     * @brief VehicleToolButton
     * @param image：按钮的图标的文件路径
     * @param size:按钮大小
     * @param parent
     */
    VehicleToolButton(QString imagePath, QSize size,QWidget *parent = nullptr);
    ~VehicleToolButton();

public:
    /**
     * @brief 得到速度
     * @return
     */
    qreal getSpeed();

    /**
     * @brief 强制设置显示位置
     * @param x
     * @param y
     */
    void setCurCoordinate(int x,int y);
    /**
     * @brief 设置现在所处位置
     * @param x
     * @param y
     */
    void setCurrentPosition(int x, int y);
    /**
     * @brief 改变速度
     * @param _speed
     */
    void setSpeed(qreal _speed);
    /**
     * @brief 设置行进参数
     * @param _v_point
     * @param _distance
     * @param _speed
     */
    void setData(QVector<QPoint> _v_point, qreal _distance, qreal _speed);
    /**
     * @brief 设置图标图片
     * @param image
     */
    void setImage(QImage image);
    /**
     * @brief 设置图标大小
     * @param size
     */
    void setSize(QSize size);

private:
    /**
     * @brief 计算从起点到终点方向距离distance的坐标点
     * @param distance
     * @param start
     * @param end
     * @return
     */
    QPoint getPoswithLinedistance(qreal distance,QPoint start,QPoint end);
    /**
     * @brief 按水平轴或者垂直线作镜像翻转，bIsHorizon为true按水平轴，false按垂直方向
     * @param image
     * @param bIsHorizon
     * @return
     */
    QImage filp(const QImage& image,bool bIsHorizon);
    /**
     * @brief 根据弧度值（角度值）起点（x1,y1)和终点(x2,y2)确定图片旋转的角度
     * @param x1
     * @param y1
     * @param x2
     * @param y2
     */
    void setImageRote(int x1,int y1,int x2,int y2);
    /**
     * @brief 将图片按顺时针方向旋转一定的角度，fAngle为角度值
     * @param image
     * @param fAngle
     * @return
     */
    QImage rotateImage(const QImage& image,qreal fAngle);
    /**
     * @brief 根据弧度值（角度值）r_x,r_y确定图片旋转的角度
     * @param r_x
     * @param r_y
     */
    void setImageRote(qreal r_x,qreal r_y);
    /**
     * @brief //根据车速和运动轨迹计算time时间之后位置，timer事件调用move()函数移动到该位置，
     * @param time
     * @param x
     * @param y
     */
    void getCurrentPos(qreal time,int& x,int& y);

public slots:
    /**
     * @brief 刷新图片
     */
    void updatedisplay();
    /**
     * @brief 开始定时器
     * @param _msec
     */
    void startTimer(int _msec);

private:
    QImage m_image;//按钮图标
    QSize m_pixSize;//按钮大小

    //车辆行进数据结构
    QVector<QPoint> m_pointVector;												//行驶路径点集合（图上位置）
    QVector<qreal> m_linedistanceVector;											//行驶路径段在图上的线段长度
    qreal m_distance;															//行驶路径总长度（单位m)
    qreal m_linedistance;														//行驶路径在图上的总长度
    int	   m_curposindex;														//当前所在点的下标
    qreal  m_curlinedistance;													//当前所在线段上距离
    qreal  m_curlinetotledistance;											//当前行驶完成的路径长度总和
    int	   m_curposx;															//当前在图上的点X坐标
    int    m_curposy;															//当前在图上的点Y坐标
    qreal m_speed;															//当前车速

    //设置刷新时间(毫秒)
    int m_refreshTime = 10;
    //计时器
    QTimer m_timer;

signals:
    /**
     * @brief 停止移动
     */
    void stopVehicleMove();
};

#endif // VEHICLE_TOOLBUTTON_H
```

```cpp
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

    QPixmap icoPix(":/Image/car.png");
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

```

## 2.2 调用方法

```cpp


class MainInterface : public QMainWindow
{
    Q_OBJECT

public:
    MainInterface(QWidget *parent = nullptr);
    ~MainInterface();

private:
    Ui::MainInterface *ui;

private:
    VehicleToolButton* train;
    QVector<QPoint> m_trainMovePointVector;
signals:
    void startTrainMove(int msec=10);

private slots:
    void on_pushButton_clicked();
};
MainInterface::MainInterface(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainInterface)
{
    ui->setupUi(this);

    QPixmap icoPix(":/Image/routeBackimage.png");
    icoPix.scaled(ui->label->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    ui->label->setScaledContents(true);
    ui->label->setPixmap(icoPix);

	//构造方法1
    // train = new VehicleToolButton(this);
    //构造方法2
    train = new VehicleToolButton(QString(":/Image/car.png"), QSize(50,50),this);

    //目标点，路径长度，速度
    train->setData(m_trainMovePointVector,  500.0,   20.0);
    train->hide();
    train->setText("车:112");

    train->setToolTip(QString("当前速度为：%1").arg(train->getSpeed()));

    connect(this, &MainInterface::startTrainMove, train, &VehicleToolButton::startTimer);

    // ui->label->hide();

}

//按钮点击事件
void MainInterface::on_pushButton_clicked()
{
    // QVector<QPoint> v_point;
    train->show();
    m_trainMovePointVector.clear();

     m_trainMovePointVector.push_back(QPoint(100,100));

    m_trainMovePointVector.push_back(QPoint(400,400));//对角线右下
    m_trainMovePointVector.push_back(QPoint(100,100));//对角线左上


     m_trainMovePointVector.push_back(QPoint(100,400));//向下开

     m_trainMovePointVector.push_back(QPoint(400,100));//对角线右上
     m_trainMovePointVector.push_back(QPoint(100,400));//对角线左下


     m_trainMovePointVector.push_back(QPoint(400,400));//向右开
     m_trainMovePointVector.push_back(QPoint(400,100));//向上开
     m_trainMovePointVector.push_back(QPoint(100,100));//向左开


    // m_trainMovePointVector.clear();
    // m_trainMovePointVector.push_back(QPoint(10, 822));
    // m_trainMovePointVector.push_back(QPoint(1200, 822));
    // m_trainMovePointVector.push_back(QPoint(10, 822));

      train->setData(m_trainMovePointVector,  300.0,   20.0);


    emit startTrainMove();
}
```
# 3 完整代码
代码仓库：

https://gitee.com/fanxinglanyu/qt-object-move-with-speed         

​              

# 参考
https://blog.csdn.net/u012739657/article/details/22645375