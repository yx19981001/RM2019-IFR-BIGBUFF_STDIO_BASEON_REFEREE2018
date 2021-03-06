#include "gpio.h"
#include "control_bigbuff.h"

/*************************
函数名：KEY_Configuration
函数功能：KEY的GPIO配置
函数输入：无
函数返回值： 无
描述：配置的引脚 PB0
*************************/
void KEY_Config(void)
{
   GPIO_InitTypeDef gpio;  
	 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
		
	 gpio.GPIO_Pin = GPIO_Pin_0 ;
	 gpio.GPIO_Mode = GPIO_Mode_IN;
	 gpio.GPIO_OType = GPIO_OType_OD;
	gpio.GPIO_PuPd = GPIO_PuPd_DOWN;
	 gpio.GPIO_Speed = GPIO_Speed_100MHz;
	 GPIO_Init(GPIOB,&gpio);
			
//	 GPIO_SetBits(GPIOB, GPIO_Pin_0);	//anjian
//	GPIO_ReadInputDataBit(GPIOB,0);
}

/*************************
函数名：LED_Configuration
函数功能：LED灯的GPIO配置
函数输入：无
函数返回值： 无
描述：配置的引脚 PC0；PC1
*************************/
void LED_Config(void)
{
   GPIO_InitTypeDef gpio;  
	 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
		
	 gpio.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_0 ;
	 gpio.GPIO_Mode = GPIO_Mode_OUT;
	 gpio.GPIO_OType = GPIO_OType_PP;
	 gpio.GPIO_Speed = GPIO_Speed_100MHz;
	 GPIO_Init(GPIOC,&gpio);
			
	 GPIO_SetBits(GPIOC, GPIO_Pin_1);	//绿 灭
	 GPIO_SetBits(GPIOC, GPIO_Pin_0);	//红 灭
}
/*************************
函数名：Laser_Config
函数功能：激光的GPIO配置
函数输入：无
函数返回值： 无
描述：配置的引脚 PC5
*************************/
void Laser_Config(void)
{
   GPIO_InitTypeDef gpio;
	 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	 gpio.GPIO_Pin = GPIO_Pin_5;
	 gpio.GPIO_Mode = GPIO_Mode_OUT;
	 gpio.GPIO_OType = GPIO_OType_PP;
	 gpio.GPIO_Speed = GPIO_Speed_100MHz;
	 GPIO_Init(GPIOC, &gpio);
	 GPIO_ResetBits(GPIOC, GPIO_Pin_5);
}

void Plate_GPIO_Init(void)
{
	GPIO_InitTypeDef gpio;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	
	gpio.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_6 | GPIO_Pin_7;
	gpio.GPIO_Mode = GPIO_Mode_OUT;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_Speed = GPIO_Speed_100MHz;
	
	GPIO_Init(GPIOA, &gpio);
	
	gpio.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_Init(GPIOB, &gpio);
	
	gpio.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_Init(GPIOC, &gpio);
	
}

