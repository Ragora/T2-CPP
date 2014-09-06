#include <LinkerAPI.h>
#include <DXAPI/DXAPI.h>

#include <WinSock.h>
#include <WinDNS.h>

#define TCPOBJECT_MAXCOUNT 256
#define TCPOBJECT_BUFFERSIZE 256
#define TCPOBJECT_SENDQUEUELENGTH 64 // Number of elements

static unsigned int TSEXTENSION_RUNNINGTCPOBJECTCOUNT = 0;
static DX::TCPObject *TSEXTENSION_RUNNINGTCPOBJECTS[TCPOBJECT_MAXCOUNT];

// Since TS wants function call arguments to be of type char*, we use this 
// helper function to painlessly pass in unsigned int arguments for things
// such as the return value from WSAGetLastError().
__forceinline static char *S32ToCharPtr(unsigned int in)
{
	char out[256];
	memset(out, NULL, 256);
	sprintf_s<256>(out, "%u", in);
	return out;
}

// Also a helper function to return the status of a socket
static bool DXTCP_GetSocketStatus(SOCKET sock)
{
	fd_set sockets;
	sockets.fd_array[0] = sock;
	sockets.fd_count = 1;
		
	// We don't want to do any waiting at all
	timeval wait_time;
	wait_time.tv_sec = 0;
	wait_time.tv_usec = 0;

	return select(sock, &sockets, &sockets, NULL, &wait_time) != SOCKET_ERROR;
}

inline DX::TCPObject *TCPObject_Find(unsigned int identifier)
{
	// Make sure it's in our list of objects
	for (unsigned int iteration = 0; iteration < TSEXTENSION_RUNNINGTCPOBJECTCOUNT; iteration++)
		if (TSEXTENSION_RUNNINGTCPOBJECTS[iteration]->identifier == identifier)
			return TSEXTENSION_RUNNINGTCPOBJECTS[iteration];

	return 0x00;
}

typedef struct
{
	char *target_hostname;
	unsigned int target_port;

	unsigned int buffer_length;
	char *buffer;

	unsigned int message_count;
	char *message_queue[TCPOBJECT_SENDQUEUELENGTH];

	bool is_connected;

	SOCKET socket;
} ConnectionInformation;


inline bool TCPObject_Disconnect(unsigned int identifier)
{
	DX::TCPObject *obj = TCPObject_Find(identifier);
	if (!obj)
		return false;

	ConnectionInformation *connection = (ConnectionInformation*)obj->state;

	closesocket(connection->socket);
	connection->socket = 0;

	// Find us in the array
	unsigned int target_index = 0;
	for (unsigned int iteration = 0; iteration < TSEXTENSION_RUNNINGTCPOBJECTCOUNT; iteration++)
		if (TSEXTENSION_RUNNINGTCPOBJECTS[iteration] == obj)
		{
			target_index = iteration;
			break;
		}
		
	// Fix the array
	for (unsigned int iteration = target_index; iteration < TSEXTENSION_RUNNINGTCPOBJECTCOUNT; iteration++)
		TSEXTENSION_RUNNINGTCPOBJECTS[iteration] = TSEXTENSION_RUNNINGTCPOBJECTS[iteration + 1];
	TSEXTENSION_RUNNINGTCPOBJECTCOUNT--;

	obj->CallMethod("onDisconnect", 0);
	return true;
}

