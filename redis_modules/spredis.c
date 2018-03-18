// #pragma GCC diagnostic ignored "-Wunused-function"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <float.h>
#include <math.h>

#include "spredis.h" //important that this coes before khash-- defines memory functions
#include "lib/ksort.h"
#include "types/spsharedtypes.h"

#include "lib/kexpr.h"



static RedisModuleType **SPREDISMODULE_TYPES; 


// static inline int ScoreComp(SPScore a, SPScore b) {
//     if (a.score < b.score) return -1;
//     if (b.score < a.score) return 1;
//     if (a.id < b.id) return -1;
//     if (b.id < a.id) return 1;
//     return 0;
// }

int HASH_NOT_EMPTY_AND_WRONGTYPE(RedisModuleKey *key, int *type, int targetType) {
    int keyType = RedisModule_KeyType(key);
    (*type) = keyType;
    if (keyType != REDISMODULE_KEYTYPE_EMPTY &&
        RedisModule_ModuleTypeGetType(key) != SPREDISMODULE_TYPES[targetType])
    {
        return 1;
    }
    return 0;
}

int HASH_NOT_EMPTY_AND_WRONGTYPE_CHECKONLY(RedisModuleKey *key, int *type, int targetType) {
    int keyType = RedisModule_KeyType(key);
    (*type) = keyType;
    if (keyType != REDISMODULE_KEYTYPE_EMPTY &&
        RedisModule_ModuleTypeGetType(key) != SPREDISMODULE_TYPES[targetType])
    {
        return 1;
    }
    return 0;
}

int HASH_EMPTY_OR_WRONGTYPE(RedisModuleKey *key, int *type, int targetType) {
    int keyType = RedisModule_KeyType(key);
    (*type) = keyType;
    if (keyType == REDISMODULE_KEYTYPE_EMPTY ||
        RedisModule_ModuleTypeGetType(key) != SPREDISMODULE_TYPES[targetType])
    {
        // RedisModule_CloseKey(key);
        return 1;
    }
    return 0;
}


void SP_GET_KEYTYPES(RedisModuleKey *key, int *type, int *spType) {
    int keyType = RedisModule_KeyType(key);
    (*type) = keyType;
    (*spType) = SPNOTYPE;
    if (keyType == REDISMODULE_KEYTYPE_EMPTY) return;
    void *t = RedisModule_ModuleTypeGetType(key);
    for (int i = 0; i < SPMAXTYPE; ++i)
    {
        if (t == SPREDISMODULE_TYPES[i]) {
            (*spType) = i;
            break;
        }

    }
}




#define scorelt(a,b) ( (a) < (b) )
KSORT_INIT(IDSHUFFLE, uint64_t, scorelt);

static inline double SPDist(double th1, double ph1, double th2, double ph2)
{
    double dx, dy, dz;
    ph1 -= ph2;
    ph1 *= 0.017453292519943295, th1 *= 0.017453292519943295, th2 *= 0.017453292519943295;
 
    dz = sin(th1) - sin(th2);
    dx = cos(ph1) * cos(th1) - cos(th2);
    dy = sin(ph1) * cos(th1);
    return asin(sqrt(dx * dx + dy * dy + dz * dz) / 2) * 2 * 6372797.560856;
}


#include "lib/sp_kbtree.h"
// #define ScoreComp(a,b) (((((b).score) < ((a).score)) - (((a).score) < ((b).score))) || ((((b).id) < ((a).id)) - (((a).id) < ((b).id))))

// KBTREE_INIT(SCORES, SPScore, SPScoreComp)
// SPSCORE_BTREE_INIT(TEST_SCORES)
// SPREDIS_SORT_INIT(SCORESORT, SPScore*, SPSortSore)

// int INBOUNDS(double lat, double lon, )

typedef struct {
    const char * formula;
    long long len;
} expr_cont;

void DoFormula(void *form, long count, int tid) {
    int err;
    kexpr_t *ke;
    expr_cont *cont = (expr_cont *)form;
    char * formula = strdup((const char *)cont->formula);
    // printf("%s, count: %ld\n", formula, count);
    ke = ke_parse(formula, &err);
    for (long i = 0; i < cont->len; ++i)
    {
        /* code */
        ke_set_real(ke, "distance", 25.786);
        ke_set_real(ke, "weight", 0.1);
        ke_eval_real(ke, &err);
    }
    ke_destroy(ke);
    free(formula);

}


