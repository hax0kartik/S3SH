#pragma once

#include <libssh2.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <fcntl.h>
#include <malloc.h>
#include <algorithm>
#include <3ds.h>
#include "util.hpp"
#include "daisywheel.hpp"
#include "keyboard.hpp"

class ssh
{
    public:
    int init();
    int mainLoop();
    int deinit();

    private:
    std::string hostname = "";
	std::string username = "username";
	std::string password = "password";
	std::string port = "22";

	int rc, sock, written, auth_pw = 0;
	struct addrinfo hints, *res = nullptr;
	LIBSSH2_SESSION *session;
	LIBSSH2_CHANNEL *channel;
	
    std::string commandbuf;
	char inputbuf[1920];
	util utils;
};