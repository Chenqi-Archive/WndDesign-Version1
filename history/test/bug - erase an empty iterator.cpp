#include <list>

using namespace std;

int main() {
	list<int> a({ 1,2,3 });
	a.erase(a.end());  // Error!
}