#pragma once

#ifdef WNDDESIGN_EXPORTS
#define WNDDESIGN_API __declspec(dllexport)
#else
#define WNDDESIGN_API __declspec(dllimport)
#endif


#define BEGIN_NAMESPACE(name) namespace name {
#define END_NAMESPACE(name)   }


#define AbstractInterface struct _declspec(novtable)


#define Anonymous
#define pure = 0


BEGIN_NAMESPACE(WndDesign)


// A tag for pointers or any resource to indicate whether it is dynamically 
//   allocated(Alloc) so you should manually release or delete it, or just a reference(Ref).

// You'd better use T& rather than Ref<T*>, unless you must change the reference.

// May add some compile-time checking later. (move only, etc)

template<class T>
using Ref = T;

template<class T>
using Alloc = T;

template<class T>
using RefArray = T;

template<class T>
using AllocArray = T;


using uchar = unsigned char;
using ushort = unsigned short;
using uint = unsigned int;
using wchar = wchar_t;


template<class T>
inline const T max(T a, T b) { return a > b ? a : b; }
template<class T>
inline const T min(T a, T b) { return a < b ? a : b; }


END_NAMESPACE(WndDesign)
