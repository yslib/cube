#pragma once
#ifndef _COMMON_H_
#define _COMMON_H_
#include "all.h"

/*
*
*一些用到的数据结构
*
*/
/************************************************************************/
/*下标值对应的位置F B L R U D                                           */
/***************|----------|*********************************************/
/*				| 37|38|39 | */
/*				|----------| */
/*				| 40|41|42 | */
/*				|----------| */
/*				| 43|44|45 | */
/*	  |---------|----------|----------|----------| */
/*    | 19|20|21| 1 |2 |3  | 28|29|30 | 10|11|12 | */
/*    |---------|----------|----------|----------| */
/*    | 22|23|24| 4 |5 |6  | 31|32|33 | 13|14|15 | */
/*    |---------|----------|----------|----------| */
/*    | 25|26|27| 7 |8 |9  | 34|35|36 | 16|17|18 | */
/*	  |---------|----------|----------|----------| */
/*				| 46|47|48 | */
/*				|----------| */
/*				| 49|50|51 | */
/*				|----------| */
/*				| 52|53|54 | */
/*				|----------| */
/************************************************************************/
/*				|----------| */
/*				| 9 |10|11 | */
/*	  |---------|----------|---| */
/*           |15| 0 |4 |1  | 18| */
/*    |---------|----------|---| */
/*           |16| 7 |8 |5  | 19| */
/*    |---------|----------|---| */
/*           |17| 3 |6 |2  | 20| */
/*	  |---------|----------|---| */
/*				| 12|13|14 | */
/*				|----------| */
/************************************************************************/

struct Point3d
{
	float x, y, z;
	Point3d(float xx = 0, float yy = 0, float zz = 0) :x(xx), y(yy), z(zz) {}
};
struct sRGB
{

	sRGB(int rr = 0, int gg = 0, int bb = 0) :r(rr), g(gg), b(bb) {}
	unsigned char r, g, b;
	
};
enum FaceColor {
	Green,
	Blue,
	Red, 
	Orange, 
	White,
	Yellow };

enum ColorThreshold {

	Green_Rmin=0,
	Green_Rmax=60,
	Green_Bmin=112,
	Green_Bmax=255,
	Green_Gmin=0,
	Green_Gmax=100,
	Green_R,
	Green_B,
	Green_G,
	Green_Err,

	Blue_Rmin = 0,
	Blue_Rmax = 60,
	Blue_Bmin = 0,
	Blue_Bmax = 112,
	Blue_Gmin = 100,
	Blue_Gmax = 255,
	Blue_R,
	Blue_B,
	Blue_G,
	Blue_Err,

	Red_Rmin=150,
	Red_Rmax=255,
	Red_Bmin=0,
	Red_Bmax=100,
	Red_Gmin=0,
	Red_Gmax=71,
	Red_R,
	Red_B,
	Red_G,
	Red_Err,

	Orange_Rmin=200,
	Orange_Rmax=255,
	Orange_Bmin=0,
	Orange_Bmax=100,
	Orange_Gmin=71,
	Orange_Gmax=150,
	Orange_R,
	Orange_B,
	Orange_G,
	Orange_Err,

	White_Rmin=200,
	White_Rmax=255,
	White_Bmin=200,
	White_Bmax=255,
	White_Gmin=200,
	White_Gmax=255,
	White_R,
	White_B,
	White_G,
	White_Err,

	Yellow_Rmin=200,
	Yellow_Rmax=255,
	Yellow_Bmin=150,
	Yellow_Bmax=208,
	Yellow_Gmin=90,
	Yellow_Gmax=150,
	Yellow_R,
	Yellow_B,
	Yellow_G,
	Yellow_Err,
};

typedef std::vector<int> DetectAreaIndex;

struct CubePosition
{
public:
	std::vector<sRGB> rgbs;			//每个方块的rgb值
	std::vector<int> colorsValue;	//颜色值


