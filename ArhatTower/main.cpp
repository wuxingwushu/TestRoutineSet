/*****************************************************************
三层罗汉塔：
   (0)        (1)         (2)
    |          |           |
   ###         |           |
  #####        |           |
 #######       |           |
=======================================
规则：
1. 大环不可以压小环
2. 环只可以一个一个取
3. 环只可以取对应柱子最顶上的一环
4. 将 0柱子 的塔全部移动到 2柱子 上为胜利


罗汉塔解题思路

假设移动四层罗汉塔：
1. 0柱子 除最底层环以外全部先移动到 1柱子。
2. 再将最底层的移动到 2柱子。
3. 再将 1柱子 全部移动到 2柱子。
（因为移动最上面的所有环，都比最底下的环小，所以没有因为最底下的环导致出现不能移动的柱子）
哪三层罗汉塔怎么移动呢，重复上步骤

可以推理出： (N)层塔移动 = (N-1)层塔移动 + 最底层移动 + (N-1)层塔移动

2层罗汉塔的最小步骤解题方法： 0->1, 0->2, 1->2
3层罗汉塔的最小步骤解题方法： 0->2, 0->1, 2->1, 0->2, 1->0, 1->2, 0->2
4层罗汉塔的最小步骤解题方法： 0->1, 0->2, 1->2, 0->1, 2->0, 2->1, 0->1, 0->2, 1->2, 1->0, 2->0, 1->2, 0->1, 0->2, 1->2

1层塔移动 = 0层塔移动 + 1层塔移动(0->2) + 0层塔移动
2层塔移动 = 1层塔移动(0->1) +  最底层移动(0->2) + 1层塔移动(1->2)
3层塔移动 = 2层塔移动(0->2, 0->1, 2->1) + 最底层移动(0->2) + 2层塔移动(1->0, 1->2, 0->2)

为什么上面相同层数移动步骤不一样，那是因为从不同柱子移动到不同的目的地柱子导致的：
以三层塔的 2层塔移动 为例,
2层塔移动(0->2, 0->1, 2->1)： 是从 0柱子 移动到 1柱子。
2层塔移动(1->0, 1->2, 0->2)： 是从 1柱子 移动到 2柱子。

那怎么选择移动到那个柱子呢？（注意：这里只以 0柱子 被移动为例）
可以观察 1. 2. 3. 的执行步骤
N层的最低层环只移动一次，只移动到目的地 2柱子。
目的地2柱子要是空出来，所以N-1层是放在 1柱子。
那 N-1层最底层环要出现在 1柱子上，怎么说 当时的 N-2层 将只可能存放在 2柱子上；


以此类推可以得出第一步放 1柱子 还是 2柱子，是根据需要移动的罗汉塔层数决定的：奇数：放2柱子，偶数：放1柱子。

*****************************************************************/

#include <iostream>
#include <vector>

#define TaSize 5 // 罗汉塔数量（最大255<unsigned char>）

/**
 * @brief 计算移动N层罗汉塔的最小执行步骤
 * @param N 层数
 * @return 最小执行数 */
unsigned int LHT_BZS(unsigned int N)
{
   if (N == 1) return 1;
   return (LHT_BZS(N - 1) * 2) + 1;
}

/**
 * @brief 罗汉塔移塔步骤生成函数
 * @param Ta 塔的数组[3][对应塔的信息]
 * @param Num t1塔的层数
 * @param t1 被移动的塔（柱子编号）
 * @param t2 空地（柱子编号）
 * @param t3 移动到的位置（柱子编号） */
void LHT_YT(std::vector<std::vector<unsigned char>> Ta, int Num, unsigned char t1, unsigned char t2, unsigned char t3)
{
   if ((Ta[2].size() == TaSize) || (Num == 0))
   {
      return;// 结束了
   }
   bool swBool = false; // 记录是否调换过
   if ((Ta[t1].size() % 2) == 0)// 通过奇偶判断放那个位置
   { 
      std::swap(t2, t3);
      swBool = true;
   }
   // 将 t1柱子 顶上的一环移到 t3柱子 去
   Ta[t3].push_back(Ta[t1].back());
   Ta[t1].pop_back();
   std::cout << (int)t1 << "->" << (int)t3 << std::endl;

   // 判断 t2柱子 是否有塔, 有塔就将塔移动到 t3柱子
   if (Ta[t2].size() > 0)
   {
      // 将 t2柱子的塔 复制一份
      std::vector<std::vector<unsigned char>> LTa;
      LTa.resize(3);
      LTa[t2] = Ta[t2];
      // 将 t2柱子的塔 移动到 t3柱子
      LHT_YT(LTa, LTa[t2].size(), t2, t1, t3);

      // 因为上面的移动是在复制体进行的，需要手动获取移动后的结果
      for (size_t i = 0; i < Ta[t2].size(); i++)
      {
         Ta[t3].push_back(Ta[t2][i]);
      }
      Ta[t2].clear();
   }
   // 如果调换过就调换回来
   if (swBool)
   {
      std::swap(t2, t3);
   }
   // 继续执行下一层塔
   LHT_YT(Ta, Ta[t1].size(), t1, t2, t3);
}

int main()
{
   std::vector<std::vector<unsigned char>> mTa;
   mTa.resize(3);
   for (int i = 0; i < TaSize; ++i)
   {
      mTa[0].push_back(TaSize - i);
   }
   LHT_YT(mTa, mTa[0].size(), 0, 1, 2);
   std::cout << "Run Size: " << LHT_BZS(TaSize) << std::endl;
   return 0;
}