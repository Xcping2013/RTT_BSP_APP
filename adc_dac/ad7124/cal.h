#ifndef __CAL_H__
#define __CAL_H__

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
extern void Cal_RamToFlash( void );
// ��flash �е����ݼ��ص�
extern void Cal_FlashToRam( void );

#endif//__CAL_H__

