#include <stdio.h>

int main(void)
{
    int i;
    int a[10][100];
    printf("<program started!!>\n");
    printf("input 5 numbers and enter for five times:\n");
    for(i=0; i<5; i++)
    {
        scanf("%d %d %d %d %d", &a[i][0],&a[i][1], &a[i][2],&a[i][3],&a[i][4]);
    }

    printf("this is the result of your inputs\n");

    for(i=0; i<5; i++)
    {
        printf("%d %d %d %d %d\n", a[i][0],a[i][1],a[i][2],a[i][3],a[i][4]);
    }

    return 0;
}
