#ifndef GRAPHICS_H
#define GRAPHICS_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <queue>
#include "font.h"
#include "Mat.h"
/******************************************************************************
*                    定义与编译器无关的数据类型
******************************************************************************/
typedef unsigned char  BOOLEAN;
typedef unsigned char  INT8U;			/* Unsigned  8 bit quantity       */
typedef signed   char  INT8S;			/* Signed    8 bit quantity       */
typedef unsigned short INT16U;			/* Unsigned 16 bit quantity       */
typedef signed   short INT16S;			/* Signed   16 bit quantity       */
typedef unsigned int   INT32U;			/* Unsigned 32 bit quantity       */
typedef signed int     INT32S;			/* Signed   32 bit quantity       */
typedef long long      INT64S;			/* Signed   64 bit quantity       */
typedef float          FP32;			/* Single precision floating point*/
typedef double         FP64;			/* Double precision floating point*/
typedef char    CHAR;			/* 字符       */
typedef INT32U  RGB;
/******************************************************************************
*                    Graphics 计算机图形学
******************************************************************************/
struct Graphics {
	/*---------------- 基础参数 ----------------*/
	INT32S gSize[2] = { 2048,2048 };										//窗口尺寸
	RGB* Map = NULL;														//图(底层)
	RGB PaintColor;															//画笔颜色
	INT32S PaintSize = 0;													//画笔大小
	Mat<FP64> gM = { 3 };													//变换矩阵
	/*---------------- 常数 ----------------*/
	const RGB TRANSPARENT = 0xFFFFFFFF;										//RGB:透明
	const INT32S FontSize = 16;												//字符大小
	/*---------------- 底层 ----------------*/
	~Graphics() { free(Map); }												//析构函数
	void init();															//初始化
	void clear();	 														//清屏
	void clear(RGB color);	 												//清屏
	void setPoint(INT32S x, INT32S y, RGB color);							//底层画点(指定颜色)
	RGB  readPoint(INT32S x, INT32S y); 									//读点 
	void PicWrite(const CHAR* filename);									//存图(底层)
	void confirmTrans();													//确认变换
	/*---------------- DRAW ----------------*/
	void drawPoint(INT32S x0, INT32S y0);									//画点
	void drawLine(INT32S x1, INT32S y1, INT32S x2, INT32S y2);				//画线
	void drawCircle(INT32S x0, INT32S y0, INT32S r);					    //画圆
	void drawEllipse(INT32S x0, INT32S y0, INT32S rx,INT32S ry);			//画椭圆
	void drawTriangle(INT32S x1, INT32S y1, INT32S x2, INT32S y2, INT32S x3, INT32S y3);//画三角形
	void drawRectangle(INT32S x1, INT32S y1, INT32S x2, INT32S y2);		   	//画矩形
	void drawWave(INT32S x[], INT32S y[], INT32S n);						//画曲线
	void drawBezier(INT32S x[], INT32S y[], INT32S n);						//画贝塞尔曲线
	void drawGrid(INT32S sx, INT32S sy, INT32S ex, INT32S ey, INT32S dx, INT32S dy);
	void drawCopy(INT32S x0, INT32S y0, Graphics* gt);						//复制别的图
	void fill(INT32S sx, INT32S sy, INT32S ex, INT32S ey, RGB color);		//填充单色(指定颜色)
	void floodfill(INT32S x0, INT32S y0, RGB color);						//泛滥填充
	void drawChar(INT32S x0, INT32S y0, CHAR charac);						//显示字符
	void drawString(INT32S x0, INT32S y0, const CHAR* str, INT32S n);		//显示字符串
	void drawNum(INT32S x0, INT32S y0, FP64 num);							//显示数字
	/*---------------- 二维变换 TRANSFORMATION ----------------*/
	void translation(INT32S dx, INT32S dy);									//平移
	void rotate(FP64 theta, INT32S x0, INT32S y0);							//旋转(基于基准点)
	void scaling(FP64 sx, FP64 sy);											//缩放(>1直接完成变换)
	/*---------------- SET ----------------*/
	bool judgeOutRange(INT32S x0, INT32S y0);								//判断坐标是否过界
	void setGSize(INT32S width, INT32S height);								//设置窗口尺寸
};
#endif
