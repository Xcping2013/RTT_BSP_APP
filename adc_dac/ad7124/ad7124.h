/***************************************************************************//**
*   @file    AD7124.h
*   @brief   AD7124 header file.
*   @devices AD7124-4, AD7124-8
*
********************************************************************************
* Copyright 2015(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*  - Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*  - Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in
*    the documentation and/or other materials provided with the
*    distribution.
*  - Neither the name of Analog Devices, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*  - The use of this software may or may not infringe the patent rights
*    of one or more patent holders.  This license does not release you
*    from the requirement that you obtain separate licenses from these
*    patent holders to use this software.
*  - Use of the software either in source or binary form, must be run
*    on or directly connected to an Analog Devices Inc. component.
*
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT, MERCHANTABILITY
* AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef __AD7124_H__
#define __AD7124_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "platform_drivers.h"

/******************************************************************************/
/******************* Register map and register definitions ********************/
/******************************************************************************/

#define	AD7124_RW 1   /* Read and Write */
#define	AD7124_R  2   /* Read only */
#define AD7124_W  3   /* Write only */

/* AD7124 Register Map */
#define AD7124_COMM_REG      0x00
#define AD7124_STATUS_REG    0x00
#define AD7124_ADC_CTRL_REG  0x01
#define AD7124_DATA_REG      0x02
#define AD7124_IO_CTRL1_REG  0x03
#define AD7124_IO_CTRL2_REG  0x04
#define AD7124_ID_REG        0x05
#define AD7124_ERR_REG       0x06
#define AD7124_ERREN_REG     0x07
#define AD7124_CH0_MAP_REG   0x09
#define AD7124_CH1_MAP_REG   0x0A
#define AD7124_CH2_MAP_REG   0x0B
#define AD7124_CH3_MAP_REG   0x0C
#define AD7124_CH4_MAP_REG   0x0D
#define AD7124_CH5_MAP_REG   0x0E
#define AD7124_CH6_MAP_REG   0x0F
#define AD7124_CH7_MAP_REG   0x10
#define AD7124_CH8_MAP_REG   0x11
#define AD7124_CH9_MAP_REG   0x12
#define AD7124_CH10_MAP_REG  0x13
#define AD7124_CH11_MAP_REG  0x14
#define AD7124_CH12_MAP_REG  0x15
#define AD7124_CH13_MAP_REG  0x16
#define AD7124_CH14_MAP_REG  0x17
#define AD7124_CH15_MAP_REG  0x18
#define AD7124_CFG0_REG      0x19
#define AD7124_CFG1_REG      0x1A
#define AD7124_CFG2_REG      0x1B
#define AD7124_CFG3_REG      0x1C
#define AD7124_CFG4_REG      0x1D
#define AD7124_CFG5_REG      0x1E
#define AD7124_CFG6_REG      0x1F
#define AD7124_CFG7_REG      0x20
#define AD7124_FILT0_REG     0x21
#define AD7124_FILT1_REG     0x22
#define AD7124_FILT2_REG     0x23
#define AD7124_FILT3_REG     0x24
#define AD7124_FILT4_REG     0x25
#define AD7124_FILT5_REG     0x26
#define AD7124_FILT6_REG     0x27
#define AD7124_FILT7_REG     0x28
#define AD7124_OFFS0_REG     0x29
#define AD7124_OFFS1_REG     0x2A
#define AD7124_OFFS2_REG     0x2B
#define AD7124_OFFS3_REG     0x2C
#define AD7124_OFFS4_REG     0x2D
#define AD7124_OFFS5_REG     0x2E
#define AD7124_OFFS6_REG     0x2F
#define AD7124_OFFS7_REG     0x30
#define AD7124_GAIN0_REG     0x31
#define AD7124_GAIN1_REG     0x32
#define AD7124_GAIN2_REG     0x33
#define AD7124_GAIN3_REG     0x34
#define AD7124_GAIN4_REG     0x35
#define AD7124_GAIN5_REG     0x36
#define AD7124_GAIN6_REG     0x37
#define AD7124_GAIN7_REG     0x38

/* Communication Register bits */
#define AD7124_COMM_REG_WEN    (0 << 7)
#define AD7124_COMM_REG_WR     (0 << 6)
#define AD7124_COMM_REG_RD     (1 << 6)
#define AD7124_COMM_REG_RA(x)  ((x) & 0x3F)

