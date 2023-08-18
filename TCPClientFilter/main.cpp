#include <iostream>
#include "event2/event.h"
#include "event2/listener.h"
#include "event2/bufferevent.h"
#include "event2/buffer.h"
#include "zlib/zlib.h"
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

int Enum = 0;
Label mLabel;

enum LabelEnum
{
	Pos = 0,
	Atlas,
	BloodVolume,
	BlueQuantity,
};

ContinuousMap<evutil_socket_t, pos> client(10);

int posxx, posyy;

//错误，超时 （连接断开会进入）
void client_event_cb(bufferevent* be, short events, void* arg);

//输入过滤器
bufferevent_filter_result client_filter_in(
	evbuffer* s,
	evbuffer* d,
	ev_ssize_t limit,
	bufferevent_flush_mode mode,
	void* arg
);

//输出过滤器
bufferevent_filter_result client_filter_out(
	evbuffer* s,
	evbuffer* d,
	ev_ssize_t limit,
	bufferevent_flush_mode mode,
	void* arg
);

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
	bufferevent_setcb(bev, 0, 0, client_event_cb,0);
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
		Zip* zip = new Zip();
		zip->y = new z_stream();
		deflateInit(zip->y, Z_DEFAULT_COMPRESSION);
		zip->j = new z_stream();
		inflateInit(zip->j);

		//创建输出过滤
		bufferevent* bev_filter = bufferevent_filter_new(be,
			client_filter_in, //输入过滤函数
			client_filter_out,//输出过滤
			BEV_OPT_CLOSE_ON_FREE | BEV_OPT_DEFER_CALLBACKS,
			0,//清理回调
			zip//传递参数
		);
		
		//设置读取、写入和事件的回调
		bufferevent_setcb(bev_filter, client_read_cb, client_write_cb, client_event_cb, 0);
		bufferevent_enable(bev_filter, EV_READ | EV_WRITE);

		bufferevent_trigger(bev_filter, EV_WRITE, 0);
	}

}
void client_write_cb(bufferevent* be, void* arg)
{
	//char str[] = { "wo kao wei sei mo ya!" };
	//写入buffer
	//bufferevent_write(be, str, sizeof(str));


	Enum++;
	if (Enum >= 4)Enum = 0;
	bufferevent_write(be, &Enum, sizeof(int));
	int Size;
	switch (Enum)
	{
	case Pos:
		Size = sizeof(pos);
		bufferevent_write(be, &Size, sizeof(int));
		mLabel.Pos.X++;
		mLabel.Pos.Y--;
		bufferevent_write(be, &mLabel.Pos, sizeof(pos));
		break;
	case Atlas:
		Size = sizeof(int) * 10;
		bufferevent_write(be, &Size, sizeof(int));
		mLabel.Atlas[0]++;
		mLabel.Atlas[9]--;
		bufferevent_write(be, &mLabel.Atlas, sizeof(int) * 10);
		break;
	case BloodVolume:
		Size = sizeof(int);
		bufferevent_write(be, &Size, sizeof(int));
		mLabel.BloodVolume++;
		bufferevent_write(be, &mLabel.BloodVolume, sizeof(int));
		break;
	case BlueQuantity:
		Size = sizeof(int);
		bufferevent_write(be, &Size, sizeof(int));
		mLabel.BlueQuantity--;
		bufferevent_write(be, &mLabel.BlueQuantity, sizeof(int));
		break;
	default:
		break;
	}
}

void client_read_cb(bufferevent* be, void* arg)
{
	//std::cout << "[client_R]" << std::flush;
	char data[1024] = { 0 };
	//读取输入缓冲数据
	int len = bufferevent_read(be, &data, sizeof(data));
	if (len <= 0)return;
	std::cout << "=====================" << std::endl;
	std::cout << data << std::endl;
}

//输入过滤器
bufferevent_filter_result client_filter_in(
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
		std::cerr << "deflate failed!" << std::endl;
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
bufferevent_filter_result client_filter_out(
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
		std::cerr << "deflate failed! : " << re <<  std::endl;
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