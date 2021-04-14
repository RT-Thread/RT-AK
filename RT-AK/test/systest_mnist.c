#if 0
#include <rt_ai_mnist_model.h>
#include <rt_ai.h>
#include <rtak_lib_test.h>
#include <systest_mnist_input.h>
#include <rt_ai_log.h>
#include <backend_cubeai.h>
#define _model_name MNIST
#define _MODEL_NAME(_name) RT_AI_##_name##_MODEL_NAME
#define _WORK_BUFFER_BYTES(_name) RT_AI_##_name##_WORK_BUFFER_BYTES

#define _IN_NUM(_name) RT_AI_##_name##_IN_NUM
#define _IN_SIZE(_name, _i) RT_AI_##_name##_IN_##_i##_SIZE
#define _IN_SIZE_BYTES(_name, _i) RT_AI_##_name##_IN_##_i##_SIZE_BYTES
#define _IN_TOTAL_SIZE_BYTES(_name) RT_AI_##_name##_IN_TOTAL_SIZE_BYTES

#define _OUT_NUM(_name) RT_AI_##_name##_OUT_NUM
#define _OUT_SIZE(_name, _i) RT_AI_##_name##_OUT_##_i##_SIZE
#define _OUT_SIZE_BYTES(_name, _i) RT_AI_##_name##_OUT_##_i##_SIZE_BYTES
#define _OUT_TOTAL_SIZE_BYTES(_name) RT_AI_##_name##_OUT_TOTAL_SIZE_BYTES

static rt_ai_buffer_t work_buffer[_WORK_BUFFER_BYTES(MNIST) + _IN_TOTAL_SIZE_BYTES(MNIST) + _OUT_TOTAL_SIZE_BYTES(MNIST)];
static rt_ai_t model = NULL;

static const float input_data0[] = MNIST_0_7;
static const float input_data1[] = MNIST_1_2;
static const float input_data2[] = MNIST_2_1;
static const float input_data3[] = MNIST_3_0;
static const float input_data4[] = MNIST_4_4;
static const int expected[] = {7, 2, 1, 0, 4};

static int done_flag = 0;
void user_callback(void *arg)
{

    AI_LOG("user_callback%d\n", *(int *)arg);
    return;
}
/* test rt_ai_find() */
int static test_rt_ai_find()
{
    model = rt_ai_find(_MODEL_NAME(MNIST));
    return RT_AI_TEST_NOT_NULL(model);
}

/* test rt_ai_init() */
int static test_rt_ai_init0()
{
    int result = 0;
    result = rt_ai_init(model, work_buffer);
    return RT_AI_TEST_INT_EQUAL(0, result);
}

/* test rt_ai_run() & rt_ai_output() */
int static test_rt_ai_run0()
{
    int result = 0;
    RT_AI_ASSERT_RETURN_0(model);
    rt_memcpy(RT_AI_INPUT(model, 0), input_data0, _IN_SIZE_BYTES(MNIST, 1));
    result = rt_ai_run(model, user_callback, &done_flag);
    return RT_AI_TEST_INT_EQUAL(0, result);
}

int static test_rt_ai_output0()
{
    RT_AI_ASSERT_RETURN_0(model);
    float *out = NULL;
    rt_uint32_t pred = 0;
    out = (float *)rt_ai_output(model, 0);
    for (int i = 0; i < _OUT_SIZE(MNIST, 1); i++)
    {
        if (out[pred] < out[i])
        {
            pred = i;
        }
    }
    return RT_AI_TEST_INT_EQUAL(expected[0], pred);
}

/* test rt_ai_init() */
int static test_rt_ai_init1()
{
    int result = 0;
    result = rt_ai_init(model, work_buffer);
    return RT_AI_TEST_INT_EQUAL(0, result);
}

/* test rt_ai_run() & rt_ai_output() */
int static test_rt_ai_run1()
{
    RT_AI_ASSERT_RETURN_0(model);
    int result = 0;
    // rt_memcpy(RT_AI_INPUT(model,0),input_data1,_IN_SIZE_BYTES(MNIST,1));
    rt_ai_config(model, CFG_INPUT_0_ADDR, (void *)input_data1);
    result = rt_ai_run(model, user_callback, &done_flag);
    return RT_AI_TEST_INT_EQUAL(0, result);
}

int static test_rt_ai_output1()
{
    RT_AI_ASSERT_RETURN_0(model);
    float *out = NULL;
    rt_uint32_t pred = 0;
    out = (float *)rt_ai_output(model, 0);
    for (int i = 0; i < _OUT_SIZE(MNIST, 1); i++)
    {
        if (out[pred] < out[i])
        {
            pred = i;
        }
    }
    return RT_AI_TEST_INT_EQUAL(expected[1], pred);
}

