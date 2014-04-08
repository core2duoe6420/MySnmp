//#include "debug.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>

#include <MySnmp/OidTree.h>
#include <MySnmp/OidNode.h>

using namespace mysnmp;
using namespace xercesc;

OidTree::OidTree(const char * xmlpath) {
	XMLPlatformUtils::Initialize();
	XMLIndexTagName = XMLString::transcode("index");
	XMLNameTagName = XMLString::transcode("name");
	XMLDescTagName = XMLString::transcode("desc");
	XMLAccessTagName = XMLString::transcode("access");
	XMLStatusTagName = XMLString::transcode("status");
	XMLTypeTagName = XMLString::transcode("type");
	XMLSyntaxTagName = XMLString::transcode("syntax");

	this->root = new OidNode(0, "root", "root");

	XercesDOMParser * xmlParser = new XercesDOMParser();
	xmlParser->setIncludeIgnorableWhitespace(false);
	xmlParser->parse(xmlpath);
	xercesc::DOMDocument *xmlDoc = xmlParser->getDocument();
	if (!xmlDoc || !xmlDoc->getDocumentElement())
		return;
	xmlAddChildNodes(root, xmlDoc->getChildNodes()->item(0));

	delete xmlParser;

	XMLString::release(&XMLIndexTagName);
	XMLString::release(&XMLNameTagName);
	XMLString::release(&XMLDescTagName);
	XMLString::release(&XMLAccessTagName);
	XMLString::release(&XMLStatusTagName);
	XMLString::release(&XMLTypeTagName);
	XMLString::release(&XMLSyntaxTagName);
	XMLPlatformUtils::Terminate();
}

void OidTree::xmlAddChildNodes(OidNode * father, xercesc::DOMNode * xmlNode) {
	DOMNodeList * childList = xmlNode->getChildNodes();
	for (int i = 0; i < childList->getLength(); i++) {
		xercesc::DOMNode * node = childList->item(i);
		if (node->getNodeType() != xercesc::DOMNode::ELEMENT_NODE)
			continue;

		OidNode * newNode = NULL;
		DOMNamedNodeMap * attrs = node->getAttributes();
		xercesc::DOMNode * indexNode = attrs->getNamedItem(XMLIndexTagName);
		char * indexStr = XMLString::transcode(indexNode->getNodeValue());
		int index = atoi(indexStr);
		xercesc::DOMNode * nameNode = attrs->getNamedItem(XMLNameTagName);
		char * name = XMLString::transcode(nameNode->getNodeValue());
		xercesc::DOMNode * descNode = attrs->getNamedItem(XMLDescTagName);
		char * desc = XMLString::transcode(descNode->getNodeValue());
		xercesc::DOMNode * typeNode = attrs->getNamedItem(XMLTypeTagName);
		char * typeStr = XMLString::transcode(typeNode->getNodeValue());
		OidTypeEnum typeEnum = OidNode::GetTypeEnum(typeStr);
		if (typeEnum != OidTypeEnum::node && typeEnum != OidTypeEnum::value) {
			xercesc::DOMNode * accessNode = attrs->getNamedItem(XMLAccessTagName);
			char * accessStr = XMLString::transcode(accessNode->getNodeValue());
			OidAccessEnum accessEnum = OidNode::GetAccessEnum(accessStr);
			xercesc::DOMNode * statusNode = attrs->getNamedItem(XMLAccessTagName);
			char * statusStr = XMLString::transcode(statusNode->getNodeValue());
			OidStatusEnum statusEnum = OidNode::GetStatusEnum(statusStr);
			newNode = new OidNode(index, name, desc, typeEnum, accessEnum, statusEnum);
			XMLString::release(&accessStr);
			XMLString::release(&statusStr);
		} else if (typeEnum == OidTypeEnum::value)
			newNode = new OidNode(index, name, desc, typeEnum);
		else
			newNode = new OidNode(index, name, desc);

		XMLString::release(&typeStr);
		XMLString::release(&indexStr);
		XMLString::release(&name);
		XMLString::release(&desc);

		father->AddChild(newNode);
		xmlAddChildNodes(newNode, node);
	}
}

OidTree::~OidTree() {
	delete this->root;
}

const OidNode * OidTree::GetOidNode(const Snmp_pp::Oid& oid) const {
	OidNode * node = this->root;
	for (int i = 0; i < oid.len(); i++) {
		int index = oid[i];
		if (node->GetTypeEnum() == OidTypeEnum::column)
			return node;
		node = node->GetChildWithIndex(index);
		if (node == NULL)
			return NULL;
	}
	return node;
}

const OidNode * OidTree::GetOidNode(const char * dottedString) const {
	Snmp_pp::Oid oid(dottedString);
	return GetOidNode(oid);
}

//int main() {
//	OidTree oidtree("oid.xml");
//	OidNode * node = oidtree.GetOidNode("1.3.6.1.2.1.1.1");
//	std::cout << node->ToNameString().c_str() << std::endl << node->ToDottedString().c_str();
//}