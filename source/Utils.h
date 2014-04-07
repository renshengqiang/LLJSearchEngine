#ifndef _RAPTILE_ENCODING_H_
#define _RAPTILE_ENCODING_H_
#include <string>
#include <vector>
#include <map>
#include <set>
#include "Page.h"
// 编码处理
char *utf8_to_ansi(const char *u_str);
char *gbk_to_ansi(const char *u_str);

// 网页编码类型
enum CodeType
{
	ANSI,
	GB2312,
	GBK,
	UTF8
};
CodeType GetType(const std::string &str);

// 字符串处理函数
// 是否是以域名开头的字符串
bool IncludeDomain(const std::string &url);		
// 是否以斜杠开头的字符串
bool BeginWithSlash(const std::string &url);	
// 将一个url分解成domain加上一个location,例如www.hust.edu.cn分解成 hust.edu.cn 和 /
void SplitUrl(const std::string &url, std::string &domain, std::string &location); 
// 将从文件中得到的url向量分解成domain和location向量
void SplitUrlsVec(const std::vector<std::string> &urlVec, std::vector<DomainLocation> &domainLocationVec);
// 去除字符串中的非法字符
void ExcludingIllegalCharacters(std::string &str);

// 文件处理
// 从文件中读取需要处理的urls集合
void ReadUrlsFromFile(std::vector<std::string> &urlVec, const std::string &filename);
// 从文件中将上次读取的结果提取出来
void ReadPagesUrlsFromFile(std::map<std::string, std::set<std::string> > &result, const std::string &filename);
// 将本次处理的结果保存在文件中
void WritePagesUrlsToFile(const std::map<std::string, std::vector<TitleUrl> > &result, const std::string &filename);
// 将上次的结果和本次的结果进行比较，结果保存到一个vector中
void CompareResult(std::map<std::string, std::set<std::string> > &preResult,
							  std::map<std::string, std::vector<TitleUrl> > &thisResult,
							  std::vector<TitleUrl> &compResult);
// 将比较的结果保存到一个文档中，写入按照html文档的格式进行编写
void WriteCompareResultToHtmlDocument(const std::vector<TitleUrl> &compResult, const std::string &fileName);

#endif