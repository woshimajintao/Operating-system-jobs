#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>

#define MAX_LINE 80 /* The maximum length command */
typedef struct
{
    char buf[MAX_LINE];
    char *args[MAX_LINE / 2 + 1];
    int bg;
    int index;
}Command;
typedef struct
{
    Command* cmd[10]; 
}History;

History history;
int cmdmaxindex=0;
Command* paraseCommand(char *str)
{
    char *tmp;
    int i=0;
    Command* cmd=(Command*)malloc(sizeof(Command));
    
    memset(cmd,0,sizeof(Command));
    tmp=strstr(str,"&");
    if(tmp!=NULL)
    {
        *tmp=0;
        cmd->bg=1;
    }
    strcpy(cmd->buf,str);
    tmp=strtok(cmd->buf," ");
    while(tmp!=NULL)
    {
        cmd->args[i++]=tmp;
        tmp=strtok(NULL," ");
    }
    return cmd;
}
void  runCommand(Command* cmd)
{
    int pid = fork();
    if (pid<0)
    {
        printf("fork() error!");
        exit(1);
    }
    else if (pid == 0)
    {
        execvp(cmd->args[0], cmd->args);
        exit(0);
    }
    else
    {
        if(cmd->bg!=1)
            wait(NULL);
    }
}
void initHistory()
{
    int i;
    for(i=0;i<10;i++)
    {
        history.cmd[i]=NULL;
    }
}
void addToHistory(Command* cmd)
{
    int i;
    
    cmdmaxindex+=1;
    cmd->index=cmdmaxindex;
    if(history.cmd[9]==NULL)
    {
        for(i=0;i<10;i++)
            if(history.cmd[i]==NULL)
            {
                history.cmd[i]=cmd;
                break;
            }
    }
    else
    {
        Command* tmp=history.cmd[0];
        for(i=0;i<9;i++)
            history.cmd[i]=history.cmd[i+1];
        history.cmd[9]=cmd;
        if(tmp!=cmd)
        {
             free(tmp);
        }
    }
}
void printHistory()
{
    int i;
    printf("history:\n");
    for(i=9;i>=0;i--)
        if(history.cmd[i]!=NULL)
        {
            printf("%d  %s\n",history.cmd[i]->index,history.cmd[i]->args[0]);
        }
}
int main(void)
{
    char *args[MAX_LINE / 2 + 1]; 
    int should_run = 1; 
    char readline[MAX_LINE];
    Command* cmd;
    
    initHistory();
    while (should_run)
    { 
        printf("osh>");
        fflush(stdout);
        memset(readline,0,sizeof(readline));
	 gets(readline);
	 if(strcmp(readline,"exit")==0) 
        {
            should_run=0;
            continue;
        }
        else if(strcmp(readline,"history")==0) 
        {
            printHistory();
            continue;
        }
        else if(readline[0]=='!')
        {   
            int i,tmp;
            if(readline[1]=='!')
            {
                tmp=cmdmaxindex;
            }else
            {
                tmp=atoi(readline+1);
            }
            for(i=0;i<10;i++)
                if(history.cmd[i]!=NULL&&history.cmd[i]->index==tmp)
                {
                    break;
                }
            if(i>=10)
            {
                if(cmdmaxindex==0)
                    printf("历史记录中没有命令。\n");
                else
                    printf("历史上没有这样的命令\n");
                cmd=NULL;
            }
            else
            {
                cmd=(Command*)malloc(sizeof(Command));
                memcpy(cmd,history.cmd[i],sizeof(Command));
                for(tmp=0;tmp<MAX_LINE / 2 + 1;tmp++)
                    if(cmd->args[tmp]!=NULL)
                    {
                        cmd->args[tmp]=(history.cmd[i]->args[tmp]-history.cmd[i]->buf)+cmd->buf;
                    }
            }
        }
        else
            cmd=paraseCommand(readline);
        if(cmd!=NULL)
        {
            runCommand(cmd);
            addToHistory(cmd);
         }
    }
    return 0;
}