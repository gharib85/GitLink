#ifndef RepoStatus_h
#define RepoStatus_h

#include <map>

typedef std::map<std::string, int> FileStatusMap;

class RepoStatus
{
public:
	RepoStatus(GitLinkRepository& repo);

	bool isValid() { return isValid_; };
	void updateStatus();
	void writeStatus(MLINK lnk);

private:
	bool isValid_;
	GitLinkRepository& repo_;
	FileStatusMap indexStatus_;
	FileStatusMap workingTreeStatus_;


	void writeFiles_(MLHelper& helper, const char* keyName, git_status_t status);

	static int statusCallback_(const char* path, unsigned int status_flags, void* payload);
};

#endif // RepoStatus_h
