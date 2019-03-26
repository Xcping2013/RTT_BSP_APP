#include "bsp.h"
//#include "app_spi.h"
void spi_hw_init(void)
{
	MX_SPI1_Init();
	pinMode(&SPI1_DEV1_CS_PIN,GPIO_MODE_OUTPUT_PP);
	pinSet(&SPI1_DEV1_CS_PIN);
//	MX_SPI2_Init();
//	pinMode(&SPI2_DEV1_CS_PIN,PIN_MODE_OUTPUT_PP);
//	pinSet(&SPI2_DEV1_CS_PIN);
//	MX_SPI3_Init();
//	pinMode(&SPI3_DEV1_CS_PIN,PIN_MODE_OUTPUT_PP);
//	pinSet(&SPI3_DEV1_CS_PIN);
	
}

UCHAR ReadWriteSPI1(UCHAR DeviceNumber, UCHAR aTxBuffer, UCHAR LastTransfer)
{
	u8 aRxBuffer=0;
  switch(DeviceNumber)
  {
    case SPI1_DEV1:
				
				 SPI1_DEV1_SELECT();
			   HAL_SPI_TransmitReceive(&hspi1, &aTxBuffer, &aRxBuffer, 1, 5000);
				 if(LastTransfer) SPI1_DEV1_DESELECT();
				 return aRxBuffer;

		case SPI1_DEV2:
				
				 SPI1_DEV2_SELECT();
			   HAL_SPI_TransmitReceive(&hspi1, &aTxBuffer, &aRxBuffer, 1, 5000);
				 if(LastTransfer) SPI1_DEV2_DESELECT();
		
      return aRxBuffer;
		
		case SPI1_DEV3:
				
				 SPI1_DEV3_SELECT();
			   HAL_SPI_TransmitReceive(&hspi1, &aTxBuffer, &aRxBuffer, 1, 5000);
				 if(LastTransfer) SPI1_DEV3_DESELECT();
		
      return aRxBuffer;

    default:
      return 0;
  }
}
//
UCHAR ReadWriteSPI2(UCHAR DeviceNumber, UCHAR aTxBuffer, UCHAR LastTransfer)
{
	u8 aRxBuffer=0;
  switch(DeviceNumber)
  {
    case SPI2_DEV1:
				
				 SPI2_DEV1_SELECT();
			   HAL_SPI_TransmitReceive(&hspi2, &aTxBuffer, &aRxBuffer, 1, 5000);
				 if(LastTransfer) SPI2_DEV1_DESELECT();
				 return aRxBuffer;

		case SPI2_DEV2:
				
				 SPI2_DEV2_SELECT();
			   HAL_SPI_TransmitReceive(&hspi2, &aTxBuffer, &aRxBuffer, 1, 5000);
				 if(LastTransfer) SPI2_DEV2_DESELECT();
		
      return aRxBuffer;
		
		case SPI2_DEV3:
				
				 SPI2_DEV3_SELECT();
			   HAL_SPI_TransmitReceive(&hspi2, &aTxBuffer, &aRxBuffer, 1, 5000);
				 if(LastTransfer) SPI2_DEV3_DESELECT();
		
      return aRxBuffer;

    default:
      return 0;
  }
}
//
UCHAR ReadWriteSPI3(UCHAR DeviceNumber, UCHAR aTxBuffer, UCHAR LastTransfer)
{
	u8 aRxBuffer=0;
  switch(DeviceNumber)
  {
    case SPI3_DEV1:
				
				 SPI3_DEV1_SELECT();
			   HAL_SPI_TransmitReceive(&hspi3, &aTxBuffer, &aRxBuffer, 1, 5000);
				 if(LastTransfer) SPI3_DEV1_DESELECT();
				 return aRxBuffer;

		case SPI3_DEV2:
				
				 SPI3_DEV2_SELECT();
			   HAL_SPI_TransmitReceive(&hspi3, &aTxBuffer, &aRxBuffer, 1, 5000);
				 if(LastTransfer) SPI3_DEV2_DESELECT();
		
      return aRxBuffer;
		
		case SPI3_DEV3:
				
				 SPI3_DEV3_SELECT();
			   HAL_SPI_TransmitReceive(&hspi3, &aTxBuffer, &aRxBuffer, 1, 5000);
				 if(LastTransfer) SPI3_DEV3_DESELECT();
		
      return aRxBuffer;

    default:
      return 0;
  }
}
//
