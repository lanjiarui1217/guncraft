#pragma once
#include "framework.h"
extern int noisedefseed;
class noise
{
public:
	int hash11(int position);
//	int hash11glbseed(int position);
	fvct2 hash22(fvct2 position2D);
	int hash21(fvct2 position2D);
	fvct hash33(fvct position3D);
	int hash31(fvct position3D);
	void setseed(int seed=107);
	//返还随机值 0~1023
	int randint(fvct2 position);
	//返还随机值 0.0f~1.0f
	float rand(fvct2 position);
	//求梯度值	
	float grad(fvct2 vtx,fvct2 position2D);
	//求梯度值(优化)
	float grad_f(fvct2 vtx,fvct2 position2D);
	//输入2D坐标，输出柏林噪声值[-1,1]
	float perlinNoise(fvct2 position2D);
	//输入2D坐标，输出value噪声值[-1,1]
	float valueNoise(fvct2 position2D);
	//输入2D坐标，输出simplex噪声值[-1,1]
	float simplexNoise(fvct2 position2D);
	//输入3D坐标，输出simplex噪声值[-1,1]
	float simplexNoise(fvct position);
	//预处理噪声（预先设置全局顶点）：初始2D坐标，晶格大小
	void prehandlePerlinNoise(fvct2 position2D,int crystalSize);
	//预处理噪声（预先设置全局顶点）：初始2D坐标，晶格大小
	void prehandleValueNoise(fvct2 position2D,int crystalSize);
	//预处理噪声（预先设置全局顶点）：初始2D坐标，晶格大小
	void prehandleSimplexNoise(fvct2 position2D,int crystalSize);
	//预处理噪声（预先设置全局顶点）：初始3D坐标，晶格大小
	void prehandleSimplexNoise(fvct position,int crystalSize);
	//二阶bezier曲线 t应[0.0f~1.0f]
	fvct2 bezier(fvct2 p0,fvct2 p1,fvct2 p2,float t);
	//三阶bezier曲线 t应[0.0f~1.0f]
	fvct2 bezier(fvct2 p0,fvct2 p1,fvct2 p2,fvct2 p3,float t);
	////三维坐标转一维坐标（压缩坐标）
	//ull Index(int x,int y,int z=0);
	////一维坐标转三维坐标（解压坐标）
	//fvct UnIndex(ull index);
	noise(int=noisedefseed);
//private:
	fvct2 ofst2;
	fvct ofst3;
	fvct2 glbvtx[4];
	fvct glbvtx3D[4];
	fvct2 glboffset;
	fvct glboffset3D;
	int glbseed;
};