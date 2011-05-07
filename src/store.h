#ifndef STORE_H
#define STORE_H

#include <string>
#include <vector>
#include <haildb.h>

#include "str.h"

#include "kvtable.h"
#include "settable.h"

class Config;

class Store {

public:
	Store(std::string db, Config &config);

	bool startup();
	bool shutdown();

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

	// deletion
	bool del(str key);
	bool flushall();

private:
	bool createDb();
	bool createTable();

	std::string m_db;
	KVTable m_main;
	SetTable m_sets;

	Config &m_config;
};

#endif