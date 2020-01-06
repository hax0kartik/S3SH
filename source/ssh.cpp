#include "ssh.hpp"
#include "daisywheel.hpp"
#include <time.h>
#include <memory>
#define EXIT_COMMAND	"exit"

unsigned char *gen_rdm_bytestream (size_t num_bytes)
{
	unsigned char *stream = (u8*)malloc (num_bytes);
	size_t i;
	for (i = 0; i < num_bytes; i++)
		stream[i] = rand () % (90 + 1 - 32) + 32;
	return stream;
}

int ssh::init()
{
	srand((unsigned int)time(nullptr));
	utils.set_print_func(ui.top_func);
	
	/*
	utils.print("Console test\n\r");
	char *data = (char*)gen_rdm_bytestream(200);
	utils.print(std::string(data));
	std::string tmp;
	
	while(aptMainLoop())
	{
		tmp = std::string(data);
		utils.print(tmp);
		tmp.clear();
		free(data);
		data = (char*)gen_rdm_bytestream(1900);
		
	}
	*/

	u32 *SOC_buffer = (u32*)memalign(0x1000, 0x100000);
	if(SOC_buffer == NULL) 
	{
		utils.error("memalign: failed to allocate\n");
		return -1;
	}
	// Now intialise soc:u service
	Result ret = 0;
	if ((ret = socInit(SOC_buffer, 0x100000)) != 0) 
	{
		utils.error("socInit:" + std::to_string(ret));
		return -1;
	}

	rc = libssh2_init(0);
	if (rc) 
	{
		utils.error("Error: libssh_init()\n");
		return -1;
		//return (EXIT_FAILURE);
	}
	return 0;
}