const char* conTCPObjectConnect(Linker::SimObject *obj, S32 argc, const char *argv[])
{
	DX::TCPObject *operand = TCPObject_Find(atoi(argv[1]));
	if (operand)
	{
		TCPObject_Disconnect(operand->identifier);
		delete operand;
	}
		
	operand = new DX::TCPObject((unsigned int)obj);

	// Copy the hostname over
	char *desired_hostname = (char*)malloc(strlen(argv[2]) + 1);
	memcpy(desired_hostname, argv[2], strlen(argv[2]) + 1);

	// Create the connection info
	ConnectionInformation *connection = new ConnectionInformation;
	connection->target_hostname = desired_hostname;
	connection->buffer = 0x00;
	connection->buffer_length = 0;
	connection->is_connected = false;
	connection->message_count = 0;
	connection->socket = 0;

	// Hack: Store the Ptr to our connection information struct in the old unused state value
	operand->state = (unsigned int)connection;

	//ConnectionInformation *connection = (ConnectionInformation*)parameters;
	char *target_hostname = strlwr(connection->target_hostname);

	// Is it an IP we got?
	if (strstr(target_hostname, "ip:"))
		target_hostname += 3; // Chop off the 'ip:' segment

	// Did we get a port #?
	unsigned int desired_port = 0;
	char *port_delineator = strstr(target_hostname, ":");
	if (port_delineator)
	{
		port_delineator[0] = 0x00; // NULL Terminate the IP Segment
		port_delineator += 1;
		desired_port = atoi(port_delineator);
	}
	else
	{
		Con::errorf(0, "No Port");
		operand->CallMethod("onConnectFailed", 1, S32ToCharPtr(1));
		return "NO_PORT";
	}

	// Perform a DNS Lookup
	wchar_t hostname_dns[128];
	std::mbstowcs(hostname_dns, target_hostname, strlen(target_hostname) + 1);

	PDNS_RECORD dns_record;
	if (DnsQuery(hostname_dns, DNS_TYPE_A, DNS_QUERY_BYPASS_CACHE, NULL, &dns_record, NULL))
	{
		Con::errorf(0, "DNS Resolution Failed");
		operand->CallMethod("onDNSFailed", 0);
		return "FAILED_DNS";
	}
	IN_ADDR result_address;
	result_address.S_un.S_addr = dns_record->Data.A.IpAddress;

	DNS_FREE_TYPE freetype;
	DnsRecordListFree(dns_record, freetype);

	target_hostname = inet_ntoa(result_address);

	SOCKADDR_IN target_host;
	target_host.sin_family = AF_INET;
	target_host.sin_port = htons(desired_port);
	target_host.sin_addr.s_addr = inet_addr(target_hostname);

	Con::errorf(0, "Target %s on port %u SimID %u", target_hostname, desired_port, operand->identifier);

	// Create the Socket
	connection->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (connection->socket == INVALID_SOCKET)
	{
		Con::errorf(0, "Failed to create Socket!");
		operand->CallMethod("onConnectFailed", 2, S32ToCharPtr(2), S32ToCharPtr(WSAGetLastError()));
		return "FAILED_SOCKET_CREATION";
	}

	// Set Blocking Mode (a non-zero for imode = non-blocking)
	u_long imode = 1;
	ioctlsocket(connection->socket, FIONBIO, &imode);

	// Stick us in the TCPObject array
	TSEXTENSION_RUNNINGTCPOBJECTS[TSEXTENSION_RUNNINGTCPOBJECTCOUNT] = operand;
	TSEXTENSION_RUNNINGTCPOBJECTCOUNT++;

	// Attempt the Connection
	connect(connection->socket, (SOCKADDR*)&target_host, sizeof(target_host));
	if (DXTCP_GetSocketStatus(connection->socket) == SOCKET_ERROR)
	{
		operand->CallMethod("onConnectFailed", 2, S32ToCharPtr(3), S32ToCharPtr(WSAGetLastError()));
		return "CANNOT_CONNECT";
	}
	else
	{
		connection->is_connected = true;
		operand->CallMethod("onConnected", 0);
		return "SUCCESS";
	}

	return "UNKNOWN_ERROR";
}

bool conTCPObjectSend(Linker::SimObject *obj, S32 argc, const char *argv[])
{
	Con::errorf(0, "Should Send? - SimID %s", argv[1]);

	if (!TCPObject_Find(atoi(argv[1])))
		return false;

	DX::TCPObject operand((unsigned int)obj);
	ConnectionInformation *connection = (ConnectionInformation*)operand.state;

	// Since we can be attempting to send data before we're connected, we'll just queue 
	// the data here and send it all in our update function next call
	if (!connection->is_connected)
		Con::errorf(0, "Attempted to send before connected.");

	// Tribes 2 probably deallocates the memory associated with the arguments at some point
	// so we'll copy the send payload into an independent chunk of memory
	char *send_payload = new char[strlen(argv[2]) + 1];
	memset(send_payload, 0x00, strlen(argv[2]) + 1);
	memcpy(send_payload, argv[2], strlen(argv[2]) + 1);

	connection->message_queue[connection->message_count] = send_payload;
	connection->message_count++;

	Con::errorf(0,"Queued data: %s", argv[2]);
	return true;
}

bool conTCPObjectDisconnect(Linker::SimObject *obj, S32 argc, const char *argv[])
{	
	return TCPObject_Disconnect(atoi(argv[1]));
}