/* Status Register bits */
#define AD7124_STATUS_REG_RDY          (1 << 7)
#define AD7124_STATUS_REG_ERROR_FLAG   (1 << 6)
#define AD7124_STATUS_REG_POR_FLAG     (1 << 4)
#define AD7124_STATUS_REG_CH_ACTIVE(x) ((x) & 0xF)

/* ADC_Control Register bits */
//15~13: 0

/*
Controls the SCLK inactive edge to DOUT/RDY high time. When DOUT_RDY_DEL is cleared, the delay is
10 ns minimum. When DOUT_RDY_DEL is set, the delay is increased to 100 ns minimum. This function is
useful when CS is tied low (the CS_EN bit is set to 0)
*/
#define AD7124_ADC_CTRL_REG_DOUT_RDY_DEL   (1 << 12)
/*
Continuous read of the data register. When this bit is set to 1 (and the data register is selected), the serial
interface is configured so that the data register can be continuously read; that is, the contents of the data
register are automatically placed on the DOUT pin when the SCLK pulses are applied after the RDY pin
goes low to indicate that a conversion is complete. The communications register does not have to be
written to for subsequent data reads. To enable continuous read, the CONT_READ bit is set. To disable
continuous read, write a read data command while the DOUT/ RDY pin is low. While continuous read is
enabled, the ADC monitors activity on the DIN line so that it can receive the instruction to disable
continuous read. Additionally, a reset occurs if 64 consecutive 1s occur on DIN; therefore, hold DIN low
until an instruction is written to the device
*/
#define AD7124_ADC_CTRL_REG_CONT_READ      (1 << 11)
/*
This bit enables the transmission of the status register contents after each data register read. When
DATA_STATUS is set, the contents of the status register are transmitted along with each data register
read. This function is useful when several channels are selected because the status register identifies the
channel to which the data register value corresponds
*/
#define AD7124_ADC_CTRL_REG_DATA_STATUS    (1 << 10)
/*
This bit controls the operation of DOUT/RDY during data read operations.
When CS_EN is cleared, the DOUT pin returns to being a RDY pin within nanoseconds of the SCLK
inactive edge (the delay is determined by the DOUT_RDY_DEL bit).
When set, the DOUT/RDY pin continues to output the LSB of the register being read until CS is taken
high. CS must frame all read operations when CS_EN is set. CS_EN must be set to use the diagnostic
functions SPI_WRITE_ERR, SPI_READ_ERR, and SPI_SCLK_CNT_ERR
*/
#define AD7124_ADC_CTRL_REG_CS_EN          (1 << 9)
/*
Internal reference voltage enable. When this bit is set, the internal reference is enabled and available at
the REFOUT pin. When this bit is cleared, the internal reference is disabled
*/
#define AD7124_ADC_CTRL_REG_REF_EN         (1 << 8)
/*
Power Mode Select. These bits select the power mode. The current consumption and output data rate
ranges are dependent on the power mode.
00 = low power.
01 = mid power.
10 = full power.
11 = full power
*/
#define AD7124_ADC_CTRL_REG_POWER_MODE(x)  (((x) & 0x3) << 6)		//--------------------------------------------
/*
0---Continuous conversion mode (default)
1---Single conversion mode
2---Standby mode
3---Power-down mode
4---Idle mode
5---Internal zero-scale (offset) calibration
6---Internal full-scale (gain) calibration
7---System zero-scale (offset) calibration
8---System full-scale (gain) calibration
*/
#define AD7124_ADC_CTRL_REG_MODE(x)        (((x) & 0xF) << 2)		//--------------------------------------------
/*
These bits select the clock source for the ADC. Either the on-chip 614.4 kHz clock can be used or an
external clock can be used. The ability to use an external clock allows several AD7124-4 devices to be
synchronized. Also, 50 Hz and 60 Hz rejection is improved when an accurate external clock drives the ADC.
00 = internal 614.4 kHz clock. The internal clock is not available at the CLK pin.
01 = internal 614.4 kHz clock. This clock is available at the CLK pin.
10 = external 614.4 kHz clock.
11 = external clock. The external clock is divided by 4
*/
#define AD7124_ADC_CTRL_REG_CLK_SEL(x)     (((x) & 0x3) << 0)

