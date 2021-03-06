#ifndef CMD_H
#define CMD_H

#include "str.h"
#include <vector>

/**
 * A Command object is a simple list of binary strings.
 */
class Command {

public:
	Command(int argc);
	~Command();

	void add(str s); 

	const str verb() const;

	size_t argc() const;
	str argv(int pos) const;

private:
	int m_argc;
	std::vector<str> m_args;

};

#endif
