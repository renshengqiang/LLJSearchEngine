#include "Connection.h"
#include "Page.h"
#include "Utils.h"
#define HTTPPORT 80

using namespace std;

Page::Page(const string &title, const string &domain, const string &url):
	mTitle(title), mDomain(domain), mUrl(url)
{
	if(mDomain.find("https") == 0)
	{
		mDomain = string(mDomain, 8, mDomain.length() - 8);
	}
	else if(mDomain.find("http") == 0)
	{
		mDomain = string(mDomain, 7, mDomain.length() - 7);
	}
}

int Page::InitContent()
{
	string head = "GET " + mUrl;

	head +=
	" HTTP/1.1\r\n"
	"Accept:*/*\r\n"
	"Accept-Encoding: gzip, deflate\r\n"
	"User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; CIBA; TheWorld)\r\n"
	"Host:";
	head += mDomain;
	head += "\r\nConnection: Keep-Alive\r\n\r\n";

	SOCKET clientSocket = ConnectByURL(mDomain.c_str(), HTTPPORT);

	if(clientSocket == INVALID_SOCKET)
	{
		return -1;	// connect to host fail
	}

	send(clientSocket, head.c_str(), head.length()+1, 0);
	char receiveBuf[4096];
	string tempContent;

	while(true)
	{
		if(recv(clientSocket, receiveBuf, 4096, 0) < 1)
			break;
		string temp = receiveBuf;
		if(temp.length() > 4096)
			temp = temp.substr(0, 4096);
		tempContent += temp;
	}

	char *ansiStr;
	switch(GetType(tempContent))
	{
	case GB2312:
		ansiStr = gb2312_to_ansi(tempContent.c_str());
		break;
	case GBK:
		ansiStr = gbk_to_ansi(tempContent.c_str());
		break;
	case UTF8:
		ansiStr = utf8_to_ansi(tempContent.c_str());
		break;
	}

	mContent = ansiStr;
	free(ansiStr);
	closesocket(clientSocket);

	return 0;
}

#include <stdio.h>
int Page::WriteToFile(const string &fileName)
{
	FILE *fp = fopen(fileName.c_str(), "w");
	if(NULL == fp)
	{
		return -1;
	}
	fprintf(fp, "%s", mContent.c_str());
	fclose(fp);
	return 0;
}

void Page::GetTitlesAndUrls(vector<PageInfo> &tuVec)
{
	string::size_type lastHrefPos = 0;

	while(true)
	{
		int firstQuotationPos, secondQuotationPos; // href后面的"和"位置
		int firstBracketPos, secondBracketPos; // href后面的>和<位置

		lastHrefPos = mContent.find("href", lastHrefPos+1);
		if(lastHrefPos == string::npos) break;

		firstBracketPos = mContent.find(">", lastHrefPos);
		secondBracketPos = mContent.find("<", firstBracketPos);
		if(firstBracketPos+1 < secondBracketPos)
		{
			firstQuotationPos = mContent.find("\"",lastHrefPos);
			secondQuotationPos = mContent.find("\"", firstQuotationPos+1);
			if(secondQuotationPos > firstQuotationPos)
			{
				string url = string(mContent, firstQuotationPos+1, secondQuotationPos-firstQuotationPos-1);
				string title = string(mContent, firstBracketPos+1, secondBracketPos-firstBracketPos-1);
				
				// 对获取的url进行判断，看是否包含了domain
				// 如果包含了domain,则进行分割处理；否则直接使用当前的domain作为下面网页的domain
				if(IncludeDomain(url))
				{
					string domain,location;
					SplitUrl(url, domain, location);
					tuVec.push_back(PageInfo(title, domain, location));
				}
				else
				{
					if(!BeginWithSlash(url))
					{
						url = string(1, '/') + url;
					}
					tuVec.push_back(PageInfo(title, mDomain, url));
				}
			}
		}
	}
}