/* IO_Control_1 Register bits */
#define AD7124_IO_CTRL1_REG_GPIO_DAT2     (1 << 23)
#define AD7124_IO_CTRL1_REG_GPIO_DAT1     (1 << 22)
#define AD7124_IO_CTRL1_REG_GPIO_CTRL2    (1 << 19)
#define AD7124_IO_CTRL1_REG_GPIO_CTRL1    (1 << 18)
#define AD7124_IO_CTRL1_REG_PDSW          (1 << 15)
/*
These bits set the value of the excitation current for IOUT1.		//--------------------------------------------
000 = off.
001 = 50 uA
010 = 100 uA
011 = 250 uA
100 = 500 uA
101 = 750 uA
110 = 1000 uA
111 = 1000 uA
*/
#define AD7124_IO_CTRL1_REG_IOUT1(x)      (((x) & 0x7) << 11)		//--------------------------------------------
/*
These bits set the value of the excitation current for IOUT0.
000 = off.
001 = 50 uA
010 = 100 uA
011 = 250 uA
100 = 500 uA
101 = 750 uA
110 = 1000 uA
111 = 1000 uA
*/
#define AD7124_IO_CTRL1_REG_IOUT0(x)      (((x) & 0x7) << 8)
/*
Channel select bits for the excitation current for IOUT1.
0000 = IOUT1 is available on the AIN0 pin.
0001 = IOUT1 is available on the AIN1 pin.
0010 = reserved
0011 = reserved
0100 = IOUT1 is available on the AIN2 pin.
0101 = IOUT1 is available on the AIN3 pin.
0110 = reserved
0111 = reserved
1000 = reserved
1001 = reserved
1010 = IOUT1 is available on the AIN4 pin.
1011 = IOUT1 is available on the AIN5 pin.
1100 = reserved
1101 = reserved
1110 = IOUT1 is available on the AIN6 pin.
0111 = IOUT1 is available on the AIN7 pin.
*/
#define AD7124_IO_CTRL1_REG_IOUT_CH1(x)   (((x) & 0xF) << 4)		//--------------------------------------------
#define AD7124_IO_CTRL1_REG_IOUT_CH0(x)   (((x) & 0xF) << 0)

/* IO_Control_1 AD7124-8 specific bits */
#define AD7124_8_IO_CTRL1_REG_GPIO_DAT4     (1 << 23)
#define AD7124_8_IO_CTRL1_REG_GPIO_DAT3     (1 << 22)
#define AD7124_8_IO_CTRL1_REG_GPIO_DAT2     (1 << 21)
#define AD7124_8_IO_CTRL1_REG_GPIO_DAT1     (1 << 20)
#define AD7124_8_IO_CTRL1_REG_GPIO_CTRL4    (1 << 19)
#define AD7124_8_IO_CTRL1_REG_GPIO_CTRL3    (1 << 18)
#define AD7124_8_IO_CTRL1_REG_GPIO_CTRL2    (1 << 17)
#define AD7124_8_IO_CTRL1_REG_GPIO_CTRL1    (1 << 16)

/* IO_Control_2 Register bits */
/*
Enable the bias voltage on the AIN7 channel. When set, the internal bias voltage is available on AIN7.
14 VBIAS6 
*/
#define AD7124_IO_CTRL2_REG_GPIO_VBIAS7   (1 << 15)
#define AD7124_IO_CTRL2_REG_GPIO_VBIAS6   (1 << 14)
#define AD7124_IO_CTRL2_REG_GPIO_VBIAS5   (1 << 11)
#define AD7124_IO_CTRL2_REG_GPIO_VBIAS4   (1 << 10)
#define AD7124_IO_CTRL2_REG_GPIO_VBIAS3   (1 << 5)
#define AD7124_IO_CTRL2_REG_GPIO_VBIAS2   (1 << 4)
#define AD7124_IO_CTRL2_REG_GPIO_VBIAS1   (1 << 1)
#define AD7124_IO_CTRL2_REG_GPIO_VBIAS0   (1 << 0)

