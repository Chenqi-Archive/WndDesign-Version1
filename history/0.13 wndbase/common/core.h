#pragma once


#ifdef WNDDESIGN_EXPORTS
#define WNDDESIGN_API __declspec(dllexport)
#else
#define WNDDESIGN_API __declspec(dllimport)
#endif


#define BEGIN_NAMESPACE(name) namespace name {
#define END_NAMESPACE(name)   }


#define Anonymous
#define pure = 0


// A Tag for pointers. Every pointer as member or variable should have a tag.
//   Usage example: Ref T* p;

#define Alloc   // The content is allocated and you should manually delete it.
				// You'd better use a whold object rather than the pointer as a member,
				//   unless you need polymorphism or dynamic array allocation.

#define Ref     // A reference to other object.
				//   You should pay attention that it might be invalid.




BEGIN_NAMESPACE(WndDesign)

using uchar = unsigned char;
using ushort = unsigned short;
using uint = unsigned int;

END_NAMESPACE(WndDesign)