int SpredisEXPR_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    // printf("Getting %d\n", TOINTKEY(argv[2]));
    RedisModule_AutoMemory(ctx);
    

   
    // kexpr_t *ke;
    // int err;

    // const char * formula = "(100 - distance) * weight";

    // ke = ke_parse("(100 - distance) * weight", &err);   // parse expression
    // // ke_set_real_func1(ke, "myexp", exp);     // define a math function

    // ke_set_real(ke, "distance", 25.786);
    // ke_set_real(ke, "weight", 0.1);
    
    // printf("score: %g\n", ke_eval_real(ke, &err));


    // ke_set_real(ke, "distance", 45.786);
    // ke_set_real(ke, "weight", 0.1);
    
    // printf("score: %g\n", ke_eval_real(ke, &err));


    // ke_set_real(ke, "distance", 67.786);
    // ke_set_real(ke, "weight", 0.1);
    
    // printf("score: %g\n", ke_eval_real(ke, &err));
    //                         // deallocate
   
    

    // long long count = 0;
    // long long  num_threads = 2;
    // RedisModule_StringToLongLong(argv[1], &count);
    // RedisModule_StringToLongLong(argv[2], &num_threads);
    // ke_destroy(ke);  

    // long long startTimer = RedisModule_Milliseconds();

    // expr_cont cont = {.formula = formula, count / num_threads};
    // kt_for((int)num_threads, DoFormula, &cont, (int)num_threads);

    // // for (long long i = 0; i < count; ++i)
    // // {
    // //     /* code */
    // //     ke_set_real(ke, "distance", 25.786);
    // //     ke_set_real(ke, "weight", 0.1);
    // //     ke_eval_real(ke, &err);
    // // }

    // // RedisModule_ReplyWithSimpleString(ctx, "OK");
    // // RedisModule_ReplyWithLongLong(ctx, RedisModule_CallReplyLength(reply));
    // RedisModule_ReplyWithLongLong(ctx, RedisModule_Milliseconds() - startTimer);
    


   
    RedisModule_ReplyWithDouble(ctx, 0);

    
    return REDISMODULE_OK;
}

int SpredisSetRedisKeyValueType(RedisModuleKey *key, int type, void *value) {
    // printf("trying to set %d\n", type);
    return RedisModule_ModuleTypeSetValue(key, SPREDISMODULE_TYPES[type],value);
}

static threadpool SP_GENERIC_WORKER_POOL;
int SPDoWorkInThreadPool(void *func, void *arg) {
    return thpool_add_work(SP_GENERIC_WORKER_POOL, func, arg);
}

static SPJobQ *SP_PQ_POOL;

void SPDoWorkInParallel(void (**func)(void*), void **arg, int jobCount) {
    sp_runjobs(SP_PQ_POOL, func, arg, jobCount);
}


typedef struct {
    RedisModuleBlockedClient *bc;
    RedisModuleString **argv;
    int argc;
    int reply;
    int (*command)(RedisModuleCtx*, RedisModuleString**, int);
} SPThreadArg;

int SPThreadedReply(RedisModuleCtx *ctx, RedisModuleString **argv, int argc)
{    
    SPThreadArg *targ = RedisModule_GetBlockedClientPrivateData(ctx);
    return targ->reply;
}

void SPThreadedReplyFree(void *arg)
{
    SPThreadArg *targ = arg;
    // printf("Freeing thread arg\n");
    RedisModule_Free(targ);
}


void SPThreadedDoWork(void *arg)
{
    SPThreadArg *targ = arg;
    RedisModuleCtx *ctx = RedisModule_GetThreadSafeContext(targ->bc);
    SPLockContext(ctx);
    RedisModule_AutoMemory(ctx);
    SPUnlockContext(ctx);
    targ->reply = targ->command(ctx, targ->argv, targ->argc);
    RedisModule_UnblockClient(targ->bc, targ);
    RedisModule_FreeThreadSafeContext(ctx);
}

