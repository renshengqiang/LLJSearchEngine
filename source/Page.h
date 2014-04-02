#ifndef _RAPTILE_PAGE_H_
#define _RAPTILE_PAGE_H_
#include <string>
#include <vector>

struct PageInfo
{
	std::string title;
	std::string domain;
	std::string url;
	PageInfo(const std::string &_title, const std::string &_domain, const std::string &_url):
		title(_title), domain(_domain), url(_url)
	{}
};

class Page
{
public:
	Page(const std::string &title, const std::string &domain, const std::string &url);

	std::string GetTitle() { return mTitle; };
	std::string GetDomain() { return mDomain; };
	std::string GetUrl() { return mUrl; };

	/*get content from the given url
	return value:
	0 for success, -1 for fail
	*/
	int InitContent(); 

	/* write the page content to the given file
	return value:
	0 for success, -1 for fail
	*/
	int WriteToFile(const std::string &fileName);

	/* iterare the file and look for urls in the page
	*/
	void GetTitlesAndUrls(std::vector<PageInfo> &tuVec);

private:
	std::string mTitle;		// ÍøÒ³±êÌâ
	std::string mDomain;	// server domain
	std::string mUrl;		// url
	std::string mContent;	// ÍøÒ³ÄÚÈİ
};

#endif