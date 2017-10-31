#include <string>
#include <iostream>
#include <memory>

class A
{
public:
	A(int i) : id(i) {}
	std::shared_ptr<A> c;
	~A() {std::cout << "~A " << id << std::endl; }
private:
	int id;
};

void print(A *a, A* b, int line)
{
	std::cout << "line: " << line;
	std::cout << ", a.count " << b->c.use_count();
	std::cout << ", b.count " << a->c.use_count() << std::endl;
}

int main()
{
	std::shared_ptr<A> a(new A(1));
	std::shared_ptr<A> b(new A(2));
	A *pa = a.get();
	A *pb = b.get();
	print(pa, pb, __LINE__);
	a->c = b;
	print(pa, pb, __LINE__);
	b->c = a;
	print(pa, pb, __LINE__);
	a.reset();
	print(pa, pb, __LINE__);
	b.reset();
	print(pa, pb, __LINE__);
}