	CubePosition()
	{
		rgbs.resize(54);
		colorsValue.resize(54);
		cube3dDetectArea.resize(54);
		
		float lenth = 2;
		float step = lenth / 3;
		float minX = 0 - step;
		float maxX = step;
		float minY = 0 - step;
		float maxY = step;
		//FBLRUD
		for (int i = 0; i < 6; i++)
		{
			//6个面
			for (int j = 0; j < 3; j++)
			{
				//每面三行
				for (int k = 0; k < 3; k++)
				{
					//每行三列
					float x, y, z;
					x = y = z = 0;
					if (i == 0 || i == 1)	//FB
					{
						if (i == 0)x = 1;
						else x = -1;
						if (i == 0)
							y = minX + k*step;
						else
							y = maxX - k*step;
						if( i == 0)
							z = maxY - j * step;
						else 
							z = maxY -j * step;
					}
					else if (i == 2 || i == 3)	//LR
					{
						if (i == 2)y = -1;
						else y = 1;
						if (i == 2)
							x = minX + k*step;
						else
							x = maxX - k * step;
						if (i == 2)
							z = maxY - j *step;
						else
							z = maxY - j * step;
					}
					else   //UD
					{
						if (i == 4)z = 1;
						else
							z = -1;
						if (i == 4)
							x = minX + j*step;
						else
							x = maxX - j*step;
						if (i == 4)
							y = minY + k*step;
						else
							y = minY + k*step;
					}
					cube3dDetectArea[i * 9 + j * 3 + k] = Point3d(x,y,z);
				}
			}
		}
	}
	const std::vector<Point3d> getAreaMapping() { return cube3dDetectArea; }
private:
	std::vector<Point3d> cube3dDetectArea;//位置映射表，传给video，从要识别颜色颜色的立方体世界坐标映射到平面CubePosition相应的位置
};

struct CaputreFrameCubeConfig
{
public:
    DetectAreaIndex detectAreaIndex;
    double theta;
    double phi;
    double rho;
};

class Rotate
{
private:
	float v11, v12, v21, v22;
	
public:
	Rotate(double theta = 3.1415926535/2)
	{
		v11 = std::cos(theta); v12 = -std::sin(theta);
		v21 = std::sin(theta); v22 = std::cos(theta);
	}
	Point3d RotateX(const Point3d & p,int direction = 1)const
	{
		if (direction > 0)
			return Point3d(p.x, p.y*v11 + p.z*v12, p.y*v21 + p.z*v22);
		else
			return Point3d(p.x, p.y*v11 - p.z*v12, -p.y*v21 + p.z*v22);
		
	}
	Point3d RotateY(const Point3d & p, int direction = 1)const
	{
		if(direction > 0)
			return Point3d(p.x*v11 + p.z*v12, p.y, p.x*v21 + p.z*v22);
		else
			return Point3d(p.x*v11 - p.z*v12, p.y, -p.x*v21 + p.z*v22);
	}
	Point3d RotateZ(const Point3d & p, int direction = 1)const
	{
		if(direction > 0)
			return Point3d(p.x*v11 + p.y*v12, p.x*v21 + p.y*v22, p.z);
		else
			return Point3d(p.x*v11 - p.y*v12, -p.x*v21 + p.y*v22, p.z);
	}
};

#endif /*_COMMON_H_*/

//const double Rotate::PI = 3.1415926535;
//class AreaMapping		
//{
//	std::vector<Point3d> cube3dDetectArea;
//	std::vector<int> toCube2dPosition;
//	std::vector<QPointF> screenPosition;
//
//public:
//	AreaMapping(){}
//	AreaMapping(const std::vector<Point3d> & points,const std::vector<int> & position)
//	{
//		if (points.size() == position.size())
//		{
//			cube3dDetectArea = points;
//			toCube2dPosition = position;
//		}
//	}
//	int mapping(int index){return toCube2dPosition[index];}
//
//	std::vector<Point3d> & setCube3dDetectArea() { return cube3dDetectArea; }
//	const std::vector<Point3d> & getCube3dDetectArea() { return cube3dDetectArea; }
//
//
//	std::vector<QPointF> & setScreenPosition() { return screenPosition; }
//	const std::vector<QPointF> & getScreenPosition() { return screenPosition; }
//};