//测试mymalloc函数

#include <stdio.h>
#include "my_malloc.h"
int main()
{
	int *a=(int *)mymalloc(10*sizeof(int));
	a[7]=8;
	printf("a[7]=%d\n",a[7]);
	myfree(a);
	return 0;
}
