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
//#include <iostream>
int Page::InitContent()
{
	string head = "GET " + mUrl;
	head +=
	" HTTP/1.1\r\n"
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
	char receiveBuf[40960];
	string tempContent;

	while(true)
	{
		memset(receiveBuf, 0, 40960);
		if(recv(clientSocket, receiveBuf, 40960, 0) < 1)
			break;
		tempContent += receiveBuf;
	}

	char *ansiStr;
	switch(GetType(tempContent))
	{
	case GB2312:
		//cout << "gb2312\n";
		//ansiStr = gb2312_to_ansi(tempContent.c_str());
		mContent = tempContent;
		break;
	case GBK:
		//cout << "gbk\n";
		ansiStr = gbk_to_ansi(tempContent.c_str());
		mContent = ansiStr;
		free(ansiStr);
		break;
	case UTF8:
		//cout << "utf8\n";
		ansiStr = utf8_to_ansi(tempContent.c_str());
		mContent = ansiStr;
		free(ansiStr);
		break;
	}

	closesocket(clientSocket);

	return 0;
}

#include <fstream>
int Page::WriteToFile(const string &fileName)
{
	ofstream file = ofstream(fileName.c_str());
	if(!file) return -1;
	file << mContent;
	file.close();
	return 0;
}

/*
 * ����ҳ�ļ��еõ����еĳ����Ӻͳ����Ӷ�Ӧ�ı��⼯��
 * �㷨˼�룺
 * һ�������Ӷ�Ӧһ��href��ǩ��Ȼ���ҵ�href�����</������ǩ
 * Ȼ��������ı���Ѱ��url��title
 * url����˫���Ż��ߵ������������Ĳ��֣����ֻҪ��href������������������
 * title��</��ǩǰ�沿�֣�ֱ��>���ֵ�����
 */
void Page::GetTitlesAndUrls(vector<TitleUrl> &tuVec)
{
	string::size_type lastHrefPos = 0;

	while(true)
	{
		int firstQuotationPos, secondQuotationPos; // href�����"��"λ��
		int firstBracketPos, secondBracketPos; // href�����>��<λ��

		lastHrefPos = mContent.find(" href=", lastHrefPos+1);
		if(lastHrefPos == string::npos) break;

		secondBracketPos = mContent.find("</", lastHrefPos);
		firstBracketPos = mContent.rfind(">", secondBracketPos);
	
		if(firstBracketPos+1 < secondBracketPos)
		{
			firstQuotationPos = lastHrefPos;
			while(mContent[firstQuotationPos] != '\'' 
				&& mContent[firstQuotationPos] != '\"' 
				&& firstQuotationPos < firstBracketPos)
			{
				firstQuotationPos++;
			}

			secondQuotationPos = firstQuotationPos + 1;
			while(mContent[secondQuotationPos] != '\''
				 && mContent[secondQuotationPos] != '\"'
				 && secondQuotationPos < firstBracketPos)
			{
				secondQuotationPos++;
			}

			if(secondQuotationPos > firstQuotationPos)
			{
				string url = string(mContent, firstQuotationPos+1, secondQuotationPos-firstQuotationPos-1);
				string title = string(mContent, firstBracketPos+1, secondBracketPos-firstBracketPos-1);
				
				// ���url�а����˷Ƿ����ַ�����ֱ��ȥ��
				ExcludingIllegalCharacters(url);

				// �Ի�ȡ��url�����жϣ����Ƿ������domain
				// ���������domain,����зָ������ֱ��ʹ�õ�ǰ��domain��Ϊ������ҳ��domain
				if(IncludeDomain(url))
				{
					tuVec.push_back(TitleUrl(title, url));
				}
				else
				{
					if(!BeginWithSlash(url))
					{
						tuVec.push_back(TitleUrl(title, mDomain + "/" + url));
					}
					else
					{
						tuVec.push_back(TitleUrl(title, mDomain + url));
					}
				}
			}
		}
	}
}