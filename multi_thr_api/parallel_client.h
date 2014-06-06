#ifndef _PARALLE_CLIENT_H_
#define _PARALLE_CLIENT_H_

#include <string>

#define MAX_ERRMSG_SIZE     1024

typedef struct QOSREQUEST_s
{
    int modid;
    int cmdid;
    std::string sip;
    int port;
}QOSREQUEST;

class CParallelCli
{
	public:
		CParallelCli();
		virtual ~CParallelCli();

		virtual int ProcessNetWork(void* oneReq, void* oneRsp,
                                    QOSREQUEST& qosReq, double timeOut) = 0;

	private:
        char m_err_msg[MAX_ERRMSG_SIZE];
};

#endif