int ssh::mainLoop()
{
	auto input_cb = std::bind(utils.put_char, &utils, std::placeholders::_1);
	auto kbd = std::make_unique<keyboard>(ui.bot_func, input_cb);
	//auto kbd = std::make_unique<daisywheelkbd>(ui.bot_func, input_cb);

	utils.print("\x1b[2J");
	utils.print("\x1b[0;0H");
	utils.print("Enter Host Name/Addr:");
	hostname = kbd->get_input();
	utils.print("\n\r");

	utils.print("Enter Username or leave blank for default:");
	username = kbd->get_input();
	if(username == "") username = "username";
	utils.print("\n\rUsername: " + username + "\n\r");
	
	utils.print("Enter port or leave blank to connect to port 22:");
	port = kbd->get_input();
	if(port == "")	port = "22";
	utils.print("\n\rPort: " + port + "\n\r");

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_ADDRCONFIG;

	if(getaddrinfo(hostname.c_str(), port.c_str(), &hints, &res) != 0)
	{
		utils.error("Host not found\n");
		return -1;
	}

	sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sock == -1) {
		utils.error("socket error:" + std::to_string(errno));
		return -1;
		//return (EXIT_FAILURE);
	}
	if (connect(sock, res->ai_addr, res->ai_addrlen) != 0) {
		utils.error("Failed to connect");
		//return (EXIT_FAILURE);
		return -1;
	}
	utils.print("Connect successful!\n");
	
	rc = fcntl(sock, F_SETFL, O_NONBLOCK);
	if (rc == -1) {
		return (EXIT_FAILURE);
	}

	utils.print("FCNTL successful\n\r");
	
	session = libssh2_session_init();
	if (!session) {
		utils.error("SSH init failed");
		return -1;
		//return (EXIT_FAILURE);
	}

	libssh2_session_set_timeout(session, 10000);
	utils.print("Session inited\n\r");

	rc = libssh2_session_handshake(session, sock);
	if (rc) {
		errno = libssh2_session_last_errno(session);
		utils.print("SSH handshake failed ");
		utils.error(std::to_string(errno));
		return -1;
		//return (EXIT_FAILURE);
	}

	utils.print("Handshaked\n\r");

	std::string userauthlist = "";
	char *userauth = libssh2_userauth_list(session, username.c_str(), username.length());
	if(userauth != NULL)
		userauthlist = std::string(userauth);

	if (userauthlist.find("password") != std::string::npos)
		auth_pw |= 1;

	// We do not support keybaord-interactive authentication right now
	//if (strstr(userauthlist, "keyboard-interactive") != NULL)
	// auth_pw |= 2;

	if (userauthlist.find("publickey") != std::string::npos)
		auth_pw |= 4;

	if(auth_pw & 1)
	{
		utils.print("Enter Password:");
		password = kbd->get_input();
		utils.print("\n\rPassword: " + password + "\n\r");

		rc = libssh2_userauth_password(session, username.c_str(), password.c_str());
		if (rc) 
		{
			utils.error("Authentication by password failed");
			return -1;
			//return (EXIT_FAILURE);
		} 
		else 
		{
			utils.print("Authentication by password succeeded");
		}
	}

	else if (auth_pw & 4)
	{
		utils.print("Enter Passphare for the private key(Leave blank if none):");
		password = kbd->get_input();
		
		if (libssh2_userauth_publickey_fromfile(session, username.c_str(), "/3ds/ssh/hostkey.pub", "/3ds/ssh/hostkey", password.c_str())) 
		{
			utils.error("Authentication by public key failed!\n\r");
			return -1;
		} 
		else 
		{
			utils.print("Authentication by public key succeeded.\n\r");
		}
	}

	channel = libssh2_channel_open_session(session);
	if (!channel) {
		utils.error("Unable to open a session");
		return -1;
		//return (EXIT_FAILURE);
	}

	rc = libssh2_channel_request_pty(channel, "xterm-256color");
	if (rc) {
		utils.error("Failed requesting pty\n");
		return -1;
		//return (EXIT_FAILURE);
	}

	rc = libssh2_channel_shell(channel);
	if (rc) {
		utils.error("Unable to request shell on allocated pty\n");
		return -1;
		//return (EXIT_FAILURE);
	}
	utils.print("Channel shell\n\r");
	libssh2_channel_set_blocking(channel, 0);
	/* Main loop starts here.
	 * In it you will be requested to input a command
	 * command will be executed at remote side
	 * an you will get output from it /*/
	 
	std::string buf;
	libssh2_channel_flush(channel);
	fd_set rfds, wfds;
	struct timeval tv;

	utils.print("Executed till main loop\n\r");
	
	kbd->disable_local_echo();
	kbd->async();

	utils.print("Keyboard set to async mode\n\r");

	do 
	{	
		FD_ZERO(&rfds);
		FD_SET(sock, &rfds);

		tv.tv_sec = 2;
		tv.tv_usec = 30;	
		int retval = select(sock + 1, &rfds, NULL, NULL, &tv);
		if(retval == 1 && !kbd->has_data())
		{
			do 
				{
					rc = libssh2_channel_read(channel, inputbuf, sizeof(inputbuf));
					buf.append(inputbuf);
					memset(inputbuf, 0, sizeof(inputbuf));
					// printf("RC: ", rc);
				} while (LIBSSH2_ERROR_EAGAIN != rc && rc > 0);
		
				if (rc < 0 && LIBSSH2_ERROR_EAGAIN != rc) 
				{
					utils.print("libssh2_channel_read error");
				}
				
				else
				{
					utils.print(buf);
					buf.clear();
				}
		}
		
		else
		{
		//	utils.print("Output\n");
			commandbuf = kbd->get_input_async(); // Get every character stroke
			
			if (commandbuf == EXIT_COMMAND)
				break;
			
			written = 0;
			do
			{
				rc = libssh2_channel_write(channel, commandbuf.c_str(), commandbuf.length());
				written += rc;
			} while (LIBSSH2_ERROR_EAGAIN != rc && rc > 0 && written != commandbuf.length());

			//for(int i = 0; i < commandbuf.size(); i++)
			//	input_cb('\b');
			commandbuf.clear();
			if (rc < 0 && LIBSSH2_ERROR_EAGAIN != rc) 
			{
				utils.print("libssh2_channel_write error code");
				return -1;
			//  return (EXIT_FAILURE);
			}
		}
	} while (aptMainLoop());
	return 0;
}

int ssh::deinit()
{
	return 0;
}