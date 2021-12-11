#pragma once


#ifdef DLLTEST_EXPORTS
#define DLLTEST_API __declspec(dllexport)
#else
#define DLLTEST_API __declspec(dllimport)
#endif

struct A {
	DLLTEST_API virtual void f() = 0;
};

struct B : public A {
	DLLTEST_API void f() override;
};