/* IO_Control_2 AD7124-8 specific bits */
#define AD7124_8_IO_CTRL2_REG_GPIO_VBIAS15  (1 << 15)
#define AD7124_8_IO_CTRL2_REG_GPIO_VBIAS14  (1 << 14)
#define AD7124_8_IO_CTRL2_REG_GPIO_VBIAS13  (1 << 13)
#define AD7124_8_IO_CTRL2_REG_GPIO_VBIAS12  (1 << 12)
#define AD7124_8_IO_CTRL2_REG_GPIO_VBIAS11  (1 << 11)
#define AD7124_8_IO_CTRL2_REG_GPIO_VBIAS10  (1 << 10)
#define AD7124_8_IO_CTRL2_REG_GPIO_VBIAS9   (1 << 9)
#define AD7124_8_IO_CTRL2_REG_GPIO_VBIAS8   (1 << 8)
#define AD7124_8_IO_CTRL2_REG_GPIO_VBIAS7   (1 << 7)
#define AD7124_8_IO_CTRL2_REG_GPIO_VBIAS6   (1 << 6)
#define AD7124_8_IO_CTRL2_REG_GPIO_VBIAS5   (1 << 5)
#define AD7124_8_IO_CTRL2_REG_GPIO_VBIAS4   (1 << 4)
#define AD7124_8_IO_CTRL2_REG_GPIO_VBIAS3   (1 << 3)
#define AD7124_8_IO_CTRL2_REG_GPIO_VBIAS2   (1 << 2)
#define AD7124_8_IO_CTRL2_REG_GPIO_VBIAS1   (1 << 1)
#define AD7124_8_IO_CTRL2_REG_GPIO_VBIAS0   (1 << 0)

/* ID Register bits */
#define AD7124_ID_REG_DEVICE_ID(x)   (((x) & 0xF) << 4)
#define AD7124_ID_REG_SILICON_REV(x) (((x) & 0xF) << 0)

/* Error Register bits */
#define AD7124_ERR_REG_LDO_CAP_ERR        (1 << 19)
#define AD7124_ERR_REG_ADC_CAL_ERR        (1 << 18)
#define AD7124_ERR_REG_ADC_CONV_ERR       (1 << 17)
#define AD7124_ERR_REG_ADC_SAT_ERR        (1 << 16)
#define AD7124_ERR_REG_AINP_OV_ERR        (1 << 15)
#define AD7124_ERR_REG_AINP_UV_ERR        (1 << 14)
#define AD7124_ERR_REG_AINM_OV_ERR        (1 << 13)
#define AD7124_ERR_REG_AINM_UV_ERR        (1 << 12)
#define AD7124_ERR_REG_REF_DET_ERR        (1 << 11)
#define AD7124_ERR_REG_DLDO_PSM_ERR       (1 << 9)
#define AD7124_ERR_REG_ALDO_PSM_ERR       (1 << 7)
#define AD7124_ERR_REG_SPI_IGNORE_ERR     (1 << 6)
#define AD7124_ERR_REG_SPI_SLCK_CNT_ERR   (1 << 5)
#define AD7124_ERR_REG_SPI_READ_ERR       (1 << 4)
#define AD7124_ERR_REG_SPI_WRITE_ERR      (1 << 3)
#define AD7124_ERR_REG_SPI_CRC_ERR        (1 << 2)
#define AD7124_ERR_REG_MM_CRC_ERR         (1 << 1)

/* Error_En Register bits */
#define AD7124_ERREN_REG_MCLK_CNT_EN           (1 << 22)
#define AD7124_ERREN_REG_LDO_CAP_CHK_TEST_EN   (1 << 21)
#define AD7124_ERREN_REG_LDO_CAP_CHK(x)        (((x) & 0x3) << 19)
#define AD7124_ERREN_REG_ADC_CAL_ERR_EN        (1 << 18)
#define AD7124_ERREN_REG_ADC_CONV_ERR_EN       (1 << 17)
#define AD7124_ERREN_REG_ADC_SAT_ERR_EN        (1 << 16)
#define AD7124_ERREN_REG_AINP_OV_ERR_EN        (1 << 15)
#define AD7124_ERREN_REG_AINP_UV_ERR_EN        (1 << 14)
#define AD7124_ERREN_REG_AINM_OV_ERR_EN        (1 << 13)
#define AD7124_ERREN_REG_AINM_UV_ERR_EN        (1 << 12)
#define AD7124_ERREN_REG_REF_DET_ERR_EN        (1 << 11)
#define AD7124_ERREN_REG_DLDO_PSM_TRIP_TEST_EN (1 << 10)
#define AD7124_ERREN_REG_DLDO_PSM_ERR_ERR      (1 << 9)
#define AD7124_ERREN_REG_ALDO_PSM_TRIP_TEST_EN (1 << 8)
#define AD7124_ERREN_REG_ALDO_PSM_ERR_EN       (1 << 7)
#define AD7124_ERREN_REG_SPI_IGNORE_ERR_EN     (1 << 6)
#define AD7124_ERREN_REG_SPI_SCLK_CNT_ERR_EN   (1 << 5)
#define AD7124_ERREN_REG_SPI_READ_ERR_EN       (1 << 4)
#define AD7124_ERREN_REG_SPI_WRITE_ERR_EN      (1 << 3)
#define AD7124_ERREN_REG_SPI_CRC_ERR_EN        (1 << 2)
#define AD7124_ERREN_REG_MM_CRC_ERR_EN         (1 << 1)