/* test rt_ai_init() */
int static test_rt_ai_init2()
{
    int result = 0;
    result = rt_ai_init(model, work_buffer);
    return RT_AI_TEST_INT_EQUAL(0, result);
}

/* test rt_ai_run() & rt_ai_output() */
int static test_rt_ai_run2()
{
    RT_AI_ASSERT_RETURN_0(model);
    int result = 0;
    rt_memcpy(RT_AI_INPUT(model, 0), input_data2, _IN_SIZE_BYTES(MNIST, 1));
    result = rt_ai_run(model, user_callback, &done_flag);
    return RT_AI_TEST_INT_EQUAL(0, result);
}

int static test_rt_ai_output2()
{
    RT_AI_ASSERT_RETURN_0(model);
    float *out = NULL;
    rt_uint32_t pred = 0;
    out = (float *)rt_ai_output(model, 0);
    for (int i = 0; i < _OUT_SIZE(MNIST, 1); i++)
    {
        if (out[pred] < out[2])
        {
            pred = i;
        }
    }
    return RT_AI_TEST_INT_EQUAL(expected[2], pred);
}

/* test rt_ai_run() & rt_ai_output() */
int static test_rt_ai_run3()
{
    RT_AI_ASSERT_RETURN_0(model);
    int result = 0;
    rt_memcpy(RT_AI_INPUT(model, 0), input_data3, _IN_SIZE_BYTES(MNIST, 1));
    result = rt_ai_run(model, user_callback, &done_flag);
    return RT_AI_TEST_INT_EQUAL(0, result);
}

int static test_rt_ai_output3()
{
    RT_AI_ASSERT_RETURN_0(model);
    float *out = NULL;
    rt_uint32_t pred = 0;
    out = (float *)rt_ai_output(model, 0);
    for (int i = 0; i < _OUT_SIZE(MNIST, 1); i++)
    {
        if (out[pred] < out[i])
        {
            pred = i;
        }
    }
    return RT_AI_TEST_INT_EQUAL(expected[3], pred);
}

/* test rt_ai_run() & rt_ai_output() */
int static test_rt_ai_run4()
{
    RT_AI_ASSERT_RETURN_0(model);
    int result = 0;
    rt_memcpy(RT_AI_INPUT(model, 0), input_data4, _IN_SIZE_BYTES(MNIST, 1));
    result = rt_ai_run(model, user_callback, &done_flag);
    return RT_AI_TEST_INT_EQUAL(0, result);
}

int static test_rt_ai_output4()
{
    RT_AI_ASSERT_RETURN_0(model);
    float *out = NULL;
    rt_uint32_t pred = 0;

    out = (float *)rt_ai_output(model, 0);
    for (int i = 0; i < _OUT_SIZE(MNIST, 1); i++)
    {
        if (out[pred] < out[i])
        {
            pred = i;
        }
    }
    return RT_AI_TEST_INT_EQUAL(expected[4], pred);
}

#include <rt_ai_runtime.h>

#define TEST_TYPE "systest"
int rtak_systest()
{

    AI_LOG("test start...\n");
    TIMESTAMP_INSERT("test_cost");
    RT_AI_RUN_TEST(TEST_TYPE, test_rt_ai_find);
    RT_AI_RUN_TEST(TEST_TYPE, test_rt_ai_init0);
    RT_AI_RUN_TEST(TEST_TYPE, test_rt_ai_run0);
    RT_AI_RUN_TEST(TEST_TYPE, test_rt_ai_output0);
    RT_AI_RUN_TEST(TEST_TYPE, test_rt_ai_init1);
    RT_AI_RUN_TEST(TEST_TYPE, test_rt_ai_run1);
    RT_AI_RUN_TEST(TEST_TYPE, test_rt_ai_output1);
    RT_AI_RUN_TEST(TEST_TYPE, test_rt_ai_init2);
    RT_AI_RUN_TEST(TEST_TYPE, test_rt_ai_run2);
    RT_AI_RUN_TEST(TEST_TYPE, test_rt_ai_output2);
    RT_AI_RUN_TEST(TEST_TYPE, test_rt_ai_run3);
    RT_AI_RUN_TEST(TEST_TYPE, test_rt_ai_output3);
    RT_AI_RUN_TEST(TEST_TYPE, test_rt_ai_run4);
    RT_AI_RUN_TEST(TEST_TYPE, test_rt_ai_output4);
    rt_ai_timestamp_t count = STATISTIC_TIME_INTERVAL("test_cost");
    AI_LOG("test cost:%d\n", count);
    return 0;
}
MSH_CMD_EXPORT
(rtak_systest, rtak systest);

#endif //compile this


