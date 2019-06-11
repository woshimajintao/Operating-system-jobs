#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<pthread.h>

#define MAXNUM 10
int originallist[MAXNUM]={7,12,19,3,18,4,2,6,15,8};
int mergelist[MAXNUM];
typedef struct
{
    int start;
    int end;
}Range;
typedef struct
{
    Range r1;
    Range r2;
    pthread_t t1;
    pthread_t t2;
}MergeParam;
static void workthread(void *param)
{
    Range *r=(Range*)param;
    int i,j,t;
    for(i=r->start;i<r->end;i++)
        for(j=i+1;j<r->end;j++)
        {
            if(originallist[i]>originallist[j])
            {
                t=originallist[i];
                originallist[i]=originallist[j];
                originallist[j]=t;
            }
        }
}
static void mergethread(void *param)
{
    MergeParam *mp=(MergeParam*)param;
    int i,j,k;
    void *status;
    
    pthread_join(mp->t1,&status);
    pthread_join(mp->t2,&status);
    for(i=0,j=mp->r1.start,k=mp->r2.start;i<MAXNUM;i++)
    {
        if(j<mp->r1.end && k<mp->r2.end)
        {
            if(originallist[j]<originallist[k])
            {
                mergelist[i]=originallist[j];
                j++;
            }
            else
            {
                mergelist[i]=originallist[k];
                k++;
            }
        }
        else if(j<mp->r1.end )
        {
            mergelist[i]=originallist[j];
            j++;
        }else
        {
            mergelist[i]=originallist[k];
            k++;
        }
    }
}
int main(void)
{
    MergeParam mp;
    pthread_t tid;
    int i;
    void *status;
    
    mp.r1.start=0;
    mp.r1.end=5;
    mp.r2.start=5;
    mp.r2.end=10;
    pthread_create(&(mp.t1),NULL,(void*(*)(void*))workthread,&(mp.r1));
    pthread_create(&(mp.t2),NULL,(void*(*)(void*))workthread,&(mp.r2));
    pthread_create(&tid,NULL,(void*(*)(void*))mergethread,&mp);
    pthread_join(tid,&status);
    for(i=0;i<MAXNUM;i++)
        printf("%d ",mergelist[i]);
    printf("\n");
    return 0;
}
       