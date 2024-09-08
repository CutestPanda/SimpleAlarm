//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//测试硬件：单片机STC89C52RC,晶振12M  单片机工作电压3.3V或5V
//QDtech-OLED液晶驱动 for C51
//xiao冯@ShenZhen QDtech co.,LTD
//公司网站:www.qdtft.com
//淘宝网站：http://qdtech.taobao.com
//wiki技术网站：http://www.lcdwiki.com
//我司提供技术支持，任何技术问题欢迎随时交流学习
//固话(传真) :+86 0755-23594567 
//手机:15989313508（冯工） 
//邮箱:lcdwiki01@gmail.com    support@lcdwiki.com    goodtft@163.com
//技术支持QQ:3002773612  3002778157
//技术交流QQ群:324828016
//创建日期:2018/8/29
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 深圳市全动电子技术有限公司 2018-2028
//All rights reserved
/****************************************************************************************************
//=========================================电源接线================================================//
// OLED模块                C51单片机
//   VCC         接       DC 5V/3.3V      //OLED屏电源正
//   GND         接          GND          //OLED屏电源地
//=======================================液晶屏数据线接线==========================================//
//本模块默认数据总线类型为4线制SPI
// OLED模块                C51单片机
//   MOSI        接          P15          //OLED屏SPI写信号
//=======================================液晶屏控制线接线==========================================//
// OLED模块                C51单片机
//   CS          接          P13          //OLED屏片选控制信号
//   RES         接          P33          //OLED屏复位控制信号
//   DC          接          P12          //OLED屏数据/命令选择控制信号
//   CLK         接          P17          //OLED屏SPI时钟信号
//=========================================触摸屏接线=========================================//
//本模块不带触摸功能，所以不需要触摸屏接线
****************************************************************************************************/	
/***************************************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, QD electronic SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
****************************************************************************************************/	
#include "oled.h"
#include "string.h"
#include "oledfont.h" 
#include "oled_gui.h"

/*******************************************************************
 * @name       :void GUI_Fill(u8 sx,u8 sy,u8 ex,u8 ey,u8 color)
 * @date       :2018-08-27 
 * @function   :fill the specified area
 * @parameters :sx:the bebinning x coordinate of the specified area
                sy:the bebinning y coordinate of the specified area，
								   it must be set 0~7
								ex:the ending x coordinate of the specified area
								ey:the ending y coordinate of the specified area,
								   it must be set 0~7
								color:the color value of the the specified area
								      1-white
											0-black
 * @retvalue   :None
********************************************************************/
void GUI_Fill(u8 sx,u8 sy,u8 ex,u8 ey,u8 color)
{  	
	u8 i,j,dat;			
	u8 width=ex-sx+1; 		//得到填充的宽度
	u8 height=ey-sy+1;		//高度
	if(color)
	{
		dat = 0xff;
	}
	else
	{
		dat = 0;
	}
	for(i=0;i<height;i++)
	{
		OLED_Set_Pos(sx, sy+i); 
		for(j=0;j<width;j++)
		{
				OLED_WR_Byte(dat,OLED_DATA);
		}		
	}
}