/* Channel Registers 0-15 bits */
#define AD7124_CH_MAP_REG_CH_ENABLE    (1 << 15)
/*
Setup select. These bits identify which of the eight setups are used to configure the ADC for this channel. A setup
comprises a set of four registers: analog configuration, output data rate/filter selection, offset register, and gain
register. All channels can use the same setup, in which case the same 3-bit value must be written to these bits on
all active channels. Alternatively, up to eight channels can be configured differently
*/
#define AD7124_CH_MAP_REG_SETUP(x)     (((x) & 0x7) << 12)
#define AD7124_CH_MAP_REG_AINP(x)      (((x) & 0x1F) << 5)	//Positive analog input AINP input select.
#define AD7124_CH_MAP_REG_AINM(x)      (((x) & 0x1F) << 0)	//Negative analog input AINM input select

/* Configuration Registers 0-7 bits */
/*
Polarity select bit. When this bit is set, bipolar operation is selected. When this bit is cleared, unipolar operation is
selected
*/
#define AD7124_CFG_REG_BIPOLAR     (1 << 11)
#define AD7124_CFG_REG_BURNOUT(x)  (((x) & 0x3) << 9)
#define AD7124_CFG_REG_REF_BUFP    (1 << 8)
#define AD7124_CFG_REG_REF_BUFM    (1 << 7)
#define AD7124_CFG_REG_AIN_BUFP    (1 << 6)
#define AD7124_CFG_REG_AINN_BUFM   (1 << 5)
/*
Reference source select bits. These bits select the reference source to use when converting on any channels using
this configuration register.
00 = REFIN1(+)/REFIN1(-).
01 = REFIN2(+)/REFIN2(-).
10 = internal reference.
11 = AVD*/
#define AD7124_CFG_REG_REF_SEL(x)  ((x) & 0x3) << 3
/*
000 1 ��2.5 V
001 2 ��1.25 V
010 4 ��625 mV
011 8 ��312.5 mV
100 16 ��156.25 mV
101 32 ��78.125 mV
110 64 ��39.06 mV
111 128 ��19.53 mV
*/
#define AD7124_CFG_REG_PGA(x)      (((x) & 0x7) << 0)

/* Filter Register 0-7 bits */
#define AD7124_FILT_REG_FILTER(x)         (((x) & 0x7) << 21)
#define AD7124_FILT_REG_REJ60             (1 << 20)
#define AD7124_FILT_REG_POST_FILTER(x)    (((x) & 0x7) << 17)
#define AD7124_FILT_REG_SINGLE_CYCLE      (1 << 16)
#define AD7124_FILT_REG_FS(x)             (((x) & 0x7FF) << 0)

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/*! Device register info */
struct ad7124_st_reg {
	int32_t addr;
	int32_t value;
	int32_t size;
	int32_t rw;
};

/*! AD7124 registers list*/
enum ad7124_registers {
	AD7124_Status = 0x00,
	AD7124_ADC_Control,
	AD7124_Data,
	AD7124_IOCon1,
	AD7124_IOCon2,
	AD7124_ID,
	AD7124_Error,
	AD7124_Error_En,
	AD7124_Mclk_Count,
	AD7124_Channel_0,
	AD7124_Channel_1,
	AD7124_Channel_2,
	AD7124_Channel_3,
	AD7124_Channel_4,
	AD7124_Channel_5,
	AD7124_Channel_6,
	AD7124_Channel_7,
	AD7124_Channel_8,
	AD7124_Channel_9,
	AD7124_Channel_10,
	AD7124_Channel_11,
	AD7124_Channel_12,
	AD7124_Channel_13,
	AD7124_Channel_14,
	AD7124_Channel_15,
	AD7124_Config_0,
	AD7124_Config_1,
	AD7124_Config_2,
	AD7124_Config_3,
	AD7124_Config_4,
	AD7124_Config_5,
	AD7124_Config_6,
	AD7124_Config_7,
	AD7124_Filter_0,
	AD7124_Filter_1,
	AD7124_Filter_2,
	AD7124_Filter_3,
	AD7124_Filter_4,
	AD7124_Filter_5,
	AD7124_Filter_6,
	AD7124_Filter_7,
	AD7124_Offset_0,
	AD7124_Offset_1,
	AD7124_Offset_2,
	AD7124_Offset_3,
	AD7124_Offset_4,
	AD7124_Offset_5,
	AD7124_Offset_6,
	AD7124_Offset_7,
	AD7124_Gain_0,
	AD7124_Gain_1,
	AD7124_Gain_2,
	AD7124_Gain_3,
	AD7124_Gain_4,
	AD7124_Gain_5,
	AD7124_Gain_6,
	AD7124_Gain_7,
	AD7124_REG_NO
};

