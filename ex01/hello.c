#include <stdio.h>
int main()
{
    char buf[128];
    printf("请输入内容：");
    scanf("%s",buf);
    printf("你输入的内容：%s\n",buf);
    return 0;
}
