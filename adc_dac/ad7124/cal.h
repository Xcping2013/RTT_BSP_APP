#ifndef __CAL_H__
#define __CAL_H__


#define USING_GAIN_AD_VALUE				1
//#define USING_GAIN_VOLTAGE_VALUE	1
// �õ�����У׼����
extern float Cal_GetK( int _Func );
// ���ñ���У׼����
extern void Cal_SetK( int _Func , float _K );

// �õ�����У׼����
extern float Cal_GetC( int _Func );
// ���ó���У׼����
extern void Cal_SetC( int _Func , float _C );

// ��У׼���һ��
extern void Cal_IdentifyRam( void );
// ��RAM�е�У׼���ݱ��浽Flash
void Cal_RamToFlash( void );
// ��flash �е����ݼ��ص�
extern void Cal_FlashToRam( void );


typedef struct
{
	float    				Gain[4];	
	float    				Offset[4];	
  unsigned char	  filterTimes;
}
DMM_KCTable;

extern DMM_KCTable DMM_CAL_Table;

void printGainOffset( void );

void DMM_CAL_KCTable_RamToEeprom(void);
void DMM_CAL_KCTable_EepromToRam(void);

void SetGainViaInputSignal( int _Func ,	int milliValue);
void SetOffsetViaInputSignal( int _Func ,	int milliValue);

#endif//__CAL_H__

