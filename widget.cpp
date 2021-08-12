#include "widget.h"
#include "ui_widget.h"
#include <stdlib.h>

#include <QStandardItemModel>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setWindowTitle("请求分页分配————LRU算法");

    page_table=new PageTable();
    last_ins=-1;
    model=behind; //默认是随机选择

    miss=0;
    count=0;
    rate=0.0;

    //设置背景图片
    this->setAutoFillBackground(true);
    QPalette palette;
    QPixmap pixmap(":/images/images/1.jpg");
    palette.setBrush(QPalette::Window, QBrush(pixmap));
    this->setPalette(palette);

    //设置虚拟页号表1
    table_visual_1=ui->table_visual_1;
    table_visual_2=ui->table_visual_2;
    table_visual_1->setColumnCount(1);//1列
    table_visual_2->setColumnCount(1);
    table_visual_1->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//自适应所有列，让它布满空间
    table_visual_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    for(int i=0;i<16;i++)
    {
        int RowCont=ui->table_visual_1->rowCount();
        table_visual_1->insertRow(RowCont);//增加一行
        table_visual_2->insertRow(RowCont);//增加一行
        //插入元素
        table_visual_1->setItem(RowCont,0,new QTableWidgetItem(QString::number(i+1)));
        table_visual_1->item(i, 0)->setTextAlignment(Qt::AlignCenter); //内容居中
        table_visual_2->setItem(RowCont,0,new QTableWidgetItem(QString::number(i+16+1)));
        table_visual_2->item(i, 0)->setTextAlignment(Qt::AlignCenter); //内容居中
    }
    table_visual_1->verticalHeader()->hide();   //隐藏行
    table_visual_1->horizontalHeader()->hide(); //隐藏列
    table_visual_2->verticalHeader()->hide();   //隐藏行
    table_visual_2->horizontalHeader()->hide(); //隐藏列


    //设置物理页号表
    table_physical=ui->table_physical;
    table_physical->setColumnCount(2);//1列
    table_physical->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//自适应所有列，让它布满空间
    for(int i=0;i<page_table->page_count;i++)
    {
        int RowCont=ui->table_physical->rowCount();
        table_physical->insertRow(RowCont);//增加一行
        //插入元素
        table_physical->setItem(RowCont,0,new QTableWidgetItem(QString::number(i+1)));
        table_physical->setItem(RowCont,1,new QTableWidgetItem("Invalid"));
        table_physical->item(i, 0)->setTextAlignment(Qt::AlignCenter); //内容居中
        table_physical->item(i, 1)->setTextAlignment(Qt::AlignCenter);
    }
    table_physical->verticalHeader()->hide();
    table_physical->horizontalHeader()->hide();

    //按钮设置
    connect(ui->btn_start,&QPushButton::clicked,ui->btn_end,[=]{ui->btn_start->setEnabled(false);});
    connect(ui->btn_start,&QPushButton::clicked,this,&Widget::Run);
    connect(ui->btn_end,&QPushButton::clicked,this,&Widget::close);
    connect(ui->btn,&QPushButton::clicked,this,[=]{if(count<320)Execute();});
    connect(ui->btn_fresh,&QPushButton::clicked,this,&Widget::Fresh);
}

Widget::~Widget()
{
    delete ui;
}

int Widget::Rand()  //没问题
{
    if(model==increase1&&last_ins!=319)
    {
        model=MODEL((model+1)%4);
        return last_ins+1;
    }
    else if(model==front&&last_ins!=0)
    {
        last_ins=rand()%last_ins;
        model=MODEL((model+1)%4);
        return last_ins;
    }
    else if(model==increase2&&last_ins!=319)
    {
        model=MODEL((model+1)%4);
        return last_ins+1;
    }
    else if(model==behind&&last_ins!=319)
    {
        last_ins=rand()%(319-last_ins-1)+(last_ins+2);
        model=MODEL((model+1)%4);
        return last_ins;
    }
    else
    {
        last_ins=rand()%320;
        model=increase1;
        return last_ins;
    }
}

