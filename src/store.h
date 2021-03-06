#ifndef STORE_H
#define STORE_H

#include <string>
#include <vector>
#include <haildb.h>

#include "str.h"

#include "kvtable.h"
#include "settable.h"
#include "listtable.h"
#include "zsettable.h"

class Config;

class Store {

public:
	Store(std::string db, Config &config);

	bool startup();
	bool shutdown();

	bool load();
	bool install();

	// basic key management
	bool get(str key, str *val);
	bool set(str key, str val);

	// increment
	bool incr(str key, int by, int &out);
	bool decr(str key, int by, int &out);

	// sets
	bool sadd(str key, str val);
	bool sismember(str key, str val);
	bool smembers(str key, std::vector<str> &out);
	bool srem(str key, str val);

	// lists
	bool lpush(str key, str val, int &out);
	bool rpush(str key, str val, int &out);
	bool lrange(str key, int start, int stop, std::vector<str> &out);
	bool llen(str key, int &out);
	bool lpop(str key, str &val);
	bool rpop(str key, str &val);
	void ldump(str key);

	// zsets
	bool zadd(str key, double score, str val, int &out);
	bool zcard(str key, int &out);
	bool zrem(str key, str val, int &out);
	bool zscore(str key, str val, double &out, bool &found);
	bool zcount(str key, double min, double max, int &out);

	// deletion
	bool del(str key);
	bool flushall();

private:
	bool createDb();
	bool createTable();

	std::string m_db;
	KVTable m_main;
	SetTable m_sets;
	ListHeadTable m_lists;
	ZSetHeadTable m_zsets;

	Config &m_config;
};

#endif
