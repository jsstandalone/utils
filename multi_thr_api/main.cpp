#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "parallel_api_multi_thread.h"

using namespace std;

class client1 : public CParallelCli
{
    public:
        int ProcessNetWork(void* oneReq, void* oneRsp,
                                    QOSREQUEST& qosReq, double timeOut)
        {
            printf("client1 process network.\n");
            usleep((int)(timeOut * 1000000));
            return 0;
        }
};

class client2 : public CParallelCli
{
    public:
        int ProcessNetWork(void* oneReq, void* oneRsp,
                                    QOSREQUEST& qosReq, double timeOut)
        {
            printf("client2 process network.\n");
            usleep((int)(timeOut * 1000000));
            return 0;
        }
};

class client3 : public CParallelCli
{
    public:
        int ProcessNetWork(void* oneReq, void* oneRsp,
                                    QOSREQUEST& qosReq, double timeOut)
        {
            printf("client3 process network.\n");
            usleep((int)(timeOut * 1000000));
            return 0;
        }
};

int main()
{
    cout << "Hello world!" << endl;
    struct timeval t1;
    gettimeofday(&t1, NULL);

    /*void* oneReq = NULL;
    void* oneRsp = NULL;
    QOSREQUEST qosReq;
    double timeOut = 1.0f;*/

    pmp_req_t reqs[3];
    client1 c1;
    client2 c2;
    client3 c3;

    for (int i = 0; i < 3; ++i)
    {
        reqs[i] = (pmp_req_t)malloc(sizeof(mp_req_t));
        reqs[i]->out_time = 1.0f;
        switch (i)
        {
            case 0:
                reqs[i]->real_cli = &c1;
                break;
            case 1:
                reqs[i]->real_cli = &c2;
                break;
            case 2:
                reqs[i]->real_cli = &c3;
                break;
        }
    }
    CParrelApiMulThr testApi;
    int ret = testApi.Run(reqs, 3);
    if (ret)
    {
        fprintf(stderr, "Run error, ret=%d.\n", ret);
        for (int i = 0; i < 3; ++i)
        {
            free(reqs[i]);
        }
        return ret;
    }
    /*c1.ProcessNetWork(oneReq, oneRsp, qosReq, timeOut);
    c2.ProcessNetWork(oneReq, oneRsp, qosReq, timeOut);
    c3.ProcessNetWork(oneReq, oneRsp, qosReq, timeOut);*/

    struct timeval t2;
    gettimeofday(&t2, NULL);
    printf("%lldus.\n", (int64_t)(t2.tv_sec - t1.tv_sec) * 1000000 + (t2.tv_usec - t1.tv_usec));

    return 0;
}
