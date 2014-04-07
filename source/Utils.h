#ifndef _RAPTILE_ENCODING_H_
#define _RAPTILE_ENCODING_H_
#include <string>
#include <vector>
#include <map>
#include <set>
#include "Page.h"
// ���봦��
char *utf8_to_ansi(const char *u_str);
char *gbk_to_ansi(const char *u_str);

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
// �Ƿ�����������ͷ���ַ���
bool IncludeDomain(const std::string &url);		
// �Ƿ���б�ܿ�ͷ���ַ���
bool BeginWithSlash(const std::string &url);	
// ��һ��url�ֽ��domain����һ��location,����www.hust.edu.cn�ֽ�� hust.edu.cn �� /
void SplitUrl(const std::string &url, std::string &domain, std::string &location); 
// �����ļ��еõ���url�����ֽ��domain��location����
void SplitUrlsVec(const std::vector<std::string> &urlVec, std::vector<DomainLocation> &domainLocationVec);
// ȥ���ַ����еķǷ��ַ�
void ExcludingIllegalCharacters(std::string &str);

// �ļ�����
// ���ļ��ж�ȡ��Ҫ�����urls����
void ReadUrlsFromFile(std::vector<std::string> &urlVec, const std::string &filename);
// ���ļ��н��ϴζ�ȡ�Ľ����ȡ����
void ReadPagesUrlsFromFile(std::map<std::string, std::set<std::string> > &result, const std::string &filename);
// �����δ���Ľ���������ļ���
void WritePagesUrlsToFile(const std::map<std::string, std::vector<TitleUrl> > &result, const std::string &filename);
// ���ϴεĽ���ͱ��εĽ�����бȽϣ�������浽һ��vector��
void CompareResult(std::map<std::string, std::set<std::string> > &preResult,
							  std::map<std::string, std::vector<TitleUrl> > &thisResult,
							  std::vector<TitleUrl> &compResult);
// ���ȽϵĽ�����浽һ���ĵ��У�д�밴��html�ĵ��ĸ�ʽ���б�д
void WriteCompareResultToHtmlDocument(const std::vector<TitleUrl> &compResult, const std::string &fileName);

#endif