

#define MEMBLOCK_QUANTITY 4
#define bool int
#define true 1
#define false 0

#include <stdio.h>
#include <stdlib.h>

// ҳ����
int page_stream[] = {
		1, 2, 3, 5, 7, 3, 4, 3, 1, 5, 0, 8, 2, 1, 4,
};

typedef struct {
	int page;
	int access;
	int modify;
} Page;

// ����ҳ��
Page memory_table[MEMBLOCK_QUANTITY];
int current_block = 0;

void print_memory();
void init_memory();
void request_page(int);


int main(void) {

	init_memory();
	print_memory("��ʼ��");
	for (int i = 0, len = sizeof(page_stream)/sizeof(int); i < len; i ++) {
		request_page(page_stream[i]);
	}

	return EXIT_SUCCESS;
}



void init_memory()
{
	for (int i = 0; i < MEMBLOCK_QUANTITY; i ++) {
		Page *p = &memory_table[i];
		p->page = -1; // ��ʾ�˿����
	}

}

void print_memory(const char* title)
{
	puts(title);
	printf("\t��ǰҳ��ָ��=%d\n", current_block);
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
 * �������ڼ���ҳ���ҳ��
 * @return �Ƿ�����
 */
bool find_block(int *block, int access, int modify, int reset_access)
{

	for (int i = 0; i < MEMBLOCK_QUANTITY; i ++) {
		Page *p = &memory_table[current_block];


		if (p->access == access && p->modify == modify) {
			// �ҵ���
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
 * ����ҳ��page��block
 */
void load_page(int page, int block)
{
	Page* p = &memory_table[block];
	p->page = page;
	p->access = 1;
	// ��������޸�
	p->modify = (rand() % 100) > 50 ? 1 : 0;
}

void request_page(int page)
{
	printf("����ҳ��%d\n", page);
	print_memory("  ����ҳ��֮ǰ");

	int block = -1;
	Page *m_ptr;
	// �����ڴ���
	if (in_memory(page, &m_ptr, &block)) {
		printf("\tҳ��%d�Ѵ���, ҳ��Ϊ%d\n", page, block);
		// ����access, ��������޸�
		m_ptr->access = 1;
		m_ptr->modify = rand() % 100 > 50 ? 1 : 0;
	}
	// ���ֲ���
	// 00
	else if (find_block(&block, 0, 0, false)) {
		load_page(page, block);
		puts("  ��1��ɨ��ɹ�");
	}
	// 01
	else if (find_block(&block, 0, 1, true)) {
		load_page(page, block);
		puts("  ��2��ɨ��ɹ�");
	}
	// 00
	else if (find_block(&block, 0, 0, false)) {
		load_page(page, block);
		puts("  ��3��ɨ��ɹ�");
	}
	// 01
	else if (find_block(&block, 0, 1, false)) {
		load_page(page, block);
		puts("  ��4��ɨ��ɹ�");
	}

	print_memory("  ����ҳ��֮��");
}

