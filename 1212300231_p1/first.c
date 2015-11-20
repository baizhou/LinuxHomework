#include<stdio.h>
#include<stdlib.h>
#include<errno.h>

char cmd[256];

void main(){
int ret;

printf("Hello world,this is Linux!");

while(1){
printf(">");
fgets(cmd,256,stdin);
cmd[strlen(cmd)]=0;

if(fork()==0){
execlp(cmd,NULL);
perror(cmd);
exit(errno);
}else{
wait(&ret);
printf("child process return %d \n",ret);
}
}
}
