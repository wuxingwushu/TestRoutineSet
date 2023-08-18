#include <iostream>
#include "event2/event.h"
#include "event2/listener.h"
#include "event2/bufferevent.h"
#include "event2/buffer.h"
#include "zlib/zlib.h"
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

struct Zip {
	z_stream* y;
	z_stream* j;
};

struct Label
{
	pos Pos;
	int Atlas[10];
	int BloodVolume;
	int BlueQuantity;
};

Label mLabel;

enum LabelEnum
{
	Pos = 0,
	Atlas,
	BloodVolume,
	BlueQuantity,
};


ContinuousMap<evutil_socket_t, pos> client(10);


//错误，超时 （连接断开会进入）
void event_cb(bufferevent* be, short events, void* arg);

//输入过滤器
bufferevent_filter_result filter_in(
	evbuffer* s,
	evbuffer* d,
	ev_ssize_t limit,
	bufferevent_flush_mode mode,
	void* arg
);

//输出过滤器
bufferevent_filter_result filter_out(
	evbuffer* s,
	evbuffer* d,
	ev_ssize_t limit,
	bufferevent_flush_mode mode,
	void* arg
);

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
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
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

int LS[2] = {0};

void read_cb(bufferevent* be, void* arg)
{
	//char data[1024] = { 0 };
	////读取输入缓冲数据
	//int len = bufferevent_read(be, data, sizeof(data) - 1);
	//if (len <= 0)return;
	//std::string kao = data;
	//bufferevent_write(be, kao.c_str(), kao.size());
	int len = 0,shusize = 0;
	char* Pointer = nullptr;
	if (LS[1] == 0) {
		len = bufferevent_read(be, LS, sizeof(LS));
		if (len != 8) {
			std::cerr << "read LS error !" << std::endl;
			LS[0] = 0;
			LS[1] = 0;
			return;
		}
		std::cout << "==============================" << std::endl;
		std::cout << "L: " << LS[0] << " S: " << LS[1] << std::endl;
		std::cout << "X: " << mLabel.Pos.X << " Y: " << mLabel.Pos.Y << std::endl;
		std::cout << "BloodVolume: " << mLabel.BloodVolume << std::endl;
		std::cout << "BlueQuantity: " << mLabel.BlueQuantity << std::endl;
		for (size_t i = 0; i < 10; i++)
		{
			std::cout << mLabel.Atlas[i] << " ";
		}
		std::cout << std::endl;
	}
	switch (LS[0])
	{
	case Pos:
		Pointer = (char*)&mLabel.Pos;
		shusize = sizeof(mLabel.Pos) - LS[1];
		Pointer += shusize;
		len = bufferevent_read(be, Pointer, LS[1]);
		break;
	case Atlas:
		Pointer = (char*)&mLabel.Atlas;
		shusize = sizeof(mLabel.Atlas) - LS[1];
		Pointer += shusize;
		len = bufferevent_read(be, Pointer, LS[1]);
		break;
	case BloodVolume:
		Pointer = (char*)&mLabel.BloodVolume;
		shusize = sizeof(mLabel.BloodVolume) - LS[1];
		Pointer += shusize;
		len = bufferevent_read(be, Pointer, LS[1]);
		break;
	case BlueQuantity:
		Pointer = (char*)&mLabel.BlueQuantity;
		shusize = sizeof(mLabel.BlueQuantity) - LS[1];
		Pointer += shusize;
		len = bufferevent_read(be, Pointer, LS[1]);
		break;
	default:
		break;
	}
	LS[1] = LS[1] - len;
	bufferevent_write(be, "OK", 2);
}

void listen_cb(evconnlistener* ev, evutil_socket_t s, sockaddr* sin, int slen, void* arg)
{
	std::cout << "新连接接入:" << s << std::endl;

	pos* P = client.New(s);
	P->X = 0;
	P->Y = 0;
	P->k = s;

	event_base* base = (event_base*)arg;

	//创建bufferevent上下文 BEV_OPT_CLOSE_ON_FREE清理bufferevent时关闭socket
	bufferevent* bev = bufferevent_socket_new(base, s, BEV_OPT_CLOSE_ON_FREE);

	//z_stream* zip = new z_stream();
	Zip* zip = new Zip();
	zip->y = new z_stream();
	deflateInit(zip->y, Z_DEFAULT_COMPRESSION);
	zip->j = new z_stream();
	inflateInit(zip->j);

	//2 添加过滤 并设置输入回调
	bufferevent* bev_filter = bufferevent_filter_new(bev,
		filter_in,//输入过滤函数
		filter_out,//输出过滤
		BEV_OPT_CLOSE_ON_FREE,//关闭filter同时管理bufferevent
		0, //清理回调
		zip	//传递参数
	);


	//添加监控事件
	bufferevent_enable(bev_filter, EV_READ | EV_WRITE);

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
	timeval t1 = { 5,0 };
	bufferevent_set_timeouts(bev_filter, &t1, 0);

	//设置回调函数
	bufferevent_setcb(bev_filter, read_cb, write_cb, event_cb, base);
}

