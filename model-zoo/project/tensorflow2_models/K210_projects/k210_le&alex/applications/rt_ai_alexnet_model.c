#include <rt_ai.h>
#include <backend_k210_kpu.h>
#include <rt_ai_alexnet_model.h>
#include <kpu.h>

extern unsigned char alexnet_kmodel[];

/* based on k210 */
#define RT_AI_ALEXNET_INFO    {       \
    RT_AI_ALEXNET_IN_NUM,             \
    RT_AI_ALEXNET_OUT_NUM,            \
    RT_AI_ALEXNET_IN_SIZE_BYTES,      \
    RT_AI_ALEXNET_OUT_SIZE_BYTES,     \
    RT_AI_ALEXNET_WORK_BUFFER_BYTES,  \
    ALLOC_INPUT_BUFFER_FLAG                 \
}

#define RT_AI_ALEXNET_HANDLE  {         \
    .info   =     RT_AI_ALEXNET_INFO    \
}

#define RT_K210_AI_ALEXNET   {   \
    .parent         = RT_AI_ALEXNET_HANDLE,   \
    .model          = alexnet_kmodel, \
    .dmac           = DMAC_CHANNEL5,        \
}

static struct k210_kpu rt_k210_ai_alexnet = RT_K210_AI_ALEXNET;

static int rt_k210_ai_alexnet_init(){
    rt_ai_register(RT_AI_T(&rt_k210_ai_alexnet),RT_AI_ALEXNET_MODEL_NAME,0,backend_k210_kpu,&rt_k210_ai_alexnet);
    return 0;
}

INIT_APP_EXPORT(rt_k210_ai_alexnet_init);
