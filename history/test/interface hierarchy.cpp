
#include <iostream>

using uint = unsigned int;
using uchar = unsigned char;

#define pure  = 0

using namespace std;

struct Interface {
	virtual void DoSomething() pure;
};

class _Impl : virtual public Interface {
public:
	void DoSomething() override{
		cout << "Interface implement.\n";
	}
};

struct Jnterface : virtual public Interface {
	virtual void DoAnything() pure;
};

class _Jmpl : virtual public Jnterface, public _Impl {
public:
	void DoSomething() override {
		cout << "Jnterface jmplement.\n";
	}
	void DoAnything() override {
		cout << "Jnterface jmplement do any thing.\n";
	}
};


int main() {
	Interface* i = new _Impl();
	Jnterface* j = new _Jmpl();

	i->DoSomething();
	j->DoSomething();
	j->DoAnything();
}