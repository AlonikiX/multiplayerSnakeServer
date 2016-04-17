//
//  ServerDaemon.cpp
//  GameServer
//
//  Created by Apple on 4/12/16.
//  Copyright © 2016 Aloniki's Study. All rights reserved.
//

#include "ServerDaemon.hpp"


void* iwSingleRoleThreadWrapper(void* arg){
    ServerDaemon* pthis = (ServerDaemon*)arg;
    pthis->newIWSingleRoleThread(NULL);
    pthread_exit(NULL);
}


bool ServerDaemon::initDaemon(in_port_t port){
    //establish socket
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == listenfd) {
        printf("socket establishment error!");
        return false;
    }
    //init server address
    bzero(&this->servaddr,sizeof(this->servaddr));
    this->servaddr.sin_family         = AF_INET;
    this->servaddr.sin_addr.s_addr    = htonl(INADDR_ANY);
    this->servaddr.sin_port           = htons(port);
    //bind server address
    auto bindResult = bind(this->listenfd, (struct sockaddr*)&(this->servaddr), sizeof(this->servaddr));
    if (-1 == bindResult) {
        printf("bind error! errno:%d",errno);
        return false;
    }
    //listen to this socket
    auto listenResult = listen(this->listenfd, LISTENQ);
    if (-1 == listenResult) {
        printf("listen error! errno:%d",errno);
        return false;
    }
    
    return true;
}

void ServerDaemon::run(){
    initDaemon(IWClientPort);
    for (; ; ) {
        this->clilen = sizeof(this->cliaddr);
        this->connfd = accept(this->listenfd, (struct sockaddr*)&(this->cliaddr), &(this->clilen));
        
        char addr[MAXLINE];
        
        inet_ntop(AF_INET, &cliaddr.sin_addr, addr, sizeof(addr));
        printf("signal comes from%s:%d\n",addr,ntohs(cliaddr.sin_port));
        fflush(stdout);
        
        pthread_t tidSingleRole;
        pthread_create(&tidSingleRole, NULL, iwSingleRoleThreadWrapper, this);
        this->tidList->push_back(tidSingleRole);
        
        std::cout<<"new thread!!"<<std::endl;
        
        //        pthread_join(tidSingleRole, NULL);
    }
}

void* ServerDaemon::newIWSingleRoleThread(void* arg){
    auto iwst = new IWSingleRoleThread(this->connfd, this->iwRole);
    iwst->run();
    //    pause();
    std::cout<<"after read!"<<std::endl;
    
    return (void*)0;
}
