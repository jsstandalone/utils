#include "lru_cache.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <string.h>

using namespace std;
using namespace KYRIE_UTILS;

#define MAX_KEY_LEN		32

int main(int argc, char** argv)
{
	if (argc == 1 || (argc == 2 && !strcmp(argv[1], "-h")))
	{
		printf("Usage: %s test_num cache_size\n", argv[0]);
		return 0;
	}
	
	int test_num = atoi(argv[1]);
	int cache_size = atoi(argv[2]);
	ClruCache oneCache(cache_size, MAX_KEY_LEN);
	
	int ret = 0;
	srand(time(NULL));
	for (int i = 0; i < test_num; i++)
	{
		printf("[%d]DEBUG: i=%d\n", __LINE__, i);

		int64_t data = rand()%test_num;
		std::string key;
		key.resize(128);
		int len = snprintf((char *)key.c_str(), key.size(), "%lld", data);
		if (len > 0)
			key.resize(len);

		printf("[%d]DEBUG: key=%s, data=%lld\n", __LINE__, key.c_str(), data);
		ret = oneCache.Insert(key, data, 7200, FAIL_IF_EXIST);
		if (ret != 0)
			printf("[%d]ret = %d, same key=%s.\n", __LINE__, ret, key.c_str());
	}

	printf("\n-----------------------------------\n\n");
	for (int i = 0; i < test_num; i++)
	{
		printf("[%d]DEBUG: i=%d\n", __LINE__, i);

		int64_t tmp = rand()%test_num;
		int64_t data;
		std::string key;
		key.resize(128);
		int len = snprintf((char *)key.c_str(), key.size(), "%lld", tmp);
		if (len > 0)
			key.resize(len);
		ret = oneCache.Get(key, data);
		if (ret == 0)
			printf("[%d]hit key=%s, data=%lld.\n", __LINE__, key.c_str(), data);
		else
			printf("[%d]miss key=%s.\n", __LINE__, key.c_str());
	}

	return 0;
}
