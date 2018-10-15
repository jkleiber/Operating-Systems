#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    /*
    int  i;

    for  (i=0;  i<4;  i++)
    {
        fork();
    }

    printf("Hello\n");
    fflush(stdout);*/

    pid_t pid1, pid2;
    int k = 5;
    int i;

    printf("%d\n",k);
    pid1  =  fork();

    printf("A\n");

    if  (pid1  ==  0){

        for  (i  =  0;  i<  2;  i++){

            printf("B\n");
            k  *=  3;
            printf("%d\n",k);
            pid2  =  fork();

            if  (pid2  ==  0)  {

                printf("C\n");

                k  /=  2;

                //break;

            }

        }

    }
    return 0;
}