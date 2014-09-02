#include <LinkerAPI.h>
#include <DXAPI/DXAPI.h>

#include <WinSock.h>
#include <WinDNS.h>

#define TCPOBJECT_MAXCOUNT 256

static unsigned int TSEXTENSION_RUNNINGTCPOBJECTCOUNT = 0;
static DX::TCPObject *TSEXTENSION_RUNNINGTCPOBJECTS[TCPOBJECT_MAXCOUNT];

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

	bool send_retry;

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

	obj->TSCall("onDisconnect", 0);
	return true;
}

#define TCPOBJECT_BUFFERSIZE 256
enum TCPObjectEvent
{
	TCPObjectEvent_FailedDNSLookup = 1,
	TCPObjectEvent_FailedConnection = 2,
	TCPObjectEvent_FailedConnectionNoPort = 3,
	TCPObjectEvent_FailedSocketCreation = 4,
	TCPObjectEvent_FailedRecv = 5,
	TCPObjectEvent_NULL = 6, // Should be the first Good Identifier
	TCPObjectEvent_GoodDNSLookup = 7,
	TCPObjectEvent_GoodConnection = 8,
	TCPObjectEvent_ReceivedData = 9,
};

const char* conTCPObjectConnect(Linker::SimObject *obj, S32 argc, const char *argv[])
{
	DX::TCPObject *operand = new DX::TCPObject((unsigned int)obj);
	DX::TCPObject *search_object = TCPObject_Find(operand->identifier);
	if (search_object)
	{
		delete operand;
		operand = search_object;
		TCPObject_Disconnect(search_object->identifier);
		
		return false;
	}

	// Copy the hostname over
	char *desired_hostname = (char*)malloc(strlen(argv[2]) + 1);
	memcpy(desired_hostname, argv[2], strlen(argv[2]) + 1);

	// Create the connection info
	ConnectionInformation *connection = new ConnectionInformation;
	connection->target_hostname = desired_hostname;
	connection->buffer = 0x00;
	connection->buffer_length = 0;
	connection->send_retry = false;
	connection->socket = 10;

	// Hack: Store the Ptr to our connection information struct in the old unused state value
	operand->state = (unsigned int)connection;

	//ConnectionInformation *connection = (ConnectionInformation*)parameters;
	char *target_hostname = strlwr(connection->target_hostname);

	// Is it an IP we got?
	bool needs_dns_translation = false;
	if (strstr(target_hostname, "ip:"))
		target_hostname += 3; // Chop off the 'ip:' segment
	else
		needs_dns_translation = true;

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
		operand->TSCall("onConnectFailed", 0);
		return "NO_PORT";
	}

	// Perform a DNS Lookup if we need to
	if (needs_dns_translation)
	{
		wchar_t hostname_dns[128];
		std::mbstowcs(hostname_dns, target_hostname, strlen(target_hostname) + 1);

		PDNS_RECORD dns_record;
		if (DnsQuery(hostname_dns, DNS_TYPE_A, DNS_QUERY_BYPASS_CACHE, NULL, &dns_record, NULL))
		{
			Con::errorf(0, "DNS Resolution Failed");
			operand->TSCall("onDNSFailed", 0);
			return "FAILED_DNS";
		}
		IN_ADDR result_address;
		result_address.S_un.S_addr = dns_record->Data.A.IpAddress;

		// Free the DNS List
		DNS_FREE_TYPE freetype;
		DnsRecordListFree(dns_record, freetype);

		target_hostname = inet_ntoa(result_address);
	}

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
		operand->TSCall("onSocketCreationFailed", 0);
		return "FAILED_SOCKET_CREATION";
	}

	// Attempt the Connection
	if(connect(connection->socket, (SOCKADDR*)&target_host, sizeof(target_host)) != 0)
	{
		Con::errorf(0, "Failed to connect!");
		operand->TSCall("onConnectFailed", 0);
		return "CANNOT_CONNECT";
	}
	else
		operand->TSCall("onConnected", 0);

	// Set Blocking Mode
	u_long imode = 1;
	ioctlsocket(connection->socket, FIONBIO, &imode);

	// Stick us in the TCPObject array
	TSEXTENSION_RUNNINGTCPOBJECTS[TSEXTENSION_RUNNINGTCPOBJECTCOUNT] = operand;
	TSEXTENSION_RUNNINGTCPOBJECTCOUNT++;

	return "unknown_error";
}

