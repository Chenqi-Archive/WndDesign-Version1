
class A {
public:
	int a = 1;
};

class B : public A {

};


int main() {
	A a;
	B* b;

	auto lm = [&a](auto& b) {
		b = static_cast<std::remove_reference<decltype(b)>::type>(&a);
	};

	lm(b);
}