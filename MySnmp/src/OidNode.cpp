#include <MySnmp/debug.h>

#include <MySnmp/OidNode.h>

using namespace mysnmp;

OidNode * OidNode::GetChildWithIndex(int index) const {
	OidNode * node = this->child;
	while (node) {
		if (node->index == index)
			return node;
		node = node->sibling;
	}
	return NULL;
}

OidNode::~OidNode() {
	if (this->child)
		delete this->child;
	if (this->sibling)
		delete this->sibling;
}

OidTypeEnum OidNode::GetTypeEnum(const char * typeStr) {
	if (strcmp(typeStr, "node") == 0)
		return OidTypeEnum::TYPE_NODE;
	if (strcmp(typeStr, "scalar") == 0)
		return OidTypeEnum::TYPE_SCALAR;
	if (strcmp(typeStr, "table") == 0)
		return OidTypeEnum::TYPE_TABLE;
	if (strcmp(typeStr, "entry") == 0)
		return OidTypeEnum::TYPE_ENTRY;
	if (strcmp(typeStr, "column") == 0)
		return OidTypeEnum::TYPE_COLUMN;
	if (strcmp(typeStr, "value") == 0)
		return OidTypeEnum::TYPE_VALUE;
}

OidAccessEnum OidNode::GetAccessEnum(const char * accessStr) {
	if (strcmp(accessStr, "read_only") == 0)
		return OidAccessEnum::ACCESS_READ_ONLY;
	if (strcmp(accessStr, "write_only") == 0)
		return OidAccessEnum::ACCESS_WRITE_ONLY;
	if (strcmp(accessStr, "read_write") == 0)
		return OidAccessEnum::ACCESS_READ_WRITE;
	if (strcmp(accessStr, "not_accessible") == 0)
		return OidAccessEnum::ACCESS_NOT_ACCESSIBLE;
	return OidAccessEnum::ACCESS_NONE;
}

OidStatusEnum OidNode::GetStatusEnum(const char * statusStr) {
	if (strcmp(statusStr, "mandatory") == 0)
		return OidStatusEnum::STATUS_MANDATORY;
	if (strcmp(statusStr, "optional") == 0)
		return OidStatusEnum::STATUS_OPTIONAL;
	if (strcmp(statusStr, "obsolete") == 0)
		return OidStatusEnum::STATUS_OBSOLETE;
	return OidStatusEnum::STATUS_NONE;
}

OidSyntaxEnum OidNode::GetSyntaxEnum(const char * syntaxStr) {
	if (strcmp(syntaxStr, "integer") == 0)
		return OidSyntaxEnum::SYNTAX_INTEGER;
	if (strcmp(syntaxStr, "oid") == 0)
		return OidSyntaxEnum::SYNTAX_OID;
	if (strcmp(syntaxStr, "string") == 0)
		return OidSyntaxEnum::SYNTAX_STRING;
	if (strcmp(syntaxStr, "counter") == 0)
		return OidSyntaxEnum::SYNTAX_COUNTER;
	if (strcmp(syntaxStr, "gauge") == 0)
		return OidSyntaxEnum::SYNTAX_GAUGE;
	if (strcmp(syntaxStr, "timeticks") == 0)
		return OidSyntaxEnum::SYNTAX_TIMETICKS;
	if (strcmp(syntaxStr, "sequence") == 0)
		return OidSyntaxEnum::SYNTAX_SEQUENCE;
	if (strcmp(syntaxStr, "entry") == 0)
		return OidSyntaxEnum::SYNTAX_ENTRY;
	return OidSyntaxEnum::SYNTAX_NONE;
}

Snmp_pp::Oid OidNode::ToOid() const {
	Snmp_pp::Oid oid;
	oid = this->ToDottedString().c_str();
	return oid;
}

std::string OidNode::ToDottedString() const {
	std::string str;
	OidNode * node = (OidNode *)this;
	char buf[32];
	while (node->father) {
		if (node == this)
			sprintf(buf, "%d", node->index);
		else
			sprintf(buf, "%d.", node->index);
		str.insert(0, buf);
		node = node->father;
	}
	return str;
}

std::string OidNode::ToNameString() const {
	std::string str;
	OidNode * node = (OidNode *)this;
	while (node->father) {
		if (node == this)
			str.insert(0, node->name);
		else
			str.insert(0, node->name + ".");
		node = node->father;
	}
	return str;
}
