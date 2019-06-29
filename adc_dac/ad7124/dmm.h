#ifndef  __DMM_H__
#define  __DMM_H__

#include "ad7124.h"

#define lenof(a) (sizeof(a)/sizeof((a)[0]))

//DMM_FUNC_TYPE

enum 
{
	DMM_FUNC_UNKNOW				,
		
	DMM_FUNC_DCV_100mV		,
	DMM_FUNC_DCV_1V				,
	DMM_FUNC_DCV_10V			,
	DMM_FUNC_DCV_25V			,

	DMM_FUNC_DCI_100uA		,	
	DMM_FUNC_DCI_1mA			,
	DMM_FUNC_DCI_100mA		,	
	DMM_FUNC_DCI_1A				,	
 	
	DMM_FUNC_OHM_10R_4W		,	
	DMM_FUNC_OHM_1K_4W		,	
	DMM_FUNC_OHM_10K_4W		,	
		
	DMM_FUNC_COUNT		
};
							   
#define FILTER_NONE				0
#define FILTER_5_AVAGE 		5
#define FILTER_10_AVAGE 	10
#define FILTER_25_AVAGE 	25
#define FILTER_50_AVAGE 	50

extern float KTable[ DMM_FUNC_COUNT ];
extern float CTable[ DMM_FUNC_COUNT ];

extern struct ad7124_dev my_ad7124;
extern struct ad7124_dev *ad7124_handler;
void ad7124_init(void);
double AD7124_ReadAverData(struct ad7124_dev *device);

void ad7124_getSetK(void);
void SetFuncViaADreg( int _Func );
void print_dmm_value( int _Func );

	
#endif//__DMM_H__

