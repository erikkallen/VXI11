#include <ruby.h>
#include "vxi11_user.h"
#include "clnt_find_services.h"
#include <rpc/pmap_clnt.h>
#include <arpa/inet.h>


void Init_vxi11();
VALUE rb_vxi11_connect(VALUE name,VALUE ip);
VALUE rb_vxi11_send_and_receive(int argc, VALUE *argv, VALUE self);
static VALUE m_vxi11;
static VALUE c_vxi11;

#define BUFFER_SIZE 1000000

void* pt2Object;
static CLINK _clink;
/*
class VXI11
{
public:
  VXI11(std::string ip="");
  void connect(std::string ip);
  void send(std::string cmd);
  std::string receive(std::string cmd,int timeout);
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
}*/

VALUE rb_vxi11_send_and_receive(int argc, VALUE *argv, VALUE self)
{
	if (argc < 1 || argc > 2) {
		rb_raise(rb_eArgError, "wrong number of arguments");
	}	
	
	VALUE timeout;
	
	if (argc == 1) {
		timeout = INT2FIX(1000);
	} else {
		timeout = argv[1];
	}
	//printf("gets here argc (%d) cmd (%s) timeout (%d)\n",argc,"bla",NUM2INT(timeout));
	char * buf = (char*) malloc(BUFFER_SIZE);
	int found = vxi11_send_and_receive(&_clink, StringValueCStr(argv[0]), buf, BUFFER_SIZE, NUM2INT(timeout));
	if (found > 0) buf[found] = '\0';
	VALUE str = rb_str_new2(buf);
	free(buf);
	return str;
}

VALUE rb_vxi11_receive(int argc, VALUE *argv, VALUE self)
{
	
	
	if (argc > 1 ) {
		rb_raise(rb_eArgError, "wrong number of arguments");
	}	
	
	VALUE timeout;
	
	if (argc == 0) {
		timeout = INT2FIX(1000);
	} else {
		timeout = argv[0];
	}
	//printf("gets here argc (%d) cmd (%s) timeout (%d)\n",argc,"bla",NUM2INT(timeout));
	char * buf = (char*) malloc(BUFFER_SIZE);
	int found = vxi11_receive_timeout(&_clink, buf, BUFFER_SIZE, NUM2INT(timeout));
	if (found > 0) buf[found] = '\0';
	VALUE str = rb_str_new2(buf);
	free(buf);
	return str;
}

VALUE rb_vxi11_send(VALUE name, VALUE cmd)
{
	vxi11_send(&_clink, StringValueCStr(cmd));
	return Qnil;
}


VALUE rb_vxi11_connect(VALUE name,VALUE ip)
{
	//VALUE str = rb_str_dup(ip);
	//printf("BLAAT: %s\n",StringValueCStr(ip));
	vxi11_open(StringValueCStr(ip), &_clink);
	return Qnil;
}

void Init_vxi11()
{
	//printf("Extention loaded");
	m_vxi11 = rb_define_module("VXI11");
	c_vxi11 = rb_define_class_under(m_vxi11,"VXI11",rb_cObject);
	rb_define_method(c_vxi11,"connect", rb_vxi11_connect,1);
	rb_define_method(c_vxi11,"send_and_receive", rb_vxi11_send_and_receive,-1);
	rb_define_method(c_vxi11,"receive", rb_vxi11_receive,-1);
	rb_define_method(c_vxi11,"send", rb_vxi11_send,1);
  /*Data_Type<VXI11> rb_cVXI11 =
    define_class<VXI11>("VXI11")
	.define_constructor(Constructor<VXI11,std::string>(),(Arg("ip")=""))
    .define_method("connect", &VXI11::connect)
	.define_method("find_devices", &VXI11::find_devices)
	.define_method("send", &VXI11::send)
  	.define_method("receive", &VXI11::receive)
	.define_method("send_and_receive", &VXI11::send_and_receive, (Arg("cmd"), Arg("timeout") = 1000));*/
}