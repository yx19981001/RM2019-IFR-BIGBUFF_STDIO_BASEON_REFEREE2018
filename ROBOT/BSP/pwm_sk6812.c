#include "pwm_sk6812.h"

//���ò���
#define SK6812BIT_HIGH 3
#define SK6812BIT_LOW 1
const u8 SK6812BitDef[2] ={SK6812BIT_LOW,SK6812BIT_HIGH};
#define SK6812_NUMS 350
#define SK6812_SIZE 24*SK6812_NUMS+1	//���һbitΪreset��ƽ	//������һ��ʵ��˼·��ʹ��DMA��������ж� ���ڸ��ж��н�CCR�Ĵ�����0

void Sk6812_Init(void)
{
	PWM2_Init();
	PWM2_1_DMA_Init();
	PWM2_2_DMA_Init();
	TIM_Cmd(TIM2, ENABLE);  /* ʹ��TIM2 */
}

void PWM2_Init(void)
{
	GPIO_InitTypeDef          gpio;
	TIM_TimeBaseInitTypeDef   tim;
	TIM_OCInitTypeDef         oc;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA ,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB ,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);   

	gpio.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;	//CH1,2
	gpio.GPIO_Mode = GPIO_Mode_AF;
	gpio.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA,&gpio);

	gpio.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;	//CH3,4
	gpio.GPIO_Mode = GPIO_Mode_AF;
	gpio.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB,&gpio);

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_TIM2);//��ʱ��2 ͨ��1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource1,GPIO_AF_TIM2);//��ʱ��2 ͨ��2
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_TIM2);//��ʱ��2 ͨ��3
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_TIM2);//��ʱ��2 ͨ��4

	/* TIM2 */
	tim.TIM_Prescaler = 19-1;	//18OK
	tim.TIM_CounterMode = TIM_CounterMode_Up;
	tim.TIM_Period = 1*5;   //4->1us	//0.5HIGH 0.75LOW 0�룻 0.75HIGH 0.5LOW 1��
	tim.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM2,&tim);	//��ʼ��ʱ�������λ

	oc.TIM_OCMode = TIM_OCMode_PWM2;
	oc.TIM_OutputState = TIM_OutputState_Enable;
	oc.TIM_OutputNState = TIM_OutputState_Disable;
	oc.TIM_Pulse = 0;
	oc.TIM_OCPolarity = TIM_OCPolarity_Low;
	oc.TIM_OCNPolarity = TIM_OCPolarity_High;
	oc.TIM_OCIdleState = TIM_OCIdleState_Reset;
	oc.TIM_OCNIdleState = TIM_OCIdleState_Set;

	TIM_OC1Init(TIM2,&oc);//��ʱ��5 ͨ��1
	TIM_OC2Init(TIM2,&oc);//��ʱ��5 ͨ��2
	TIM_OC3Init(TIM2,&oc);//��ʱ��5 ͨ��3
	TIM_OC4Init(TIM2,&oc);//��ʱ��5 ͨ��4

	TIM_OC1PreloadConfig(TIM2,TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM2,TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM2,TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM2,TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM2,ENABLE);

	TIM_Cmd(TIM2,DISABLE);
}

u32 Pwm2_1_DMABuffer[SK6812_SIZE]={0};
void PWM2_1_DMA_Init(void)
{
	NVIC_InitTypeDef nvic;
    DMA_InitTypeDef dma;
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);   //ʹ��DMA����
    delay_ms(5);

	DMA_Cmd(DMA1_Stream5, DISABLE);
	DMA_DeInit(DMA1_Stream5);
    dma.DMA_Channel = DMA_Channel_3;
    dma.DMA_PeripheralBaseAddr = (uint32_t)(&TIM2->CCR1);  /* DMA�������ַ *///DMA����TIM5-CCR3��ַ/
    dma.DMA_Memory0BaseAddr = (uint32_t)Pwm2_1_DMABuffer; ///* DMA�ڴ����ַ */DMA�ڴ����ַ/
    dma.DMA_DIR = DMA_DIR_MemoryToPeripheral;/* ���ݴ��䷽�򣬴��ڴ��ȡ���͵����� */
    dma.DMA_BufferSize = SK6812_SIZE;/* DMAͨ����DMA����Ĵ�С */
    dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;/* �����ַ�Ĵ������� */
    dma.DMA_MemoryInc = DMA_MemoryInc_Enable;/* �ڴ��ַ�Ĵ������� */
    dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;/* ���ݿ���Ϊ32λ */
    dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;/* ���ݿ���Ϊ32λ */
    dma.DMA_Mode = DMA_Mode_Normal;      ///* ����������ģʽ */��������������ģʽ
    dma.DMA_Priority = DMA_Priority_Medium;   //DMAͨ�� xӵ�������ȼ� 
    dma.DMA_FIFOMode = DMA_FIFOMode_Disable;   //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
    dma.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;//DMA_FIFOThreshold_HalfFull;//DMA_FIFOThreshold_1QuarterFull;
    dma.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    dma.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
		
	    /* ����DMA */
    DMA_Init(DMA1_Stream5, &dma);

    /*ʹ��TIM��DMA�ӿ� */
	//TIM_SelectCCDMA(TIM5,ENABLE);
	//TIM_DMAConfig(TIM5, TIM_DMABase_CCR3, TIM_DMABurstLength_16Bytes);
	
    //TIM_DMACmd(TIM2, TIM_DMA_Update, ENABLE);	/* �����Ҫ����ռ�ձȾͰ�����ȥ��ע�ͣ�ע����һ�У����޸���Ӧͨ�� */
	TIM_DMACmd(TIM2, TIM_DMA_CC1, ENABLE);	/* �����Ҫ����Ƶ�ʾͰ�����ȥ��ע�ͣ�ע����һ�У����޸���Ӧͨ�� */

    DMA_Cmd(DMA1_Stream5, DISABLE);	 /*��ʹ��DMA */  
	////TIM_Cmd(TIM2, ENABLE);  /* ʹ��TIM5 */ //4��ͨ��ֻʹ��һ��
}

