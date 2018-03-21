#include "../spredis.h"
#include <inttypes.h>

void __SPFreeSortData(SpredisSortData *sd) {
    if ((sd)->scores != NULL) RedisModule_Free((sd)->scores);
    RedisModule_Free(sd);
}

void SpredisTMPResDBSave(RedisModuleIO *io, void *ptr) {
	//we are not replicating these values
}
void SpredisTMPResRewriteFunc(RedisModuleIO *aof, RedisModuleString *key, void *value) {
	//we are not replicating these values
}

void *SpredisTMPResRDBLoad(RedisModuleIO *io, int encver) {
	//we are not replicating these values
	SpredisTempResult *res = RedisModule_Calloc(1, sizeof(SpredisTempResult));
	res->size = 0;
	res->data = NULL;//RedisModule_Alloc(sizeof(SpredisSortData*) * 0);
	return res;
}
void SpredisTempResultModuleInit() {
};

void _SpredisDestroyTmpResult(void *value) {
    if (value == NULL) return;
    SpredisTempResult *tr = value;
    SpredisSortData *sd;
    if (tr->data != NULL) {
        for (int i = 0; i < tr->size; ++i)
        {
            sd = tr->data[i];
            if (sd != NULL && sd->scores != NULL) RedisModule_Free(sd->scores);
            if (sd != NULL) RedisModule_Free(sd);
        }
    }
    if (tr->data != NULL) RedisModule_Free(tr->data);
    RedisModule_Free(tr);
}

void SpredisTMPResFreeCallback(void *value) {

    // _SpredisDestroyTmpResult(value);
    SP_TWORK(_SpredisDestroyTmpResult, value, {
        //do nothing
    });
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
	// RedisModule_CloseKey(key);
	return res;

}

int SpredisTMPResGetDocs_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    RedisModule_AutoMemory(ctx);

    if (argc != 5) return RedisModule_WrongArity(ctx);

    RedisModuleKey *key = RedisModule_OpenKey(ctx,argv[1],
            REDISMODULE_READ);
    RedisModuleKey *valKey = RedisModule_OpenKey(ctx,argv[2],
            REDISMODULE_READ);
    // printf("%s\n", "WTF1");
    int keyType;
    if (HASH_EMPTY_OR_WRONGTYPE(key, &keyType, SPTMPRESTYPE) != 0) {
        // printf("%s\n", "WTF2");
        return RedisModule_ReplyWithError(ctx,REDISMODULE_ERRORMSG_WRONGTYPE);   
    }

    if (HASH_EMPTY_OR_WRONGTYPE(valKey, &keyType, SPDOCTYPE) != 0) {
        // printf("%s\n", "WTF2");
        return RedisModule_ReplyWithError(ctx,REDISMODULE_ERRORMSG_WRONGTYPE);   
    }


    SpredisTempResult *res = RedisModule_ModuleTypeGetValue(key);
    SPDocContainer *dc = RedisModule_ModuleTypeGetValue(valKey);

    long long start;
    long long count;
    int startOk = RedisModule_StringToLongLong(argv[3],&start);
    int countOk = RedisModule_StringToLongLong(argv[4],&count);

    // printf("%s\n", "WTF3");
    if (startOk == REDISMODULE_ERR || countOk == REDISMODULE_ERR) {
        // RedisModule_CloseKey(key);
        
        return RedisModule_ReplyWithError(ctx,REDISMODULE_ERRORMSG_WRONGTYPE);
    }
    // printf("%s %lld %lld %zu\n", "hmmm", start, count, res->size);
    long long finalCount = 0;    
    SpredisSortData *d;
    RedisModule_ReplyWithArray(ctx, REDISMODULE_POSTPONED_ARRAY_LEN);
    khint_t k;
    while(start < res->size && finalCount < count) {
        d = res->data[start];
        k = kh_get(DOC, dc->documents, d->id);
        if (k != kh_end(dc->documents)) {
            RedisModule_ReplyWithStringBuffer(ctx, kh_value(dc->documents,k), strlen(kh_value(dc->documents,k)));
        } else {
            RedisModule_ReplyWithNull(ctx);
        }
        finalCount++;
        start++;
    }
    RedisModule_ReplySetArrayLength(ctx, finalCount);
    // printf("%s %lld\n", "WTF6", finalCount);
    
    // RedisModule_CloseKey(key);
    return REDISMODULE_OK;
}


int SpredisTMPResGetIds_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
	RedisModule_AutoMemory(ctx);

    if (argc != 5) return RedisModule_WrongArity(ctx);

    RedisModuleKey *key = RedisModule_OpenKey(ctx,argv[1],
            REDISMODULE_READ);
    RedisModuleKey *valKey = RedisModule_OpenKey(ctx,argv[2],
            REDISMODULE_READ);
    // printf("%s\n", "WTF1");
    int keyType;
    if (HASH_EMPTY_OR_WRONGTYPE(key, &keyType, SPTMPRESTYPE) != 0) {
        // printf("%s\n", "WTF2");
        return RedisModule_ReplyWithError(ctx,REDISMODULE_ERRORMSG_WRONGTYPE);   
    }

    if (HASH_EMPTY_OR_WRONGTYPE(valKey, &keyType, SPDOCTYPE) != 0) {
        // printf("%s\n", "WTF2");
        return RedisModule_ReplyWithError(ctx,REDISMODULE_ERRORMSG_WRONGTYPE);   
    }


    SpredisTempResult *res = RedisModule_ModuleTypeGetValue(key);
    SPDocContainer *dc = RedisModule_ModuleTypeGetValue(valKey);

    long long start;
    long long count;
    int startOk = RedisModule_StringToLongLong(argv[3],&start);
    int countOk = RedisModule_StringToLongLong(argv[4],&count);

    // printf("%s\n", "WTF3");
    if (startOk == REDISMODULE_ERR || countOk == REDISMODULE_ERR) {
        // RedisModule_CloseKey(key);
        
        return RedisModule_ReplyWithError(ctx,REDISMODULE_ERRORMSG_WRONGTYPE);
    }
    // printf("%s %lld %lld %zu\n", "hmmm", start, count, res->size);
    long long finalCount = 0;    
    SpredisSortData *d;
    RedisModule_ReplyWithArray(ctx, REDISMODULE_POSTPONED_ARRAY_LEN);
    khint_t k;
    while(start < res->size && finalCount < count) {
        d = res->data[start];
        k = kh_get(DOC, dc->revId, d->id);
        if (k != kh_end(dc->revId)) {
            RedisModule_ReplyWithStringBuffer(ctx, kh_value(dc->revId,k), strlen(kh_value(dc->revId,k)));
        } else {
            RedisModule_ReplyWithNull(ctx);
        }
        finalCount++;
        start++;
    }
    RedisModule_ReplySetArrayLength(ctx, finalCount);
    // printf("%s %lld\n", "WTF6", finalCount);
    
    // RedisModule_CloseKey(key);
    return REDISMODULE_OK;
}