int SPThreadedWork(RedisModuleCtx *ctx, RedisModuleString **argv, int argc, int (*command)(RedisModuleCtx*, RedisModuleString**, int)) {
    RedisModule_AutoMemory(ctx);

    // for (int i = 0; i < argc; ++i)
    // {
        // printf("%s\n", RedisModule_StringPtrLen(argv[0], NULL));
    // }
    SPThreadArg *targ = RedisModule_Alloc(sizeof(SPThreadArg));
    targ->bc = RedisModule_BlockClient(ctx, SPThreadedReply /*reply*/, SPThreadedReply /*timeout*/, SPThreadedReplyFree /*free*/ ,0);
    targ->argv = argv;
    targ->argc = argc;
    targ->command = command;
    if (SPDoWorkInThreadPool(SPThreadedDoWork, targ) != 0) {
        RedisModule_AbortBlock(targ->bc);
        SPThreadedReplyFree(targ);
        return RedisModule_ReplyWithError(ctx, "ERR Could not do threaded work");
    }
    return REDISMODULE_OK;
}


void SpredisDebug(RedisModuleCtx *ctx, const char *fmt,...) {
    va_list ap;
    va_start(ap, fmt);
    RedisModule_Log(ctx, "debug", fmt, ap);
    va_end(ap);   
}

void SpredisLog(RedisModuleCtx *ctx, const char *fmt,...) {
    va_list ap;
    va_start(ap, fmt);
    RedisModule_Log(ctx, "notice", fmt, ap);
    va_end(ap);   
}

void SpredisWarn(RedisModuleCtx *ctx, const char *fmt,...) {
    va_list ap;
    va_start(ap, fmt);
    RedisModule_Log(ctx, "warning", fmt, ap);
    va_end(ap);   
}

