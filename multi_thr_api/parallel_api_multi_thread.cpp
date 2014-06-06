#include "parallel_api_multi_thread.h"
#include <errno.h>
#include <string.h>
#include <signal.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  cond  = PTHREAD_COND_INITIALIZER;
pthread_mutex_t sync_mutex = PTHREAD_MUTEX_INITIALIZER;

CParrelApiMulThr::CParrelApiMulThr() : m_thr_num(DEFAULT_THR_NUM),
                                        m_active_thr_num(0)
{
    int ret;
    for (int i = 0; i < m_thr_num; ++i)
    {
        //m_td_ars[i] = {this, i};
        m_td_ars[i].pRealCli = this;
        m_td_ars[i].index = i;
        ret = pthread_create(m_td_id + i,
                             NULL,
                             ThreadRoutine,
                             (void *)&m_td_ars[i]);
        if (ret)
        {
            fprintf(stderr, "pthread_create error, errno=%d, msg%s.\n",
                                                errno, strerror(errno));
            return ;
        }
    }

    m_ta_arr.init(MAX_QUEUE_SIZE);
    ret = sem_init(&m_switch_sem, 0, 0);
    if (ret)
    {
        fprintf(stderr, "sem_init error, ret=%d, msg=%s.\n", ret, strerror(errno));
        return ;
    }
    pthread_mutex_lock(&mutex);
    pthread_mutex_lock(&sync_mutex);
}

CParrelApiMulThr::CParrelApiMulThr(int thr_num) : m_thr_num(thr_num),
                                                    m_active_thr_num(0)
{
    if (m_thr_num > MAX_THREAD_NUM)
    {
        fprintf(stderr, "Too many threads, return.\n");
        return ;
    }

    int ret;
    for (int i = 0; i < m_thr_num; ++i)
    {
        m_td_ars[i].pRealCli = this;
        m_td_ars[i].index = i;
        ret = pthread_create(m_td_id + i,
                             NULL,
                             ThreadRoutine,
                             (void *)&m_td_ars[i]);
        if (ret)
        {
            fprintf(stderr, "pthread_create error, errno=%d, msg%s.\n",
                                                errno, strerror(errno));
            return ;
        }
    }

    m_ta_arr.init(MAX_QUEUE_SIZE);
    ret = sem_init(&m_switch_sem, 0, 0);
    if (ret)
    {
        fprintf(stderr, "sem_init error, ret=%d, msg=%s.\n", ret, strerror(errno));
        return ;
    }
    pthread_mutex_lock(&mutex);
    pthread_mutex_lock(&sync_mutex);
}

CParrelApiMulThr::~CParrelApiMulThr()
{
    for (int i = 0; i < m_thr_num; ++i)
    {
        pthread_kill(m_td_id[i], SIGKILL);
    }
    sem_destroy(&m_switch_sem);
}

void* CParrelApiMulThr::ThreadRoutine(void *pta)
{
    printf("%p.\n", pta);
    pthr_ar_t pt = (pthr_ar_t)pta;
    printf("%p, %d\n", pt->pRealCli, pt->index);

    CParrelApiMulThr* pth = pt->pRealCli;
    while (1)
    {
        if (sem_wait(&(pth->m_switch_sem)))
        {
            fprintf(stderr, "sem_wait error, msg=%s.\n", strerror(errno));
            continue ;
        }

        ///从队列中取出数据
        work_chunk_t tmp_chunk;
        if (!(pth->m_ta_arr.pop(tmp_chunk)))
        {
            fprintf(stderr, "queue pop error.\n");
            continue;
        }

        CParallelCli *pcli = tmp_chunk.realCli;
        int ret = pcli->ProcessNetWork(tmp_chunk.ReqData, tmp_chunk.RspData,
                                       *(tmp_chunk.qosReq), tmp_chunk.outTime);
        if (ret)
        {
            fprintf(stderr, "ProcessNetWork error, ret=%d.\n", ret);
            continue;
        }

        if (__sync_add_and_fetch(tmp_chunk.fin_size, 1) == tmp_chunk.fin_stat_num)
        {
            pthread_mutex_lock(&sync_mutex);
            ret = pthread_cond_signal(&cond);
            if (ret)
            {
                fprintf(stderr, "pthread_cond_signal error, ret=%d, msg=%s.\n", ret,
                                                                    strerror(errno));
                continue;
            }
        }
    }

    return (void *)0;
}

int CParrelApiMulThr::Run(pmp_req_t reqs[], int in_nr)
{
    int ret;
    int fin_stat = 0;

    for (int i = 0; i < in_nr; ++i)
    {
        work_chunk_t one_chunk;
        one_chunk.realCli = reqs[i]->real_cli;
        one_chunk.ReqData = reqs[i]->req_data;
        one_chunk.RspData = reqs[i]->rsp_data;
        one_chunk.outTime = reqs[i]->out_time;
        one_chunk.qosReq = reqs[i]->qos_req;
        one_chunk.fin_stat_num = in_nr;
        one_chunk.fin_size = &fin_stat;

        if (!m_ta_arr.push(one_chunk))
        {
            fprintf(stderr, "queue push error.\n");
            continue ;
        }

        if (sem_post(&m_switch_sem))
        {
            fprintf(stderr, "sem_post error, msg=%s.\n", strerror(errno));
            continue;
        }
    }

    pthread_mutex_unlock(&sync_mutex);
    ret = pthread_cond_wait(&cond, &mutex);
    if (ret)
    {
        fprintf(stderr, "pthread_cond_wait error, ret=%d, msg=%s.\n", ret,
                                                            strerror(errno));
        return ret;
    }

    return 0;
}