u32 Pwm2_2_DMABuffer[SK6812_SIZE]={0};
void PWM2_2_DMA_Init(void)
{
	NVIC_InitTypeDef nvic;
    DMA_InitTypeDef dma;
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);   //ʹ��DMA����
    delay_ms(1);

	DMA_Cmd(DMA1_Stream6, DISABLE);
	DMA_DeInit(DMA1_Stream6);
    dma.DMA_Channel = DMA_Channel_3;
    dma.DMA_PeripheralBaseAddr = (uint32_t)(&TIM2->CCR2);  /* DMA�������ַ *///DMA����TIM5-CCR3��ַ/
    dma.DMA_Memory0BaseAddr = (uint32_t)Pwm2_2_DMABuffer; ///* DMA�ڴ����ַ */DMA�ڴ����ַ/
    dma.DMA_DIR = DMA_DIR_MemoryToPeripheral;/* ���ݴ��䷽�򣬴��ڴ��ȡ���͵����� */
    dma.DMA_BufferSize = SK6812_SIZE;/* DMAͨ����DMA����Ĵ�С */
    dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;/* �����ַ�Ĵ������� */
    dma.DMA_MemoryInc = DMA_MemoryInc_Enable;/* �ڴ��ַ�Ĵ������� */
    dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;/* ���ݿ���Ϊ32λ */
    dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;/* ���ݿ���Ϊ32λ */
    dma.DMA_Mode = DMA_Mode_Normal;      ///* ����������ģʽ */��������������ģʽ
    dma.DMA_Priority = DMA_Priority_Medium;   //DMAͨ�� xӵ�������ȼ� 
    dma.DMA_FIFOMode = DMA_FIFOMode_Disable;   //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
    dma.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;//DMA_FIFOThreshold_HalfFull;//DMA_FIFOThreshold_1QuarterFull;
    dma.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    dma.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
		
	    /* ����DMA */
    DMA_Init(DMA1_Stream6, &dma);

    /*ʹ��TIM��DMA�ӿ� */
	//TIM_SelectCCDMA(TIM5,ENABLE);
	//TIM_DMAConfig(TIM5, TIM_DMABase_CCR3, TIM_DMABurstLength_16Bytes);
	
    //TIM_DMACmd(TIM2, TIM_DMA_Update, ENABLE);	/* �����Ҫ����ռ�ձȾͰ�����ȥ��ע�ͣ�ע����һ�У����޸���Ӧͨ�� */
	TIM_DMACmd(TIM2, TIM_DMA_CC2, ENABLE);	/* �����Ҫ����Ƶ�ʾͰ�����ȥ��ע�ͣ�ע����һ�У����޸���Ӧͨ�� */

    DMA_Cmd(DMA1_Stream6, DISABLE);	 /*��ʹ��DMA */  
	////TIM_Cmd(TIM2, ENABLE);  /* ʹ��TIM5 */	//4��ͨ�����ֻ��ʹ��һ��
}



