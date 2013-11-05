#include "lru_cache.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <string.h>

int main(int argc, char** argv)
{
    if (argc == 1 || (argc == 2 && !strcmp(argv[1], "-h")))
    {
        printf("Usage: %s test_num cache_size\n", argv[0]);
        return 0;
    }

    int test_num = atoi(argv[1]);
    int cache_size = atoi(argv[2]);
    ClruCache oneCache(cache_size, sizeof(int64_t));

    int ret = 0;
    srand(time(NULL));
    for (int i = 0; i < test_num; i++)
    {
        printf("[%d]DEBUG: i=%d\n", __LINE__, i);

        long key = rand()%test_num;
        std::string data;
        data.resize(sizeof(key));
        memcpy((char *)data.c_str(), &key, sizeof(key));
        printf("[%d]DEBUG: key=%ld\n", __LINE__, key);

        ret = oneCache.Insert(key, data, -1, FAIL_IF_EXIST);
        if (ret != 0)
            printf("[%d]ret = %d, same key=%ld.\n", __LINE__, ret, key);
    }

    for (int i = 0; i < test_num; i++)
    {
        printf("[%d]DEBUG: i=%d\n", __LINE__, i);

        long key = rand()%test_num;
        std::string data;
        ret = oneCache.Get(key, data);
        if (ret == 0)
            printf("[%d]hit key=%ld.\n", __LINE__, key);
        else
            printf("[%d]miss key=%ld.\n", __LINE__, key);
    }

    return 0;
}
