#include <ruby.h>
#include "vxi11_user.h"
#include "clnt_find_services.h"
#include <rpc/pmap_clnt.h>
#include <arpa/inet.h>


void Init_vxi11();
VALUE rb_vxi11_connect(VALUE name,VALUE ip);
VALUE rb_vxi11_send_and_receive(int argc, VALUE *argv, VALUE self);
VALUE rb_vxi11_find_devices();

static VALUE c_vxi11;

#define BUFFER_SIZE 1000000

static CLINK _clink;
VALUE found_devs;
int connected = 0;

bool_t who_responded(struct sockaddr_in *addr);
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
*/
VALUE rb_vxi11_find_devices() {
	if (!connected) {
		rb_raise(rb_eException, "No connected");
	}
	enum clnt_stat clnt_stat;
    const size_t MAXSIZE = 100;
    //char rcv[MAXSIZE];
    struct timeval t;
    t.tv_sec = 1;
    t.tv_usec = 0;

	found_devs = rb_ary_new();
    // Why 6 for the protocol for the VXI-11 devices?  Not sure, but the devices
    // will otherwise not respond. 
    clnt_stat = clnt_find_services(DEVICE_CORE, DEVICE_CORE_VERSION, 6, &t, who_responded);
	
	return found_devs;
}

bool_t who_responded(struct sockaddr_in *addr) 
{
	VALUE h = rb_hash_new();
  	char str[INET_ADDRSTRLEN];
  	const char* an_addr = inet_ntop(AF_INET, &(addr->sin_addr), str, INET_ADDRSTRLEN);
	
	char * buf = (char*) malloc(BUFFER_SIZE);
	int found = vxi11_send_and_receive(&_clink, "*IDN?", buf, BUFFER_SIZE, 1000);
	if (found > 0) buf[found] = '\0';
	
	rb_hash_aset(h, rb_str_new2(an_addr), rb_str_new2(buf));
	
	rb_ary_push(found_devs,h);
	free(buf);

	return 0;
}

VALUE rb_vxi11_send_and_receive(int argc, VALUE *argv, VALUE self)
{
	if (argc < 1 || argc > 2) {
		rb_raise(rb_eArgError, "wrong number of arguments");
	}	
	if (!connected) {
		rb_raise(rb_eException, "No connected");
	}
	
	VALUE timeout;
	
	if (argc == 1) {
		timeout = INT2FIX(1000);
	} else {
		timeout = argv[1];
	}
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
	if (!connected) {
		rb_raise(rb_eException, "No connected");
	}
	
	VALUE timeout;
	
	if (argc == 0) {
		timeout = INT2FIX(1000);
	} else {
		timeout = argv[0];
	}
	char * buf = (char*) malloc(BUFFER_SIZE);
	int found = vxi11_receive_timeout(&_clink, buf, BUFFER_SIZE, NUM2INT(timeout));
	if (found > 0) buf[found] = '\0';
	VALUE str = rb_str_new2(buf);
	free(buf);
	return str;
}

VALUE rb_vxi11_init(int argc, VALUE *argv, VALUE self) {
	if (argc > 1) {
		rb_raise(rb_eArgError, "wrong number of arguments");
	}
	
	if (argc == 1){
		rb_vxi11_connect(self,argv[0]);
	}
	
	return self;
}

VALUE rb_vxi11_send(VALUE name, VALUE cmd)
{
	if (!connected) {
		rb_raise(rb_eException, "No connected");
	}
	vxi11_send(&_clink, StringValueCStr(cmd));
	return Qnil;
}


VALUE rb_vxi11_connect(VALUE name,VALUE ip)
{
	vxi11_open(StringValueCStr(ip), &_clink);
	connected = 1;
	return Qnil;
}

void Init_vxi11()
{
	c_vxi11 = rb_define_class("VXI11",rb_cObject);
	rb_define_method(c_vxi11,"connect", rb_vxi11_connect,1);
	rb_define_method(c_vxi11,"send_and_receive", rb_vxi11_send_and_receive,-1);
	rb_define_method(c_vxi11,"receive", rb_vxi11_receive,-1);
	rb_define_method(c_vxi11,"send", rb_vxi11_send,1);
	rb_define_method(c_vxi11,"find_devices", rb_vxi11_find_devices,0);
	rb_define_method(c_vxi11,"initialize", rb_vxi11_init,-1);
}