#include <iostream>
#include <cmath>

/*

计算正方形的转动惯量，可以计算小正方形的相对质心的转动惯量 * 4, 其他3个小正方形都是一样的，这种同轴转动惯量是累加的
小正方形是使用微积分的方式计算出转动惯量的，因为一个点的转动惯量是点到转轴的距离的平方乘上点的质量。（这些点拼接成小正方形）

┏━┳━┓ <- 计算其中一个小正方形对于质心的转动惯量
┣━╋━┫
┗━┻━┛
*/


int main()
{
   double L = 3;// 正方形边长
   double M = 9;// 正方形质量（密度均匀）
   int Num = 100000;// 微积分精度（越大精度越高, 不要过大，防止数据溢出和过小归零）
   double I = 0;// 存储转动惯量（正方形几何中心）

   double m = M / Num / Num / 4;
   double d = L / 2 / Num;
   for (size_t x = 0; x < Num; ++x)
   {
      double lx = d * (x + 0.5);
      double lx2 = lx * lx;
      for (size_t y = 0; y < Num; ++y)
      {
         double ly = d * (y + 0.5);
         //double l = sqrt(lx2 + (ly * ly));
         //I += m*l*l;
         // 优化
         I += lx2 + (ly * ly);
      }
   }

   I = I * 4 * m;
   
   std::cout << I << std::endl;
   return 0;
}
