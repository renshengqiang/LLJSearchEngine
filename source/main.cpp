#include <iostream>
#include <process.h>
#include "Connection.h"
#include "Page.h"
#include "Utils.h"

#pragma comment(lib,"ws2_32.lib")
using namespace std;

int nextPageIndex;
map<string, vector<TitleUrl> > result;
CRITICAL_SECTION nextPageCS, resultCS;
std::vector<DomainLocation> domainLocationVec;

DWORD WINAPI CalcFunc(LPVOID lpParameter)
{
	int nextIndex;

	do
	{
		EnterCriticalSection(&nextPageCS);
		nextIndex = nextPageIndex;
		nextPageIndex++;
		LeaveCriticalSection(&nextPageCS);

		if(nextPageIndex > domainLocationVec.size() - 1) return 0;
		std::vector<TitleUrl> tuVec;
		Page page = Page("", domainLocationVec[nextIndex].domain, domainLocationVec[nextIndex].location);
		
		if(-1 == page.InitContent() )
		{
			cout << domainLocationVec[nextIndex].domain + domainLocationVec[nextIndex].location << " get content error\n";
			goto error;
		}
		page.GetTitlesAndUrls(tuVec);

error:
		EnterCriticalSection(&resultCS);
		result.insert(make_pair(domainLocationVec[nextIndex].domain + domainLocationVec[nextIndex].location, tuVec));
		LeaveCriticalSection(&resultCS);
	}while(true);
}
/*
 * 对给定的网页进行遍历，得到每个网页中的链接
 * 返回的结果保存在一个vector中，每个网页内容中的链接保存在一个子vector中,在map中使用
 */
void IterateAllPages()
{
	int numThreads;

	SYSTEM_INFO info;  
    GetSystemInfo(&info);  
    info.dwNumberOfProcessors; 
	
	numThreads = info.dwNumberOfProcessors < domainLocationVec.size() ? info.dwNumberOfProcessors : domainLocationVec.size();
	//cout << "create " << numThreads << " threads\n";
	
	HANDLE *threads = new HANDLE[numThreads];
	for(int i=0; i<numThreads; ++i)
	{
		threads[i] = CreateThread(NULL, 0, CalcFunc, NULL, 0, NULL);
	}
	WaitForMultipleObjects(numThreads, threads, true, INFINITE);
	delete[] threads;
}

/*
 * 请求处理函数，处理过程如下：
 * 1. 从文件in.txt中获得需要查询的网站url
 * 2. 从文件out.txt中获得上次查询的结果
 * 3. 调用IterateAllPages对所有的页面进行查询，得到本次查询的结果
 * 4. 对上次的查询结果和这次查询的结果进行比较，比较的结果存放在compResult中
 */
void ProcessRequest(vector<TitleUrl> &compResult)
{
	// 为多线程做准备
	nextPageIndex = 0;
	result.clear();
	domainLocationVec.clear();

	// 从文件中取得上次处理结果
	map<string, set<string> > preResult;
	ReadPagesUrlsFromFile(preResult, "out.txt");

	// 从文件中读取需要处理的url集合
	std::vector<std::string> urlVec;
	ReadUrlsFromFile(urlVec, "in.txt");
	SplitUrlsVec(urlVec, domainLocationVec);

	// 对需要处理的url进行处理
	IterateAllPages();

	if(result.size() > 0)		// 防止网络不通的情况存在，此时不应该写入到out.txt
	{
		WritePagesUrlsToFile(result, "out.txt");
	}

	CompareResult(preResult, result, compResult);
}

int main()
{
	if(InitNetEnvironment() == -1)
	{
		cout << "Init Net Environment fail\n";
		return -1;
	}
	InitializeCriticalSection(&nextPageCS);
	InitializeCriticalSection(&resultCS);
	vector<TitleUrl> compResult;
	ProcessRequest(compResult);

//  /*
	cout << compResult.size() << endl;
	for(unsigned i=0; i<compResult.size(); ++i)
	{
		cout << compResult[i].title << " " << compResult[i].url << endl;
	}
//  */

//	/*
	char ch;
	cin >> ch;
//	*/

	CleanUpNetEnvironment();
    return 0;
}