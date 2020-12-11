#include <cstdio>

class MyClass {
	public:
		static void print() {
			printf("Hello world!\n");
		}
};

int main(int argc, char* argv[]) {
	MyClass::print();
	return 0;
}
