#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

int main()
{
    int filedesc = open("sample.txt", O_WRONLY | O_APPEND);
    printf("file descriptor: %d \n", filedesc);

    int filedesc2 = open("sample2.txt", O_WRONLY | O_APPEND);
    printf("file descriptor: %d \n", filedesc2);
    if (filedesc < 0)
        return 1;

    return 0;
}