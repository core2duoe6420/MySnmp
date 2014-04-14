#ifndef __OID_NODE_H
#define __OID_NODE_H

#include <libsnmp.h>
#include <snmp_pp/snmp_pp.h>

namespace mysnmp {
	enum OidTypeEnum {
		TYPE_NODE, TYPE_SCALAR, TYPE_TABLE, TYPE_ENTRY, TYPE_COLUMN, TYPE_VALUE,
	};

	enum OidAccessEnum {
		ACCESS_NONE, ACCESS_READ_ONLY, ACCESS_READ_WRITE, ACCESS_WRITE_ONLY, ACCESS_NOT_ACCESSIBLE,
	};

	enum OidStatusEnum {
		STATUS_NONE, STATUS_MANDATORY, STATUS_OPTIONAL, STATUS_OBSOLETE,
	};

	enum OidSyntaxEnum {
		SYNTAX_NONE, SYNTAX_STRING, SYNTAX_INTEGER, SYNTAX_TIMETICKS,
		SYNTAX_GAUGE, SYNTAX_OID, SYNTAX_COUNTER, SYNTAX_SEQUENCE, SYNTAX_ENTRY,
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
				OidTypeEnum type = OidTypeEnum::TYPE_NODE,
				OidSyntaxEnum syntax = OidSyntaxEnum::SYNTAX_NONE,
				OidAccessEnum access = OidAccessEnum::ACCESS_NONE,
				OidStatusEnum status = OidStatusEnum::STATUS_NONE)
				: index(index), name(name), description(description),
				type(type), access(access), syntax(syntax),
				father(NULL), child(NULL), sibling(NULL) {};

		~OidNode();

		OidNode * GetChildWithIndex(int index) const;

		static OidTypeEnum GetTypeEnum(const char * typeStr);
		static OidAccessEnum GetAccessEnum(const char * accessStr);
		static OidStatusEnum GetStatusEnum(const char * statusStr);
		static OidSyntaxEnum GetSyntaxEnum(const char * syntaxStr);

		Snmp_pp::Oid ToOid() const;
		std::string ToDottedString() const;
		std::string ToNameString() const;

		void AddChild(OidNode * child) {
			child->sibling = this->child;
			this->child = child;
			child->father = this;
		}

		OidTypeEnum GetType() const { return this->type; }
		OidAccessEnum GetAccess() const { return this->access; }
		OidStatusEnum GetStatus() const { return this->status; }
		OidSyntaxEnum GetSyntax() const { return this->syntax; }
		OidNode * GetFirstChild() const { return this->child; }
		OidNode * GetFather() const { return this->father; }
		OidNode * GetSibling() const { return this->sibling; }
		int GetIndex() const { return this->index; }
		const std::string& GetDescription() const { return this->description; }
		const std::string& GetName() const { return this->name; }

	};

}

#endif