bool conTCPObjectSend(Linker::SimObject *obj, S32 argc, const char *argv[])
{
	DX::TCPObject operand((unsigned int)obj);

	Con::errorf(0, "Should Send? - SimID %u", operand.identifier);
	//if (!TCPObject_Find(operand.identifier))
	//	return false;

	Con::errorf(0, "Sending");

	ConnectionInformation *connection = (ConnectionInformation*)operand.state;

	// Pause the thread as we're doing an recv in the main thread
	if (send(connection->socket, argv[2], strlen(argv[2]), 0) == SOCKET_ERROR)
	{
		Con::errorf(0, "Failed to send: %u (Socket: %u, SimID: %u)", WSAGetLastError(), connection->socket, operand.identifier);
		return false;
	}
	
	return true;
}

bool conTCPObjectDisconnect(Linker::SimObject *obj, S32 argc, const char *argv[])
{	
	DX::TCPObject operand((unsigned int)obj);
	return TCPObject_Disconnect(operand.identifier);
}

bool conTSExtensionUpdate(Linker::SimObject *obj, S32 argc, const char *argv[])
{
	// Iterate through any active sockets
	static char *character_buffer = (char*)malloc(TCPOBJECT_BUFFERSIZE);

	// List of objects to D/C
	//TCPObject_Disconnect
	unsigned int disconnected_object_count = 0;
	static DX::TCPObject **disconnected_objects = (DX::TCPObject**)malloc(sizeof(DX::TCPObject*) * TCPOBJECT_MAXCOUNT);

	for (unsigned int iteration = 0; iteration < TSEXTENSION_RUNNINGTCPOBJECTCOUNT; iteration++)
	{
		DX::TCPObject *current_connection = TSEXTENSION_RUNNINGTCPOBJECTS[iteration];
		ConnectionInformation *connection_information = (ConnectionInformation*)current_connection->state;

		// TODO: Kill the TCPObject
		if (connection_information->socket == 0)
			continue;

		unsigned int data_length = recv(connection_information->socket, character_buffer, TCPOBJECT_BUFFERSIZE, 0);

		int currentError = WSAGetLastError();
		if (currentError != WSAEWOULDBLOCK && currentError != 0)
			Con::errorf(0, "Got an error! %u", currentError);
		else if (data_length == 0)
		{
			Con::errorf(0, "Finished receiving?");

			// Put us on the D/C list
			disconnected_objects[disconnected_object_count] = current_connection;
			disconnected_object_count++;

			// Stream the data into ::onLine
			unsigned int current_start = 0;
			for (unsigned int split_iteration = 0; split_iteration < connection_information->buffer_length; split_iteration++)
				if (connection_information->buffer[split_iteration] == '\n' || split_iteration == connection_information->buffer_length - 1)
				{
					unsigned int desired_length = (split_iteration - current_start);
					char *current_line = (char*)malloc(desired_length + 1);
					memset(current_line, 0x00, desired_length + 1);
					memcpy(current_line, &connection_information->buffer[current_start], desired_length);

					current_start = split_iteration + 1;
					current_connection->TSCall("onLine", 1, current_line);
					free(current_line);
				}

			closesocket(connection_information->socket);
			connection_information->socket = 0;
			free(connection_information->buffer);
		}
		else if (data_length <= TCPOBJECT_BUFFERSIZE)
		{
			Con::errorf(0, "Received Data: %u", data_length);

			// If our connection hasn't buffered anything yet
			if (connection_information->buffer == 0x00)
			{
				connection_information->buffer = (char*)malloc(data_length);
				memset(connection_information->buffer, 0x00, data_length);

				connection_information->buffer_length = data_length;
				memcpy(connection_information->buffer, character_buffer, data_length);
			}
			else
			{
				unsigned int new_buffer_length = data_length + connection_information->buffer_length;
				char *new_buffer = (char*)malloc(new_buffer_length);
				memset(new_buffer, 0x00, new_buffer_length);

				// Copy the two halves
				memcpy(new_buffer, connection_information->buffer, connection_information->buffer_length);
				memcpy(&new_buffer[connection_information->buffer_length], character_buffer, data_length);

				connection_information->buffer = new_buffer;
				connection_information->buffer_length = new_buffer_length;
			}

			memset(character_buffer, 0x00, TCPOBJECT_BUFFERSIZE);
		}
	}

	// Process Disconnect list
	for (unsigned int iteration = 0; iteration < disconnected_object_count; iteration++)
		TCPObject_Disconnect(disconnected_objects[iteration]->identifier);

	return true;
}