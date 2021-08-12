#ifndef PAGETABLE_H
#define PAGETABLE_H

/*这是一个“页表”数据结构类*/
class PageTable
{
public:
    int page_count; //进程分配的页数,默认是4
    int* page_table;//页表
public:
    PageTable(int count=4)
    {
        page_count=count;
        page_table=new int[page_count+1];
        //初始化页表
        for(int i=0;i<=page_count;i++)
        {
            page_table[i]=-1; //"invalid"
        }
    }

    int FindVisualPage(int visual_page); //在物理内存块中找visual——page所在的物理页号
    int Displace(int visual_page,int physical_page);//将physical_page放入visual_page，并返回被挤出来的虚拟页号
};


#endif // PAGETABLE_H