bool conTSExtensionUpdate(Linker::SimObject *obj, S32 argc, const char *argv[])
{
	// Iterate through any active sockets
	static char *incoming_buffer = new char[TCPOBJECT_BUFFERSIZE];

	// List of objects to D/C
	unsigned int disconnected_object_count = 0;
	static DX::TCPObject **disconnected_objects = (DX::TCPObject**)malloc(sizeof(DX::TCPObject*) * TCPOBJECT_MAXCOUNT);

	for (unsigned int iteration = 0; iteration < TSEXTENSION_RUNNINGTCPOBJECTCOUNT; iteration++)
	{
		// Zero out the incoming buffer per iteration
		memset(incoming_buffer, 0x00, TCPOBJECT_BUFFERSIZE);

		DX::TCPObject *current_connection = TSEXTENSION_RUNNINGTCPOBJECTS[iteration];
		ConnectionInformation *connection_information = (ConnectionInformation*)current_connection->state;

		// FIXME: ::onConnect is never called if is where we finally realize we're connected
		// Check if we're ready to be performing network operations
		if (DXTCP_GetSocketStatus(connection_information->socket))
			connection_information->is_connected = true;
		else
		{
			Con::errorf(0,"Socket status error!");
			disconnected_objects[disconnected_object_count] = current_connection;
			disconnected_object_count++;
			break;
		}

		// Process the send queue first
		bool connection_is_ready = true;
		if (connection_information->is_connected && connection_information->message_count != 0)
			for (unsigned int queue_iteration = 0; queue_iteration < connection_information->message_count; queue_iteration++)
				if (send(connection_information->socket, connection_information->message_queue[queue_iteration], strlen(connection_information->message_queue[queue_iteration]), 0) == SOCKET_ERROR)
				{
					int wsa_error = WSAGetLastError();
					// We're not ready yet, just break and we should eventually be ready
					if (wsa_error == WSAEWOULDBLOCK)
					{
						connection_is_ready = false;
						break;
					}

					connection_information->is_connected = false;
					disconnected_objects[disconnected_object_count] = current_connection;
					disconnected_object_count++;

					Con::errorf(0,"Got a send error! SimID: %u - Error %u", current_connection->identifier, wsa_error);
					break;
				}
				else
					delete[] connection_information->message_queue[queue_iteration];
		
		// We can break if the connection was never made yet or if there was an error processing the message queue
		if (!connection_information->is_connected || !connection_is_ready)
			break;

		// FIXME: Under send() error conditions we can't deallocate all of the associated memory
		connection_information->message_count = 0;
		unsigned int data_length = recv(connection_information->socket, incoming_buffer, TCPOBJECT_BUFFERSIZE, 0);

		int currentError = WSAGetLastError();
		if (currentError != WSAEWOULDBLOCK && currentError != 0)
		{
			Con::errorf(0, "Got an error! %u - SimID %u", currentError, current_connection->identifier);
			disconnected_objects[disconnected_object_count] = current_connection;
			disconnected_object_count++;
		}
		else if (data_length == 0)
		{
			Con::errorf(0, "Finished receiving?");

			// Put us on the D/C list
			disconnected_objects[disconnected_object_count] = current_connection;
			disconnected_object_count++;

			// Our actual buffer is +1 bytes in length, so will set the extra byte to 0x00 to ensure NULL termination
			connection_information->buffer[connection_information->buffer_length] = 0x00;

			Con::errorf(0, "Stream Len: %u Bytes", connection_information->buffer_length);
			// Stream the data into ::onLine
			unsigned int current_start = 0;
			for (unsigned int split_iteration = 0; split_iteration < connection_information->buffer_length; split_iteration++)
			{
				bool streaming_line = false;
				if (connection_information->buffer[split_iteration] == '\n') //  || split_iteration == connection_information->buffer_length - 1
				{
					connection_information->buffer[split_iteration] = 0x00;
					streaming_line = true;
				}
				else if (split_iteration == connection_information->buffer_length - 1)
					streaming_line = true;

				//unsigned int desired_length = (split_iteration - current_start);
	
				//if (desired_length == data_length)
				//	current_connection->TSCall("onLine", 1, connection_information->buffer);
				//else
				//{
				//if(split_iteration != connection_information->buffer_length - 1)
				//	connection_information->buffer[split_iteration] = 0x00;

				if (streaming_line)
				{
					// Time to be clever: Since T2 doesn't care what happens to the string after it's passed in, I'm not
					// Bothering to allocate more memory for the results. I'm just going to manipulate it to appear as
					// different lines but in reality they're all sourced from the same memory.
					char *current_line = &connection_information->buffer[current_start];

					Con::errorf(0, "Streamed: %s", current_line);

					// If we just have a blank line (a carriage return), replace it with the space character
					if (strlen(current_line) == 1 && current_line[0] == 0xD)
						current_line[0] = 0x20;

					current_start = split_iteration + 1;
					current_connection->CallMethod("onLine", 1, current_line);
				}
			}

			closesocket(connection_information->socket);
			connection_information->socket = 0;
			delete[] connection_information->buffer;
		}
		else if (data_length <= TCPOBJECT_BUFFERSIZE)
		{
			Con::errorf(0, "Received Data: %u", data_length);

			// If our connection hasn't buffered anything yet
			if (connection_information->buffer == 0x00)
			{
				// Allocate our memory with a +1 Byte Size (to ensure it's properly NULL terminated when we stream to ::onLine)
				connection_information->buffer = new char[data_length + 1];
				memset(connection_information->buffer, 0x00, data_length + 1);

				connection_information->buffer_length = data_length;
				memcpy(connection_information->buffer, incoming_buffer, data_length);
			}
			else
			{
				unsigned int new_buffer_length = data_length + connection_information->buffer_length;
				char *new_buffer = new char[new_buffer_length + 1];
				memset(new_buffer, 0x00, new_buffer_length + 1);

				// Copy the two halves
				memcpy(new_buffer, connection_information->buffer, connection_information->buffer_length);
				memcpy(&new_buffer[connection_information->buffer_length], incoming_buffer, data_length);

				connection_information->buffer = new_buffer;
				connection_information->buffer_length = new_buffer_length;
			}
		}
	}

	// Process Disconnect list
	for (unsigned int iteration = 0; iteration < disconnected_object_count; iteration++)
		TCPObject_Disconnect(disconnected_objects[iteration]->identifier);

	return true;
}

bool conHTTPObjectDoNothing(Linker::SimObject *obj, S32 argc, const char *argv[])
{
	return true;
}