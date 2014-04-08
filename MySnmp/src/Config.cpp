#include <MySnmp/debug.h>

#include <MySnmp/Config.h>

using namespace mysnmp;

void Config::addConfigItem(const char * key, const char * value) {
	std::string keystr(key), valuestr(value);
	this->configs.insert(HashMap::value_type(key, value));
}

bool Config::setConfigItemValue(const char * key, const char * value) {
	HashMap::iterator iter = this->configs.find(key);
	if (iter == this->configs.end())
		return false;
	iter->second = value;
	return true;
}

bool Config::setConfigItemValue(const char * key, int value) {
	char buf[16];
	sprintf(buf, "%d", value);
	return setConfigItemValue(key, buf);
}

int Config::GetIntConfigItem(const char * key) const {
	HashMap::iterator iter = ((Config *)this)->configs.find(key);
	if (iter == this->configs.end())
		return 0;
	return atoi(iter->second.c_str());
}

const char * Config::GetConfigItem(const char * key) const {
	HashMap::iterator iter = ((Config *)this)->configs.find(key);
	if (iter == this->configs.end())
		return NULL;
	return iter->second.c_str();
}

//
//#include <iostream>
//void main() {
//	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//	Config config;
//	config.addConfigItem("123", "456");
//	int a = config.GetIntConfigItem("123");
//	int b = config.GetIntConfigItem("234");
//	const char * astr = config.GetConfigItem("123");
//	const char * bstr = config.GetConfigItem("345");
//	std::cout << config.setConfigItemValue("1223", "123");
//}