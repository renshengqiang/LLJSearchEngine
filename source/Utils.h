#ifndef _RAPTILE_ENCODING_H_
#define _RAPTILE_ENCODING_H_
#include <string>
#include <vector>
#include <set>
#include "Page.h"
// 编码处理
char *utf8_to_ansi(const char *u_str);
char *gbk_to_ansi(const char *u_str);
char *gb2312_to_ansi(const char *u_str);

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
bool IncludeDomain(const std::string &url);		// 是否是以域名开头的字符串
bool BeginWithSlash(const std::string &url);	// 是否以斜杠开头的字符串
void SplitUrl(const std::string &url, std::string &domain, std::string &location);// 分割字符串


// 文件处理
void ReadUrlsFromFile(std::set<std::string> &urlSet, const std::string &filename);
void WriteUrlsToFile(const std::vector<PageInfo> &pageVec, const std::string &filename);
#endif