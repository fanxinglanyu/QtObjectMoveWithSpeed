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
     * @brief 设置刷新时间
     * @param refreshTime
     */
    void setRefreshTime(int refreshTime);
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
