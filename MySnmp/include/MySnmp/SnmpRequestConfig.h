#ifndef __SNMP_REQUEST_CONFIG_H
#define __SNMP_REQUEST_CONFIG_H

#include <MySnmp/Config.h>

namespace mysnmp {
	class SnmpGetRequestConfig : public Config {
	private:
		const char * BULK_NON_REPEATER = "bulk_non_repeater";
		const char * BULK_MAX_REPEATER = "bulk_max_repeater";
		const char * MAX_VB_COUNT = "max_vb_count";
	public:
		SnmpGetRequestConfig() {
			Config::addConfigItem(BULK_NON_REPEATER, "0");
			Config::addConfigItem(BULK_MAX_REPEATER, "10");
			Config::addConfigItem(MAX_VB_COUNT, "10");
		}

		virtual ~SnmpGetRequestConfig() {}

		inline void SetBulkNonRepeater(int nonRepeater) {
			Config::setConfigItemValue(BULK_NON_REPEATER, nonRepeater);
		}

		inline int GetBulkNonRepeater() const {
			return Config::GetIntConfigItem(BULK_NON_REPEATER);
		}

		inline void SetBulkMaxRepeater(int maxRepeater) {
			Config::setConfigItemValue(BULK_MAX_REPEATER, maxRepeater);
		}

		inline int GetBulkMaxRepeater() const {
			return Config::GetIntConfigItem(BULK_MAX_REPEATER);
		}

		inline void SetMaxVbCount(int maxVbCount) {
			Config::setConfigItemValue(BULK_MAX_REPEATER, maxVbCount);
		}

		inline int GetMaxVbCount() const {
			return Config::GetIntConfigItem(MAX_VB_COUNT);
		}
	};
}

#endif