int RedisModule_OnLoad(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    // SPLazyPool = thpool_init(1);

    if (RedisModule_Init(ctx,"spredis",1,REDISMODULE_APIVER_1)
        == REDISMODULE_ERR) return REDISMODULE_ERR;

    SPREDISMODULE_TYPES = RedisModule_Alloc(sizeof(RedisModuleType*) * 128);
    SpredisTempResultModuleInit();
    SpredisZsetMultiKeySortInit();
    SpredisFacetInit();
    SPStoreRangeInit();
    // SPSetCommandInit();
    SP_GENERIC_WORKER_POOL = thpool_init(SP_GEN_TPOOL_SIZE);
    SP_PQ_POOL = sp_pq_init(SP_PQ_POOL_SIZE, SP_PQ_TCOUNT_SIZE);

    SpredisLog(ctx, "SPREDIS_MEMORIES initialized (bleep bleep, blorp blorp, pew pew)\n");
    
    // for (int j = 0; j < argc; j++) {
    //     const char *s = RedisModule_StringPtrLen(argv[j],NULL);
    //     SpredisLog(ctx, "Module loaded with ARGV[%d] = %s\n", j, s);
    // }

    if (SpredisInitDocumentCommands(ctx) != REDISMODULE_OK) return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx,"spredis.exprtest",
        SpredisEXPR_RedisCommand,"readonly",0,0,0) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    /* support commands */
    if (RedisModule_CreateCommand(ctx,"spredis.storerangebyscore",
        SpredisStoreRangeByScore_RedisCommand,"write",0,0,0) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx,"spredis.storerangebylex",
        SpredisStoreLexRange_RedisCommand,"write",0,0,0) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx,"spredis.storerangebyradius",
        SpredisStoreRangeByRadius_RedisCommand,"write",0,0,0) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    
    if (RedisModule_CreateCommand(ctx,"spredis.sort",
        SpredisZsetMultiKeySort_RedisCommand,"write",0,0,0) == REDISMODULE_ERR)
        return REDISMODULE_ERR;


     if (RedisModule_CreateCommand(ctx,"spredis.facets",
        SpredisFacets_RedisCommand,"readonly",0,0,0) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx,"spredis.getresids",
        SpredisTMPResGetIds_RedisCommand,"readonly",0,0,0) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx,"spredis.getresdocs",
        SpredisTMPResGetDocs_RedisCommand,"readonly",0,0,0) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx,"spredis.setgeoresolver",
        SpredisSetGeoResolver_RedisCommand,"write",0,0,0) == REDISMODULE_ERR)
        return REDISMODULE_ERR;
    
    if (RedisModule_CreateCommand(ctx,"spredis.resolveexpr",
        SpredisExprResolve_RedisCommand,"readonly",0,0,0) == REDISMODULE_ERR)
        return REDISMODULE_ERR;


    /* type commands */
    
    if (RedisModule_CreateCommand(ctx,"spredis.hsetstr",
        SpredisHashSetStr_RedisCommand,"write",0,0,0) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx,"spredis.hsetdbl",
        SpredisHashSetDouble_RedisCommand,"write",0,0,0) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx,"spredis.hgetstr",
        SpredisHashGetStr_RedisCommand,"write",0,0,0) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx,"spredis.hgetdbl",
        SpredisHashGetDouble_RedisCommand,"write",0,0,0) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx,"spredis.hdel",
        SpredisHashDel_RedisCommand,"write",0,0,0) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    
    if (RedisModule_CreateCommand(ctx,"spredis.zadd",
        SpredisZSetAdd_RedisCommand,"write",0,0,0) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx,"spredis.zscore",
        SpredisZSetScore_RedisCommand,"readonly",0,0,0) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx,"spredis.zrem",
        SpredisZSetRem_RedisCommand,"write",0,0,0) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx,"spredis.zcard",
        SpredisZSetCard_RedisCommand,"readonly",0,0,0) == REDISMODULE_ERR)
        return REDISMODULE_ERR;


    if (RedisModule_CreateCommand(ctx,"spredis.zladd",
        SpredisZLexSetAdd_RedisCommand,"write",0,0,0) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx,"spredis.zlscore",
        SpredisZLexSetScore_RedisCommand,"readonly",0,0,0) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx,"spredis.zlrem",
        SpredisZLexSetRem_RedisCommand,"write",0,0,0) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx,"spredis.zlcard",
        SpredisZLexSetCard_RedisCommand,"readonly",0,0,0) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx,"spredis.zlapplyscores",
        SpredisZLexSetApplySortScores_RedisCommand,"write",0,0,0) == REDISMODULE_ERR)
        return REDISMODULE_ERR;




    if (RedisModule_CreateCommand(ctx,"spredis.geoadd",
        SpredisZGeoSetAdd_RedisCommand,"write",0,0,0) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx,"spredis.geoscore",
        SpredisZGeoSetScore_RedisCommand,"readonly",0,0,0) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx,"spredis.georem",
        SpredisZGeoSetRem_RedisCommand,"write",0,0,0) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx,"spredis.geocard",
        SpredisZGeoSetCard_RedisCommand,"readonly",0,0,0) == REDISMODULE_ERR)
        return REDISMODULE_ERR;



    if (RedisModule_CreateCommand(ctx,"spredis.sadd",
        SpredisSetAdd_RedisCommand,"write",0,0,0) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx,"spredis.smember",
        SpredisSetMember_RedisCommand,"readonly",0,0,0) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx,"spredis.srem",
        SpredisSetRem_RedisCommand,"write",0,0,0) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx,"spredis.scard",
        SpredisSetCard_RedisCommand,"readonly",0,0,0) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx,"spredis.staddall",
        SpredisSTempAddAll_RedisCommand,"write",0,0,0) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx,"spredis.stinterstore",
        SpredisSTempInterstore_RedisCommand,"write",0,0,0) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx,"spredis.stdiffstore",
        SpredisSTempDifference_RedisCommand,"write",0,0,0) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx,"spredis.stunionstore",
        SpredisSTempUnion_RedisCommand,"write",0,0,0) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx,"spredis.documentadd",
        SpredisDocAdd_RedisCommand,"write",0,0,0) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx,"spredis.documentrem",
        SpredisDocRem_RedisCommand,"write",0,0,0) == REDISMODULE_ERR)
        return REDISMODULE_ERR;
    
    RedisModuleTypeMethods rm = {
        .version = REDISMODULE_TYPE_METHOD_VERSION,
        .rdb_load = SpredisTMPResRDBLoad,
        .rdb_save = SpredisTMPResDBSave,
        .aof_rewrite = SpredisTMPResRewriteFunc,
        .free = SpredisTMPResFreeCallback
    };

    SPREDISMODULE_TYPES[SPTMPRESTYPE] = RedisModule_CreateDataType(ctx, "Sp_TMPRSS",
        SPREDISDHASH_ENCODING_VERSION, &rm);


    RedisModuleTypeMethods stm = {
        .version = REDISMODULE_TYPE_METHOD_VERSION,
        .rdb_load = SpredisSetRDBLoad,
        .rdb_save = SpredisSetRDBSave,
        .aof_rewrite = SpredisSetRewriteFunc,
        .free = SpredisSetFreeCallback
    };

    SPREDISMODULE_TYPES[SPSETTYPE] = RedisModule_CreateDataType(ctx, "SPpSPeTSS",
        SPREDISDHASH_ENCODING_VERSION, &stm);

    if (SPREDISMODULE_TYPES[SPSETTYPE] == NULL) return REDISMODULE_ERR;


    RedisModuleTypeMethods ztm = {
        .version = REDISMODULE_TYPE_METHOD_VERSION,
        .rdb_load = SpredisZSetRDBLoad,
        .rdb_save = SpredisZSetRDBSave,
        .aof_rewrite = SpredisZSetRewriteFunc,
        .free = SpredisZSetFreeCallback
    };

    SPREDISMODULE_TYPES[SPZSETTYPE] = RedisModule_CreateDataType(ctx, "SPpSZeTSS",
        SPREDISDHASH_ENCODING_VERSION, &ztm);


    if (SPREDISMODULE_TYPES[SPZSETTYPE] == NULL) return REDISMODULE_ERR;

    RedisModuleTypeMethods zltm = {
        .version = REDISMODULE_TYPE_METHOD_VERSION,
        .rdb_load = SpredisZLexSetRDBLoad,
        .rdb_save = SpredisZLexSetRDBSave,
        .aof_rewrite = SpredisZLexSetRewriteFunc,
        .free = SpredisZLexSetFreeCallback
    };

    SPREDISMODULE_TYPES[SPZLSETTYPE] = RedisModule_CreateDataType(ctx, "LsPpSZTSS",
        SPREDISDHASH_ENCODING_VERSION, &zltm);


    if (SPREDISMODULE_TYPES[SPZLSETTYPE] == NULL) return REDISMODULE_ERR;



    RedisModuleTypeMethods htm = {
        .version = REDISMODULE_TYPE_METHOD_VERSION,
        .rdb_load = SpredisHashRDBLoad,
        .rdb_save = SpredisHashRDBSave,
        .aof_rewrite = SpredisHashRewriteFunc,
        .free = SpredisHashFreeCallback
    };

    SPREDISMODULE_TYPES[SPHASHTYPE] = RedisModule_CreateDataType(ctx, "HsPpShTSS",
        SPREDISDHASH_ENCODING_VERSION, &htm);

    if (SPREDISMODULE_TYPES[SPHASHTYPE] == NULL) return REDISMODULE_ERR;

    


    RedisModuleTypeMethods gtm = {
        .version = REDISMODULE_TYPE_METHOD_VERSION,
        .rdb_load = SpredisZGeoSetRDBLoad,
        .rdb_save = SpredisZGeoSetRDBSave,
        .aof_rewrite = SpredisZGeoSetRewriteFunc,
        .free = SpredisZGeoSetFreeCallback
    };

    SPREDISMODULE_TYPES[SPGEOTYPE] = RedisModule_CreateDataType(ctx, "GsPpSZTSS",
        SPREDISDHASH_ENCODING_VERSION, &gtm);


    if (SPREDISMODULE_TYPES[SPGEOTYPE] == NULL) return REDISMODULE_ERR;

    RedisModuleTypeMethods etm = {
        .version = REDISMODULE_TYPE_METHOD_VERSION,
        .rdb_load = SpredisExpRslvrRDBLoad,
        .rdb_save = SpredisExpRslvrDBSave,
        .aof_rewrite = SpredisExpRslvrRewriteFunc,
        .free = SpredisExpRslvrFreeCallback
    };

    SPREDISMODULE_TYPES[SPEXPRTYPE] = RedisModule_CreateDataType(ctx, "EsRvSZTSS",
        SPREDISDHASH_ENCODING_VERSION, &etm);


    if (SPREDISMODULE_TYPES[SPEXPRTYPE] == NULL) return REDISMODULE_ERR;


    RedisModuleTypeMethods dtm = {
        .version = REDISMODULE_TYPE_METHOD_VERSION,
        .rdb_load = SpredisDocRDBLoad,
        .rdb_save = SpredisDocRDBSave,
        .aof_rewrite = SpredisDocRewriteFunc,
        .free = SpredisDocFreeCallback
    };

    SPREDISMODULE_TYPES[SPDOCTYPE] = RedisModule_CreateDataType(ctx, "DoCvSZTSS",
        SPREDISDHASH_ENCODING_VERSION, &dtm);


    if (SPREDISMODULE_TYPES[SPDOCTYPE] == NULL) return REDISMODULE_ERR;

    // if (SPDBLTYPE == NULL) return REDISMODULE_ERR;
    return REDISMODULE_OK;
}



