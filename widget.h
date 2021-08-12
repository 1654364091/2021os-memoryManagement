#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtGui>
#include <QTableWidget>

#include "PageTable.h"
#include <vector>

using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

enum MODEL{increase1,front,increase2,behind};

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    Ui::Widget *ui;


    PageTable * page_table; //页表
    vector<int> past_visual_page;//存储过去被调入的页号
    int last_ins;//两个标记点
    MODEL model;   //指明挑选的方式

    double miss;  //不命中的次数
    double count; //指令数计数
    double rate;  //缺失率

    //QStandardItemModel* model_1_1,*model_1_2, * model_2;//表格模型
    QTableWidget *table_physical,*table_visual_1,*table_visual_2;//表格

public:
    int Rand();//随机挑选一个指令
    int LRU(); //LRU算法找到一个将要被占用的物理页号
    int min(int a[],int n); //返回最小元素的下标
    void Execute();  //执行一个指令


public slots:
    void Run();  //“开始执行”
    void Refresh(); //刷新控件内容
    void Fresh(); //重置
};
#endif // WIDGET_H
