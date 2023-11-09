/**************************************************************************************
极简单的MCU51的串口shell.
name it minishell.

  
***************************************************************************************/

#include "reg52.h"			 //此文件中定义了单片机的一些特殊功能寄存器

typedef unsigned int u16;	  //对数据类型进行声明定义
typedef unsigned char u8;

u8 code table[]="STM89C52RC>";
u8 code help[]="ex: exit this COM";
u8 code nf[]="command not found";
u8 code logout[]="logout succefully";
u8 code error[]="error, length > 3";
void delay(u16 i)
{
	while(i--);	
}


/*******************************************************************************
* 函数名         :UsartInit()
* 函数功能		   :设置串口
* 输入           : 无
* 输出         	 : 无
*******************************************************************************/
void UsartInit()
{
	SCON=0X50;			//设置为工作方式1; 0101 0000, REN=1, SM1=1
	TMOD=0X20;			//设置计数器工作方式2 ; 0010 0000 ;定时器1
	PCON=0X80;			//波特率加倍, 最高位SMOD, 不加倍是2400=11059200/32/12/12
	TH1=0XF4;			//计数器初始值设置，注意严格计算这里是F3,有的MCU内部时钟不够快(比如我的STC89C52RC)，F4是正确的。
	TL1=0XF4;         
	ES=1;						//打开接收中断
	EA=1;						//打开总中断
	TR1=1;					//打开计数器
}

u8 i;
u8 flag;
u8 cmd_flag=0;
u8 ex_flag=0;
u8 rcmd1; //算是全局变量
u8 rcmd2;

void print_error()
{
	
	for(i=0;i<17;i++)
	{
		SBUF=error[i];
		while(!TI);
		TI=0;
	}

}

void type_enter()
{
	SBUF=0X0D;
	while(!TI);
	TI=0;
	SBUF=0X0A;
	while(!TI);
	TI=0;
}

void print_header()
{
	for(i=0;i<11;i++)
	{
		SBUF=table[i];
		while(!TI);
		TI=0;
	}
}

void print_nf()
{
	for(i=0;i<17;i++)
	{
		SBUF=nf[i];
		while(!TI);
		TI=0;
	}
}

void print_hp()
{
	for(i=0;i<17;i++)
	{
		SBUF=help[i];
		while(!TI);
		TI=0;
	}
}

void print_logout()
{
	for(i=0;i<17;i++)
	{
		SBUF=logout[i];
		while(!TI);
		TI=0;
	}
}

/*******************************************************************************
* 函 数 名       : main
* 函数功能		 : 主函数
* 输    入       : 无
* 输    出    	 : 无
*******************************************************************************/
void main()
{	
	UsartInit();  //	串口初始化
	ES=0;
	print_header();
	ES=1;
	flag=1;
	while(!ex_flag)
	{
		if(flag==1)
		{
			ES=0;
			SBUF=0X0D;
			while(!TI);
			TI=0;
			print_header();
			ES=1;
			delay(50000);
		}
	}
	SBUF=0X0D;
        while(!TI)
        TI=0;
//      delay(10000);
        print_logout();
        while(1); //不然会再次进入main();
}

/*******************************************************************************
* 函数名         : Usart() interrupt 4
* 函数功能		  : 串口通信中断函数
* 输入           : 无
* 输出         	 : 无
*******************************************************************************/
void Usart() interrupt 4
{
	u8 receiveData;
	//u8 pre;
	//pre=SBUF;
	ES=0;
	receiveData=SBUF;//出去接收到的数据, 结束后会RI=1.
	RI = 0;//清除接收中断标志位
	SBUF=receiveData;//将接收到的数据放入到发送寄存器
	while(!TI);			 //等待发送数据完成
	TI=0;			     //清除发送完成标志位
	flag=0;  //停止刷新提示符
	cmd_flag=cmd_flag+1;
	if(cmd_flag==1)
	{
		rcmd1=receiveData;
		if(receiveData==0X0D)
		{
			type_enter();
			// print_nf();
			type_enter();
			print_header();
			flag=1; //刷新前置字符
			cmd_flag=0;
		}
	}
	if(cmd_flag==2)
	{
		rcmd2=receiveData;
		if(receiveData==0X0D)
		{
			type_enter();
			print_nf();
			type_enter();
			print_header();
			flag=1; //刷新前置字符
			cmd_flag=0;

		}
	}
	if(cmd_flag==3) 
	{
		type_enter();
		if(receiveData==0X0D) //回车处理
		{	
			if(rcmd1==0X68 && rcmd2==0X70)
			{
				print_hp();
				type_enter();
				print_header();
			}
			else if (rcmd1==0X65 && rcmd2==0X78)
			{
				print_logout();
				//type_enter();
				//print_header();
				ex_flag=1;
			}
			else
			{
				print_nf();
				type_enter();
				print_header();
			}

			flag=1; //刷新前置字符
		}
		else
		{
			print_error();
			type_enter();
			print_header();
		}
		cmd_flag=0;
	}
	ES=1;
}

