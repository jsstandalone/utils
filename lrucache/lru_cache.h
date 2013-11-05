#ifndef _LRU_CACHE_H_
#define _LRU_CACHE_H_

#include "list.h"
#include <ext/hash_map>

#define MIN_BUCKET_SIZE			128
#define DEFAULT_BUCKET_SIZE		4

enum INSERT_OPTION
{
	FAIL_IF_EXIST	= 0,
	OVERWRITE		= 1,
};

enum ENUM_ERRCODE
{
	E_OK				= 0,
	E_NOT_EXIST			= 1,
	E_ALREADY_EXSIT		= 2,
	E_EXPIRED			= 3,
	E_VALUE_TOO_LARGE	= 4,
};

struct CacheData
{
	struct list_head list;
	long key;
	long insert_time;
	long expire_time;
	int total_len;
	int data_len;
	char data[0];					///占位符，gcc支持
};

class ClruCache
{
	public:
		ClruCache(int ele_size, int value_len);
		~ClruCache();
		
	public:
		int Insert(long key, const std::string& data, long expire_time, int option);
		int Get(long key, std::string& data);
		int Delete(long key);
		
	private:
		struct list_head cache_head;
		__gnu_cxx::hash_map<long, struct CacheData*> mapCache;
		int max_ele_num;
		int now_ele_num;
		int max_value_size;				///<k, v>对里面，value的最大长度，可根据需要调整，为了减少free & malloc将value拼在节点数据的后面
};

#endif
