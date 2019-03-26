
#ifndef __APP_SPI_H
#define __APP_SPI_H

#include "bsp_spi.h"

#define SPI1_DEV1 		1	 		 
#define SPI1_DEV2			2		 		  
#define SPI1_DEV3 		3		

#define SPI2_DEV1 		4	 		 
#define SPI2_DEV2			5		 		  
#define SPI2_DEV3 		6		

#define SPI3_DEV1 		7	 		 
#define SPI3_DEV2			8		 		  
#define SPI3_DEV3 		9		

#define	SPI1_DEV1_CS_PIN		PC4
#define	SPI1_DEV2_CS_PIN		PC4
#define	SPI1_DEV3_CS_PIN		PC4
#define	SPI2_DEV1_CS_PIN		PC4
#define	SPI2_DEV2_CS_PIN		PC4
#define	SPI2_DEV3_CS_PIN		PC4
#define	SPI3_DEV1_CS_PIN		PC4
#define	SPI3_DEV2_CS_PIN		PC4
#define	SPI3_DEV3_CS_PIN		PC4


#define SPI_DEV_TMC429 		 				SPI1_DEV1	 		                     				
#define SPI1_DEV1_SELECT()        GPIOC->BRR=BIT4
#define SPI1_DEV1_DESELECT()      GPIOC->BSRR=BIT4

#define SPI1_DEV2_SELECT()    		    
#define SPI1_DEV2_DESELECT()  			   
#define SPI1_DEV3_SELECT()     				   
#define SPI1_DEV3_DESELECT()  	

#define SPI2_DEV1_SELECT()    		    
#define SPI2_DEV1_DESELECT()  			   
#define SPI2_DEV2_SELECT()    		    
#define SPI2_DEV2_DESELECT()
#define SPI2_DEV3_SELECT()    		    
#define SPI2_DEV3_DESELECT()  

#define SPI3_DEV1_SELECT()    		    
#define SPI3_DEV1_DESELECT()  		
#define SPI3_DEV2_SELECT()    		    
#define SPI3_DEV2_DESELECT()  	
#define SPI3_DEV3_SELECT()    		    
#define SPI3_DEV3_DESELECT()  	


void spi_hw_init(void);

UCHAR ReadWriteSPI1(UCHAR DeviceNumber, UCHAR aTxBuffer, UCHAR LastTransfer);
UCHAR ReadWriteSPI2(UCHAR DeviceNumber, UCHAR aTxBuffer, UCHAR LastTransfer);
UCHAR ReadWriteSPI3(UCHAR DeviceNumber, UCHAR aTxBuffer, UCHAR LastTransfer);

#endif 