void Widget::Execute()
{
    srand((unsigned int)(time(NULL)+count));
    int ins=Rand();           //挑选一个指令等待执行
    int visual_page=ins/10+1; //求得指令所在虚拟页号

    int result=page_table->FindVisualPage(visual_page);//物理内存快的查找结果
    int replaced,temp;
    if(result==-1)//内存中不存在该虚拟页
    {
        miss++;

        replaced=LRU(); //求出可覆盖的物理页号
        temp=page_table->page_table[replaced];

        //修改虚拟页号表格
        if(-1<page_table->page_table[replaced]&&page_table->page_table[replaced]<=16)
        {

            table_visual_1->item(page_table->page_table[replaced]-1,0)->setBackground(QBrush(QColor(243, 255, 242)));//设置单元格背景颜色
        }
        else if(page_table->page_table[replaced]>16)
        {
            table_visual_2->item(page_table->page_table[replaced]-1-16,0)->setBackground(QBrush(QColor(243, 255, 242)));
        }
        if(visual_page<=16)
        {
            table_visual_1->item(visual_page-1,0)->setBackground(QBrush(QColor(0, 0, 255)));
        }
        else
        {
            table_visual_2->item(visual_page-1-16,0)->setBackground(QBrush(QColor(0, 0, 255)));
        }
        //修改物理页号表格
        page_table->page_table[replaced]=visual_page;//覆盖
        for(int i=0;i<page_table->page_count;i++)
        {
            table_physical->item(i,1)->setBackground(QBrush(QColor(212, 248, 255)));
        }
        table_physical->item(replaced-1,1)->setBackground(QBrush(QColor(255, 255, 0)));
        table_physical->item(replaced-1,1)->setText(QString::number(visual_page));
    }

    past_visual_page.push_back(visual_page); //加入到“过去”队列中
    //信息栏添加信息
    if(result==-1)
    {
        ui->listWidget->addItem("第"+QString::number(count+1)+"次执行指令"+QString::number(ins)+"：内存第"+QString::number(replaced)+"页中的虚拟"+QString::number(temp)+"号换出，虚拟页"+QString::number(visual_page)+"换入。");
    }
    else
    {
        ui->listWidget->addItem("第"+QString::number(count+1)+"次执行指令"+QString::number(ins)+"：虚拟页号"+QString::number(visual_page)+"已经存在于内存第"+QString::number(result)+"页，不发生置换。");
    }
    count++;
    rate=miss/count; 
    ui->label_rate->setText("缺失率: "+QString::number(rate*100,'f', 2)+" %");
    ui->label_details->setText("当前正在执行第"+QString::number(ins)+"条指令");
    ui->label->setText("已经执行了"+QString::number(count)+"指令");
    Refresh();
}


int Widget::LRU()   //没问题
{
    for(int i=1;i<=page_table->page_count;i++)
    {
        if(page_table->page_table[i]==-1)
            return i;
    }

    int* position=new int[page_table->page_count+1];//记录虚拟页号前溯的“位置”
    for(int i=1;i<=page_table->page_count;i++)
    {
        for(int j=past_visual_page.size()-1;j>=0;j--)
        {
            if(past_visual_page[j]==page_table->page_table[i])
            {
                position[i]=j;
                break;
            }
        }
    }

    return min(position,page_table->page_count);
}

int Widget::min(int a[],int n)//没问题
{
    int min=1;
    for(int i=2;i<=n;i++)
    {
        if(a[i]<a[min])
        {
            min=i;
        }
    }
    return min;
}

void Widget::Refresh()
{
    ui->label_rate->update();
    ui->label_details->update();
}

void Widget::Run()
{
    while(count<320)
    {
        Execute();
    }
}

void Widget::Fresh()
{
    last_ins=-1;
    model=behind; //默认是随机选择

    miss=0;
    count=0;
    rate=0.0;

    past_visual_page.clear();

    //修改虚拟块颜色
    for(int i=0;i<16;i++)
    {
        table_visual_1->item(i,0)->setBackground(QBrush(QColor(243, 255, 242)));
        table_visual_2->item(i,0)->setBackground(QBrush(QColor(243, 255, 242)));
    }

    //修改物理页块颜色和内容
    for(int i=0;i<page_table->page_count;i++)
    {
        table_physical->item(i, 1)->setText("Invalid");
        table_physical->item(i,1)->setBackground(QBrush(QColor(212, 248, 255)));
    }

    //改页表
    for(int i=0;i<=page_table->page_count;i++)
    {
        page_table->page_table[i]=-1;
    }

    //清空信息栏
    ui->listWidget->clear();

    ui->btn_start->setEnabled(true);
    //修改label
    ui->label_rate->setText("缺失率：0.0%");
    ui->label_details->setText("当前正在执行第 条指令");
    ui->label->setText("已经执行了 指令");
}

