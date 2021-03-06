#include "dispatcher.h"
#include "store.h"
#include "reply.h"
#include "cmd.h"

#include <string>
#include <cstdlib>
#include <iostream>

using namespace std;
using namespace std::tr1::placeholders;
using tr1::bind;

CommandHandler::CommandHandler(CommandFun f, size_t min, size_t max):
	m_fun(f), m_min(min), m_max(max) {

}

Reply*
CommandHandler::operator()(Command &cmd) {

	if(cmd.argc() < m_min || cmd.argc() > m_max) {
		return new ErrorReply("wrong number of arguments");
	}
	return m_fun(cmd);
}

////////////////////////////////////////////////////////////////////////////////

Dispatcher::Dispatcher(Store &s) : m_store(s) {

	// bind internal, private methods to keywords.

	m_functions["GET"] = CommandHandler(bind(&Dispatcher::get, this, _1));
	m_functions["SET"] = CommandHandler(bind(&Dispatcher::set, this, _1), 1, 2);
	m_functions["DEL"] = CommandHandler(bind(&Dispatcher::del, this, _1));

	m_functions["INCR"] = CommandHandler(bind(&Dispatcher::incr, this, _1), 1, 2);
	m_functions["DECR"] = CommandHandler(bind(&Dispatcher::decr, this, _1), 1, 2);

	m_functions["SADD"] = CommandHandler(bind(&Dispatcher::sadd, this, _1), 2, 2);
	m_functions["SMEMBERS"] = CommandHandler(bind(&Dispatcher::smembers, this, _1), 2, 2);
	m_functions["SISMEMBER"] = CommandHandler(bind(&Dispatcher::sismember, this, _1), 2, 2);
	m_functions["SREM"] = CommandHandler(bind(&Dispatcher::srem, this, _1), 2, 2);

	m_functions["LPUSH"] = CommandHandler(bind(&Dispatcher::lpush, this, _1), 2, 2);
	m_functions["RPUSH"] = CommandHandler(bind(&Dispatcher::rpush, this, _1), 2, 2);
	m_functions["LLEN"] = CommandHandler(bind(&Dispatcher::llen, this, _1), 1, 1);
	m_functions["LRANGE"] = CommandHandler(bind(&Dispatcher::lrange, this, _1), 3, 3);
	m_functions["LPOP"] = CommandHandler(bind(&Dispatcher::lpop, this, _1), 1, 1);
	m_functions["RPOP"] = CommandHandler(bind(&Dispatcher::rpop, this, _1), 1, 1);

	m_functions["ZADD"] = CommandHandler(bind(&Dispatcher::zadd, this, _1), 3, 3);
	m_functions["ZCARD"] = CommandHandler(bind(&Dispatcher::zcard, this, _1), 1, 1);
	m_functions["ZREM"] = CommandHandler(bind(&Dispatcher::zrem, this, _1), 2, 2);
	m_functions["ZSCORE"] = CommandHandler(bind(&Dispatcher::zscore, this, _1), 2, 2);
	m_functions["ZCOUNT"] = CommandHandler(bind(&Dispatcher::zcount, this, _1), 3, 3);
}


Reply*
Dispatcher::run(Command &cmd) {

	// look for command
	map<str, CommandHandler>::iterator fun = m_functions.find(cmd.verb());

	if(fun == m_functions.end()) {	// not found in command table.
		return new ErrorReply("Unknown command");
	} else {
		// execute
		Reply *r = fun->second(cmd);

		// detect error
		if(!r) r = new ErrorReply("Unknown error");

		// return reply object
		return r;
	}
}

////////////////////////////////////////////////////////////////////////////////

Reply*
Dispatcher::get(Command &cmd) {

	str key(cmd.argv(1)), val;
	if(m_store.get(key, &val)) {
		return new StringReply(val); // the reply object owns `val'.
	} else {
		return new EmptyReply();
	}
}


Reply*
Dispatcher::set(Command &cmd) {

	if(m_store.set(cmd.argv(1), cmd.argv(2))) {
		return new SuccessReply();
	} else {
		return 0;
	}
}

Reply*
Dispatcher::del(Command &cmd) {

	if(m_store.del(cmd.argv(1))) {
		return new IntReply(1);
	} else {
		return new IntReply(0);
	}
}

Reply*
Dispatcher::incr(Command &cmd) {

	int out;
	int by = cmd.argc() == 1 ? 1 : ::atol(cmd.argv(2).c_str());
	if(m_store.incr(cmd.argv(1), by, out)) {
		return new IntReply(out);
	} else {
		return 0;
	}
}

