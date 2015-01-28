#include <ReverseTCPConnection.h>
int startServer() {
	WSADATA wsa;
	SOCKET master,new_socket, client_socket[256],s;
	struct sockaddr_in server,address;
	int max_clients=256,activity,addrlen,i,valread;
	int MAXRECV=1024;
	fd_set readfds;
	char *buffer;
	buffer = (char *) malloc((MAXRECV+1)*sizeof(char));
	for (i=0; i<256;i++)
	{
		client_socket[i]=0;
	}
	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
	{
		return -1;
	}
	if ((master = socket(AF_INET,SOCK_STREAM,0))==INVALID_SOCKET)
	{
		return -1;
	}
	server.sin_family = AF_INET;
	server.sin_addr.s_addr=INADDR_ANY;
	server.sin_port = htons(27666);
	if (bind(master , (struct sockaddr *)&server, sizeof(server))==SOCKET_ERROR){
		return -1;
	}
	listen(master,3);
	addrlen = sizeof(struct sockaddr_in);
	while (TRUE)
	{
		FD_ZERO(&readfds);
		FD_SET(master,&readfds);
		for (  i = 0 ; i < max_clients ; i++) 
        {
            s = client_socket[i];
            if(s > 0)
            {
                FD_SET( s , &readfds);
            }
        }
		activity = select(0,&readfds, NULL, NULL, NULL);
		if (activity==SOCKET_ERROR)
		{
			return -2;
		}
		if (FD_ISSET(master,&readfds))
		{
			if ((new_socket = accept(master, (struct sockaddr *)&address, (int *)&addrlen))<0)
			{
				return -3;
			}
			for (i=0; i<max_clients; i++) {
				if(client_socket[i] == 0)
				{
				
				client_socket[i] = new_socket;
				break;
				}
			}
		}
		for (i=0; i<max_clients; i++)
		{
			s=client_socket[i];
			if (FD_ISSET(s,&readfds))
			{
				getpeername(s,(struct sockaddr*)&address, (int*)&addrlen);
				valread = recv(s,buffer,MAXRECV,0);
				if (valread == SOCKET_ERROR) 
				{
					
					int error_code = WSAGetLastError();
					if (error_code == WSAECONNRESET)
					{
						closesocket(s);
						client_socket[i]=0;
					} 
					else
					{

					}


				}
				if (valread==0)
				{
					closesocket(s);
					client_socket[i]=0;
				}
				else 
				{
					buffer[valread]=0;
					
				}
			
			
			}

		}
		

	}
}