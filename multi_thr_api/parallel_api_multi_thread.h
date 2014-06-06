#ifndef _PARALLE_API_MULTI_THREAD_H_
#define _PARALLE_API_MULTI_THREAD_H_

#include "circle_queue.h"
#include "parallel_client.h"
#include <semaphore.h>
#include <pthread.h>

#define DEFAULT_THR_NUM			3
#define MAX_THREAD_NUM			32
#define MAX_QUEUE_SIZE          10000

typedef int (*pnw)(void* oneReq,
                   void* oneRsp,
                   QOSREQUEST& qosReq,
                   double timeOut);

typedef struct mp_req_s
{
    CParallelCli *real_cli;
    void *req_data;
    void *rsp_data;
    QOSREQUEST* qos_req;
    double out_time;
}mp_req_t, *pmp_req_t;

class CParrelApiMulThr
{
	typedef struct work_chunk_s
	{
        CParallelCli *realCli;
		void* ReqData;
		void* RspData;
		QOSREQUEST* qosReq;
		double outTime;
		int *fin_size;
		int fin_stat_num;
	}work_chunk_t, *pwork_chunk_t;

	typedef struct thr_ar_s
	{
		CParrelApiMulThr* pRealCli;
		int index;                          //线程的index
	}thr_ar_t, *pthr_ar_t;

	public:
		CParrelApiMulThr();
		CParrelApiMulThr(int thr_num);
		~CParrelApiMulThr();

		int Run(pmp_req_t reqs[], int nr);

    public:
        static void* ThreadRoutine(void *pta);

	private:
		int m_thr_num;
		pthread_t m_td_id[MAX_THREAD_NUM];

        int m_active_thr_num;               //活动线程个数
        thr_ar_t m_td_ars[MAX_THREAD_NUM];

		sem_t m_switch_sem;
		CCircleQueue<work_chunk_t> m_ta_arr;
};

#endif
