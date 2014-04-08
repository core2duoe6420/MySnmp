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
		return OidTypeEnum::node;
	if (strcmp(typeStr, "scalar") == 0)
		return OidTypeEnum::scalar;
	if (strcmp(typeStr, "table") == 0)
		return OidTypeEnum::table;
	if (strcmp(typeStr, "entry") == 0)
		return OidTypeEnum::entry;
	if (strcmp(typeStr, "column") == 0)
		return OidTypeEnum::column;
	if (strcmp(typeStr, "value") == 0)
		return OidTypeEnum::value;
}

OidAccessEnum OidNode::GetAccessEnum(const char * accessStr) {
	if (strcmp(accessStr, "read_only") == 0)
		return OidAccessEnum::read_only;
	if (strcmp(accessStr, "write_only") == 0)
		return OidAccessEnum::write_only;
	if (strcmp(accessStr, "read_write") == 0)
		return OidAccessEnum::read_write;
	if (strcmp(accessStr, "not_accessible") == 0)
		return OidAccessEnum::not_accessible;
	return OidAccessEnum::access_none;
}

OidStatusEnum OidNode::GetStatusEnum(const char * statusStr) {
	if (strcmp(statusStr, "mandatory") == 0)
		return OidStatusEnum::mandatory;
	if (strcmp(statusStr, "optional") == 0)
		return OidStatusEnum::optional;
	if (strcmp(statusStr, "obsolete") == 0)
		return OidStatusEnum::obsolete;
	return OidStatusEnum::status_none;
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