//输入过滤器
bufferevent_filter_result filter_in(
	evbuffer* s,
	evbuffer* d,
	ev_ssize_t limit,
	bufferevent_flush_mode mode,
	void* arg
) {
	//解压上下文
	Zip* z = (Zip*)arg;
	z_stream* p = z->j;

	if (!p)
	{
		return BEV_ERROR;
	}
	//解压
	evbuffer_iovec v_in[1];
	//读取数据 不清理缓冲
	int n = evbuffer_peek(s, -1, NULL, v_in, 1);
	if (n <= 0)
		return BEV_NEED_MORE;

	//zlib 输入数据大小
	p->avail_in = v_in[0].iov_len;
	//zlib  输入数据地址
	p->next_in = (Byte*)v_in[0].iov_base;

	//申请输出空间大小
	evbuffer_iovec v_out[1];
	evbuffer_reserve_space(d, 4096, v_out, 1);

	//zlib 输出空间大小
	p->avail_out = v_out[0].iov_len;

	//zlib 输出空间地址
	p->next_out = (Byte*)v_out[0].iov_base;
	//解压数据
	int re = inflate(p, Z_SYNC_FLUSH);

	if (re != Z_OK)
	{
		std::cerr << "in deflate failed!" << std::endl;
	}

	//解压用了多少数据，从source evbuffer中移除
	//p->avail_in 未处理的数据大小
	int nread = v_in[0].iov_len - p->avail_in;

	//解压后数据大小 传入 des evbuffer
	//p->avail_out 剩余空间大小
	int nwrite = v_out[0].iov_len - p->avail_out;

	//移除source evbuffer中数据
	evbuffer_drain(s, nread);

	//cout << "filter_out" << endl;
	//传入 des evbuffer
	v_out[0].iov_len = nwrite;
	evbuffer_commit_space(d, v_out, 1);

	return BEV_OK;
}

//输出过滤器
bufferevent_filter_result filter_out(
	evbuffer* s,
	evbuffer* d,
	ev_ssize_t limit,
	bufferevent_flush_mode mode,
	void* arg
) {
	Zip* z = (Zip*)arg;
	z_stream* p = z->y;
	if (!p)
	{
		return BEV_ERROR;
	}
	//取出buffer中数据的引用
	evbuffer_iovec v_in[1];
	int n = evbuffer_peek(s, -1, 0, v_in, 1);
	if (n <= 0)
	{
		//没有数据 BEV_NEED_MORE 不会进入写入回调
		return BEV_NEED_MORE;
	}
	//zlib 输入数据大小
	p->avail_in = v_in[0].iov_len;
	//zlib  输入数据地址
	p->next_in = (Byte*)v_in[0].iov_base;

	//申请输出空间大小
	evbuffer_iovec v_out[1];
	evbuffer_reserve_space(d, 4096, v_out, 1);

	//zlib 输出空间大小
	p->avail_out = v_out[0].iov_len;

	//zlib 输出空间地址
	p->next_out = (Byte*)v_out[0].iov_base;

	//zlib 压缩
	int re = deflate(p, Z_SYNC_FLUSH);
	if (re != Z_OK)
	{
		std::cerr << "out deflate failed!" << std::endl;
	}


	//压缩用了多少数据，从source evbuffer中移除
	//p->avail_in 未处理的数据大小
	int nread = v_in[0].iov_len - p->avail_in;

	//压缩后数据大小 传入 des evbuffer
	//p->avail_out 剩余空间大小
	int nwrite = v_out[0].iov_len - p->avail_out;

	//std::cout << "size: " << nread << " - " << float(nwrite) / nread << "%" << std::endl;


	//移除source evbuffer中数据
	evbuffer_drain(s, nread);

	//cout << "filter_out" << endl;
	//传入 des evbuffer
	v_out[0].iov_len = nwrite;
	evbuffer_commit_space(d, v_out, 1);
	return BEV_OK;
}