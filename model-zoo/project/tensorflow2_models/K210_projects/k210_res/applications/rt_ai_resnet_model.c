#include <rt_ai.h>
#include <backend_k210_kpu.h>
#include <rt_ai_resnet_model.h>
#include <kpu.h>

extern unsigned char resnet_kmodel[];

/* based on k210 */
#define RT_AI_RESNET_INFO    {       \
    RT_AI_RESNET_IN_NUM,             \
    RT_AI_RESNET_OUT_NUM,            \
    RT_AI_RESNET_IN_SIZE_BYTES,      \
    RT_AI_RESNET_OUT_SIZE_BYTES,     \
    RT_AI_RESNET_WORK_BUFFER_BYTES,  \
    ALLOC_INPUT_BUFFER_FLAG                 \
}

#define RT_AI_RESNET_HANDLE  {         \
    .info   =     RT_AI_RESNET_INFO    \
}

#define RT_K210_AI_RESNET   {   \
    .parent         = RT_AI_RESNET_HANDLE,   \
    .model          = resnet_kmodel, \
    .dmac           = DMAC_CHANNEL5,        \
}

static struct k210_kpu rt_k210_ai_resnet = RT_K210_AI_RESNET;

static int rt_k210_ai_resnet_init(){
    rt_ai_register(RT_AI_T(&rt_k210_ai_resnet),RT_AI_RESNET_MODEL_NAME,0,backend_k210_kpu,&rt_k210_ai_resnet);
    return 0;
}

INIT_APP_EXPORT(rt_k210_ai_resnet_init);
