#include "ds1302.h"

#define w_protect 	0x8e//写保护位地址
#define w_powerkz 	0x90//写充电方式地址
#define w_sec 	0x80 //写
#define w_min 	0x82
#define w_hour 	0x84
#define w_day 	0x86
#define w_mon 	0x88
#define w_week 	0x8a
#define w_year 	0x8c
#define r_sec 	0x81//读
#define r_min 	0x83
#define r_hour 	0x85
#define r_day 	0x87
#define r_mon 	0x89
#define r_week 	0x8b
#define r_year 	0x8d

void delayp (u16 p)
{  
 u16 i;
  for (i=0;i<p;i++) 
  {
  }
}

void send_b(u8 data)
{	u8 i;
	for(i=0;i<8;i++)
	{
		time_clk_0;
		if(data & 0x01)
			time_o_1;
		else	time_o_0;
		data>>=1;
    delayp(14);	//2019.11.22	
		time_clk_1;	
	  delayp(42);		
	}
}

void  wr_da(u8 addr,u8 da)
{	u8 dat,dat1,dat2;
	dat1=da/10;
  dat2=da%10;
	dat=(dat1<<4) | dat2;
	time_ce_0;
  delayp(14);
	time_clk_0;
	time_ce_1;
	send_b(addr);
	send_b(dat);
	time_ce_1;
	delayp(14);
}

void  wr_da_protect(u8 addr,u8 da)
{
	time_ce_0;
  delayp(14);
	time_clk_0;
	time_ce_1;
	send_b(addr);
	send_b(da);
	time_ce_1;
	delayp(14);
}

u8  rd_da(u8 addr)
{	u8 i,dat,dat1,dat2;
	dat=0;
	time_ce_0;
	time_clk_0;
	delayp(14);
	time_ce_1;
	send_b(addr);
  delayp(14);
  time_clk_1;	
  delayp(14);	//2019.11.22	 	 	
	for(i=0;i<8;i++)
	{
		time_clk_0;
    delayp(42);
		dat>>=1;
		if(time_i)
			dat|=0x80;		
		time_clk_1;
		delayp(14);	//2019.11.22	
	}
	time_ce_0;
	dat1=dat>>4;
	dat2=dat&0x0f;
	dat=dat1*10+dat2;
	return dat;
}

void correct_time(void)
{ 
	wr_da_protect(w_protect,0x00);
	wr_da(w_sec,TimeNow.second);
	wr_da(w_min,TimeNow.minute);
	wr_da(w_hour,TimeNow.hour);
	wr_da(w_day,TimeNow.date);
	wr_da(w_mon,TimeNow.month);
	wr_da(w_year,TimeNow.year);
	wr_da_protect(w_protect,0x80);
}

void write_time(void)
{ 
	if(SystemSet.para.start_sec <60  && SystemSet.para.start_min < 60   && SystemSet.para.start_hour < 24 &&
		 SystemSet.para.start_date <32 && SystemSet.para.start_month < 13 && SystemSet.para.start_year < 100)
	{
		wr_da_protect(w_protect,0x00);
		wr_da(w_sec,SystemSet.para.start_sec);
		wr_da(w_min,SystemSet.para.start_min);
		wr_da(w_hour,SystemSet.para.start_hour);
		wr_da(w_day,SystemSet.para.start_date);
		wr_da(w_mon,SystemSet.para.start_month);
		wr_da(w_year,SystemSet.para.start_year);
		wr_da_protect(w_protect,0x80);
	}
}

void read_time(void)
{    
	TimeNow.second=rd_da(r_sec) & 0x3f;
	TimeNow.minute=rd_da(r_min) & 0x3f;
	TimeNow.hour=rd_da(r_hour)  & 0x1f;
	TimeNow.date=rd_da(r_day)   & 0x1f;
	TimeNow.month=rd_da(r_mon)  & 0x0f;
	TimeNow.year=rd_da(r_year)  & 0x7f;
}

void ds1302_init(void)
{
	read_time();
	wr_da_protect(w_protect,0x00);
	if(TimeNow.second > 60)
	  wr_da_protect(w_sec,0);
	else
		wr_da_protect(w_sec,TimeNow.second);
	wr_da_protect(w_powerkz,0xa6); //1010+充电二极管2：10（1：01），电阻4k：10
	wr_da_protect(w_protect,0x80);	
}



