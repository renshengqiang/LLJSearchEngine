#include <Windows.h>
#include <stdio.h>
#include <string>
#include "Utils.h"

WCHAR *utf8_to_unicode(const char *u_str)
{
	int w_len, len;
	WCHAR *w_str;

	w_len = MultiByteToWideChar(CP_UTF8, 0, u_str, -1, NULL, 0);
	if (w_len <= 0)
		return NULL;
	w_str = (WCHAR *)malloc(w_len * sizeof(WCHAR)) ;
	if (w_str == NULL) {
		printf("error: no memory!\n");
		return NULL;
	}
	len = MultiByteToWideChar(CP_UTF8, 0, u_str, -1, w_str, w_len);
	
	if (len < 0) 
	{
		free(w_str);
		return NULL;
	} 
	else if (len < w_len)
		w_str[len] = 0;
	
	return w_str;
}

WCHAR *gbk_to_unicode(const char *u_str)
{
	int w_len, len;
	WCHAR *w_str;

	w_len = MultiByteToWideChar(CP_ACP, 0, u_str, -1, NULL, 0);
	if (w_len <= 0)
		return NULL;
	w_str = (WCHAR *)malloc(w_len * sizeof(WCHAR)) ;
	if (w_str == NULL) {
		printf("error: no memory!\n");
		return NULL;
	}
	len = MultiByteToWideChar(CP_ACP, 0, u_str, -1, w_str, w_len);
	
	if (len < 0) 
	{
		free(w_str);
		return NULL;
	} 
	else if (len < w_len)
		w_str[len] = 0;
	
	return w_str;
}

char *unicode_to_utf8(const WCHAR *w_str)
{
	int u_len, len;
	char *u_str;

	u_len = WideCharToMultiByte(CP_UTF8, 0, w_str, -1, NULL, 0, NULL, NULL);
	if (u_len <= 0)
		return NULL;
	u_str = (char *)malloc(u_len * sizeof(char)) ;
	if (u_str == NULL) {
		printf("error: no memory!\n");
		return NULL;
	}
	len = WideCharToMultiByte(CP_UTF8, 0, w_str, -1, u_str, u_len, NULL, NULL);
	
	if (len < 0) {
		free(u_str);
		return NULL;
	} else if (len < u_len)
		u_str[len] = 0;
	
	return u_str;
}

WCHAR *ansi_to_unicode(const char *a_str)
{
	int w_len, len;
	WCHAR *w_str;

	w_len = MultiByteToWideChar(CP_ACP, 0, a_str, -1, NULL, 0);
	if (w_len <= 0)
		return NULL;
	w_str = (WCHAR *)malloc(w_len * sizeof(WCHAR)) ;
	if (w_str == NULL) {
		printf("error: no memory!\n");
		return NULL;
	}
	len = MultiByteToWideChar(CP_ACP, 0, a_str, -1, w_str, w_len);
	
	if (len < 0) {
		free(w_str);
		return NULL;
	} else if (len < w_len)
		w_str[len] = 0;
	
	return w_str;
}

char *unicode_to_ansi(const WCHAR *w_str)
{
	int a_len, len;
	char *a_str;

	a_len = WideCharToMultiByte(CP_ACP, 0, w_str, -1, NULL, 0, NULL, NULL);
	if (a_len <= 0)
		return NULL;
	a_str = (char *)malloc(a_len * sizeof(char)) ;
	if (a_str == NULL) {
		printf("error: no memory!\n");
		return NULL;
	}
	len = WideCharToMultiByte(CP_ACP, 0, w_str, -1, a_str, a_len, NULL, NULL);
	
	if (len < 0) {
		free(a_str);
		return NULL;
	} else if (len < a_len)
		a_str[len] = 0;
	
	return a_str;
}

char *utf8_to_ansi(const char *u_str)
{
	WCHAR *w_str;
	char *a_str;

	w_str = utf8_to_unicode(u_str);
	a_str = unicode_to_ansi(w_str);
	
	free(w_str);

	return a_str;
}

char *gbk_to_ansi(const char *u_str)
{
	WCHAR *w_str = gbk_to_unicode(u_str);
	char *a_str = unicode_to_ansi(w_str);
	free(w_str);
	return a_str;
}

char *ansi_to_utf8(const char *a_str)
{
	WCHAR *w_str;
	char *u_str;

	w_str = ansi_to_unicode(a_str);
	u_str = unicode_to_utf8(w_str);
	
	free(w_str);

	return u_str;
}


/*从网页文件的开始部分进行搜索，用以确定网页的编码*/
CodeType GetType(const std::string &str)
{
	int pos = str.find("charset");

	if(pos == std::string::npos)
	{
		return UTF8;	// default UTF8
	}

	if(pos + 8 < str.length())
	{
		std::string typeStr(str.c_str()+pos+8, 10);
		
		if(typeStr.find("gb2312") == 0 || typeStr.find("GB2312") == 0)
			return GB2312;
		else if(typeStr.find("gbk") == 0 || typeStr.find("GBK") == 0)
			return GBK;
		else if(typeStr.find("utf-8") == 0 || typeStr.find("UTF-8") == 0)
			return UTF8;
	}
	return UTF8;
}

using namespace std;
// 字符串处理函数
bool IncludeDomain(const string &url)
{
	bool _1 = url.find(".cn") != string::npos;
	bool _2 = url.find(".net") != string::npos;
	bool _3 = url.find(".com") != string::npos;
	bool _4 = url.find(".gov") != string::npos;
	bool _5 = url.find(".cc") !=  string::npos;

	return _1 || _2 || _3 || _4 || _5;
}

