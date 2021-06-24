/*
Copyright 2020,2021 LiGuer. All Rights Reserved.
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
	http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
#ifndef FRACTAL
#define FRACTAL
#include <math.h>
#include <complex>
#include <vector>
#include "../../LiGu_AlgorithmLib/Mat.h"
#include "../../LiGu_AlgorithmLib/Tensor.h"
#include "GraphicsND.h"
#define PI 3.141692653589

namespace Fractal {
/*################################################################################################
isMandelbrotSet		(C, Z0, iterateTimes)
Mandelbrot			(C, min, max, resSize, iterateTimes, Set)
Julia				(C, min, max, resSize, iterateTimes, Set)
HilbertCurve_xy2d	(n, x, y)
HilbertCurve_d2xy	(n, distance, int& x, int& y)
PerlinNoise			(x, y, randomGridGradient)
PerlinNoise			(output, frequency)
FractalTree3D		(linesSt, linesEd, level, alpha, fork)
Boids				(birds)
BoidsRule			(birds, index)
################################################################################################*/
/*************************************************************************************************
*								Mandelbrot集
*	[定义]: Zn+1 = Zn² + C
			所有能使Zn+1不发散的复数点C, 所构成的集合,即 Mandelbrot Set
			(不发散,不一定收敛,有可能在几个不同点来回跳)
*	[性质]: |Zn|>2不可能收敛, 即Mandelbrot Set在半径为2的圆内.
*************************************************************************************************/
// 是否属于MandelbrotSet/JuliaSet
int isMandelbrotSet(std::complex<double> C, std::complex<double> Z0, int iterateTimes) {
	std::complex<double> z = Z0;
	for (int epoch = 0; epoch < iterateTimes; epoch++) {	// 迭代
		if (abs(z) > 2) return epoch;						// |Zn|>2不可能收敛
		z = z * z + C;										// Zn+1 = Zn² + C   
	} return 0;							//属于,输出0; 不属于,输出判断出的当次迭代数
}
void Mandelbrot(std::complex<double> min, std::complex<double> max, int resSize, int iterateTimes, Mat<int> Set) {
	Set.zero(resSize, resSize);
	double deltaReal = (max.real() - min.real()) / resSize,
	       deltaImag = (max.imag() - min.imag()) / resSize;
	for (int i = 0; i < resSize; i++) {
		for (int j = 0; j < resSize; j++) {
			double real = min.real() + deltaReal * i,
				   imag = min.imag() + deltaImag * j;
			Set(i, j) = isMandelbrotSet(
				std::complex<double>(real, imag), 
				std::complex<double>(0, 0), 
				iterateTimes
			);
		}
	}
}
/*************************************************************************************************
*								Julia集
*	[定义]: Zn+1 = Zn² + C
			对于某复数值C,所有能使Zn+1不发散的Z0的集合,即 Julia Set
			类似于. Mandelbrot 曼德布洛特集
*************************************************************************************************/
void Julia(std::complex<double> C, std::complex<double> min, std::complex<double> max, int resSize, int iterateTimes, Mat<int> Set) {
	Set.zero(resSize, resSize);
	double deltaReal = (max.real() - min.real()) / resSize,
	       deltaImag = (max.imag() - min.imag()) / resSize;
	for (int i = 0; i < resSize; i++) {
		for (int j = 0; j < resSize; j++) {
			double real = min.real() + deltaReal * i, 
				   imag = min.imag() + deltaImag * j;
			Set(i, j) = isMandelbrotSet(C, std::complex<double>(real, imag), iterateTimes);
		}
	}
}
/*************************************************************************************************
*								Hilbert 曲线
*	[定义]: 一种自相似的分形曲线
*	[生成方法]: 四象限复制四分，翻转左下、右下, 使左下末同左上初、右下初同右上末,能够最短连接.
*	[性质]:
		* 边长: nth 2^n
		* 长度: nth 2^n - 1 / 2^n
		* 因为是四进制自相似, 所以曲线上位置 distance, 可以不断判断子象限，按二进制在位上叠加
*	[用途]: 1D to 2D 的映射算法, 随 n 增加, 映射的位置趋于收敛
*	[1,2阶曲线]: "┌┐"	   "┌┐┌┐"    "┌┐┌┐ "
					︱︱  =>	︱︱︱︱ =>   ︱└┘︱
							┌┐┌┐      └┐┌┘
							︱︱︱︱       -┘└-
*	[程序]: rotation : 翻转坐标, 使Hilbert曲线性质, 自相似地适用于左下、右下象限
		[1] xy2d(): 坐标 -> 曲线位置    [2] d2xy(): 曲线位置 -> 坐标
*	[Author]: 1891.Hilbert
*	[Reference]: [1] Wikipedia.Hilbert curve
*************************************************************************************************/
int HilbertCurve_xy2d(int n, int x, int y) {	//convert (x,y) to d
	int distance = 0;
	for (int s = n / 2; s > 0; s /= 2) {
		int rx = (x & s) > 0, 
			ry = (y & s) > 0;
		distance += s * s * ((0b11 * rx) ^ ry);
		// rotation
		if (ry == 0) {
			if (rx == 1) { 
				x = n - 1 - x; 
				y = n - 1 - y;
			}
			int t = x; x = y; y = t;
		}
	}return distance;
}
void HilbertCurve_d2xy(int n, int distance, int& x, int& y) {//convert d to (x,y)
	x = y = 0;
	for (int s = 1; s < n; s *= 2) {
		int rx = 1 & (distance / 2), 
			ry = 1 & (distance ^ rx);
		// rotation
		if (ry == 0) {
			if (rx == 1) { 
				x = n - 1 - x; 
				y = n - 1 - y;
			}
			int t = x; x = y; y = t;
		}
		x += s * rx; 
		y += s * ry;
		distance /= 4;
	}
}
/*************************************************************************************************
*								Perlin Noise
*	Function to linearly interpolate between a0 and a1 , Weight w should be in the range [0.0, 1.0]
*	[流程]:
		[1] 格点随机梯度矢量
		[2] (x,y)与格点距离,梯度点积
		[3] 插值
*************************************************************************************************/
double PerlinNoise(double x, double y, Mat<>& randomGridGradient) {
	// 对四个格点
	int x0[] = { x, x + 1, x, x + 1 }, 
		y0[] = { y, y, y + 1, y + 1 };
	double n[4];
	for (int i = 0; i < 4; i++) {
		//[1][2]
		double random = randomGridGradient(x0[i], y0[i]);
		n[i] = (x - x0[i]) * cos(random) 
			 + (y - y0[i]) * sin(random);
	}
	//[3]
	double 
		sx = x - (int)x,
		sy = y - (int)y,
		ix0 = (n[1] - n[0]) * (3.0 - sx * 2.0) * sx * sx + n[0],
		ix1 = (n[3] - n[2]) * (3.0 - sx * 2.0) * sx * sx + n[2];
	return (ix1 - ix0) * (3.0 - sy * 2.0) * sy * sy + ix0;
}
Mat<>& PerlinNoise(Mat<>& output, int frequency) {
	Mat<> randomGridGradient;
	randomGridGradient.rands(frequency + 1, frequency + 1, 0, 256);
	for (int y = 0; y < output.cols; y++)
		for (int x = 0; x < output.rows; x++)
			output(x, y) = PerlinNoise(
				(double)x / output.rows * frequency,
				(double)y / output.cols * frequency,
				randomGridGradient
			);
	return output;
}
/*************************************************************************************************
*								三维分形树 Fractal Tree 3D
*************************************************************************************************/
void FractalTree3D(std::vector<Mat<>>& linesSt, std::vector<Mat<>>& linesEd, int level, double alpha, int fork = 3) {
	if (level <= 0) return;
	// 确定旋转矩阵
	Mat<> st = linesSt.back(), ed = linesEd.back(), direction, rotateAxis, rotateMat, zAxis(3), tmp; zAxis.set(0, 0, 1);
	direction.sub(ed, st);
	if (direction[0] != 0 || direction[1] != 0) {
		GraphicsND::rotate(
			rotateAxis.crossProduct(direction, zAxis),
			-acos(tmp.dot(direction, zAxis) / direction.norm()),
			tmp.zero(3), rotateMat.E(4)
		); rotateMat.block(1, 3, 1, 3, rotateMat);
	}
	else rotateMat.E(3);
	//递归
	double Lenth = direction.norm(); 
	Mat<> endPoint(3);
	for (int i = 0; i < fork; i++) {
		endPoint.set(
			sin(alpha) * cos((double)i * 2 * PI / fork), 
			sin(alpha) * sin((double)i * 2 * PI / fork), 
			cos(alpha)
		);
		endPoint.add(ed, endPoint.mul(0.7 * Lenth, endPoint.mul(rotateMat, endPoint)));
		linesSt.push_back(ed);
		linesEd.push_back(endPoint);
		FractalTree3D(linesSt, linesEd, level - 1, alpha);
	}
}
/*************************************************************************************************
*								Marching Cubes
*	[Reference]:
	[1] Thanks for http://paulbourke.net/geometry/polygonise/
*************************************************************************************************/
int MarchingCubes_TriTable[256][16] = {
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{0, 8, 3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{0, 1, 9,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{1, 8, 3, 9, 8, 1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{1, 2,10,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{0, 8, 3, 1, 2,10,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{9, 2,10, 0, 2, 9,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{2, 8, 3, 2,10, 8,10, 9, 8,-1,-1,-1,-1,-1,-1,-1},
	{3,11, 2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{0,11, 2, 8,11, 0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{1, 9, 0, 2, 3,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{1,11, 2, 1, 9,11, 9, 8,11,-1,-1,-1,-1,-1,-1,-1},
	{3,10, 1,11,10, 3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{0,10, 1, 0, 8,10, 8,11,10,-1,-1,-1,-1,-1,-1,-1},
	{3, 9, 0, 3,11, 9,11,10, 9,-1,-1,-1,-1,-1,-1,-1},
	{9, 8,10,10, 8,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{4, 7, 8,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{4, 3, 0, 7, 3, 4,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{0, 1, 9, 8, 4, 7,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{4, 1, 9, 4, 7, 1, 7, 3, 1,-1,-1,-1,-1,-1,-1,-1},
	{1, 2,10, 8, 4, 7,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{3, 4, 7, 3, 0, 4, 1, 2,10,-1,-1,-1,-1,-1,-1,-1},
	{9, 2,10, 9, 0, 2, 8, 4, 7,-1,-1,-1,-1,-1,-1,-1},
	{2,10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4,-1,-1,-1,-1},
	{8, 4, 7, 3,11, 2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{11,4, 7,11, 2, 4, 2, 0, 4,-1,-1,-1,-1,-1,-1,-1},
	{9, 0, 1, 8, 4, 7, 2, 3,11,-1,-1,-1,-1,-1,-1,-1},
	{4, 7,11, 9, 4,11, 9,11, 2, 9, 2, 1,-1,-1,-1,-1},
	{3,10, 1, 3,11,10, 7, 8, 4,-1,-1,-1,-1,-1,-1,-1},
	{1,11,10, 1, 4,11, 1, 0, 4, 7,11, 4,-1,-1,-1,-1},
	{4, 7, 8, 9, 0,11, 9,11,10,11, 0, 3,-1,-1,-1,-1},
	{4, 7,11, 4,11, 9, 9,11,10,-1,-1,-1,-1,-1,-1,-1},
	{9, 5, 4,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{9, 5, 4, 0, 8, 3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{0, 5, 4, 1, 5, 0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{8, 5, 4, 8, 3, 5, 3, 1, 5,-1,-1,-1,-1,-1,-1,-1},
	{1, 2,10, 9, 5, 4,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{3, 0, 8, 1, 2,10, 4, 9, 5,-1,-1,-1,-1,-1,-1,-1},
	{5, 2,10, 5, 4, 2, 4, 0, 2,-1,-1,-1,-1,-1,-1,-1},
	{2,10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8,-1,-1,-1,-1},
	{9, 5, 4, 2, 3,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{0,11, 2, 0, 8,11, 4, 9, 5,-1,-1,-1,-1,-1,-1,-1},
	{0, 5, 4, 0, 1, 5, 2, 3,11,-1,-1,-1,-1,-1,-1,-1},
	{2, 1, 5, 2, 5, 8, 2, 8,11, 4, 8, 5,-1,-1,-1,-1},
	{10,3,11,10, 1, 3, 9, 5, 4,-1,-1,-1,-1,-1,-1,-1},
	{4, 9, 5, 0, 8, 1, 8,10, 1, 8,11,10,-1,-1,-1,-1},
	{5, 4, 0, 5, 0,11, 5,11,10,11, 0, 3,-1,-1,-1,-1},
	{5, 4, 8, 5, 8,10,10, 8,11,-1,-1,-1,-1,-1,-1,-1},
	{9, 7, 8, 5, 7, 9,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{9, 3, 0, 9, 5, 3, 5, 7, 3,-1,-1,-1,-1,-1,-1,-1},
	{0, 7, 8, 0, 1, 7, 1, 5, 7,-1,-1,-1,-1,-1,-1,-1},
	{1, 5, 3, 3, 5, 7,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{9, 7, 8, 9, 5, 7,10, 1, 2,-1,-1,-1,-1,-1,-1,-1},
	{10,1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3,-1,-1,-1,-1},
	{8, 0, 2, 8, 2, 5, 8, 5, 7,10, 5, 2,-1,-1,-1,-1},
	{2,10, 5, 2, 5, 3, 3, 5, 7,-1,-1,-1,-1,-1,-1,-1},
	{7, 9, 5, 7, 8, 9, 3,11, 2,-1,-1,-1,-1,-1,-1,-1},
	{9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7,11,-1,-1,-1,-1},
	{2, 3,11, 0, 1, 8, 1, 7, 8, 1, 5, 7,-1,-1,-1,-1},
	{11,2, 1,11, 1, 7, 7, 1, 5,-1,-1,-1,-1,-1,-1,-1},
	{9, 5, 8, 8, 5, 7,10, 1, 3,10, 3,11,-1,-1,-1,-1},
	{5, 7, 0, 5, 0, 9, 7,11, 0, 1, 0,10,11,10, 0,-1},
	{11,10,0,11, 0, 3,10, 5, 0, 8, 0, 7, 5, 7, 0,-1},
	{11,10,5, 7,11, 5,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{10,6, 5,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{0, 8, 3, 5,10, 6,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{9, 0, 1, 5,10, 6,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{1, 8, 3, 1, 9, 8, 5,10, 6,-1,-1,-1,-1,-1,-1,-1},
	{1, 6, 5, 2, 6, 1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{1, 6, 5, 1, 2, 6, 3, 0, 8,-1,-1,-1,-1,-1,-1,-1},
	{9, 6, 5, 9, 0, 6, 0, 2, 6,-1,-1,-1,-1,-1,-1,-1},
	{5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8,-1,-1,-1,-1},
	{2, 3,11,10, 6, 5,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{11,0, 8,11, 2, 0,10, 6, 5,-1,-1,-1,-1,-1,-1,-1},
	{0, 1, 9, 2, 3,11, 5,10, 6,-1,-1,-1,-1,-1,-1,-1},
	{5,10, 6, 1, 9, 2, 9,11, 2, 9, 8,11,-1,-1,-1,-1},
	{6, 3,11, 6, 5, 3, 5, 1, 3,-1,-1,-1,-1,-1,-1,-1},
	{0, 8,11, 0,11, 5, 0, 5, 1, 5,11, 6,-1,-1,-1,-1},
	{3,11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9,-1,-1,-1,-1},
	{6, 5, 9, 6, 9,11,11, 9, 8,-1,-1,-1,-1,-1,-1,-1},
	{5,10, 6, 4, 7, 8,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{4, 3, 0, 4, 7, 3, 6, 5,10,-1,-1,-1,-1,-1,-1,-1},
	{1, 9, 0, 5,10, 6, 8, 4, 7,-1,-1,-1,-1,-1,-1,-1},
	{10,6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4,-1,-1,-1,-1},
	{6, 1, 2, 6, 5, 1, 4, 7, 8,-1,-1,-1,-1,-1,-1,-1},
	{1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7,-1,-1,-1,-1},
	{8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6,-1,-1,-1,-1},
	{7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9,-1},
	{3,11, 2, 7, 8, 4,10, 6, 5,-1,-1,-1,-1,-1,-1,-1},
	{5,10, 6, 4, 7, 2, 4, 2, 0, 2, 7,11,-1,-1,-1,-1},
	{0, 1, 9, 4, 7, 8, 2, 3,11, 5,10, 6,-1,-1,-1,-1},
	{9, 2, 1, 9,11, 2, 9, 4,11, 7,11, 4, 5,10, 6,-1},
	{8, 4, 7, 3,11, 5, 3, 5, 1, 5,11, 6,-1,-1,-1,-1},
	{5, 1,11, 5,11, 6, 1, 0,11, 7,11, 4, 0, 4,11,-1},
	{0, 5, 9, 0, 6, 5, 0, 3, 6,11, 6, 3, 8, 4, 7,-1},
	{6, 5, 9, 6, 9,11, 4, 7, 9, 7,11, 9,-1,-1,-1,-1},
	{10,4, 9, 6, 4,10,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{4,10, 6, 4, 9,10, 0, 8, 3,-1,-1,-1,-1,-1,-1,-1},
	{10,0, 1,10, 6, 0, 6, 4, 0,-1,-1,-1,-1,-1,-1,-1},
	{8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1,10,-1,-1,-1,-1},
	{1, 4, 9, 1, 2, 4, 2, 6, 4,-1,-1,-1,-1,-1,-1,-1},
	{3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4,-1,-1,-1,-1},
	{0, 2, 4, 4, 2, 6,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{8, 3, 2, 8, 2, 4, 4, 2, 6,-1,-1,-1,-1,-1,-1,-1},
	{10,4, 9,10, 6, 4,11, 2, 3,-1,-1,-1,-1,-1,-1,-1},
	{0, 8, 2, 2, 8,11, 4, 9,10, 4,10, 6,-1,-1,-1,-1},
	{3,11, 2, 0, 1, 6, 0, 6, 4, 6, 1,10,-1,-1,-1,-1},
	{6, 4, 1, 6, 1,10, 4, 8, 1, 2, 1,11, 8,11, 1,-1},
	{9, 6, 4, 9, 3, 6, 9, 1, 3,11, 6, 3,-1,-1,-1,-1},
	{8,11, 1, 8, 1, 0,11, 6, 1, 9, 1, 4, 6, 4, 1,-1},
	{3,11, 6, 3, 6, 0, 0, 6, 4,-1,-1,-1,-1,-1,-1,-1},
	{6, 4, 8,11, 6, 8,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{7,10, 6, 7, 8,10, 8, 9,10,-1,-1,-1,-1,-1,-1,-1},
	{0, 7, 3, 0,10, 7, 0, 9,10, 6, 7,10,-1,-1,-1,-1},
	{10,6, 7, 1,10, 7, 1, 7, 8, 1, 8, 0,-1,-1,-1,-1},
	{10,6, 7,10, 7, 1, 1, 7, 3,-1,-1,-1,-1,-1,-1,-1},
	{1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7,-1,-1,-1,-1},
	{2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9,-1},
	{7, 8, 0, 7, 0, 6, 6, 0, 2,-1,-1,-1,-1,-1,-1,-1},
	{7, 3, 2, 6, 7, 2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{2, 3,11,10, 6, 8,10, 8, 9, 8, 6, 7,-1,-1,-1,-1},
	{2, 0, 7, 2, 7,11, 0, 9, 7, 6, 7,10, 9,10, 7,-1},
	{1, 8, 0, 1, 7, 8, 1,10, 7, 6, 7,10, 2, 3,11,-1},
	{11,2, 1,11, 1, 7,10, 6, 1, 6, 7, 1,-1,-1,-1,-1},
	{8, 9, 6, 8, 6, 7, 9, 1, 6,11, 6, 3, 1, 3, 6,-1},
	{0, 9, 1,11, 6, 7,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{7, 8, 0, 7, 0, 6, 3,11, 0,11, 6, 0,-1,-1,-1,-1},
	{7,11, 6,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{7, 6,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{3, 0, 8,11, 7, 6,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{0, 1, 9,11, 7, 6,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{8, 1, 9, 8, 3, 1,11, 7, 6,-1,-1,-1,-1,-1,-1,-1},
	{10,1, 2, 6,11, 7,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{1, 2,10, 3, 0, 8, 6,11, 7,-1,-1,-1,-1,-1,-1,-1},
	{2, 9, 0, 2,10, 9, 6,11, 7,-1,-1,-1,-1,-1,-1,-1},
	{6,11, 7, 2,10, 3,10, 8, 3,10, 9, 8,-1,-1,-1,-1},
	{7, 2, 3, 6, 2, 7,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{7, 0, 8, 7, 6, 0, 6, 2, 0,-1,-1,-1,-1,-1,-1,-1},
	{2, 7, 6, 2, 3, 7, 0, 1, 9,-1,-1,-1,-1,-1,-1,-1},
	{1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6,-1,-1,-1,-1},
	{10,7, 6,10, 1, 7, 1, 3, 7,-1,-1,-1,-1,-1,-1,-1},
	{10,7, 6, 1, 7,10, 1, 8, 7, 1, 0, 8,-1,-1,-1,-1},
	{0, 3, 7, 0, 7,10, 0,10, 9, 6,10, 7,-1,-1,-1,-1},
	{7, 6,10, 7,10, 8, 8,10, 9,-1,-1,-1,-1,-1,-1,-1},
	{6, 8, 4,11, 8, 6,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{3, 6,11, 3, 0, 6, 0, 4, 6,-1,-1,-1,-1,-1,-1,-1},
	{8, 6,11, 8, 4, 6, 9, 0, 1,-1,-1,-1,-1,-1,-1,-1},
	{9, 4, 6, 9, 6, 3, 9, 3, 1,11, 3, 6,-1,-1,-1,-1},
	{6, 8, 4, 6,11, 8, 2,10, 1,-1,-1,-1,-1,-1,-1,-1},
	{1, 2,10, 3, 0,11, 0, 6,11, 0, 4, 6,-1,-1,-1,-1},
	{4,11, 8, 4, 6,11, 0, 2, 9, 2,10, 9,-1,-1,-1,-1},
	{10,9, 3,10, 3, 2, 9, 4, 3,11, 3, 6, 4, 6, 3,-1},
	{8, 2, 3, 8, 4, 2, 4, 6, 2,-1,-1,-1,-1,-1,-1,-1},
	{0, 4, 2, 4, 6, 2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8,-1,-1,-1,-1},
	{1, 9, 4, 1, 4, 2, 2, 4, 6,-1,-1,-1,-1,-1,-1,-1},
	{8, 1, 3, 8, 6, 1, 8, 4, 6, 6,10, 1,-1,-1,-1,-1},
	{10,1, 0,10, 0, 6, 6, 0, 4,-1,-1,-1,-1,-1,-1,-1},
	{4, 6, 3, 4, 3, 8, 6,10, 3, 0, 3, 9,10, 9, 3,-1},
	{10,9, 4, 6,10, 4,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{4, 9, 5, 7, 6,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{0, 8, 3, 4, 9, 5,11, 7, 6,-1,-1,-1,-1,-1,-1,-1},
	{5, 0, 1, 5, 4, 0, 7, 6,11,-1,-1,-1,-1,-1,-1,-1},
	{11,7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5,-1,-1,-1,-1},
	{9, 5, 4,10, 1, 2, 7, 6,11,-1,-1,-1,-1,-1,-1,-1},
	{6,11, 7, 1, 2,10, 0, 8, 3, 4, 9, 5,-1,-1,-1,-1},
	{7, 6,11, 5, 4,10, 4, 2,10, 4, 0, 2,-1,-1,-1,-1},
	{3, 4, 8, 3, 5, 4, 3, 2, 5,10, 5, 2,11, 7, 6,-1},
	{7, 2, 3, 7, 6, 2, 5, 4, 9,-1,-1,-1,-1,-1,-1,-1},
	{9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7,-1,-1,-1,-1},
	{3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0,-1,-1,-1,-1},
	{6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8,-1},
	{9, 5, 4,10, 1, 6, 1, 7, 6, 1, 3, 7,-1,-1,-1,-1},
	{1, 6,10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4,-1},
	{4, 0,10, 4,10, 5, 0, 3,10, 6,10, 7, 3, 7,10,-1},
	{7, 6,10, 7,10, 8, 5, 4,10, 4, 8,10,-1,-1,-1,-1},
	{6, 9, 5, 6,11, 9,11, 8, 9,-1,-1,-1,-1,-1,-1,-1},
	{3, 6,11, 0, 6, 3, 0, 5, 6, 0, 9, 5,-1,-1,-1,-1},
	{0,11, 8, 0, 5,11, 0, 1, 5, 5, 6,11,-1,-1,-1,-1},
	{6,11, 3, 6, 3, 5, 5, 3, 1,-1,-1,-1,-1,-1,-1,-1},
	{1, 2,10, 9, 5,11, 9,11, 8,11, 5, 6,-1,-1,-1,-1},
	{0,11, 3, 0, 6,11, 0, 9, 6, 5, 6, 9, 1, 2,10,-1},
	{11,8, 5,11, 5, 6, 8, 0, 5,10, 5, 2, 0, 2, 5,-1},
	{6,11, 3, 6, 3, 5, 2,10, 3,10, 5, 3,-1,-1,-1,-1},
	{5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2,-1,-1,-1,-1},
	{9, 5, 6, 9, 6, 0, 0, 6, 2,-1,-1,-1,-1,-1,-1,-1},
	{1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8,-1},
	{1, 5, 6, 2, 1, 6,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{1, 3, 6, 1, 6,10, 3, 8, 6, 5, 6, 9, 8, 9, 6,-1},
	{10,1, 0,10, 0, 6, 9, 5, 0, 5, 6, 0,-1,-1,-1,-1},
	{0, 3, 8, 5, 6,10,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{10,5, 6,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{11,5,10, 7, 5,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{11,5,10,11, 7, 5, 8, 3, 0,-1,-1,-1,-1,-1,-1,-1},
	{5,11, 7, 5,10,11, 1, 9, 0,-1,-1,-1,-1,-1,-1,-1},
	{10,7, 5,10,11, 7, 9, 8, 1, 8, 3, 1,-1,-1,-1,-1},
	{11,1, 2,11, 7, 1, 7, 5, 1,-1,-1,-1,-1,-1,-1,-1},
	{0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2,11,-1,-1,-1,-1},
	{9, 7, 5, 9, 2, 7, 9, 0, 2, 2,11, 7,-1,-1,-1,-1},
	{7, 5, 2, 7, 2,11, 5, 9, 2, 3, 2, 8, 9, 8, 2,-1},
	{2, 5,10, 2, 3, 5, 3, 7, 5,-1,-1,-1,-1,-1,-1,-1},
	{8, 2, 0, 8, 5, 2, 8, 7, 5,10, 2, 5,-1,-1,-1,-1},
	{9, 0, 1, 5,10, 3, 5, 3, 7, 3,10, 2,-1,-1,-1,-1},
	{9, 8, 2, 9, 2, 1, 8, 7, 2,10, 2, 5, 7, 5, 2,-1},
	{1, 3, 5, 3, 7, 5,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{0, 8, 7, 0, 7, 1, 1, 7, 5,-1,-1,-1,-1,-1,-1,-1},
	{9, 0, 3, 9, 3, 5, 5, 3, 7,-1,-1,-1,-1,-1,-1,-1},
	{9, 8, 7, 5, 9, 7,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{5, 8, 4, 5,10, 8,10,11, 8,-1,-1,-1,-1,-1,-1,-1},
	{5, 0, 4, 5,11, 0, 5,10,11,11, 3, 0,-1,-1,-1,-1},
	{0, 1, 9, 8, 4,10, 8,10,11,10, 4, 5,-1,-1,-1,-1},
	{10,11,4,10, 4, 5,11, 3, 4, 9, 4, 1, 3, 1, 4,-1},
	{2, 5, 1, 2, 8, 5, 2,11, 8, 4, 5, 8,-1,-1,-1,-1},
	{0, 4,11, 0,11, 3, 4, 5,11, 2,11, 1, 5, 1,11,-1},
	{0, 2, 5, 0, 5, 9, 2,11, 5, 4, 5, 8,11, 8, 5,-1},
	{9, 4, 5, 2,11, 3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{2, 5,10, 3, 5, 2, 3, 4, 5, 3, 8, 4,-1,-1,-1,-1},
	{5,10, 2, 5, 2, 4, 4, 2, 0,-1,-1,-1,-1,-1,-1,-1},
	{3,10, 2, 3, 5,10, 3, 8, 5, 4, 5, 8, 0, 1, 9,-1},
	{5,10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2,-1,-1,-1,-1},
	{8, 4, 5, 8, 5, 3, 3, 5, 1,-1,-1,-1,-1,-1,-1,-1},
	{0, 4, 5, 1, 0, 5,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5,-1,-1,-1,-1},
	{9, 4, 5,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{4,11, 7, 4, 9,11, 9,10,11,-1,-1,-1,-1,-1,-1,-1},
	{0, 8, 3, 4, 9, 7, 9,11, 7, 9,10,11,-1,-1,-1,-1},
	{1,10,11, 1,11, 4, 1, 4, 0, 7, 4,11,-1,-1,-1,-1},
	{3, 1, 4, 3, 4, 8, 1,10, 4, 7, 4,11,10,11, 4,-1},
	{4,11, 7, 9,11, 4, 9, 2,11, 9, 1, 2,-1,-1,-1,-1},
	{9, 7, 4, 9,11, 7, 9, 1,11, 2,11, 1, 0, 8, 3,-1},
	{11,7, 4,11, 4, 2, 2, 4, 0,-1,-1,-1,-1,-1,-1,-1},
	{11,7, 4,11, 4, 2, 8, 3, 4, 3, 2, 4,-1,-1,-1,-1},
	{2, 9,10, 2, 7, 9, 2, 3, 7, 7, 4, 9,-1,-1,-1,-1},
	{9,10, 7, 9, 7, 4,10, 2, 7, 8, 7, 0, 2, 0, 7,-1},
	{3, 7,10, 3,10, 2, 7, 4,10, 1,10, 0, 4, 0,10,-1},
	{1,10, 2, 8, 7, 4,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{4, 9, 1, 4, 1, 7, 7, 1, 3,-1,-1,-1,-1,-1,-1,-1},
	{4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1,-1,-1,-1,-1},
	{4, 0, 3, 7, 4, 3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{4, 8, 7,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{9,10, 8,10,11, 8,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{3, 0, 9, 3, 9,11,11, 9,10,-1,-1,-1,-1,-1,-1,-1},
	{0, 1,10, 0,10, 8, 8,10,11,-1,-1,-1,-1,-1,-1,-1},
	{3, 1,10,11, 3,10,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{1, 2,11, 1,11, 9, 9,11, 8,-1,-1,-1,-1,-1,-1,-1},
	{3, 0, 9, 3, 9,11, 1, 2, 9, 2,11, 9,-1,-1,-1,-1},
	{0, 2,11, 8, 0,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{3, 2,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{2, 3, 8, 2, 8,10,10, 8, 9,-1,-1,-1,-1,-1,-1,-1},
	{9,10, 2, 0, 9, 2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{2, 3, 8, 2, 8,10, 0, 1, 8, 1,10, 8,-1,-1,-1,-1},
	{1,10, 2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{1, 3, 8, 9, 1, 8,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{0, 9, 1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{0, 3, 8,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}
};
int MarchingCubes_Point[12][3]{
	{1, 0, 0}, {2, 1, 0}, {1, 2, 0}, {0, 1, 0},
	{1, 0, 2}, {2, 1, 2}, {1, 2, 2}, {0, 1, 2},
	{0, 0, 1}, {2, 0, 1}, {2, 2, 1}, {0, 2, 1}
};
int MarchingCubes_Vertex[8] = { 0b0, 0b1,0b11,0b10,0b100, 0b101,0b111,0b110 };
void MarchingCubes(Tensor<>& X, Mat<>& zero, Mat<>& dx, std::vector<Mat<>>& triangleSet, double isolevel = 0.7) {
	Mat<> tri(3, 3);
	for (int i = 0; i < X.size(); i++) {
		int cubeindex = 0;
		for (int j = 0; j < 8; j++) {
			int x = X.i2x(i) +  (MarchingCubes_Vertex[j] & 0b1),
				y = X.i2y(i) + ((MarchingCubes_Vertex[j] & 0b10) >> 1),
				z = X.i2z(i) + ((MarchingCubes_Vertex[j] & 0b100)>> 2);
			if (x < 0 || x >= X.dim[0]
			||  y < 0 || y >= X.dim[1]
			||  z < 0 || z >= X.dim[2]) continue;
			if (X(x, y, z) > isolevel) cubeindex |= (1 << j);
		}
		for (int j = 0; j < 15; j += 3) {
			if (MarchingCubes_TriTable[cubeindex][j] == -1) break;
			for (int k = 0; k < 3; k++) {
				int point = MarchingCubes_TriTable[cubeindex][j + k]; 
				tri(0, k) = zero[0] + (X.i2x(i) + MarchingCubes_Point[point][0] / 2.0) * dx[0],
				tri(1, k) = zero[1] + (X.i2y(i) + MarchingCubes_Point[point][1] / 2.0) * dx[1],
				tri(2, k) = zero[2] + (X.i2z(i) + MarchingCubes_Point[point][2] / 2.0) * dx[2];
			}
			triangleSet.push_back(tri);
		}
	}
}
/******************************************************************************
*                    Boids 鸟群
*	[Rule]:
		[1] Collision Avoidance: avoid collisions with nearby flockmates
		[2] Velocity Matching  : attmpt to match velocity with nearby flockmates
		[3] Flock Centering    : attmpt to stay close to nearby flockmates
*	[过程]:
		[可见域内]:
			[1] 可见距离判定
			[2] 可见角度判定
		[Rule 1]: 可见域内，同各个个体的反向方向，除以距离，作为加速度 a1
		[Rule 2]: 可见域内, 质心速度, 作为加速度 a2
		[Rule 3]: 可见域内, 质心位置方向, 作为加速度 a3
		[障碍规避]:
			[1] 生成均匀球面点分布
			[2] 依次从速度方向,向四周,发出射线进行障碍检测,射线距离在可见域内
			[3] 选择第一束探测无障碍的射线, 作为避障加速度 a4
		[ v ]: v = (Σwi·ai)·dt
			   v 归一化单位矢量: ^v  =>  v = v0·^v
		[ r ]: r = v·dt = v0·dt·^v
*	[Referance]:
		[1] Thanks for https://github.com/SebLague/Boids
******************************************************************************/
struct BoidsBird { Mat<> r{ 3 }, v{ 3 }, a{ 3 }; };
//[ rule 规则 ]
void BoidsRule(std::vector<BoidsBird>& birds, int index, 
	double visualField, double visualAngle, double* weight, void(*obstacleAvoidance)(BoidsBird& birds)
) {
	Mat<> distance(3), avoidDirection(3), groupVelocity(3), groupCenter(3), tmp;
	int groupNum = 0;
	for (int i = 0; i < birds.size(); i++) {
		if ( i == index
		|| distance.add(birds[i].r,birds[index].r.negative(distance)).norm()                   > visualField
		|| distance.dot(distance,  birds[index].v) / (distance.norm() * birds[index].v.norm()) < visualAngle
		) continue;
		groupNum++;
		//[Rule 1]: collisionAvoid  [Rule 2]: velocityMatching  [Rule 3]: flockCentering
		avoidDirection+=(tmp.mul(1 / distance.norm(), distance));
		groupVelocity += birds[i].v;
		groupCenter   += distance;
	}
	avoidDirection *= -1;
	// Update Acceleration
	birds[index].a.zero(); if (groupNum == 0) return;
	birds[index].a += (avoidDirection.mul(weight[0], avoidDirection.normalized()));
	birds[index].a += (groupVelocity .mul(weight[1], groupVelocity .normalized()));
	birds[index].a += (groupCenter   .mul(weight[2], groupCenter   .normalized()));
	obstacleAvoidance(birds[index]);
}
//[ play 运行 ]
void Boids(std::vector<BoidsBird>& birds, void(*obstacleAvoidance)(BoidsBird& birds),
	double visualField, double visualAngle, double* weight, double dt = 1, double speed = 3 // 能见范围、能见角度cos、各规则权值
) {
	for (int i = 0; i < birds.size(); i++) BoidsRule(birds, i, visualField, visualAngle, weight, obstacleAvoidance);
	Mat<> tmp(3);
	for (int i = 0; i < birds.size(); i++) {
		birds[i].v += (tmp.mul(dt,         birds[i].a));
		birds[i].r += (tmp.mul(dt * speed, birds[i].v.normalized()));
	}
}
}
#endif