#include <iostream>
#include "event2/event.h"
#include "event2/listener.h"
#include "event2/bufferevent.h"
#include <thread>
#define MAP_Debug
#define MAP_Timeout			// 开启  超时检测
#include "../ContinuousMap.h"

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

int posxx, posyy;

//错误，超时 （连接断开会进入）
void client_event_cb(bufferevent* be, short events, void* arg);

void client_write_cb(bufferevent* be, void* arg);

void client_read_cb(bufferevent* be, void* arg);

int main() {	
	std::cin >> posxx;
	posyy = posxx * 2;
	std::cout << "cin:" << posxx << " - " << posyy << std::endl;


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
	//调用客户端代码
	//-1内部创建socket 
	bufferevent* bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);
	sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(5001);
	evutil_inet_pton(AF_INET, "127.0.0.1", &sin.sin_addr.s_addr);

	//设置回调函数
	bufferevent_setcb(bev, client_read_cb, client_write_cb, client_event_cb,0);
	//设置为读写
	bufferevent_enable(bev, EV_READ | EV_WRITE);
	int re = bufferevent_socket_connect(bev, (sockaddr*)&sin, sizeof(sin));
	if (re == 0)
	{
		std::cout << "connected" << std::endl;
	}

	//进入事件主循环
	while (true)
	{
		event_base_loop(base, EVLOOP_ONCE);
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		/*NPC* NP = client.GetData();
		for (size_t i = 0; i < client.GetNumber(); i++)
		{
			NP[i].time--;
			if (NP[i].time < 0) {
				std::cout << "销毁:" << client.GetKey(i) << std::endl;
				client.Delete(client.GetKey(i));
			}
		}*/
	}

	event_base_free(base);


#ifdef _WIN32 
	WSACleanup();
#else

#endif
	return 0;
}


//错误，超时 （连接断开会进入）
void client_event_cb(bufferevent* be, short events, void* arg)
{
	std::cout << "[client_E]" << std::flush;
	//读取超时时间发生后，数据读取停止
	if (events & BEV_EVENT_TIMEOUT && events & BEV_EVENT_READING)
	{
		std::cout << "BEV_EVENT_READING BEV_EVENT_TIMEOUT" << std::endl;
		//bufferevent_enable(be,EV_READ);
		//bufferevent_free(be);
		return;
	}
	else if (events & BEV_EVENT_ERROR)
	{
		//bufferevent_free(be);
		return;
	}
	//服务端的关闭事件
	if (events & BEV_EVENT_EOF)
	{
		std::cout << "BEV_EVENT_EOF" << std::endl;
		//bufferevent_free(be);
	}
	if (events & BEV_EVENT_CONNECTED)
	{
		std::cout << "BEV_EVENT_CONNECTED" << std::endl;
		//触发write
		bufferevent_trigger(be, EV_WRITE, 0);
	}

}
void client_write_cb(bufferevent* be, void* arg)
{
	//std::cout << "[client_W]" << std::flush;
	pos w;
	w.X = posxx;
	w.Y = posyy;
	//写入buffer
	bufferevent_write(be, &w, sizeof(pos));

}

void client_read_cb(bufferevent* be, void* arg)
{
	//std::cout << "[client_R]" << std::flush;
	pos data[1024] = { 0 };
	//读取输入缓冲数据
	int len = 0;
	int dddd;
	do
	{
		dddd = bufferevent_read(be, &data[len], sizeof(pos)); 
		len += dddd / sizeof(pos);
	} while (dddd != 0);
	
	if (len <= 0)return;
	for (size_t i = 0; i < len; i++)
	{
		pos* N = client.Get(data[i].k);
		std::cout << "len " << len << " - " << data[i].k << " - " << data[i].X << " - " << data[i].Y << std::endl;
	}
	if (len < client.GetNumber()) {
		client.TimeoutDetection();
	}
}