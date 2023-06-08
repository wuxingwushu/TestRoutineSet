#include <iostream>
#include "event2/event.h"
#include "event2/listener.h"
#include "event2/bufferevent.h"
#define MAP_Debug
#include "../ContinuousMap.h"
#include <thread>

#ifdef _WIN32
#include <winsock2.h> // 注意要包含Winsock2头文件
#pragma comment(lib, "ws2_32.lib") // 链接Winsock库
#else
#include <signal.h>
#endif

struct pos {
	int X;
	int Y;
	evutil_socket_t k;
};

ContinuousMap<evutil_socket_t, pos> client(10);


//错误，超时 （连接断开会进入）
void event_cb(bufferevent* be, short events, void* arg);

//读事件
void read_cb(bufferevent* be, void* arg);

//写事件
void write_cb(bufferevent* be, void* arg);

//链接事件
void listen_cb(evconnlistener* ev, evutil_socket_t s, sockaddr* sin, int slen, void* arg);

int main() {
#ifdef _WIN32 
	//初始化socket库
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
#else
	//忽略管道信号，发送数据给已关闭的socket
	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		return 1;
#endif
	event_base* base = event_base_new();
	//创建网络服务器

	//设定监听的端口和地址
	sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(5001);

	evconnlistener* ev = evconnlistener_new_bind(
		base,
		listen_cb,		//回调函数
		base,			//回调函数的参数arg
		LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
		10,				//listen back
		(sockaddr*)&sin,
		sizeof(sin)
	);

	pos* p = client.New(0);
	p->k = 0;
	p->X = 250;
	p->Y = 520;

	while (true)
	{
		event_base_loop(base, EVLOOP_NONBLOCK);
		//std::cout << "loop" << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
	//进入事件主循环
	//event_base_dispatch(base);
	evconnlistener_free(ev);
	event_base_free(base);

#ifdef _WIN32 
	WSACleanup();
#else

#endif
	return 0;
}


//错误，超时 （连接断开会进入）
void event_cb(bufferevent* be, short events, void* arg)
{
	std::cout << "[E]" << std::flush;
	//读取超时时间发生后，数据读取停止
	if (events & BEV_EVENT_TIMEOUT && events & BEV_EVENT_READING)
	{
		std::cout << "BEV_EVENT_READING BEV_EVENT_TIMEOUT" << std::endl;
		evutil_socket_t fd = bufferevent_getfd(be);
		client.Delete(fd);
		bufferevent_free(be);
	}
	else if (events & BEV_EVENT_ERROR)
	{
		std::cout << "BEV_EVENT_ERROR" << std::endl;
		evutil_socket_t fd = bufferevent_getfd(be);
		client.Delete(fd);
		bufferevent_free(be);
	}
	else
	{
		std::cout << "OTHERS" << std::endl;
	}
}
void write_cb(bufferevent* be, void* arg)
{
	//std::cout << "[W]" << std::flush;	
}

void read_cb(bufferevent* be, void* arg)
{
	//std::cout << "[R]" << std::flush;
	pos data[1024] = { 0 };
	//读取输入缓冲数据
	int len = bufferevent_read(be, data, sizeof(data) - 1);
	if (len <= 0)return;
	evutil_socket_t fd = bufferevent_getfd(be);
	pos* c = client.Get(fd);
	c->X = data[0].X;
	c->Y = data[0].Y;
	std::cout << "len " << len/sizeof(pos) << " fd " << fd << " : " << c->X << " - " << c->Y << std::endl;
	pos* posssss = client.GetKeyData(fd);
	for (size_t i = 0; i < client.GetKeyNumber(); i++)
	{
		bufferevent_write(be, &posssss[i], sizeof(pos));
	}
}

void listen_cb(evconnlistener* ev, evutil_socket_t s, sockaddr* sin, int slen, void* arg)
{
	std::cout << "新连接接入:" << s << std::endl;

	pos* P = client.New(s);
	P->X = 0;
	P->Y = 0;
	P->k = s;

	//std::cout << "evconnlistener=" << ev << std::endl;
	//std::cout << "slen=" << slen << std::endl;
	//std::cout << "evutil_socket_t=" << s << std::endl;

	//char ipstr[INET6_ADDRSTRLEN];
	//int port = -1;
	//if (sin->sa_family == AF_INET) { // IPv4
	//	struct sockaddr_in* addr = (struct sockaddr_in*)sin;
	//	inet_ntop(AF_INET, &(addr->sin_addr), ipstr, INET_ADDRSTRLEN);
	//	port = ntohs(addr->sin_port);
	//}
	//else { // IPv6
	//	struct sockaddr_in6* addr = (struct sockaddr_in6*)sin;
	//	inet_ntop(AF_INET6, &(addr->sin6_addr), ipstr, INET6_ADDRSTRLEN);
	//	port = ntohs(addr->sin6_port);
	//}
	//printf("New connection from %s:%d\n", ipstr, port);
	event_base* base = (event_base*)arg;

	//创建bufferevent上下文 BEV_OPT_CLOSE_ON_FREE清理bufferevent时关闭socket
	bufferevent* bev = bufferevent_socket_new(base, s, BEV_OPT_CLOSE_ON_FREE);

	//添加监控事件
	bufferevent_enable(bev, EV_READ | EV_WRITE);

	//设置水位
	//读取水位
	//bufferevent_setwatermark(bev, EV_READ,
	//	5,	//低水位 0就是无限制 默认是0
	//	10	//高水位 0就是无限制 默认是0
	//);

	//bufferevent_setwatermark(bev, EV_WRITE,
	//	5,	//低水位 0就是无限制 默认是0 缓冲数据低于5 写入回调被调用
	//	0	//高水位无效
	//);

	//超时时间的设置
	timeval t1 = { 3,0 };
	bufferevent_set_timeouts(bev, &t1, 0);

	//设置回调函数
	bufferevent_setcb(bev, read_cb, write_cb, event_cb, base);
}