u32 Pwm2_1_Dmasetcounter=0;
bool PWM2_1_DMA_Enable(void)	//DMA1_S5C3
{
	static u8 start_flag=0;	//�����һ���޷�����
	if(DMA_GetFlagStatus(DMA1_Stream5,DMA_FLAG_TCIF5)!= RESET || start_flag!=1)	//������ɱ�־�����ڴ���������ٴ����ý������ʱ��
	{
		start_flag=1;
		DMA_ClearFlag(DMA1_Stream5,DMA_IT_TCIF5);
		DMA_Cmd(DMA1_Stream5, DISABLE );
		DMA_SetCurrDataCounter(DMA1_Stream5,Pwm2_1_Dmasetcounter);	//SK6812_SIZE

		DMA_Cmd(DMA1_Stream5, ENABLE);
		//TIM_Cmd(TIM5, ENABLE);  /* ʹ��TIM3 */
		TIM2->EGR |= 0x00000001;	/* �������һ��ARRֵΪ0������Ϊ��ֹͣ��ʱ����io�ڵĲ��������ǲ�Ҫ������һ�㣺CNT��û��ֹͣ�����������ǲ�����ͣ���������û���ֶ������Ļ���������Ҫ��ÿ��dmaʹ��ʱ����һ�䣬��EGR���UGλ��1����������� */
		return true;
	}
	else
	{
		return false;
	}
}

u32 Pwm2_2_Dmasetcounter=0;
bool PWM2_2_DMA_Enable(void)	//DMA1_S6C3
{
	static u8 start_flag=0;	//�����һ���޷�����
	if(DMA_GetFlagStatus(DMA1_Stream6,DMA_FLAG_TCIF6)!= RESET || start_flag!=1)	//������ɱ�־�����ڴ���������ٴ����ý������ʱ��
	{
		start_flag=1;
		DMA_ClearFlag(DMA1_Stream6,DMA_IT_TCIF6);
		DMA_Cmd(DMA1_Stream6, DISABLE );
		DMA_SetCurrDataCounter(DMA1_Stream6,Pwm2_2_Dmasetcounter);	//SK6812_SIZE

		DMA_Cmd(DMA1_Stream6, ENABLE);
		//TIM_Cmd(TIM5, ENABLE);  /* ʹ��TIM3 */
		TIM2->EGR |= 0x00000001;	/* �������һ��ARRֵΪ0������Ϊ��ֹͣ��ʱ����io�ڵĲ��������ǲ�Ҫ������һ�㣺CNT��û��ֹͣ�����������ǲ�����ͣ���������û���ֶ������Ļ���������Ҫ��ÿ��dmaʹ��ʱ����һ�䣬��EGR���UGλ��1����������� */
		return true;
	}
	else
	{
		return false;
	}
}


bool PAGE1_UpdateColor(u8 colors[][3],u16 led_nums)	//GRB ��λ�ȷ�	//PWM2_1
{
	if(led_nums>SK6812_NUMS)	return false;
	for(int i=0;i<led_nums;i++)
	{
		for(int channel=SK6812_GREEN;channel<SK6812_BLUE+1;channel++)	//������for�ɺϲ�Ϊһ��
		{
			for(int bit=7;bit>=0;bit--)
			{
				Pwm2_1_DMABuffer[i*24+channel*8+(7-bit)]=SK6812BitDef[*(colors[i] + channel)>>bit&0x01];
			}
		}
	}
	Pwm2_1_DMABuffer[led_nums*24]=0;
	Pwm2_1_Dmasetcounter=led_nums*24+1;
	PWM2_1_DMA_Enable();
	return true;
}

bool PAGE2_UpdateColor(u8 colors[][3],u16 led_nums)	//GRB ��λ�ȷ�  //PWM2_2
{
	if(led_nums>SK6812_NUMS)	return false;
	for(int i=0;i<led_nums;i++)
	{
		for(int channel=SK6812_GREEN;channel<SK6812_BLUE+1;channel++)	//������for�ɺϲ�Ϊһ��
		{
			for(int bit=7;bit>=0;bit--)
			{
				Pwm2_2_DMABuffer[i*24+channel*8+(7-bit)]=SK6812BitDef[*(colors[i] + channel)>>bit&0x01];
			}
		}
	}
	Pwm2_2_DMABuffer[led_nums*24]=0;
	Pwm2_2_Dmasetcounter=led_nums*24+1;
	PWM2_2_DMA_Enable();
	return true;
}


//////void SK6812_TIM5_3_DMA_Init(void)
//////{
//////	NVIC_InitTypeDef nvic;
//////    DMA_InitTypeDef dma;
//////    
//////    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);   //ʹ��DMA����
//////    delay_ms(5);

