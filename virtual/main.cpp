#include <iostream>

class Person //父类
{
public:
	virtual void BuyTickets()
	{
		std::cout << "全价票" << std::endl;
	}
};

class Student : public Person //派生类继承父类
{
public:
	virtual void BuyTickets() //虚函数重写
	{
		std::cout << "半价票" << std::endl;
	}
};

void func(Person& p) //代码以引用为例，指针同理
{
	p.BuyTickets(); // 调用"同一个"成员函数（实际不是同一个）
}

int main()
{
	Person p;
	Student s;

	//父类的引用接收不同类型的对象，实现了多态
	func(p);
	func(s);

	return 0;
}
