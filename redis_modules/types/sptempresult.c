#include "../spredis.h"
#include <inttypes.h>

void SpredisTMPResDBSave(RedisModuleIO *io, void *ptr) {
	//we are not replicating these values
}
void SpredisTMPResRewriteFunc(RedisModuleIO *aof, RedisModuleString *key, void *value) {
	//we are not replicating these values
}

void *SpredisTMPResRDBLoad(RedisModuleIO *io, int encver) {
	//we are not replicating these values
	SpredisTempResult *res = RedisModule_Alloc(sizeof(SpredisTempResult));
	res->size = 0;
	res->data = NULL;//RedisModule_Alloc(sizeof(SpredisSortData*) * 0);
	return res;
}
void SpredisTempResultModuleInit() {
};

void SpredisTMPResFreeCallback(void *value) {
	SpredisTempResult *tr = value;
	SpredisSortData *sd;
	for (int i = 0; i < tr->size; ++i)
	{
		sd = tr->data[i];
		if (sd != NULL && sd->scores != NULL) RedisModule_Free(sd->scores);
		RedisModule_Free(sd);
	}
	if (tr->data != NULL) RedisModule_Free(tr->data);
	RedisModule_Free(tr);
}


SpredisTempResult *SpredisTempResultCreate(RedisModuleCtx *ctx,RedisModuleString *keyName, size_t size) {

    RedisModuleKey *key = RedisModule_OpenKey(ctx,keyName,
            REDISMODULE_WRITE);
    int keyType = RedisModule_KeyType(key);
    if (keyType != REDISMODULE_KEYTYPE_EMPTY) {
    	return NULL;
    }
	SpredisTempResult *res = RedisModule_Alloc(sizeof(SpredisTempResult));
	res->size = size;
	res->data = RedisModule_Alloc(sizeof(SpredisSortData*) * size);
	SpredisSetRedisKeyValueType(key,SPTMPRESTYPE,res);
	RedisModule_CloseKey(key);
	return res;

}


int SpredisTMPResGetIds_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
	// printf("Getting %d\n", TOINTKEY(argv[2]));
    RedisModule_AutoMemory(ctx);

    if (argc != 4) return RedisModule_WrongArity(ctx);

    RedisModuleKey *key = RedisModule_OpenKey(ctx,argv[1],
            REDISMODULE_READ);
    // printf("%s\n", "WTF1");
    int keyType;
    if (HASH_EMPTY_OR_WRONGTYPE(key, &keyType, SPTMPRESTYPE) != 0) {
        // printf("%s\n", "WTF2");
        return RedisModule_ReplyWithError(ctx,REDISMODULE_ERRORMSG_WRONGTYPE);   
    }

    SpredisTempResult *res = RedisModule_ModuleTypeGetValue(key);
    

	long long start;
	long long count;
	int startOk = RedisModule_StringToLongLong(argv[2],&start);
	int countOk = RedisModule_StringToLongLong(argv[3],&count);

    // printf("%s\n", "WTF3");
	if (startOk == REDISMODULE_ERR || countOk == REDISMODULE_ERR) {
		RedisModule_CloseKey(key);
        
		return RedisModule_ReplyWithError(ctx,REDISMODULE_ERRORMSG_WRONGTYPE);
	}
    // printf("%s %lld %lld %zu\n", "hmmm", start, count, res->size);
    long long finalCount = 0;
    /* make sure we don't overrun the array length */
    // if (start > count) {
    //     finalCount = 0;
    // }
    // if (start + count > res->size) {
    //     finalCount = res->size - (start + count);
    //     if (finalCount < 0) finalCount = 0;
    // }
    // printf("%s %lld\n", "WTF5", finalCount);
    
    SpredisSortData *d;
    RedisModule_ReplyWithArray(ctx, REDISMODULE_POSTPONED_ARRAY_LEN);
    while(start < res->size && finalCount < count) {
        d = res->data[start];
        // char ress[32];
        // printf("replying with %" PRIx32 "\n", d->id);
        RedisModule_ReplyWithString(ctx, RedisModule_CreateStringPrintf(ctx, "%" PRIx32, d->id));
        finalCount++;
        start++;
    }
    RedisModule_ReplySetArrayLength(ctx, finalCount);
    // printf("%s %lld\n", "WTF6", finalCount);
    
    RedisModule_CloseKey(key);
    return REDISMODULE_OK;
}
