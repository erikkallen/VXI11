#include <rice/Class.hpp>
#include <rice/String.hpp>
#include <rice/Hash.hpp>
#include <rice/Data_Type.hpp>
#include <rice/Constructor.hpp>
#include "vxi11_user.h"

#include "clnt_find_services.h"
#include <rpc/pmap_clnt.h>
#include <arpa/inet.h>
#include <iostream>
using namespace Rice;
using std::endl;
using  std::cout;

#define BUFFER_SIZE 1000000

void* pt2Object;
	
class VXI11
{
public:
  VXI11(std::string ip="");
  void connect(std::string ip);
  std::string send_and_receive(std::string cmd, int timeout=1000);
  Hash find_devices();
  static int who_responded_s(struct sockaddr_in *addr);
  
private:
	bool_t who_responded(struct sockaddr_in *addr);
	CLINK _clink;
	class Ports {
	  public:
	    Ports(int tcp = 0, int udp = 0) : tcp_port(tcp), udp_port(udp) {}
	    int tcp_port;
	    int udp_port;
	};

	typedef std::map<std::string, Ports> AddrMap;
	AddrMap gfFoundDevs;
};

int VXI11::who_responded_s( struct sockaddr_in *addr) {
	VXI11 * mySelf = (VXI11*) pt2Object;
	return mySelf->who_responded(addr);
}

VXI11::VXI11(std::string ip)
{
	if (ip!="") {
		connect(ip);
	}
}

Hash VXI11::find_devices() {
	Hash h;//new Hash();

    enum clnt_stat clnt_stat;
    const size_t MAXSIZE = 100;
    char rcv[MAXSIZE];
    timeval t;
    t.tv_sec = 1;
    t.tv_usec = 0;

	pt2Object = (void*) this;
    // Why 6 for the protocol for the VXI-11 devices?  Not sure, but the devices
    // will otherwise not respond. 
    clnt_stat = clnt_find_services(DEVICE_CORE, DEVICE_CORE_VERSION, 6, &t,VXI11::who_responded_s);
								   
	AddrMap::const_iterator iter;
	for (iter=gfFoundDevs.begin();iter!= gfFoundDevs.end();iter++) {
	   //const Ports& port = iter->second;
	   
	   //cout << " Found: " << iter->first << " : TCP " << port.tcp_port 
	    //    << "; UDP " << port.udp_port << endl;
	   CLINK vxi_link;
	   rcv[0] = '\0';
	   if ( vxi11_open_device(iter->first.c_str(), &vxi_link) < 0 ) continue;
	   int found = vxi11_send_and_receive(&vxi_link, "*IDN?", rcv, MAXSIZE, 10);
	   if (found > 0) rcv[found] = '\0';
	   //cout << "  Output: " << rcv << endl;
	   h[String(iter->first)] = String(rcv);
	}
	
	//h[String("hello")] = String("bla");
	
	return h;
}

bool_t VXI11::who_responded(struct sockaddr_in *addr) 
{
  char str[INET_ADDRSTRLEN];
  const char* an_addr = inet_ntop(AF_INET, &(addr->sin_addr), str, INET_ADDRSTRLEN);
  if ( gfFoundDevs.find( std::string(an_addr) ) != gfFoundDevs.end() ) return 0;
  int port_T = pmap_getport(addr, DEVICE_CORE, DEVICE_CORE_VERSION, IPPROTO_TCP);
  int port_U = pmap_getport(addr, DEVICE_CORE, DEVICE_CORE_VERSION, IPPROTO_UDP);
  gfFoundDevs[ std::string( an_addr ) ] = Ports(port_T, port_U);
  return 0;
}

std::string VXI11::send_and_receive(std::string cmd,int timeout)
{
	char * buf = new char[BUFFER_SIZE];
	int found = vxi11_send_and_receive(&_clink, cmd.c_str(), buf, BUFFER_SIZE, timeout);
	if (found > 0) buf[found] = '\0';
	std::string str(buf);
	delete buf;
	return str;
}

void VXI11::connect(std::string ip)
{
	vxi11_open_device(ip.c_str(), &_clink);
}


extern "C"
void Init_vxi11()
{
  Data_Type<VXI11> rb_cVXI11 =
    define_class<VXI11>("VXI11")
	.define_constructor(Constructor<VXI11,std::string>(),(Arg("ip")=""))
    .define_method("connect", &VXI11::connect)
	.define_method("find_devices", &VXI11::find_devices)
	.define_method("send_and_receive", &VXI11::send_and_receive, (Arg("cmd"), Arg("timeout") = 1000));
}