Reply*
Dispatcher::decr(Command &cmd) {

	int out;
	int by = cmd.argc() == 1 ? 1 : ::atol(cmd.argv(2).c_str());
	if(m_store.decr(cmd.argv(1), by, out)) {
		return new IntReply(out);
	} else {
		return 0;
	}
}

Reply*
Dispatcher::sadd(Command &cmd) {

	if(m_store.sadd(cmd.argv(1), cmd.argv(2))) {
		return new IntReply(1);
	} else {
		return new IntReply(0);
	}
}

Reply*
Dispatcher::smembers(Command &cmd) {

	vector<str> out;
	if(m_store.smembers(cmd.argv(1), out)) {
		ListReply *r = new ListReply;
		for(vector<str>::iterator i = out.begin(); i != out.end(); i++) {
			r->add(new StringReply(*i));
		}
		return r;
	} else {
		return 0;
	}
}

Reply*
Dispatcher::sismember(Command &cmd) {

	if(m_store.sismember(cmd.argv(1), cmd.argv(2))) {
		return new IntReply(1);
	} else {
		return 0;
	}
}

Reply*
Dispatcher::srem(Command &cmd) {

	if(m_store.srem(cmd.argv(1), cmd.argv(2))) {
		return new IntReply(1);
	} else {
		return new IntReply(0);
	}
}

Reply*
Dispatcher::lpush(Command &cmd) {

	int out;
	if(m_store.lpush(cmd.argv(1), cmd.argv(2), out)) {
		return new IntReply(out);
	} else {
		return 0;
	}
}

Reply*
Dispatcher::rpush(Command &cmd) {

	int out;
	if(m_store.rpush(cmd.argv(1), cmd.argv(2), out)) {
		return new IntReply(out);
	} else {
		return 0;
	}
}

Reply*
Dispatcher::llen(Command &cmd) {

	int out;
	if(m_store.llen(cmd.argv(1), out)) {
		return new IntReply(out);
	} else {
		return 0;
	}
}

Reply*
Dispatcher::lrange(Command &cmd) {

	vector<str> out;
	if(m_store.lrange(cmd.argv(1),
				::atol(cmd.argv(2).c_str()),
				::atol(cmd.argv(3).c_str()),
				out)) {
		ListReply *r = new ListReply;
		for(vector<str>::iterator i = out.begin(); i != out.end(); i++) {
			r->add(new StringReply(*i));
		}
		return r;
	} else {
		return 0;
	}
}

Reply*
Dispatcher::lpop(Command &cmd) {

	str key(cmd.argv(1)), val;
	if(m_store.lpop(key, val)) {
		return new StringReply(val); // the reply object owns `val'.
	} else {
		return new EmptyReply();
	}
}

Reply*
Dispatcher::rpop(Command &cmd) {

	str key(cmd.argv(1)), val;
	if(m_store.rpop(key, val)) {
		return new StringReply(val); // the reply object owns `val'.
	} else {
		return new EmptyReply();
	}
}

////////////////////////////////////////////////////////////////////////////////

Reply*
Dispatcher::zadd(Command &cmd)
{
	int out;
	if(m_store.zadd(cmd.argv(1), ::atof(cmd.argv(2).c_str()),
				cmd.argv(3), out)) {
		return new IntReply(out);
	} else {
		return 0;
	}
}

Reply*
Dispatcher::zcard(Command &cmd)
{
	int out;
	if(m_store.zcard(cmd.argv(1), out)) {
		return new IntReply(out);
	} else {
		return 0;
	}
}

Reply*
Dispatcher::zrem(Command &cmd)
{
	int out;
	if(m_store.zrem(cmd.argv(1), cmd.argv(2), out)) {
		return new IntReply(out);
	} else {
		return 0;
	}
}

Reply*
Dispatcher::zscore(Command &cmd)
{
	double out;
	bool found;
	if(m_store.zscore(cmd.argv(1), cmd.argv(2), out, found)) {
		if(found)
			return new DoubleReply(out);
		else
			return 0;
	} else {
		return 0;
	}
}

Reply*
Dispatcher::zcount(Command &cmd)
{
	int out;
	if(m_store.zcount(cmd.argv(1), ::atof(cmd.argv(2).c_str()), ::atof(cmd.argv(3).c_str()), out)) {
		return new IntReply(out);
	} else {
		return 0;
	}
}