bool BeginWithSlash(const std::string &url)
{
	return url[0] == '/';
}

/*
 * 将一个完整的url分解成一个domain和一个location
 * 将前面的http去除，查找第一个/的位置
 */
void SplitUrl(const string &_url, string &domain, string &location)
{
	// 去除前面的http或者https前缀
	string url = _url;
	if(url.find("https") == 0)
	{
		url = string(url, 8, url.length() - 8);
	}
	else if(url.find("http") == 0)
	{
		url = string(url, 7, url.length() - 7);
	}

	// 查找第一个斜杠的位置
	string::size_type pos;
	pos = url.find("/");
	if(pos != url.npos)
	{
		domain = string(url, 0, pos);
		location = string(url, pos, url.length()-pos);
	}
	else
	{
		domain = url;
		location = "/";
	}
}

void SplitUrlsVec(const std::vector<std::string> &urlVec, std::vector<DomainLocation> &domainLocationVec)
{
	for(unsigned i = 0; i<urlVec.size(); ++i)
	{
		std::string domain, location;
		SplitUrl(urlVec[i], domain, location);
		domainLocationVec.push_back(DomainLocation(domain, location));
	}
}

void ExcludingIllegalCharacters(string &str)
{
	int firstIllegalPos = 0, secondIllegalPos;

	while(firstIllegalPos < str.length() && 
		str[firstIllegalPos]>31 && str[firstIllegalPos]<127)
	{
		firstIllegalPos++;
	}
	if(firstIllegalPos == str.length()) return;

	secondIllegalPos = firstIllegalPos;
	while(secondIllegalPos < str.length() &&
		(str[secondIllegalPos]>126 || str[secondIllegalPos]<32))
	{
		secondIllegalPos++;
	}

	string firstPart, secondPart;
	if(firstIllegalPos > 0)
	{
		firstPart = string(str, 0, firstIllegalPos);
	}
	if(secondIllegalPos < str.length())
	{
		secondPart = string(str, secondIllegalPos, str.length() - secondIllegalPos);
	}
	str = firstPart+secondPart;
	return;
}


#include <fstream>
void ReadUrlsFromFile(std::vector<std::string> &urlVec, const std::string &filename)
{
	ifstream file = ifstream(filename.c_str());
	string line;

	if(file)
	{
		while(getline(file, line))
		{
			urlVec.push_back(line);
		}
		file.close();
	}
	return;
}

void ReadPagesUrlsFromFile(std::map<std::string, std::set<std::string> > &result, const std::string &filename)
{
	ifstream file = ifstream(filename.c_str());
	std::string temp;
	if(!file) return;

	unsigned num;

	file >> num;
	getline(file, temp);
	for(unsigned i=0; i<num; ++i)
	{
		std::string url;
		getline(file, url);

		unsigned innNum;
		std::set<string> tuSet;
		file >> innNum;
		getline(file, temp);
		for(unsigned j=0; j<innNum; ++j)
		{
			std::string innUrl;
			getline(file, innUrl);
			tuSet.insert(innUrl);
		}
		result.insert(make_pair(url, tuSet));
	}
	file.close();
}

void WritePagesUrlsToFile(const std::map<std::string, std::vector<TitleUrl> > &result, const std::string &filename)
{
	ofstream file = ofstream(filename.c_str());
	int num = result.size();

	file << num << "\n";
	if(file)
	{
		file.clear();
		for(std::map<std::string, std::vector<TitleUrl> >::const_iterator it = result.begin();
			it != result.end(); ++it)
		{
			file << it->first << "\n" << it->second.size() << "\n";
			for(unsigned i=0; i<it->second.size(); ++i)
			{
				file << it->second[i].url << "\n";
			}
		}
		file.close();
	}

	return;
}

void CompareResult(map<string, set<string> > &preResult,
				   map<string, vector<TitleUrl> > &thisResult,
				   vector<TitleUrl> &compResult)
{
	for(std::map<std::string, std::vector<TitleUrl> >::iterator thisIter = thisResult.begin();
		thisIter != thisResult.end();
		++thisIter)
	{
		map<string, set<string> >::iterator preIter = preResult.find(thisIter->first);
		if(preIter != preResult.end()) // 遍历每个url，看是否有更新的
		{
			for(std::vector<TitleUrl>::iterator innIter = thisIter->second.begin();
				innIter != thisIter->second.end();
				++innIter)
			{
				if(preIter->second.find(innIter->url) == preIter->second.end())	// 未找到则插入
				{
					compResult.push_back(TitleUrl(innIter->title, innIter->url));
				}
			}
		}
		else   // 没有找到对应的主页url
		{
			for(std::vector<TitleUrl>::iterator innIter = thisIter->second.begin();
				innIter != thisIter->second.end();
				++innIter)
			{
				compResult.push_back(TitleUrl(innIter->title, innIter->url));
			}
		}
	}
}

void WriteCompareResultToHtmlDocument(const std::vector<TitleUrl> &compResult, const std::string &fileName)
{
	ofstream file = ofstream(fileName.c_str());

	if(!file) return;

	file << "<html>\n";
	file << "<body>\n";
	for(unsigned i = 0; i < compResult.size(); ++i)
	{
		file << "<a href=\"http://" 
			 << compResult[i].url
			 << "\" target=\"_blank\">"
			 << compResult[i].title
			 << "</a>";
		file << "<p></p>\n";
	}
	file << "</body>\n";
	file << "</html>";

	file.close();
}