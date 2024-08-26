#pragma once
#include <iostream>



struct MyStruct {
	int MI = 10;
	float MF = 0.5f;
	char MC;
};

class SonClass {
public:

	void SonFunc() {
		int a = 1;
	}

	void SonFuncKao(int Pd, char C = 'a') {
		int a = Pd;
	}

	int Sonfield = 0;
};

class SonClass22 {
public:

	void SonFuncddd() {
		int a = 1;
	}

	void SonFuncKaoddd(int Pd, char C = 'a') {
		int a = Pd;
	}

	int Sonfieldddd = 0;
};


class MyClass:public SonClass, SonClass22 {
public:

	void Func() {
		int a = 1;
	}

	void FuncKao(int Pd, char C = 'a') {
		int a = Pd;
	}

	int field = 0;
};
