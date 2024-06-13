#include "main_interface.h"
#include "ui_maininterface.h"
//#include "xpdcar.h"
#include <QTimer>

MainInterface::MainInterface(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainInterface)
{
    ui->setupUi(this);

    QPixmap icoPix(":/Image/routeBackimage.png");
    icoPix.scaled(ui->label->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    ui->label->setScaledContents(true);
    ui->label->setPixmap(icoPix);


    train = new VehicleToolButton(this);
    train->hide();


    train->setToolTip(QString("当前速度为：%1").arg(train->getSpeed()));

    connect(this, &MainInterface::startTrainMove, train, &VehicleToolButton::startTimer);

    // ui->label->hide();

    car = new VehicleToolButton(QString(":/Image/car.png"), QSize(50,50),this);
    //car->setText("车:112");
    car->hide();
    connect(this, &MainInterface::startCarMove, car, &VehicleToolButton::startTimer);
}

MainInterface::~MainInterface()
{
    delete ui;
}

void MainInterface::on_pushButton_clicked()
{
    train->show();

    m_trainMovePointVector.clear();
    m_trainMovePointVector.push_back(QPoint(10, 822));
    m_trainMovePointVector.push_back(QPoint(1200, 822));
    m_trainMovePointVector.push_back(QPoint(10, 822));

    //目标点，路径长度，速度
    train->setData(m_trainMovePointVector,  600.0,   60.0);



    // QVector<QPoint> v_point;



     emit startTrainMove();
}


void MainInterface::on_pushButton_2_clicked()
{
    car->show();
    m_carMovePointVector.clear();

    m_carMovePointVector.push_back(QPoint(100,100));

    m_carMovePointVector.push_back(QPoint(400,400));//对角线右下
    m_carMovePointVector.push_back(QPoint(100,100));//对角线左上

    m_carMovePointVector.push_back(QPoint(100,400));//向下开

    m_carMovePointVector.push_back(QPoint(400,100));//对角线右上
    m_carMovePointVector.push_back(QPoint(100,400));//对角线左下

    m_carMovePointVector.push_back(QPoint(400,400));//向右开
    m_carMovePointVector.push_back(QPoint(400,100));//向上开
    m_carMovePointVector.push_back(QPoint(100,100));//向左开

    //目标点，路径长度，速度
    car->setData(m_carMovePointVector,  600.0,   60.0);

    emit startCarMove();

}

