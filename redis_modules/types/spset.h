#ifndef __SPREDIS_SET
#define __SPREDIS_SET

#include "../spredis.h"


// typedef struct _SpredisSMap_t {
// 	char full;
// 	char *value;
// } SpredisSMap_t;


// typedef struct SpredisSetCont;

// char *SpredisSMapValue(SpredisSMapCont *map, unsigned long id);
// KHASH_DECLARE(SIDS, khint32_t, char)



typedef struct _SpredisSetCont {
	khash_t(SIDS) *set;
	pthread_rwlock_t mutex;
	int linkedSet;
} SpredisSetCont;

// void * _SpredisInitSet();



// SpredisSetCont *SpredisSIntersect(SpredisSetCont **sets, int count);
// SpredisSetCont *SpredisSDifference(SpredisSetCont **sets, int count);
// SpredisSetCont *SpredisSUnion(SpredisSetCont **sets, int count);

void * _SpredisInitSet();
void * _SpredisInitWithLinkedSet(khash_t(SIDS) *s, pthread_rwlock_t rwlock);
void SPSetCommandInit();
void _SpredisDestroySet(void *dhash);
void SpredisSetRDBSave(RedisModuleIO *io, void *ptr);
void SpredisSetRewriteFunc(RedisModuleIO *aof, RedisModuleString *key, void *value);
void *SpredisSetRDBLoad(RedisModuleIO *io, int encver);
void SpredisSetFreeCallback(void *value);
int SpredisSetAdd_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);
int SpredisSetMember_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);
int SpredisSetRem_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);
int SpredisSetCard_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);
int SpredisSTempInterstore_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);
int SpredisSTempDifference_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);
int SpredisSTempUnion_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);
int SpredisSTempAddAll_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);
#endif

