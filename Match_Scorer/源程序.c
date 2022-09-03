#include<reg51.h>
typedef unsigned char uchar;
typedef unsigned int  uint;
uchar code seg[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};  
//对应数码管显示数字0-9
int u_m=20,u_n=0,u_x=0,u_y=0,u_i=0,h=0,k;
//定义全局变量并初始化，m,n表示分钟与秒，x,y表示A,B两队分数
sbit P1_0=P1^0;
sbit P1_1=P1^1;
sbit P1_2=P1^2;
sbit P1_3=P1^3;
sbit P3_2=P3^2;
sbit P3_3=P3^3;
sbit P3_6=P3^6;
sbit P3_7=P3^7;
void delay(uint x)//定义延时程序,通过设置参数x,改变延时时间长度
{
	while(x--)
	{
		uint y;
		for(y=0;y<200;y++);
	}
}
/*显示子程序，变量i,j分别是分秒,变量x,y是A,B两队的分数*/
void display(int u_i,int u_j,int u_x,int u_y)
{		
	P2=~0xfe;   //刷新数码管显示时间的分钟十位,调用延时程序      			
	P0=~seg[u_i%100/10];
	delay(1);			//延时数码管点亮		
	P2=0x00;
	P0=0xff;
	P2=~0xfd;	//刷新刷管分钟位的个位				
	P0=~seg[u_i%10];
	delay(1);
	P2=0x00;
	P0=0xff;
	P2=~0xfb;	//刷新数码管秒位的十位		
	P0=~seg[u_j%100/10];
	delay(1);
	P2=0x00;
	P0=0xff;
	P2=~0xf7;	//刷新数码管秒位的个位		
	P0=~seg[u_j%10];
	delay(1);
	P2=0x00;
	P0=0xff;
	P2=~0xef;	//刷新数码管A对分数的十位			
	P0=~seg[u_x%100/10];
	delay(1);
	P2=0x00;
	P0=0xff;
	P2=~0xdf;	//刷新数码管A对分数的个位			
	P0=~seg[u_x%10];
	delay(1);
	P2=0x00;
	P0=0xff;
	P2=~0xbf;	//刷新数码管B队分数的十位		
	P0=~seg[u_y%100/10];
	delay(1);
	P2=0x00;
	P0=0xff;
	P2=~0x7f;	//刷新数码管B队分数的个位	
	P0=~seg[u_y%10];
	delay(1);
	P2=0x00;
	P0=0xff;
}
void t0() interrupt 1 //定义中断程序，调用定时器T0
{  
	TH0=0xb1;				//对定时器T0送入初值,TH0=0xb1   
	TL0=0x10;				//TL0=0X10定时器定时为20ms
	if(u_n<0)					//每20ms调用一次
	{ 
		u_n=59;
		u_m--;
		
	}
	u_i++;
	if(u_i==50)			//定义I为50,50*20ms=1s
	{
		u_n--;
		u_i=0;
	}
	display(u_m,u_n,u_x,u_y);//调用数码管显示程序,每20ms刷新
}
void int0() interrupt 0 //外部中断0的控制程序，每次按键按下，加2操作
{u_x+=2;if(u_x>98)u_x=98;}
void int1() interrupt 2 //外部中断1的控制程序，每次按键按下，加2操作
{u_y+=2;if(u_y>98)u_y=98;}
void timing()							//定义调时程序
{
if(P1_2==0)						//当按下P1_2时
{
delay(1);					//延时消抖
if(P1_2==0)
{
u_n++;			//秒位加1
if(u_n==60)//当秒位到60时
{
u_m++;//分位加1,秒变1
u_n=1;
}
} 
}if(P1_3==0)						//当按下P1_3时
{
delay(1);	
if(P1_3==0)
{
u_n--;//秒位减1
if(u_n<0)	          //当秒位减到0时,分位减1,秒位变59
{ u_n=59;
	u_m--;	
	
}}}}
void exchange()						  //定义比分交换程序
{
	int u_t;							  //定义中间变量
	if(P1_1==0)						  //当P1_1按下时
	{ 
		delay(1);					  //延时消抖
		while(P1_1==0);
		u_t=u_x;					//利用中间变量t,将A,B两队比分交换
		u_x=u_y;
		u_y=u_t;
		display(u_m,u_n,u_x,u_y);			//刷新数码管
	}
}
void suspend()							//定义暂停程序
{
	if(P1_0==0)								//当按下P1_0时
	{
	delay(1);						  //延时消抖
	while(P1_0==0);
	EA=0;							  //关中断，进入死循环
	while(1)
	{
	display(u_m,u_n,u_x,u_y);
	if(P1_0==0)		//当再次按下P1_0时，跳出死循环
	{	
	delay(1);
	while(P1_0==0)
	EA=1;
	break;	
	}}}}
void alertor()						 //定义报警程序
{  
	if(((u_m==0)&&(u_n==0))|(P3_2==0)|(P3_3==0)|(P1_0==0)|(P1_1==0)|(P1_2==0)|(P1_3==0))
	{
		P3_7=1;				
		P3_6=1;//P3_7和P3_6间隔输出高低电平，使蜂鸣器响
		delay(5);
		P3_7=0;
		P3_6=0;
		delay(5);
	}
}
void conclude()						//定义结束子程序
{
	M:	if(h==1)			//当时间条件满足时，关闭中断，使数码管保持状态显示
{
	EA=0;
	delay(20);
  u_m=0;
	u_n=0;
	P3_6=1;
	P3_7=1;
	delay(200);
	P3_7=0;
	P3_6=0;
	while(1)
	{
	 display(u_m,u_n,u_x,u_y);
	 exchange();
	 if(P1_0==0)			//当P1_0再次按下时，跳出循环
	{                     
		delay(1);
		while(P1_0==0)  
	{
		EA=1;
		u_m=20;    //重新赋予初始值,开始比赛
		u_n=0;
		h=0;
		goto M ;
	}		
	}
	}
}
}
void main()								//定义主程序
{
EX0=1;//外部中断0的分开关打开
EX1=1;//外部中断1的分开关打开
IT0=1;//外部中断0为下降沿触发
IT1=1;//外部中断1为下降沿触发
PX1=1;//外部中断1为高优先级
PX0=0;//外部中断0为低优先级
	TMOD=0x01;		 					//初始化程序
	EA=1;	//开关总中断打开			 
	ET0=1;
	P3_7=0;
	P0=0xff;
	P2=0x00;
	if(P1_0==0)		//第一次按下P1_0，单片机启动，显示初始值
	{			  				//由于调用了一次暂停功能，会显示初始值
		TH0=0xb1;
		TL0=0x10;
		TR0=1;								//启动定时器
		P3=0xff;	 
		P1=0x7f;
		P3_7=0;
		while(1)
		{ 		       
			timing();			    //调时子程序
			exchange();		       //比分交换程序
			suspend();			   //暂停程序
			alertor();			   //报警程序
			if((u_m==0)&&(u_n==1))	   //判断子程序结束调用条件
			{
				h=1; 
			}
			conclude();			      //结束子程序
		}
	}
}
