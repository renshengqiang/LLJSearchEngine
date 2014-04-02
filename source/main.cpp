#include <iostream>
#include <set>
#include <process.h>
#include "Connection.h"
#include "Page.h"
#include "Utils.h"

#pragma comment(lib,"ws2_32.lib")

using namespace std;

// 多线程共享使用的数据结构
int nextPageIndex = 0;
vector<PageInfo> pageVec;
set<string> visitSet;
CRITICAL_SECTION nextPageCS, pageVecCS, visitSetCS;
const int threadNum = 10;

DWORD WINAPI CalcFunc(LPVOID lpParameter)
{
	int nextIndex, allSize;

	do
	{
		EnterCriticalSection(&nextPageCS);
		EnterCriticalSection(&pageVecCS);
		nextIndex = nextPageIndex;
		allSize = pageVec.size();
		nextPageIndex++;
		LeaveCriticalSection(&pageVecCS);
		LeaveCriticalSection(&nextPageCS);

		if(nextIndex >= allSize) break;

		Page page = Page(pageVec[nextIndex].title, pageVec[nextIndex].domain, pageVec[nextIndex].url);

		if(page.InitContent() == -1)
		{
			continue;
		}

		vector<PageInfo> tempVec;
		page.GetTitlesAndUrls(tempVec);

		for(vector<PageInfo>::iterator it = tempVec.begin(); it != tempVec.end(); ++it)
		{
			if(it->title.find("聘") != string::npos)
			{
				int count;
				EnterCriticalSection(&visitSetCS);
				count = visitSet.count(it->domain + it->url);
				LeaveCriticalSection(&visitSetCS);

				if( count == 0)
				{
					EnterCriticalSection(&visitSetCS);
					visitSet.insert(it->domain + it->url);
					LeaveCriticalSection(&visitSetCS);

					EnterCriticalSection(&pageVecCS);
					pageVec.push_back(*it);
					LeaveCriticalSection(&pageVecCS);
				}
			}
		}

		//cout << page.GetTitle() << endl;
		//cout << pageVec.size() << endl;
	}while(true);

	return 0;
}
void BFS(const string &title, const string &domain, const string &url)
{
	pageVec.push_back(PageInfo(title, domain, url));
	visitSet.insert(domain+url);

	InitializeCriticalSection(&nextPageCS);
	InitializeCriticalSection(&pageVecCS);
	InitializeCriticalSection(&visitSetCS);

	while(nextPageIndex < pageVec.size() && pageVec.size() < threadNum)
	{
		Page page = Page(pageVec[nextPageIndex].title, pageVec[nextPageIndex].domain, pageVec[nextPageIndex].url);

		if(page.InitContent() == -1)
		{
			++nextPageIndex;
			cout << "error\n";
			continue;
		}

		vector<PageInfo> tempVec;
		page.GetTitlesAndUrls(tempVec);

		int tempSize = tempVec.size();
		for(vector<PageInfo>::iterator it = tempVec.begin(); it != tempVec.end(); ++it)
		{
			if(it->title.find("聘") != string::npos)
			{
				if(visitSet.count(it->domain + it->url) == 0)
				{
					visitSet.insert(it->domain + it->url);
					pageVec.push_back(*it);
				}
			}
		}

		++nextPageIndex;
		//cout << page.GetTitle() << endl;
		//cout << pageVec.size() << endl;
	}

	HANDLE threads[threadNum];
	for(int i=0; i<threadNum; ++i)
	{
		threads[i] = CreateThread(NULL, 0, CalcFunc, NULL, 0, NULL);
	}
	WaitForMultipleObjects(threadNum, threads, true, INFINITE);
/*
	for(vector<PageInfo>::iterator it = pageVec.begin(); it != pageVec.end(); ++it)
	{
		cout << it-pageVec.begin() << " " << it->title << " : " << it->domain << it->url << endl;
	}
*/
}

int main()
{
	if(InitNetEnvironment() == -1)
	{
		cout << "Init Net Environment fail\n";
		return -1;
	}	
/*
	Page hustPage = Page("hust", "http://www.hust.edu.cn", "/");

	if(hustPage.InitContent() == -1)
	{
		cout << "Get content error\n";
		return -1;
	}
	if(hustPage.WriteToFile("hust.txt") == -1)
	{
		cout << "Write to file error\n";
		return -1;
	}

	// analysis the titles and urls in the page content
	vector<PageInfo> tuVec;
	hustPage.GetTitlesAndUrls(tuVec);

	for(vector<PageInfo>::iterator it = tuVec.begin(); it != tuVec.end(); ++it)
	{
		if(it->title.find("聘") != string::npos)
			cout << it->title << " : " << it->domain << it->url << endl;
	}

	char i;
	cin >> i;
*/

	//BFS("华中科技大学", "employment.hust.edu.cn", "/");
	BFS("华中师范大学", "renshi.ccnu.edu.cn", "/");

	//int num = pageVec.size();
	set<string> urlSet;
	ReadUrlsFromFile(urlSet, "result.txt");
	WriteUrlsToFile(pageVec, "result.txt");

	for(vector<PageInfo>::iterator it = pageVec.begin(); it != pageVec.end(); ++it)
	{
		string url = it->domain + it->url;
		if(urlSet.count(url) == 0)
			cout << it-pageVec.begin() << " " << it->title << " : " << it->domain << it->url << endl;
	}

	char i;
	cin >> i;


	CleanUpNetEnvironment();
    return 0;
}