/*
 * The structure describes the device and is used with the ad7124 driver.
 * @spi_desc: A reference to the SPI configuration of the device.
 * @regs: A reference to the register list of the device that the user must
 *       provide when calling the Setup() function.
 * @userCRC: Whether to do or not a cyclic redundancy check on SPI transfers.
 * @check_ready: When enabled all register read and write calls will first wait
 *               until the device is ready to accept user requests.
 * @spi_rdy_poll_cnt: Number of times the driver should read the Error register
 *                    to check if the device is ready to accept user requests,
 *                    before a timeout error will be issued.
 */
struct ad7124_dev {
	/* SPI */
	spi_desc		*spi_desc;
	/* Device Settings */
	struct ad7124_st_reg	*regs;
	int16_t use_crc;
	int16_t check_ready;
	int16_t spi_rdy_poll_cnt;
};

struct ad7124_init_param {
	/* SPI */
	spi_init_param		spi_init;
	/* Device Settings */
	struct ad7124_st_reg	*regs;
	int16_t spi_rdy_poll_cnt;
};

/******************************************************************************/
/******************* AD7124 Constants *****************************************/
/******************************************************************************/
#define AD7124_CRC8_POLYNOMIAL_REPRESENTATION 0x07 /* x8 + x2 + x + 1 */
#define AD7124_DISABLE_CRC 0
#define AD7124_USE_CRC 1

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/*! Reads the value of the specified register. */
int32_t ad7124_read_register(struct ad7124_dev *dev,
			     struct ad7124_st_reg* p_reg);

/*! Writes the value of the specified register. */
int32_t ad7124_write_register(struct ad7124_dev *dev,
			      struct ad7124_st_reg reg);

/*! Reads the value of the specified register without a device state check. */
int32_t ad7124_no_check_read_register(struct ad7124_dev *dev,
				      struct ad7124_st_reg* p_reg);

/*! Writes the value of the specified register without a device state check. */
int32_t ad7124_no_check_write_register(struct ad7124_dev *dev,
				       struct ad7124_st_reg reg);

/*! Resets the device. */
int32_t ad7124_reset(struct ad7124_dev *dev);

/*! Waits until the device can accept read and write user actions. */
int32_t ad7124_wait_for_spi_ready(struct ad7124_dev *dev,
				  uint32_t timeout);

/*! Waits until the device finishes the power-on reset operation. */
int32_t ad7124_wait_to_power_on(struct ad7124_dev *dev,
				uint32_t timeout);

/*! Waits until a new conversion result is available. */
int32_t ad7124_wait_for_conv_ready(struct ad7124_dev *dev,
				   uint32_t timeout);

/*! Reads the conversion result from the device. */
int32_t ad7124_read_data(struct ad7124_dev *dev,
			 int32_t* p_data);

/*! Computes the CRC checksum for a data buffer. */
uint8_t ad7124_compute_crc8(uint8_t* p_buf,
			    uint8_t buf_size);

/*! Updates the CRC settings. */
void ad7124_update_crcsetting(struct ad7124_dev *dev);

/*! Updates the device SPI interface settings. */
void ad7124_update_dev_spi_settings(struct ad7124_dev *dev);

/*! Initializes the AD7124. */
int32_t ad7124_setupold(struct ad7124_dev **device,
		     struct ad7124_init_param init_param);

int32_t ad7124_setup(struct ad7124_dev *device);

/*! Free the resources allocated by AD7124_Setup(). */
int32_t ad7124_remove(struct ad7124_dev *dev);

#endif /* __AD7124_H__ */
