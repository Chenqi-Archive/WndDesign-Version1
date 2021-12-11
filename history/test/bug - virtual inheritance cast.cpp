class A {
public:
	virtual int f() {
		return 1;
	}
};

class B : virtual public A {  // virtual inheritance
	int a;
public:
	B(int a) :a(a) {}
	int f() override {
		return a;
	}
};

int main() {
	A* a = new B(3);
	B* b1 = reinterpret_cast<B*>(a);
	B* b2 = dynamic_cast<B*>(a);
}