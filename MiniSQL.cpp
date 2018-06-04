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
	//buffer.CreateFile("test");
	auto pMemPage = buffer.GetMemFile("test");
	pMemPage->AddOnePage(buffer.GetPtr2Clock());
	system("pause");
}