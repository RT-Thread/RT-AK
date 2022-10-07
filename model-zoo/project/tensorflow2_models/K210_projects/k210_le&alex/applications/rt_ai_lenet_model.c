#include <rt_ai.h>
#include <backend_k210_kpu.h>
#include <rt_ai_lenet_model.h>
#include <kpu.h>

extern unsigned char lenet_kmodel[];

/* based on k210 */
#define RT_AI_LENET_INFO    {       \
    RT_AI_LENET_IN_NUM,             \
    RT_AI_LENET_OUT_NUM,            \
    RT_AI_LENET_IN_SIZE_BYTES,      \
    RT_AI_LENET_OUT_SIZE_BYTES,     \
    RT_AI_LENET_WORK_BUFFER_BYTES,  \
    ALLOC_INPUT_BUFFER_FLAG                 \
}

#define RT_AI_LENET_HANDLE  {         \
    .info   =     RT_AI_LENET_INFO    \
}

#define RT_K210_AI_LENET   {   \
    .parent         = RT_AI_LENET_HANDLE,   \
    .model          = lenet_kmodel, \
    .dmac           = DMAC_CHANNEL5,        \
}

static struct k210_kpu rt_k210_ai_lenet = RT_K210_AI_LENET;

static int rt_k210_ai_lenet_init(){
    rt_ai_register(RT_AI_T(&rt_k210_ai_lenet),RT_AI_LENET_MODEL_NAME,0,backend_k210_kpu,&rt_k210_ai_lenet);
    return 0;
}

INIT_APP_EXPORT(rt_k210_ai_lenet_init);
