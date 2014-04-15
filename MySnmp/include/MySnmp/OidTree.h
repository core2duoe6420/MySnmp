#ifndef __OID_TREE_H
#define __OID_TREE_H

#include <snmp_pp/oid.h>
#include <xercesc/util/Xerces_autoconf_config.hpp>

namespace xercesc_3_1 {
	class DOMNode;
}

namespace mysnmp {
	class OidNode;

	class OidTree {
	private:
		static OidTree defaultOidTree;

		XMLCh * XMLIndexTagName;
		XMLCh * XMLNameTagName;
		XMLCh * XMLDescTagName;
		XMLCh * XMLAccessTagName;
		XMLCh * XMLStatusTagName;
		XMLCh * XMLTypeTagName;
		XMLCh * XMLSyntaxTagName;

		OidNode * root;
		void xmlAddChildNodes(OidNode * father, xercesc_3_1::DOMNode * xmlNode);
	public:
		static OidTree& GetDefaultOidTree() {
			return defaultOidTree;
		}
		OidTree(const char * xmlpath);
		virtual ~OidTree();
		const OidNode * GetOidNode(const Snmp_pp::Oid& oid) const;
		const OidNode * GetOidNode(const char * dottedString) const;
	};

}

#endif