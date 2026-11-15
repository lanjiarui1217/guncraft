#include <cstdio>
#include <cstring>
#include <algorithm>
#include <vector>
#include <windows.h>
#include <queue>
//using namespace std;
typedef long long ll;
typedef unsigned long long ull;
int main()
{
	while(1)
	{
		system("cls");
		FILE* f=fopen("console.txt","r");
		char ch=fgetc(f);
		while(ch!=EOF) putchar(ch),ch=fgetc(f);
		fclose(f);f=nullptr;
		Sleep(333);
	}
	return 0;
}
