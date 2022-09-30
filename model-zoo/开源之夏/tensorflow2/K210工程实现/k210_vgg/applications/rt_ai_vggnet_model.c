#include <rt_ai.h>
#include <backend_k210_kpu.h>
#include <rt_ai_vggnet_model.h>
#include <kpu.h>

extern unsigned char vggnet_kmodel[];

/* based on k210 */
#define RT_AI_VGGNET_INFO    {       \
    RT_AI_VGGNET_IN_NUM,             \
    RT_AI_VGGNET_OUT_NUM,            \
    RT_AI_VGGNET_IN_SIZE_BYTES,      \
    RT_AI_VGGNET_OUT_SIZE_BYTES,     \
    RT_AI_VGGNET_WORK_BUFFER_BYTES,  \
    ALLOC_INPUT_BUFFER_FLAG                 \
}

#define RT_AI_VGGNET_HANDLE  {         \
    .info   =     RT_AI_VGGNET_INFO    \
}

#define RT_K210_AI_VGGNET   {   \
    .parent         = RT_AI_VGGNET_HANDLE,   \
    .model          = vggnet_kmodel, \
    .dmac           = DMAC_CHANNEL5,        \
}

static struct k210_kpu rt_k210_ai_vggnet = RT_K210_AI_VGGNET;

static int rt_k210_ai_vggnet_init(){
    rt_ai_register(RT_AI_T(&rt_k210_ai_vggnet),RT_AI_VGGNET_MODEL_NAME,0,backend_k210_kpu,&rt_k210_ai_vggnet);
    return 0;
}

INIT_APP_EXPORT(rt_k210_ai_vggnet_init);
