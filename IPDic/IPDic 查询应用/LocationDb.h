#include <iostream>
#include <boost/noncopyable.hpp>
#include <map>
#include <cstring>

using namespace std;

// ÐÂIP×Öµä;
enum
{
	E_NEWIP_STARTIP = 0,
	E_NEWIP_ENDIP,
	E_NEWIP_CODE,
	E_NEWIP_COUNT
};

struct CNewIP
{
	unsigned long	m_lStartIP;
	unsigned long	m_lEndIP;
	std::string		m_strCode;
};

typedef std::map<unsigned long, CNewIP>  MapNewIP;

class LocationDb : boost::noncopyable
{
public:
	LocationDb();
	~LocationDb();

	std::string getLocation(const std::string& strIP);
	
private:
	bool LoadIPData();

	MapNewIP	m_mapNewIP;
};