//////	DMA_Cmd(DMA1_Stream0, DISABLE);
//////	DMA_DeInit(DMA1_Stream0);
//////    dma.DMA_Channel = DMA_Channel_6;
//////    dma.DMA_PeripheralBaseAddr = (uint32_t)(&TIM5->CCR3);  /* DMA�������ַ *///DMA����TIM5-CCR3��ַ/
//////    dma.DMA_Memory0BaseAddr = (uint32_t)Pwm5_3_DMABuffer; ///* DMA�ڴ����ַ */DMA�ڴ����ַ/
//////    dma.DMA_DIR = DMA_DIR_MemoryToPeripheral;/* ���ݴ��䷽�򣬴��ڴ��ȡ���͵����� */
//////    dma.DMA_BufferSize = SK6812_SIZE;/* DMAͨ����DMA����Ĵ�С */
//////    dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;/* �����ַ�Ĵ������� */
//////    dma.DMA_MemoryInc = DMA_MemoryInc_Enable;/* �ڴ��ַ�Ĵ������� */
//////    dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;/* ���ݿ���Ϊ32λ */
//////    dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;/* ���ݿ���Ϊ32λ */
//////    dma.DMA_Mode = DMA_Mode_Normal;      ///* ����������ģʽ */��������������ģʽ
//////    dma.DMA_Priority = DMA_Priority_Medium;   //DMAͨ�� xӵ�������ȼ� 
//////    dma.DMA_FIFOMode = DMA_FIFOMode_Disable;   //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
//////    dma.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;//DMA_FIFOThreshold_HalfFull;//DMA_FIFOThreshold_1QuarterFull;
//////    dma.DMA_MemoryBurst = DMA_MemoryBurst_Single;
//////    dma.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
//////		
//////	    /* ����DMA */
//////    DMA_Init(DMA1_Stream0, &dma);

//////    /*ʹ��TIM��DMA�ӿ� */
//////	//TIM_SelectCCDMA(TIM5,ENABLE);
//////	//TIM_DMAConfig(TIM5, TIM_DMABase_CCR3, TIM_DMABurstLength_16Bytes);
//////	
//////    TIM_DMACmd(TIM5, TIM_DMA_Update, ENABLE);	/* �����Ҫ����ռ�ձȾͰ�����ȥ��ע�ͣ�ע����һ�У����޸���Ӧͨ�� */
//////	//TIM_DMACmd(TIM3, TIM_DMA_CC1, ENABLE);	/* �����Ҫ����Ƶ�ʾͰ�����ȥ��ע�ͣ�ע����һ�У����޸���Ӧͨ�� */

//////    DMA_Cmd(DMA1_Stream0, DISABLE);	 /*��ʹ��DMA */  
//////	TIM_Cmd(TIM5, ENABLE);  /* ʹ��TIM5 */
//////}

//////u32 DMAsetcounter=0;
//////bool PWM5_3_DMA_Enable(void)
//////{
//////	static u8 start_flag=0;
//////	if(DMA_GetFlagStatus(DMA1_Stream0,DMA_FLAG_TCIF0)!= RESET || start_flag!=1)	//������ɱ�־�����ڴ���������ٴ����ý������ʱ��
//////	{
//////		start_flag=1;
//////		DMA_ClearFlag(DMA1_Stream0,DMA_IT_TCIF0);
//////		DMA_Cmd(DMA1_Stream0, DISABLE );
//////		DMA_SetCurrDataCounter(DMA1_Stream0,DMAsetcounter);	//SK6812_SIZE

//////		DMA_Cmd(DMA1_Stream0, ENABLE);
//////		//TIM_Cmd(TIM5, ENABLE);  /* ʹ��TIM3 */
//////		TIM5->EGR |= 0x00000001;	/* �������һ��ARRֵΪ0������Ϊ��ֹͣ��ʱ����io�ڵĲ��������ǲ�Ҫ������һ�㣺CNT��û��ֹͣ�����������ǲ�����ͣ���������û���ֶ������Ļ���������Ҫ��ÿ��dmaʹ��ʱ����һ�䣬��EGR���UGλ��1����������� */
//////		return true;
//////	}
//////	else
//////	{
//////		return false;
//////	}
//////}

//////bool PAGE1_UpdateColor(u8 colors[][3],u16 led_nums)	//GRB ��λ�ȷ�
//////{
//////	if(led_nums>SK6812_NUMS)	return false;
//////	for(int i=0;i<led_nums;i++)
//////	{
//////		for(int channel=SK6812_GREEN;channel<SK6812_BLUE+1;channel++)	//������for�ɺϲ�Ϊһ��
//////		{
//////			for(int bit=7;bit>=0;bit--)
//////			{
//////				Pwm5_3_DMABuffer[i*24+channel*8+(7-bit)]=SK6812BitDef[*(colors[i] + channel)>>bit&0x01];
//////			}
//////		}
//////	}
//////	Pwm5_3_DMABuffer[led_nums*24]=0;
//////	DMAsetcounter=led_nums*24+1;
//////	PWM5_3_DMA_Enable();
//////	return true;
//////}