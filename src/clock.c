

#define MEMBLOCK_QUANTITY 4
#define bool int
#define true 1
#define false 0

#include <stdio.h>
#include <stdlib.h>

// 页面流
int page_stream[] = {
		1, 2, 3, 5, 7, 3, 4, 3, 1, 5, 0, 8, 2, 1, 4,
};

typedef struct {
	int page;
	int access;
	int modify;
} Page;

// 物理页块
Page memory_table[MEMBLOCK_QUANTITY];
int current_block = 0;

void print_memory();
void init_memory();
void request_page(int);


int main(void) {

	init_memory();
	print_memory("初始化");
	for (int i = 0, len = sizeof(page_stream)/sizeof(int); i < len; i ++) {
		request_page(page_stream[i]);
	}

	return EXIT_SUCCESS;
}



void init_memory()
{
	for (int i = 0; i < MEMBLOCK_QUANTITY; i ++) {
		Page *p = &memory_table[i];
		p->page = -1; // 表示此框空着
	}

}

void print_memory(const char* title)
{
	puts(title);
	printf("\t当前页面指针=%d\n", current_block);
	for (int i = 0; i < MEMBLOCK_QUANTITY; i ++) {
		Page *p = &memory_table[i];
		printf("\tblock=%d, page=%d, access=%d, modify=%d\n", i, p->page, p->access, p->modify);
	}
}


bool in_memory(int page, Page **m_ptr, int *block)
{
	for (int i = 0; i < MEMBLOCK_QUANTITY; i ++) {
		Page *p = &memory_table[i];
		if (p->page == page) {
			*m_ptr = p;
			*block = i;
			return true;
		}
	}

	return false;
}

int next_block(int n)
{
	return (n + 1) % MEMBLOCK_QUANTITY;
}

/**
 * 查找用于加载页面的页框
 * @return 是否命中
 */
bool find_block(int *block, int access, int modify, int reset_access)
{

	for (int i = 0; i < MEMBLOCK_QUANTITY; i ++) {
		Page *p = &memory_table[current_block];


		if (p->access == access && p->modify == modify) {
			// 找到块
			if (reset_access) {
				p->access = 0;
			}
			*block = current_block;
			return true;
		}
		if (reset_access) {
			p->access = 0;
		}

		current_block = next_block(current_block);
	}

	return false;
}

/**
 * 加载页面page到block
 */
void load_page(int page, int block)
{
	Page* p = &memory_table[block];
	p->page = page;
	p->access = 1;
	// 设置随机修改
	p->modify = (rand() % 100) > 50 ? 1 : 0;
}

void request_page(int page)
{
	printf("请求页面%d\n", page);
	print_memory("  请求页面之前");

	int block = -1;
	Page *m_ptr;
	// 已在内存中
	if (in_memory(page, &m_ptr, &block)) {
		printf("\t页面%d已存在, 页框为%d\n", page, block);
		// 设置access, 设置随便修改
		m_ptr->access = 1;
		m_ptr->modify = rand() % 100 > 50 ? 1 : 0;
	}
	// 四轮查找
	// 00
	else if (find_block(&block, 0, 0, false)) {
		load_page(page, block);
		puts("  第1轮扫描成功");
	}
	// 01
	else if (find_block(&block, 0, 1, true)) {
		load_page(page, block);
		puts("  第2轮扫描成功");
	}
	// 00
	else if (find_block(&block, 0, 0, false)) {
		load_page(page, block);
		puts("  第3轮扫描成功");
	}
	// 01
	else if (find_block(&block, 0, 1, false)) {
		load_page(page, block);
		puts("  第4轮扫描成功");
	}

	print_memory("  请求页面之后");
}

