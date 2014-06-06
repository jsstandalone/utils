#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simple_ptl.h"

typedef struct mctx_thr_para_s
{
	int idx;
	mctx_t* main_ctx;
	mctx_t* this_ctx;
}mctx_thr_para_t;

void mctx_func(void *arg)
{
	mctx_thr_para_t *pa = (mctx_thr_para_t *)arg;
	printf("idx=%d.\n", pa->idx);
	mctx_switch(pa->this_ctx, pa->main_ctx);				/*切换上下文, 包括栈. 如果是类似堆实现的线程栈, 只需要恢复寄存器*/
}

int main(int argc, char **argv)
{
    if (argc == 1 || (argc == 2 && !strcmp(argv[1], "-h")))
	{
		printf("%s <thr_num>\n", argv[0]);
		return 0;
	}
	
	int thr_num = atoi(argv[1]);
	mctx_t* p_mctx_arr = (mctx_t *)malloc(thr_num * sizeof(mctx_t));
	if (!p_mctx_arr)
	{
		fprintf(stderr, "malloc p_mctx_arr OOM!\n");
		return -1;
	}
	
	size_t sk_size = 8*1024;
	void* p_mctx_stack = malloc(thr_num *sk_size);
	if (!p_mctx_stack)
	{
		fprintf(stderr, "malloc thread stack OOM!.\n");
		free(p_mctx_arr);
		return -2;
	}

	mctx_thr_para_t* pp =  (mctx_thr_para_t *)malloc(thr_num * sizeof(mctx_thr_para_t));
	if (!pp)
	{
		fprintf(stderr, "malloc thread args OOM!.\n");
		free(p_mctx_stack);
		free(p_mctx_arr);
		return -2;
	}

	mctx_t main_ctx;	
	for (int i = 0; i < thr_num; ++i)
	{
		pp[i].idx = i;
		pp[i].main_ctx = &main_ctx;
		pp[i].this_ctx = p_mctx_arr + i;
		mctx_create(p_mctx_arr + i, mctx_func, (void *)(pp + i),
				(char *)p_mctx_stack + i*sk_size, sk_size);
	}

	for (int i = 0; i < thr_num; ++i)
	{
		mctx_switch(&main_ctx, p_mctx_arr + i);
	}
	free(pp);
	free(p_mctx_stack);
	free(p_mctx_arr);

	return 0;
}

