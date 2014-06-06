#include <signal.h>
#include <setjmp.h>

/* machine context data structure */
typedef struct mctx_st
{
	jmp_buf jb;
}mctx_t;

/* save machine context */
#define mctx_save(mctx) \
			setjmp((mctx)->jb)

/* restore machine context */
#define mctx_restore(mctx) \
			longjmp((mctx)->jb, 1)

/* switch machine context */
#define mctx_switch(mctx_old, mctx_new) \
			if(setjmp((mctx_old)->jb) == 0) \
				longjmp((mctx_new)->jb,	1)

/* create machine context */
void mctx_create(mctx_t *mctx, 
				void(*sf_addr)(void *), void *sf_arg,
				void *sk_addr, size_t sk_size);
