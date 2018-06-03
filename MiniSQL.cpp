#include <iostream>
#include <io.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include "Buffer.h"

using namespace std;
int main()
{
	//MemFile file("test.data");
	BUFFER buffer;
	auto x = buffer.ReadFile("test.data", 0);
	cout << x.filePageID << endl;
	cout << x.offSet << endl;
	system("pause");
}