/*****************************************************************************
 * @name       :void GUI_ShowChar(u8 x,u8 y,u8 chr,u8 Char_Size,u8 mode)
 * @date       :2018-08-27 
 * @function   :Display a single English character
 * @parameters :x:the bebinning x coordinate of the Character display position
                y:the bebinning y coordinate of the Character display position,
								  it must be set 0~7
								chr:the ascii code of display character(0~94)
								Char_Size:the size of display character(8,16)
								mode:0-white background and black character
								     1-black background and white character
 * @retvalue   :None
******************************************************************************/ 
void GUI_ShowChar(u8 x,u8 y,u8 chr,u8 Char_Size,u8 mode)
{      	
	  unsigned char c=0,i=0,j=0;	
		c=chr-' ';//得到偏移后的值			
		if(x>WIDTH-1)
		{
			x=0;
			y=y+2;
		}
		if(Char_Size ==16)
		{
		  for(j=0;j<2;j++)
			{
				OLED_Set_Pos(x,y+j);
				for(i=0;i<8;i++)
				{
					if(mode)
					{
						OLED_WR_Byte(F8X16[c*16+i+j*8],OLED_DATA);
					}
					else
					{
						OLED_WR_Byte(~(F8X16[c*16+i+j*8]),OLED_DATA);
					}
				}
			}
		 }
		 else 
		 {	
				OLED_Set_Pos(x,y);
				for(i=0;i<6;i++)
				{
				  if(mode)
					{
						OLED_WR_Byte(F6x8[c][i],OLED_DATA);
					}
					else
					{
						OLED_WR_Byte(~(F6x8[c][i]),OLED_DATA);
					}
				}
		}
}

/*****************************************************************************
 * @name       :void GUI_ShowString(u8 x,u8 y,u8 *chr,u8 Char_Size,u8 mode)
 * @date       :2018-08-27 
 * @function   :Display English string
 * @parameters :x:the bebinning x coordinate of the English string
                y:the bebinning y coordinate of the English string
								  it must be set 0~7
								chr:the start address of the English string
								Char_Size:the size of display character
								mode:0-white background and black character
								     1-black background and white character
 * @retvalue   :None
******************************************************************************/   	  
void GUI_ShowString(u8 x,u8 y,u8 *chr,u8 Char_Size,u8 mode)
{
	unsigned char j=0,csize;
	if(Char_Size == 16)
  {
	  csize = Char_Size/2;
	}
  else if(Char_Size == 8)
  {
	  csize = Char_Size/2+2;
	}
	else
	{
		return;
	}
	while (chr[j]!='\0')
	{		
		GUI_ShowChar(x,y,chr[j],Char_Size,mode);
		x+=csize;
		if(x>120)
		{
			x=0;
			y+=Char_Size/8;
		}
		j++;
	}
}

/*****************************************************************************
 * @name       :u32 mypow(u8 m,u8 n)
 * @date       :2018-08-27 
 * @function   :get the nth power of m (internal call)
 * @parameters :m:the multiplier
                n:the power
 * @retvalue   :the nth power of m
******************************************************************************/ 
static u32 mypow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}

/*****************************************************************************
 * @name       :void GUI_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 Size,u8 mode)
 * @date       :2018-08-27 
 * @function   :Display number
 * @parameters :x:the bebinning x coordinate of the number
                y:the bebinning y coordinate of the number
								  it must be set 0~7
								num:the number(0~4294967295)
								len:the length of the display number
								Size:the size of display number
								mode:0-white background and black character
								     1-black background and white character
 * @retvalue   :None
******************************************************************************/  			 
void GUI_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 Size,u8 mode)
{         	
	u8 t,temp;
	u8 enshow=0,csize;
  if(Size == 16)
  {
	  csize = Size/2;
	}
  else if(Size == 8)
  {
	  csize = Size/2+2;
	} 	
	else
	{
		return;
	}
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				GUI_ShowChar(x+csize*t,y,' ',Size,mode);
				continue;
			}
			else 
			{
				enshow=1; 
		 	}
		}
	 	GUI_ShowChar(x+csize*t,y,temp+'0',Size,mode); 
	}
}  

