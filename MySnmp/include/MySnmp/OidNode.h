#ifndef __OID_NODE_H
#define __OID_NODE_H

#include <libsnmp.h>
#include <snmp_pp/snmp_pp.h>

namespace mysnmp {
	enum OidTypeEnum {
		node, scalar, table, entry, column, value,
	};

	enum OidAccessEnum {
		access_none, read_only, read_write, write_only, not_accessible,
	};

	enum OidStatusEnum {
		status_none, mandatory, optional, obsolete,
	};

	enum OidSyntaxEnum {
		string, integer, timeticks,
	};

	class OidNode {
	private:
		OidNode * father;
		OidNode * child;
		OidNode * sibling;
		int index;
		std::string name;
		std::string description;
		OidTypeEnum type;
		OidAccessEnum access;
		OidStatusEnum status;
		OidSyntaxEnum syntax;

	public:
		OidNode(int index, const char * name, const char * description,
			OidTypeEnum type = OidTypeEnum::node,
			OidAccessEnum access = OidAccessEnum::access_none,
			OidStatusEnum status = OidStatusEnum::status_none)
			: index(index), name(name), description(description),
			type(type), access(access),
			father(NULL), child(NULL), sibling(NULL) {};

		~OidNode();

		OidNode * GetChildWithIndex(int index) const;

		static OidTypeEnum GetTypeEnum(const char * typeStr);
		static OidAccessEnum GetAccessEnum(const char * accessStr);
		static OidStatusEnum GetStatusEnum(const char * statusStr);

		Snmp_pp::Oid ToOid() const;
		std::string ToDottedString() const;
		std::string ToNameString() const;

		inline OidTypeEnum GetTypeEnum() const {
			return this->type;
		}

		inline OidAccessEnum GetAccessEnum() const {
			return this->access;
		}

		inline OidStatusEnum GetStatusEnum() const {
			return this->status;
		}

		inline void AddChild(OidNode * child) {
			child->sibling = this->child;
			this->child = child;
			child->father = this;
		}

		inline OidNode * GetFirstChild() const {
			return this->child;
		}

		inline OidNode * GetFather() const {
			return this->father;
		}

		inline OidNode * GetSibling() const {
			return this->sibling;
		}

		inline int GetIndex() const {
			return this->index;
		}

		inline const std::string& GetDescription() const {
			return this->description;
		}

		inline const std::string& GetName() const {
			return this->name;
		}

	};

}

#endif