#ifndef __RTAK_LIB_TEST__
#define __RTAK_LIB_TEST__

#define ENDLESS

#define TEST_PASS(_yes)  rt_kprintf(":%d",_yes);
#define test_print(_type,_func,fmt , ...)  rt_kprintf("%-10s" ":%-30s" fmt ,_type,#_func ,##__VA_ARGS__);
#define TEST_ASSERT(_expr)  (_expr) ? 0:1 ;
// if(_expr)  \
// {   \
//     TEST_PASS(0); \
// } \
// else    \
// {   \
//     TEST_PASS(1); \
// }   \

#define RT_AI_TEST_TURE(_a) TEST_ASSERT(_a)

#define RT_AI_TEST_FALSE(_a)    TEST_ASSERT(!_a)  
  

#define RT_AI_TEST_NULL(_a)                     RT_AI_TEST_FALSE(_a)

#define RT_AI_TEST_NOT_NULL(_a)                 RT_AI_TEST_TURE(_a)

#define RT_AI_TEST_INT_EQUAL(_a , _b)   TEST_ASSERT(_a == _b)  

#define RT_AI_TEST_INT_NOT_EQUAL(_a , _b)   TEST_ASSERT(_a != _b)  

#define RT_AI_TEST_STR_EQUAL(_a , _b)   TEST_ASSERT(!rt_strcmp(_a,_b))  

#define RT_AI_TEST_STR_NOT_EQUAL(_a , _b)   TEST_ASSERT(rt_strcmp(_a,_b))  


#define RT_AI_TEST_IN_RANGE(_a , _min , _max)   TEST_ASSERT((( (_a>=_min) & (_a<=_max) )? 1 : 0))

#define RT_AI_TEST_NOT_IN_RANGE(_a , _min , _max)   TEST_ASSERT((( (_a>=_min) & (_a<=_max) )? 0 : 1))

#define RT_AI_RUN_TEST(_type,_func) \
test_print(_type,_func,":%d\n",_func() )  \

#endif

