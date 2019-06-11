#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<pthread.h>

int sudo[9][9]=
{
    {6,2,4,5,3,9,1,8,7},
    {5,1,9,7,2,8,6,3,4},
    {8,3,7,6,1,4,2,9,5},
    {1,4,3,8,6,5,7,2,9},
    {9,5,8,2,4,7,3,6,1},
    {7,6,2,3,9,1,4,5,8},
    {3,7,1,9,5,6,8,4,2},
    {4,9,6,1,8,2,5,7,3},
    {2,8,5,4,7,3,9,1,6}
};
int status[27];
typedef struct
{
    int left;
    int top;
    int right;
    int buttom;
    int index;
}CheckRange;

static void workthread(void *param)
{
    CheckRange *cr=(CheckRange*)param;
    int i,j,k;
    for(k=1;k<=9;k++)
    {
        for(i=cr->left;i<cr->right;i++)
        {
            for(j=cr->top;j<cr->buttom;j++)
                if(k==sudo[i][j])
                    break;
            if(j<cr->buttom)
                break;
         }
         if(i>=cr->right||j>=cr->buttom)
            break;
    }
    if(k>9)
        status[cr->index]=1;
    else
        status[cr->index]=2;
}
int main(void)
{
    CheckRange *cr;
    pthread_t tid;
    int i,j,index=0;
    for(i=0;i<27;i++)
        status[i]=0;
    //check row
    for(i=0;i<9;i++)
    {
        cr=(CheckRange *)malloc(sizeof(CheckRange));
        cr->left=0;
        cr->right=9;
        cr->top=i;
        cr->buttom=i+1;
        cr->index=index++;
        pthread_create(&tid,NULL,(void*(*)(void*))workthread,cr);
    }
     //check col
    for(i=0;i<9;i++)
    {
        cr=(CheckRange *)malloc(sizeof(CheckRange));
        cr->left=i;
        cr->right=i+1;
        cr->top=0;
        cr->buttom=9;
        cr->index=index++;
        pthread_create(&tid,NULL,(void*(*)(void*))workthread,cr);
    }   
    //check 3*3
    for(i=0;i<9;i+=3)
    {
        for(j=0;j<9;j+=3)
        {
            cr=(CheckRange *)malloc(sizeof(CheckRange));
            cr->left=i;
            cr->right=i+3;
            cr->top=j;
            cr->buttom=j+3;
            cr->index=index++;
            pthread_create(&tid,NULL,(void*(*)(void*))workthread,cr);
        }
    } 
    i=0;
    while(i<27)
    {
        if(status[i]==0)
            continue;
        if(status[i]==2)
            break;
        i++;
    }
    if(i<27)
        printf("sudo is not match status[%d]=%d\n",i,status[i]);
    else
        printf("sudo is match\n");
    return 0;
}
       