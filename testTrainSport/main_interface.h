#ifndef MAIN_INTERFACE_H
#define MAIN_INTERFACE_H

#include <QMainWindow>
#include "vehicle_toolbutton.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainInterface;
}
QT_END_NAMESPACE

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
    VehicleToolButton* car;
    QVector<QPoint> m_trainMovePointVector;
     QVector<QPoint> m_carMovePointVector;
signals:
    void startTrainMove(int msec=10);
    void startCarMove(int msec=10);

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
};
#endif // MAIN_INTERFACE_H