/*****************************************************************************
 * @name       :void GUI_ShowFont16(u8 x,u8 y,u8 *s,u8 mode)
 * @date       :2018-08-27 
 * @function   :Display a single 16x16 Chinese character
 * @parameters :x:the bebinning x coordinate of the Chinese character
                y:the bebinning y coordinate of the Chinese character
								  it must be set 0~7
								id:the index of character in chinese array
								mode:0-white background and black character
								     1-black background and white character
 * @retvalue   :None
******************************************************************************/ 
void GUI_ShowFont16(u8 x,u8 y,u32 id,u8 mode)
{
	u8 j,k,tmp;
	
	for(k=0;k<2;k++)
	{
		OLED_Set_Pos(x,y+k);
		for(j=0;j<16;j++)
		{
			if(mode)
			{
				tmp = cfont16[id].Msk[j+k*16];
			}
			else
			{
				tmp = ~(cfont16[id].Msk[j+k*16]);
			}
			OLED_WR_Byte(tmp,OLED_DATA);
		}
	}
}

/*****************************************************************************
 * @name       :void GUI_ShowFont24(u8 x,u8 y,u8 *s,u8 mode)
 * @date       :2018-08-27 
 * @function   :Display a single 24x24 Chinese character
 * @parameters :x:the bebinning x coordinate of the Chinese character
                y:the bebinning y coordinate of the Chinese character
								  it must be set 0~7
								id:the index of character in chinese array
								mode:0-white background and black character
								     1-black background and white character
 * @retvalue   :None
******************************************************************************/ 
void GUI_ShowFont24(u8 x,u8 y,u32 id,u8 mode)
{
	u8 j,k,tmp;

	for(k=0;k<3;k++)
	{
		OLED_Set_Pos(x,y+k);
		for(j=0;j<24;j++)
		{
			if(mode)
			{
				tmp = cfont24[id].Msk[j+k*24];
			}
			else
			{
				tmp = ~(cfont24[id].Msk[j+k*24]);
			}
			OLED_WR_Byte(tmp,OLED_DATA);
		}
	}	
}

/*****************************************************************************
 * @name       :void GUI_ShowFont32(u8 x,u8 y,u8 *s,u8 mode)
 * @date       :2018-08-27 
 * @function   :Display a single 32x32 Chinese character
 * @parameters :x:the bebinning x coordinate of the Chinese character
                y:the bebinning y coordinate of the Chinese character
								  it must be set 0~7
								id:the index of character in chinese array
								mode:0-white background and black character
								     1-black background and white character
 * @retvalue   :None
******************************************************************************/ 
void GUI_ShowFont32(u8 x,u8 y,u32 id,u8 mode)
{
	u8 j,k,tmp;
	
	for(k=0;k<4;k++)
	{
		OLED_Set_Pos(x,y+k);
		for(j=0;j<32;j++)
		{
			if(mode)
			{
				tmp = cfont32[id].Msk[j+k*32];
			}
			else
			{
				tmp = ~(cfont32[id].Msk[j+k*32]);
			}
			OLED_WR_Byte(tmp,OLED_DATA);
		}
	}	
}

/*****************************************************************************
 * @name       :void GUI_ShowCHinese(u8 x,u8 y,u8 hsize,u8 *str,u8 mode)
 * @date       :2018-08-27 
 * @function   :Display Chinese strings
 * @parameters :x:the bebinning x coordinate of the Chinese strings
                y:the bebinning y coordinate of the Chinese strings
								  it must be set 0~7
								size:the size of Chinese strings
								id:the index pointer of character in chinese array
								mode:0-white background and black character
								     1-black background and white character
								len:length of string
 * @retvalue   :None
******************************************************************************/	   		   
void GUI_ShowCHinese(u8 x,u8 y,u8 hsize,u32* id_ptr,u8 mode, u32 len)
{
	for(u32 i = 0;i < len;i++){
		if(hsize == 16)
		{
			GUI_ShowFont16(x,y,id_ptr[i],mode);
		}
		else if(hsize == 24)
		{
			GUI_ShowFont24(x,y,id_ptr[i],mode);
		}
		else if(hsize == 32)
		{
			GUI_ShowFont32(x,y,id_ptr[i],mode);
		}
		else
		{
			return;
		}
		x+=hsize;
		if(x>WIDTH-hsize)
		{
			x=0;
			y+=hsize/8;
		}	
	}
}
