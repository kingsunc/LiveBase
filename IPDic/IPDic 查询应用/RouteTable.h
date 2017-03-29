#include "trie.h"
#include <boost/noncopyable.hpp>

enum
{
	E_ROUTE_CODE = 0,
	E_ROUTE_NO,
	E_ROUTE_COUNT
};

class RouteTable : boost::noncopyable
{
public:
	RouteTable();
	~RouteTable();

	Routes getRoute(const std::string& strCode);

private:
	bool LoadRouteData();

	Trie m_objTrie;
};