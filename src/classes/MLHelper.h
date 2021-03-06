#ifndef MLHelper_h_
#define MLHelper_h_ 1

#include <deque>
#include <string>
#include <cstring>

#include "PathString.h"

class GitLinkRepository;
class MLExpr;
class Signature;

class MLHelper
{
public:
	MLHelper(MLINK lnk);
	MLHelper(MLEnvironment env, MLExpr& expr);
	~MLHelper();

	void processAndIgnore(WolframLibraryData libData);

	void beginFunction(const char* head);
	void beginFunction(const MLExpr& expr);
	void endFunction();
	void endAllFunctions();

	void beginList() { beginFunction("List"); };
	void endList() { endFunction(); };

	void putString(const char* value);
	void putString(const std::string& value) { putString(value.c_str()); };
	void putString(const PathString& value) { putString(value.native()); };
	void putSymbol(const char* value);
	void putMint(mint value);
	void putInt(int value);
	void putOid(const git_oid& value);
	void putRepo(const GitLinkRepository& repo);
	void putGitObject(const git_oid& value, const GitLinkRepository& repo);
	void putGitObject(const git_oid& value, const std::string& repoKey);
	void putExpr(const MLExpr& expr);
	void putMessage(const char* symbol, const char* tag);
	void putBlobUTF8String(const git_blob* value);
	void putBlobByteString(const git_blob* value);

	void putRule(const char* key);
	void putRule(const char* key, int value); // boolean
	void putRule(const char* key, double value);
	void putRule(const char* key, const MLExpr& value);
	void putRule(const char* key, const git_time& value);
	void putRule(const char* key, const char* value, const char* symbolFallback = "$Failed");
	void putRule(const char* key, const std::string& value) { putRule(key, value.c_str()); };
	void putRule(const char* key, const PathString& value) { putRule(key, value.native()); };
	void putRule(const char* key, const git_oid& value);
	void putRule(const char* key, const git_oid& value, const GitLinkRepository& repo);
	void putRule(const char* key, git_repository_state_t value);
	void putRule(const char* key, const Signature& value);

private:
	MLINK lnk_;
	std::deque<MLINK> tmpLinks_;
	std::deque<int> argCounts_;
	std::deque<bool> unfinishedRule_;

	inline void incrementArgumentCount_() { if (unfinishedRule_.front()) unfinishedRule_.front() = false; else argCounts_.front()++; };
};

class MLString
{
public:
	MLString(MLINK lnk) : lnk_(lnk)
	{
		int unused;
		MLGetUTF8String(lnk, &str_, &len_, &unused);
	};
	virtual ~MLString()
	{
		MLReleaseUTF8String(lnk_, str_, len_);
	};

	const char* str() const {return (const char*) str_; };
	operator const char*() const { return (const char*)str_; };

private:
	const unsigned char* str_;
	int len_;
	MLINK lnk_;
};

class MLBoolean : MLString
{
public:
	MLBoolean(MLINK lnk) : MLString(lnk) { };
	virtual ~MLBoolean() { };

	operator bool() const { return (strcmp(str(), "True") == 0);}
};

class MLAutoMark
{
public:
	MLAutoMark(MLINK lnk, bool rewindOnDestroy) :
		rewindOnDestroy_(rewindOnDestroy), lnk_(lnk), mark_(MLCreateMark(lnk))
	{ };

	~MLAutoMark()
	{
		if (rewindOnDestroy_)
			rewind();
		MLDestroyMark(lnk_, mark_);
		MLClearError(lnk_);
	}

	void rewind() { MLSeekToMark(lnk_, mark_, 0); };

private:
	bool rewindOnDestroy_;
	MLINK lnk_;
	MLMARK mark_;
};

extern void MLHandleError(WolframLibraryData libData, const char* functionName,
							const char* messageName, const char* param = NULL,
							const char* param2 = NULL);

extern MLExpr MLToExpr(WolframLibraryData libData, const MLExpr& expr);
extern std::string MLToLower(WolframLibraryData libData, const std::string& str);
extern std::string MLGetCPPString(MLINK lnk);

inline int MLGetMint(MLINK mlp, mint* mp)
{
	mlint64 i;
	int result = MLGetInteger64(mlp, &i);
	*mp = (mint) i;
	return result;
}

inline int MLPutMint(MLINK mlp, mint w)
{
	return MLPutInteger64(mlp, (mlint64) w);
}

const char* OtypeToString(git_otype otype);
#endif // MLHelper_h_
