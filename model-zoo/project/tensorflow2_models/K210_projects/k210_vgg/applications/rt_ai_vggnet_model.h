#ifndef __RT_AI_VGGNET_MODEL_H
#define __RT_AI_VGGNET_MODEL_H

/* model info ... */

// model name
#define RT_AI_VGGNET_MODEL_NAME			"vggnet"

#define RT_AI_VGGNET_WORK_BUFFER_BYTES	(4352)

#define AI_VGGNET_DATA_WEIGHTS_SIZE		(1493120) //unused

#define RT_AI_VGGNET_BUFFER_ALIGNMENT		(4)

#define RT_AI_VGGNET_IN_NUM				(1)

#define RT_AI_VGGNET_IN_1_SIZE			(1 * 3 * 32 * 32)
#define RT_AI_VGGNET_IN_1_SIZE_BYTES		((1 * 3 * 32 * 32) * 4)
#define RT_AI_VGGNET_IN_SIZE_BYTES		{	\
	((1 * 3 * 32 * 32) * 4) ,	\
}

#define RT_AI_VGGNET_IN_TOTAL_SIZE_BYTES	((1 * 3 * 32 * 32) * 4)


#define RT_AI_VGGNET_OUT_NUM				(1)

#define RT_AI_VGGNET_OUT_1_SIZE			(1 * 10)
#define RT_AI_VGGNET_OUT_1_SIZE_BYTES		((1 * 10) * 4)
#define RT_AI_VGGNET_OUT_SIZE_BYTES		{	\
	((1 * 10) * 4) ,	\
}

#define RT_AI_VGGNET_OUT_TOTAL_SIZE_BYTES	((1 * 10) * 4)



#define RT_AI_VGGNET_TOTAL_BUFFER_SIZE		//unused

#endif	//end
