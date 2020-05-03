#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <stdlib.h>

int main(void)
{
    int status;
    int ret;
    pid_t pid;

    ret=syscall(397);
    printf("hello\n");
    pid=fork();

    if(pid==-1){
        printf("error!!\n");
        return -1;
    } else if (pid == 0){
        printf("hello2\n");
        execl("./goalprogram","./goalprogram", NULL);
    } else {
        wait(&status);
    }

    ret = syscall(398);
    pid = fork();

    if(pid == -1){
        printf("error!!\n");
        return -1;
    } else if (pid == 0){
        execl("./goalprogram","./goalprogram", NULL);
    } else {
        wait(&status);
        syscall(399);
    }

    return 0;
}
