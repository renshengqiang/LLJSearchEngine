#ifndef _RAPTILE_ENCODING_H_
#define _RAPTILE_ENCODING_H_
#include <string>
#include <vector>
#include <set>
#include "Page.h"
// ���봦��
char *utf8_to_ansi(const char *u_str);
char *gbk_to_ansi(const char *u_str);
char *gb2312_to_ansi(const char *u_str);

// ��ҳ��������
enum CodeType
{
	ANSI,
	GB2312,
	GBK,
	UTF8
};
CodeType GetType(const std::string &str);

// �ַ���������
bool IncludeDomain(const std::string &url);		// �Ƿ�����������ͷ���ַ���
bool BeginWithSlash(const std::string &url);	// �Ƿ���б�ܿ�ͷ���ַ���
void SplitUrl(const std::string &url, std::string &domain, std::string &location);// �ָ��ַ���


// �ļ�����
void ReadUrlsFromFile(std::set<std::string> &urlSet, const std::string &filename);
void WriteUrlsToFile(const std::vector<PageInfo> &pageVec, const std::string &filename);
#endif