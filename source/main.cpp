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
 * �Ը�������ҳ���б������õ�ÿ����ҳ�е�����
 * ���صĽ��������һ��vector�У�ÿ����ҳ�����е����ӱ�����һ����vector��,��map��ʹ��
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
 * ��������������������£�
 * 1. ���ļ�in.txt�л����Ҫ��ѯ����վurl
 * 2. ���ļ�out.txt�л���ϴβ�ѯ�Ľ��
 * 3. ����IterateAllPages�����е�ҳ����в�ѯ���õ����β�ѯ�Ľ��
 * 4. ���ϴεĲ�ѯ�������β�ѯ�Ľ�����бȽϣ��ȽϵĽ�������compResult��
 */
void ProcessRequest(vector<TitleUrl> &compResult)
{
	// Ϊ���߳���׼��
	nextPageIndex = 0;
	result.clear();
	domainLocationVec.clear();

	// ���ļ���ȡ���ϴδ�����
	map<string, set<string> > preResult;
	ReadPagesUrlsFromFile(preResult, "out.txt");

	// ���ļ��ж�ȡ��Ҫ�����url����
	std::vector<std::string> urlVec;
	ReadUrlsFromFile(urlVec, "in.txt");
	SplitUrlsVec(urlVec, domainLocationVec);

	// ����Ҫ�����url���д���
	IterateAllPages();

	if(result.size() > 0)		// ��ֹ���粻ͨ��������ڣ���ʱ��Ӧ��д�뵽out.txt
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