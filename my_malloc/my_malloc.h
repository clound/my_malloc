#include<unistd.h>
#include<stdio.h>
//has_initialized:表示内存分配器是否已经初始化了，“0”表示未初始化，“1”表示已初始化
int has_initialized=0;
//mangaged_memory_start：堆内存的起始地址
void *mangaged_memory_start;
//last_valid_address：当前已经被映射的堆内存的最后一个有效地址
void *last_valid_address;

//初始化内存分配器
void malloc_init(){
	//而sbrk将break从当前位置移动increment所指定的增量，sbrk(0)则获得当前break的地址
	last_valid_address=sbrk(0);
	//初始化时，未映射内存，起始地址和被映射的堆内存的最后一个有效地址相同
	mangaged_memory_start=last_valid_address;
	//has_initialized=1,则说明已初始化
	has_initialized=1;
}

//定义内存控制块
struct mem_control_block{
        //表示某内存块是否空闲，“0”表示不空闲，“1”表示空闲
	int is_available;
        //某内存块的大小
	int size;
};

//内存释放函数实现
void myfree(void *firstbyte){

	printf("using myfree!\n");

	struct mem_control_block *temp;
	//获取firstbyte对应的内存控制块的地址
	temp=(struct mem_control_block *)(firstbyte - sizeof(struct mem_control_block));
        //将该内存块置为“空闲”	
	temp->is_available=1;
}

//内存分配函数实现
void *mymalloc(size_t num){

	printf("using mymalloc!\n");

	//判断是否初始化，否则调用malloc_init()函数进行初始化
	if(has_initialized == 0)
		malloc_init();
	//num + sizeof(struct mem_control_block)：才为内存块中对应的size大小
	size_t numbytes = num + sizeof(struct mem_control_block);
	void *ret = NULL;
	struct mem_control_block *tmp;
	//堆内存的起始地址,赋给变量p			
	void *p = mangaged_memory_start;
	//遍历[managed_memory_start, last_valid_address]中的每个内存块		
	while(p != last_valid_address){
		tmp = (struct mem_control_block *)p;
		//判断当前内存块是否满足要求
		if(tmp->is_available == 1 && tmp->size >= numbytes){
			//满足要求，将空闲状态设为非空闲，返回满足要求的空闲块的地址，退出遍历过程
			tmp->is_available == 0;
			ret = p;
			break;
		}
		//不满足要求，继续遍历
		p = p + tmp->size;
	}
	//一个满足要求的块都没找到，需要重新分配新的空间，返回新分配的空间的首地址
	if(ret == NULL){
		//调用sbrk函数分配新的内存块
		sbrk(numbytes);
		tmp = (struct mem_control_block *)last_valid_address;
		//设置新的内存块的内存控制块（is_available, size）
		tmp->is_available = 0;
		tmp->size = numbytes;
		//从last_valid_address分配新的内存块，赋给ret,记录新的内存块首地址
		ret = last_valid_address;
		//调整全局变量last_valid_address
		last_valid_address+=numbytes;
	}
	//返回新分配的内存块的地址
	return ret+sizeof(struct mem_control_block);
}
