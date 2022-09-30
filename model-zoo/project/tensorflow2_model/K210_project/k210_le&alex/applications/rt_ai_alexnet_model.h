#ifndef __RT_AI_ALEXNET_MODEL_H
#define __RT_AI_ALEXNET_MODEL_H

/* model info ... */

// model name
#define RT_AI_ALEXNET_MODEL_NAME			"alexnet"

#define RT_AI_ALEXNET_WORK_BUFFER_BYTES	(16128)

#define AI_ALEXNET_DATA_WEIGHTS_SIZE		(1493120) //unused

#define RT_AI_ALEXNET_BUFFER_ALIGNMENT		(4)

#define RT_AI_ALEXNET_IN_NUM				(1)

#define RT_AI_ALEXNET_IN_1_SIZE			(1 * 3 * 32 * 32)
#define RT_AI_ALEXNET_IN_1_SIZE_BYTES		((1 * 3 * 32 * 32) * 4)
#define RT_AI_ALEXNET_IN_SIZE_BYTES		{	\
	((1 * 3 * 32 * 32) * 4) ,	\
}

#define RT_AI_ALEXNET_IN_TOTAL_SIZE_BYTES	((1 * 3 * 32 * 32) * 4)


#define RT_AI_ALEXNET_OUT_NUM				(1)

#define RT_AI_ALEXNET_OUT_1_SIZE			(1 * 10)
#define RT_AI_ALEXNET_OUT_1_SIZE_BYTES		((1 * 10) * 4)
#define RT_AI_ALEXNET_OUT_SIZE_BYTES		{	\
	((1 * 10) * 4) ,	\
}

#define RT_AI_ALEXNET_OUT_TOTAL_SIZE_BYTES	((1 * 10) * 4)



#define RT_AI_ALEXNET_TOTAL_BUFFER_SIZE		//unused

#endif	//end
