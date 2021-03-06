#include "protect.h"
#include "control.h"
/*
该文件用途：提供传感器冗余算法，传感器切换，以及车体实时状态检测，保护状态切换
预定义功能：
1.云台姿态，底盘姿态多来源冗余算法
2.车体误操作检测及有效制止
3.车体各模块在线检测检错
4.防疯防抢防盗
5.待续...
增加一个记录帧率、周期的运算？
*/

#define LOST_THRESHOLD 10

Error_check_t Error_Check={LOST_CYCLE,{0},{0}};

u8 Error_check_workstate=1;

s16 t_error_record=0;
void LostCountAdd(u16* lostcount)	//无需改变
{
	if(*lostcount<0xFFFE)
	(*lostcount)++;
}

void LostCountFeed(u16* lostcount)	//无需改变
{
	*lostcount=0;
}

u8 LostCountCheck(u16 lostcount,u8* statu,const u16 cycle)	//无需改变
{
	if(lostcount>LOST_THRESHOLD*cycle)
		*statu=1;
	else
		*statu=0;
	return *statu;
}

s32 test_error_Satrt=0;
void Check_Task(void)
{
	for(int i=0;i<LOST_TYPE_NUM;i++)	//无需改变
	{
		LostCountAdd(&Error_Check.count[i]);
		LostCountCheck(Error_Check.count[i],&Error_Check.statu[i],Error_Check.cycle[i]);
	}
	
	
	if(Error_check_workstate==1)	//工作状态
	{
		if(Error_Check.statu[LOST_IMU]==1)
		{
			test_error_Satrt=1;
			t_error_record=LOST_IMU;
			SetWorkState(ERROR_STATE);
		}
//		
		for(int i=5;i<LOST_TYPE_NUM-1;i++)	//电机比控更重要
		{
			if(Error_Check.statu[i]==1)	//&&i!=LOST_BULLETROTATE1
			{
				test_error_Satrt=-1;
				t_error_record=i;
				SetWorkState(ERROR_STATE);
			}
				
		}

	}
	
	
	if(Error_Check.statu[LOST_DBUS]==1)
	{
		if(GetWorkState()==CHECK_STATE||GetWorkState()==PREPARE_STATE||GetWorkState()==CALI_STATE)
		{
			SetWorkState(LOST_STATE);	//启动时没有遥控信号的选择
		}
		else
		{
			SetWorkState(PROTECT_STATE);
		}
		
	}

}




