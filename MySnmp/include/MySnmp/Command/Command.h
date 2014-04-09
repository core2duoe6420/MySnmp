#ifndef __COMMAND_H
#define __COMMAND_H

namespace mysnmp {
	class Command {
	public:
		virtual int Execute() = 0;
	};
}
#endif