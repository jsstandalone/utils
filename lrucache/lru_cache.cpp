#include "lru_cache.h"
#include <string.h>
#include <stdint.h>

ClruCache::ClruCache(int size, int value_len)
{
    max_ele_num = ((size >= MIN_BUCKET_SIZE) ? size:MIN_BUCKET_SIZE);
    now_ele_num = 0;
    max_value_size = ((value_len > DEFAULT_BUCKET_SIZE) ? value_len:DEFAULT_BUCKET_SIZE);
    INIT_LIST_HEAD(&cache_head);
    mapCache.clear();
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
int ClruCache::Insert(long key, const std::string& data, long expire_time, int option)
{
    if (data.size() > (uint32_t)max_value_size)
        return E_VALUE_TOO_LARGE;

    ///检查是否已存在
    __gnu_cxx::hash_map<long, struct CacheData*>::iterator it;
    it = mapCache.find(key);
    if (it == mapCache.end())                                                                       ///没找到
    {
        if (now_ele_num >= max_ele_num)
        {
            struct CacheData* tail = list_entry(&cache_head, CacheData, list);                      ///根据双向循环链表的头找到tail
            mapCache.erase(tail->key);                                                              ///从hash_map里面删除尾节点对应的key
            list_del(&(tail->list));                                                                ///从双向循环链表里面删除尾节点
            free(tail);                                                                             ///delete尾节点
            --now_ele_num;                                                                          ///总的节点数量减一
        }

        struct CacheData *pc = (struct CacheData *)malloc(sizeof(struct CacheData) + max_value_size);
        pc->key = key;
        pc->expire_time = expire_time;
        pc->insert_time = time(NULL);
        pc->data_len = data.size();
        memcpy(pc->data, data.c_str(), pc->data_len);
        list_add(&(pc->list), &cache_head);

        mapCache[key] = pc;
        ++now_ele_num;
    }
    else                                                                                            ///找到了
    {
        CacheData* pc = (it->second);                                                               ///找到对用的cache_data指针
        if (((pc->expire_time <= 0) || (pc->expire_time + pc->insert_time >= time(NULL)))
                && (option == FAIL_IF_EXIST)) {                                                     ///没过期，并且选项是FAIL_IF_EXIST
            return E_ALREADY_EXSIT;
        }
        else
        {
            list_move(&(pc->list), &cache_head);                                                    ///重要：将其移到链表头，LRU
            pc->expire_time = expire_time;
            pc->insert_time = time(NULL);
            pc->data_len = data.size();
            memcpy(pc->data, data.c_str(), pc->data_len);                                           ///data_len不会超过申请的内存
        }
    }

    return 0;
}

int ClruCache::Get(long key, std::string& data)
{
    __gnu_cxx::hash_map<long, struct CacheData*>::iterator it;
    it = mapCache.find(key);
    if (it == mapCache.end())                                                                       ///没找到
        return E_NOT_EXIST;

    struct CacheData* pc = (it->second);
    if ((pc->expire_time > 0)
            && (pc->expire_time + pc->insert_time < time(NULL))) {
        return E_EXPIRED;
    }

    data.resize(pc->data_len);
    memcpy((char *)data.c_str(), pc->data, data.size());
    list_move(&(pc->list), &cache_head);                                                            ///重要：将其移到链表头，LRU

    return 0;
}

int ClruCache::Delete(long key)
{
    __gnu_cxx::hash_map<long, struct CacheData*>::iterator it;
    it = mapCache.find(key);
    if (it == mapCache.end())                                                                       ///没找到
        return E_NOT_EXIST;

    struct CacheData *pc = (it->second);
    mapCache.erase(key);                                                                            ///从hash_map里面删除尾节点对应的key
    list_del(&(pc->list));                                                                          ///从双向循环链表里面删除尾节点
    free(pc);                                                                                       ///delete尾节点
    --now_ele_num;                                                                                  ///总的节点数量减一

    return 0;
}
