#include "PageTable.h"

int PageTable::FindVisualPage(int visual_page)
{
    for(int i=1;i<=page_count;i++)
    {
        if(page_table[i]==visual_page)
            return i;//找到了，返回位置
    }

    return -1;//没找到
}

int PageTable::Displace(int visual_page, int physical_page)
{
    int temp=page_table[physical_page];
    page_table[physical_page]=visual_page;
    return temp;//返回被挤出来的虚拟页号
}
