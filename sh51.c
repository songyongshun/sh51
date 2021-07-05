/**************************************************************************************
���򵥵�MCU51�Ĵ���shell.
name it minishell.

  
***************************************************************************************/

#include "reg52.h"			 //���ļ��ж����˵�Ƭ����һЩ���⹦�ܼĴ���

typedef unsigned int u16;	  //���������ͽ�����������
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
* ������         :UsartInit()
* ��������		   :���ô���
* ����           : ��
* ���         	 : ��
*******************************************************************************/
void UsartInit()
{
	SCON=0X50;			//����Ϊ������ʽ1; 0101 0000, REN=1, SM1=1
	TMOD=0X20;			//���ü�����������ʽ2 ; 0010 0000 ;��ʱ��1
	PCON=0X80;			//�����ʼӱ�, ���λSMOD, ���ӱ���2400=11059200/32/12/12
	TH1=0XF4;				//��������ʼֵ���ã�ע�Ⲩ������4800��,1111 0011, F3
	TL1=0XF4;         
	ES=1;						//�򿪽����ж�
	EA=1;						//�����ж�
	TR1=1;					//�򿪼�����
}

u8 i;
u8 flag;
u8 cmd_flag=0;
u8 ex_flag=0;
u8 rcmd1; //����ȫ�ֱ���
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
* �� �� ��       : main
* ��������		 : ������
* ��    ��       : ��
* ��    ��    	 : ��
*******************************************************************************/
void main()
{	
	UsartInit();  //	���ڳ�ʼ��
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
}

/*******************************************************************************
* ������         : Usart() interrupt 4
* ��������		  : ����ͨ���жϺ���
* ����           : ��
* ���         	 : ��
*******************************************************************************/
void Usart() interrupt 4
{
	u8 receiveData;
	//u8 pre;
	//pre=SBUF;
	ES=0;
	receiveData=SBUF;//��ȥ���յ�������, �������RI=1.
	RI = 0;//��������жϱ�־λ
	SBUF=receiveData;//�����յ������ݷ��뵽���ͼĴ���
	while(!TI);			 //�ȴ������������
	TI=0;			     //���������ɱ�־λ
	flag=0;  //ֹͣˢ����ʾ��
	cmd_flag=cmd_flag+1;
	if(cmd_flag==1)
	{
		rcmd1=receiveData;
		if(receiveData==0X0D)
		{
			type_enter();
			print_nf();
			type_enter();
			print_header();
			flag=1; //ˢ��ǰ���ַ�
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
			flag=1; //ˢ��ǰ���ַ�
			cmd_flag=0;

		}
	}
	if(cmd_flag==3) //��ô���ܵ�������?�ƺ�û��ֹͣ�Ķ���
	{
		type_enter();
		if(receiveData==0X0D) //�س�����
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

			flag=1; //ˢ��ǰ���ַ�
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

