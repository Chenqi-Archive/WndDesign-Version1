#include <memory>

struct Uncopyable {
protected:
	Uncopyable() = default;
	~Uncopyable() = default;
public:
	Uncopyable(Uncopyable&&) = default;
	Uncopyable& operator=(Uncopyable&&) = default;
	Uncopyable(const Uncopyable&) = delete;
	Uncopyable& operator=(const Uncopyable&) = delete;
};


// The wrapper class for an dynamically allocated pointer, similar to std::unique_ptr.
template <class T>
class alloc {
private:
	T* _ptr;
public:
	explicit alloc(T* ptr = nullptr) :_ptr(ptr) {}

	alloc(alloc&& right) noexcept : _ptr(right._ptr) { right._ptr = nullptr; }
	alloc& operator=(alloc&& right) noexcept { 
		reset(right._ptr); right._ptr = nullptr; return *this;
	}
	
	alloc(const alloc&) = delete;
	alloc& operator=(const alloc&) = delete;

	~alloc() { reset(); }

	void reset(T* ptr = nullptr) {
		delete _ptr;  // No need to check if _ptr == nullptr.
		_ptr = ptr;
	}

	T* operator->() { return _ptr; }
	T& operator*() { return *_ptr; }

	template<class T2>
	friend class ref;
};


// The wrapper class for a pointer as a reference, similar to std::weak_ptr.
// It can reference an object or a alloc pointer. !!! It may be UNSAFE if the object has been deleted !!!
template <class T>
class ref {
private:
	T* _ptr;
public:
	ref() :_ptr(nullptr) {}

	ref(T* ptr) :_ptr(ptr) {}
	template<class T2>
	ref(const ref<T2>& other) {
		_ptr = other._ptr;
	}
	template<class T2>
	ref(const alloc<T2>& other) {
		_ptr = other._ptr;
	}

	ref& operator=(T* ptr) { _ptr = ptr; }

	template<class T2>
	ref& operator=(const ref<T2>& other) {
		_ptr = other._ptr; return *this;
	}
	template<class T2>
	ref& operator=(const alloc<T2>& other) { 
		_ptr = other._ptr; return *this; 
	}

	~ref() {}

	T* operator->() { return _ptr; }
	T& operator*() { return *_ptr; }

	template<class T2>
	friend class ref;
};


template<class T>
using ref = T*;


// T* or &object should never appear in the main program.

class A {
private:
	int a;
public:
	A(int a) :a(a) {}
	void f(int b) { a = b; }
};

class B : public A {
public:
	B(int a) :A(a) {}
};

int main() {
	//alloc<int> a(new int(3));
	//*a = 4;
	//ref<int> b(a);
	//*b = 5;
	//
	//ref<const int> c(b);
	//// *c = 6;  // error
	//ref<int> d(c);  // error

	//int e = 7;
	//ref<int> f = &e;

	//alloc<A> aa(new A(3));
	//alloc<A> ab(std::move(aa));
	//ref<A> ac(aa);
	//ac->f(1);  // runtime error
	//ac = ab;
	//ac->f(1);  // ok

	std::shared_ptr<int> i(new int(7));
	std::shared_ptr<const int> j(i);
	std::shared_ptr<int> k(j);
}