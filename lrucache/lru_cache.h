#ifndef _LRU_CACHE_H_
#define _LRU_CACHE_H_

#include "list.h"
#include <ext/hash_map>

#define MIN_BUCKET_SIZE			128
#define DEFAULT_BUCKET_SIZE		64

namespace KYRIE_UTILS
{
	enum INSERT_OPTION
	{
		FAIL_IF_EXIST	= 0,
		OVERWRITE		= 1,
	};
	
	enum ENUM_ERRCODE
	{
		E_LRU_OK				= 0,
		E_LRU_NOT_EXIST			= 1,
		E_LRU_ALREADY_EXSIT		= 2,
		E_LRU_EXPIRED			= 3,
		E_LRU_KEY_TOO_LARGE		= 4,
	};

	struct CacheData					///定制化的lru cachedata, map<string, int64_t>
	{
		struct list_head list;
		int64_t data;
		long insert_time;
		long expire_time;
		int total_len;
		short key_len;
		char key[0];					///占位符，gcc支持
	};

	template <class _Tp>
	struct str_equal_to
	{
		bool  
		operator()(const _Tp& __x, const _Tp& __y) const  
		{ return strcmp( __x, __y ) == 0; }  
	};

	typedef __gnu_cxx::hash_map<const char*, 
								struct CacheData*, 
								__gnu_cxx::hash<const char*>, 
								str_equal_to<const char*> > map_cache_data_t;
	class ClruCache
	{
		public:
			ClruCache(int ele_size, int key_len);
			~ClruCache();
			
		public:
			int Insert(const std::string& key, int64_t data, long expire_time, int option);
			int Get(std::string& key, int64_t& data);
			int Delete(std::string& key);
			
		private:
			struct list_head cache_head;
			map_cache_data_t mapCache;
			int max_ele_num;
			int now_ele_num;
			int max_key_size;				///<k, v>对里面，key的最大长度，可根据需要调整，为了减少free & malloc将key拼在节点数据的后面
	};
}

#endif
