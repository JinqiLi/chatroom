
#include <stdio.h>    
#include <string.h>    
#include <iostream>  
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <event.h>
#include <sys/socket.h>
#include <sys/types.h>
using namespace std;

//事件base
struct event_base *base;

void onWrite(int sock, short iEvent, void* arg){
	char* buf = (char*)arg;
	send(sock, buf, strlen(buf), 0);

	//delete buf;
}

void onRead(int iCliFd, short iEvent, void* arg){
	int iLen;
	char buf[1024];

	iLen = recv(iCliFd, buf, sizeof(buf), 0);

	if(iLen <= 0){
		cout << "client close" << endl;

		//连接结束(=0)或连接错误(<0),将事件删除并释放内存空间
		struct event *pEvRead = (struct event*)arg;
		event_del(pEvRead);
		delete pEvRead;

		close(iCliFd);

		return;
	}

	buf[iLen] = 0;
	cout << "Client Info:" << buf << endl;

	char MESSAGE[] = "welcome to server...";
	struct event* write_ev = new event;
	event_set(write_ev, iCliFd, EV_WRITE, onWrite, MESSAGE);
	event_base_set(base, write_ev);
	event_add(write_ev, NULL);

}

//连接请求事件回调函数
void onAccept(int iSvrFd, short iEvent, void* arg){
	int iCliFd;
	struct sockaddr_in sCliAddr;

	socklen_t iSinSize = sizeof(sCliAddr);
	iCliFd = accept(iSvrFd, (sockaddr*)&sCliAddr, &iSinSize);

	//连接注册为新事件，
	struct event *pEvRead = new event;
	event_set(pEvRead, iCliFd, EV_READ|EV_PERSIST, onRead, pEvRead);
	event_base_set(base, pEvRead);
	event_add(pEvRead, NULL);
}

int main(){
	int iSvrFd;
	struct sockaddr_in sSvrAddr;

	memset(&sSvrAddr, 0, sizeof(sSvrAddr));
	sSvrAddr.sin_family = AF_INET;
	sSvrAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	sSvrAddr.sin_port = htons(8888);

	//创建tcp socket，监听本机8888端口
	iSvrFd = socket(AF_INET, SOCK_STREAM, 0);
	bind(iSvrFd, (struct sockaddr*)&sSvrAddr, sizeof(sSvrAddr));
	listen(iSvrFd, 10);

	//初始化base
	base = (struct event_base*)event_init();

	struct event evListen;

	//设置事件
	event_set(&evListen, iSvrFd, EV_READ|EV_PERSIST, onAccept, NULL);

	//设置为base事件
	event_base_set(base, &evListen);

	//添加事件
	event_add(&evListen, NULL);

	//事件循环
	event_base_dispatch(base); 
}

