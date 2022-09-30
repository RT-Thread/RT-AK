#ifndef __RT_AI_RESNET_MODEL_H
#define __RT_AI_RESNET_MODEL_H

/* model info ... */

// model name
#define RT_AI_RESNET_MODEL_NAME			"resnet"

#define RT_AI_RESNET_WORK_BUFFER_BYTES	(84992)

#define AI_RESNET_DATA_WEIGHTS_SIZE		(1493120) //unused

#define RT_AI_RESNET_BUFFER_ALIGNMENT		(4)

#define RT_AI_RESNET_IN_NUM				(1)

#define RT_AI_RESNET_IN_1_SIZE			(1 * 3 * 32 * 32)
#define RT_AI_RESNET_IN_1_SIZE_BYTES		((1 * 3 * 32 * 32) * 4)
#define RT_AI_RESNET_IN_SIZE_BYTES		{	\
	((1 * 3 * 32 * 32) * 4) ,	\
}

#define RT_AI_RESNET_IN_TOTAL_SIZE_BYTES	((1 * 3 * 32 * 32) * 4)


#define RT_AI_RESNET_OUT_NUM				(1)

#define RT_AI_RESNET_OUT_1_SIZE			(1 * 10)
#define RT_AI_RESNET_OUT_1_SIZE_BYTES		((1 * 10) * 4)
#define RT_AI_RESNET_OUT_SIZE_BYTES		{	\
	((1 * 10) * 4) ,	\
}

#define RT_AI_RESNET_OUT_TOTAL_SIZE_BYTES	((1 * 10) * 4)



#define RT_AI_RESNET_TOTAL_BUFFER_SIZE		//unused

#endif	//end
