#include "lru_cache.h"
#include <string.h>
#include <stdint.h>

static int GetMinPrim(int limit)
{					
	if (limit % 2 == 0)								///判断奇偶
	{
		limit++;
	}
	
	for (int i = limit; ; i += 2)
    {
        int cnt = 0;
        for (int j = 2; j*j <= i; ++j)
        {
             if ((i % j) == 0)
             {
                 cnt++;
                 break;
             }
        }

        if (cnt == 0)
        {
			return i;
        }
    }
	
	return limit;
}

namespace KYRIE_UTILS
{
	ClruCache::ClruCache(int size, int key_len)
	{
		max_ele_num = ((size >= MIN_BUCKET_SIZE) ? size:MIN_BUCKET_SIZE);
		now_ele_num = 0;
		max_key_size = ((key_len > DEFAULT_BUCKET_SIZE) ? key_len:DEFAULT_BUCKET_SIZE);
		INIT_LIST_HEAD(&cache_head);
		
		mapCache.clear();
		int min_prim = GetMinPrim(max_ele_num);															///获取大于元素个数的最小质数
		mapCache.resize(min_prim);
	}

	ClruCache::~ClruCache()
	{
		mapCache.clear();
		struct list_head *pl;
		struct list_head *tmp;
		
		list_for_each_safe(pl, tmp, &cache_head)
		{
			struct CacheData* pc = list_entry(pl, CacheData, list);
			///释放内存
			free(pc);
		}
	}
	
	///确保max_ele_size是一个大于2的值
	int ClruCache::Insert(const std::string& key, int64_t data, long expire_time, int option)
	{
		if (key.size() > (uint32_t)max_key_size)
			return E_LRU_KEY_TOO_LARGE;

		///检查是否已存在
		map_cache_data_t::iterator it;
		it = mapCache.find(key.c_str());
		if (it == mapCache.end())																		///没找到
		{
			if (now_ele_num >= max_ele_num)
			{
				struct CacheData* tail = list_entry(cache_head.prev, CacheData, list);
				mapCache.erase(tail->key);																///删除旧key的hash_map记录

				tail->data = data;																		///重新赋值
				tail->expire_time = expire_time;
				tail->insert_time = time(NULL);
				tail->key_len = key.size();
				memcpy(tail->key, key.c_str(), tail->key_len);
				tail->key[tail->key_len] = '\0';
				list_move(&(tail->list), &cache_head);													///重要：将其移到链表头，LRU
				mapCache[tail->key] = tail;																///在hash_map中记录新key
			}
			else
			{
				struct CacheData *pc = (struct CacheData *)malloc(sizeof(struct CacheData) + max_key_size + 1);
				pc->data = data;
				pc->expire_time = expire_time;
				pc->insert_time = time(NULL);
				pc->key_len = key.size();
				memcpy(pc->key, key.c_str(), pc->key_len);
				pc->key[pc->key_len] = '\0';
				list_add(&(pc->list), &cache_head);
		
				mapCache[pc->key] = pc;																	///重要，使用pc->key而不是传入的key
				++now_ele_num;
			}
		}
		else																							///找到了
		{
			CacheData* pc = (it->second);																///找到对用的cache_data指针
			if (((pc->expire_time <= 0) || (pc->expire_time + pc->insert_time >= time(NULL))) 
					&& (option == FAIL_IF_EXIST)) {														///没过期，并且选项是FAIL_IF_EXIST
				return E_LRU_ALREADY_EXSIT;
			}
			else
			{
				list_move(&(pc->list), &cache_head);													///重要：将其移到链表头，LRU
				pc->expire_time = expire_time;
				pc->insert_time = time(NULL);
				pc->key_len = key.size();
				memcpy(pc->key, key.c_str(), pc->key_len);												///data_len不会超过申请的内存
				pc->key[pc->key_len] = '\0';
			}
		}
	
		return 0;
	}
	
	int ClruCache::Get(std::string& key, int64_t& data)
	{
		map_cache_data_t::iterator it;
		it = mapCache.find(key.c_str());
		if (it == mapCache.end())																		///没找到
			return E_LRU_NOT_EXIST;
		
		struct CacheData* pc = (it->second);
		if ((pc->expire_time > 0) 
				&& (pc->expire_time + pc->insert_time < time(NULL))) {
			return E_LRU_EXPIRED;
		}
	
		data = pc->data;
		list_move(&(pc->list), &cache_head);															///重要：将其移到链表头，LRU
	
		return 0;	
	}
	
	int ClruCache::Delete(std::string& key)
	{
		map_cache_data_t::iterator it;
		it = mapCache.find(key.c_str());
		if (it == mapCache.end())																		///没找到
			return E_LRU_NOT_EXIST;
		
		struct CacheData *pc = (it->second);
		mapCache.erase(key.c_str());																	///从hash_map里面删除尾节点对应的key
		list_del(&(pc->list));																			///从双向循环链表里面删除尾节点
		free(pc);																						///delete尾节点
		--now_ele_num;																					///总的节点数量减一
	
		return 0;
	}
}
