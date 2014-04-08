#ifndef __CONFIG_H
#define __CONFIG_H

#include <unordered_map>

namespace mysnmp {
	class Config {
		typedef std::unordered_map<std::string, std::string> HashMap;
	private:
		HashMap configs;

#ifdef _DEBUG
	public:
#else
	protected:
#endif
		void addConfigItem(const char * key, const char * value);
		bool setConfigItemValue(const char * key, const char * value);
		bool setConfigItemValue(const char * key, int value);
	public:
		virtual ~Config() {}
		int GetIntConfigItem(const char * key) const;
		const char * GetConfigItem(const char * key) const;
	};

}

#endif