#include <iostream>
#include <vector>//FIFO
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PAGE (256)
using namespace std;

typedef struct
{
    int status;//页有没有分配，0表示没有分配，1表示分配
    int framenumber;//这个页对应的帧号
}Page;

typedef struct
{
    int pagenumber;
    int framenumber;
}TLB;

Page pageTable[MAX_PAGE];//虚拟内存和物理内存一样大的页表、256个页表
vector<TLB>  tlb;//定义TLB
int nextframenumber=0;
char mem[65536];
int totalcnt=0;
int pagecnt=0;
int tlbcnt=0;
void virtualToPhysical(int vir)
{
    int pagenumber,offset;
    int tlbindex;
    int physicaladdr=-1;
    
    pagenumber=(vir>>8)&0xff;//获取页的数字第几页
    offset=vir&0xff;//获取也上面的偏移地址
    for(int i=0;i<tlb.size();i++)//加快内存访问，相当于缓存，页码和帧
    {
        if(tlb[i].pagenumber==pagenumber)
        {
            physicaladdr=(tlb[i].framenumber<<8)|offset;//找到物理地址
	        tlbcnt++;
            break;
       }
    }
    if(physicaladdr==-1)//没有找到物理地址时进入，TLB里面没有
    {
        if(pageTable[pagenumber].status==0)//0的时候没有分配页，去从BACKING_STORE.bin里面加载内容到分配页
        {
            FILE* fp=fopen("BACKING_STORE.bin","rb");
            fseek(fp, pagenumber*256,0);
            fread(mem+(256*nextframenumber),1,256,fp);
            fclose(fp);
            pageTable[pagenumber].status=1;//pagenumber就是分配的页
            pageTable[pagenumber].framenumber=nextframenumber++;//分配的帧号，物理内存分段，++是为了指向下一个空闲位置（没有分配的物理地址
        }
	    else
	        pagecnt++;//页表命中的次数计数
        TLB t;
        t.pagenumber=pagenumber;
        t.framenumber=pageTable[pagenumber].framenumber;
	    tlb.push_back(t);//当前访问的页表记录到TLB里面
	    if(tlb.size()>16)//控制TLB不大于16控制大小
	        tlb.erase(tlb.begin());
        physicaladdr=(pageTable[pagenumber].framenumber<<8)|offset;//从页表里面访问的时候计算物理地址
    }
    printf("Virtual address: %d Physical address: %d Value: %d\n",vir,physicaladdr,mem[physicaladdr]);//输出
}
int main(int argc,char *argv[])
{
    FILE* fp;
    char buf[20];
    
    if(argc!=2)
        return -1;
    memset(pageTable,0,sizeof(Page)*MAX_PAGE);//初始化页表
    fp=fopen(argv[1],"rb");
    if(fp==NULL)
        return -1;
    while(!feof(fp))//判断有没有读到文件末尾
    {
        memset(buf,0,sizeof(buf));
        fgets(buf,20,fp);//每次读一行
        if(strlen(buf)<=0)
            continue;
        virtualToPhysical(atoi(buf));
        totalcnt++;
    }
    fclose(fp);
    if(totalcnt>0)
        printf("page rate:%d%%,tlb rate:%d%%\n",100*pagecnt/totalcnt,100*tlbcnt/totalcnt);//统计页表和TLB命中的概率
    return 0;
}