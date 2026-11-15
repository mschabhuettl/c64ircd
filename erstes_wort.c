#include <stdio.h>

int main(void)
{
    char line[] = "JOIN #40c3";
    int n = 0;
    while (line[n] != ' ' && line[n] != '\0')
    {
        putchar(line[n]);
        n = n + 1;
    }
    putchar('\n');
    return 0;
}
