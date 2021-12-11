#include <vector>
#include <list>
#include <iostream>

using namespace std;

int main() {

	vector<int> v = { 1,2,3,4,5 };
	list<int> l = { 1,2,3,4,5 };

	cout << *v.rbegin().base() << endl;	// Error!
	cout << *l.rbegin().base() << endl;	// Error!

	cout << *(v.rbegin().base() - 1) << endl;	// Ok!
	cout << *(--l.rbegin().base()) << endl;	// Ok!

	cout << *v.rend().base() << endl;	// Ok!
	cout << *l.rend().base() << endl;	// Ok!
}