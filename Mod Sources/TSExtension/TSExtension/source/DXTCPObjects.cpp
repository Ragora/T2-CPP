#include <WinSock.h>
#include <WinDNS.h>

#include <map>
#include <set>
#include <string>
#include <memory>

#include <LinkerAPI.h>
#include <DXAPI/DXAPI.h>


#define TCPOBJECT_MAXCOUNT 256
#define TCPOBJECT_BUFFERSIZE 256
#define TCPOBJECT_SENDQUEUELENGTH 64 // Number of elements

//! A mapping of object ID's to DX::TCPObject instance pointers.
static std::map<int, std::unique_ptr<DX::TCPObject>> sRunningTCPObjects;

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
static unsigned int DXTCP_GetSocketTime(SOCKET sock)
{
	unsigned int optVal;
	int optLen = sizeof(unsigned int);

	getsockopt(sock, SOL_SOCKET, SO_CONNECT_TIME, (char*)&optVal, &optLen);
	return optVal;
}

typedef struct
{
	char *target_hostname;
	unsigned int target_port;

	unsigned int buffer_length;

	std::set<std::string> mOutgoingQueue;
	std::string mIncomingBuffer;
	bool is_connected;

	SOCKET socket;
} ConnectionInformation;

inline void TCPObject_Disconnect(DX::TCPObject *obj)
{
	ConnectionInformation *connectionInfo = (ConnectionInformation*)obj->state;

	if (connectionInfo->socket == 0)
	{
		closesocket(connectionInfo->socket);
		connectionInfo->socket = 0;

		Con::errorf(0, "Attempted to disconnect an already disconnected TCPObject - %u", obj->identifier);
		return;
	}

	obj->CallMethod("onDisconnect", 0);
	delete connectionInfo;

	// Causes the ptr to our DX::TCPObject to deallocate
	sRunningTCPObjects.erase(obj->identifier);

	Con::errorf(0, "Processed Disconnect");
}

const char* conTCPObjectConnect(Linker::SimObject *obj, S32 argc, const char *argv[])
{
	if (sRunningTCPObjects.count(obj->mId) >= 1)
		TCPObject_Disconnect(sRunningTCPObjects[obj->mId].get());
		
	DX::TCPObject *operand = new DX::TCPObject((unsigned int)obj);

	// Copy the hostname over
	char *desired_hostname = (char*)malloc(strlen(argv[2]) + 1);
	memcpy(desired_hostname, argv[2], strlen(argv[2]) + 1);

	// Create the connection info
	ConnectionInformation *connection = new ConnectionInformation;
	connection->target_hostname = desired_hostname;
	connection->buffer_length = 0;
	connection->is_connected = false;
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

	sRunningTCPObjects[obj->mId] = std::unique_ptr<DX::TCPObject>(operand);

	// Attempt the Connection
	connect(connection->socket, (SOCKADDR*)&target_host, sizeof(target_host));

	if (getsockopt(connection->socket, SOL_SOCKET, SO_ERROR, NULL, NULL) < 0)
	{
		operand->CallMethod("onConnectFailed", 2, S32ToCharPtr(3), S32ToCharPtr(WSAGetLastError()));
		return "CANNOT_CONNECT";
	}
	else
	{
		operand->CallMethod("onConnected", 0);
		return "SUCCESS";
	}

	return "UNKNOWN_ERROR";
}

bool conTCPObjectSend(Linker::SimObject *obj, S32 argc, const char *argv[])
{
	Con::errorf(0, "Should Send? - SimID %s", argv[1]);

	if (sRunningTCPObjects.count(obj->mId) == 0)
		return false;

	DX::TCPObject *operand = sRunningTCPObjects[obj->mId].get();
	ConnectionInformation *connection = (ConnectionInformation*)operand->state;

	// Since we can be attempting to send data before we're connected, we'll just queue 
	// the data here and send it all in our update function next call
	if (!connection->is_connected)
		Con::errorf(0, "Attempted to send before connected.");

	connection->mOutgoingQueue.insert(connection->mOutgoingQueue.end(), argv[2]);

	Con::errorf(0,"Queued data: %s", argv[2]);
	return true;
}

bool conTCPObjectDisconnect(Linker::SimObject *obj, S32 argc, const char *argv[])
{	
	if (sRunningTCPObjects.count(obj->mId) == 0)
		return false;

	TCPObject_Disconnect(sRunningTCPObjects[obj->mId].get());
	return true;
}

bool conTSExtensionUpdate(Linker::SimObject *obj, S32 argc, const char *argv[])
{
	// Iterate through any active sockets
	static char *incoming_buffer = new char[TCPOBJECT_BUFFERSIZE];

	// List of objects to D/C
	std::set<std::unique_ptr<DX::TCPObject>> disconnections;

	for (std::map<int, std::unique_ptr<DX::TCPObject>>::iterator it = sRunningTCPObjects.begin(); it != sRunningTCPObjects.end(); it++)
	{
		memset(incoming_buffer, 0x00, TCPOBJECT_BUFFERSIZE);

		const std::unique_ptr<DX::TCPObject> &currentConnection = (*it).second;

		ConnectionInformation *connectionInfo = (ConnectionInformation*)currentConnection.get()->state;

		const int socketTime = DXTCP_GetSocketTime(connectionInfo->socket);
		if (socketTime >= 1 && !connectionInfo->is_connected)
		{
			connectionInfo->is_connected = true;
			currentConnection.get()->CallMethod("onConnected", 0);
		}

		// Process the send queue first
		bool connection_is_ready = true;
		if (connectionInfo->is_connected)
		{
			for (std::set<std::string>::iterator it = connectionInfo->mOutgoingQueue.begin(); it != connectionInfo->mOutgoingQueue.end(); it++)
			{
				const std::string &currentMessage = *it;
				Con::errorf(0, "Processing Send: %s", currentMessage.c_str());

				if (send(connectionInfo->socket, currentMessage.c_str(), currentMessage.length(), 0) == SOCKET_ERROR)
				{
					int wsa_error = WSAGetLastError();

					// We're not ready yet, just break and we should eventually be ready
					if (wsa_error == WSAEWOULDBLOCK)
					{
						connection_is_ready = false;
						break;
					}

					connectionInfo->is_connected = false;
					disconnections.insert(disconnections.end(), currentConnection.get());
					Con::errorf(0,"Got a send error! SimID: %u - Error %u", currentConnection->identifier, wsa_error);

					break;
				}
			}

			// Empty the queue
			connectionInfo->mOutgoingQueue.clear();
		}

		// We can break if the connection was never made yet or if there was an error processing the message queue
		if (!connectionInfo->is_connected || !connection_is_ready)
			continue;

		// FIXME: Under send() error conditions we can't deallocate all of the associated memory
		unsigned int data_length = recv(connectionInfo->socket, incoming_buffer, TCPOBJECT_BUFFERSIZE, 0);

		int currentError = WSAGetLastError();

		Con::errorf(0, "%u", DXTCP_GetSocketTime(connectionInfo->socket));

		if (currentError != WSAEWOULDBLOCK && currentError != 0)
		{
			Con::errorf(0, "Got an error! %u - SimID %u", currentError, currentConnection.get()->identifier);

			disconnections.insert(disconnections.end(), currentConnection.get());
		}
		else if (data_length == 0)
		{
			Con::errorf(0, "Finished receiving?");

			// Put us on the D/C list
			disconnections.insert(disconnections.end(), currentConnection.get());

			// Stream the data into ::onLine
			unsigned int current_start = 0;

			for (unsigned int split_iteration = 0; split_iteration < connectionInfo->mIncomingBuffer.length(); split_iteration++)
			{
				if (connectionInfo->mIncomingBuffer[split_iteration] == '\r')
					connectionInfo->mIncomingBuffer[split_iteration] = ' ';

				if (connectionInfo->mIncomingBuffer[split_iteration] == '\n') //  || split_iteration == connection_information->buffer_length - 1
				{
					const std::string currentLine = connectionInfo->mIncomingBuffer.substr(current_start, split_iteration - current_start);

					current_start = split_iteration + 1;

					Con::errorf(0, "Streaming: %s", currentLine.c_str());
					currentConnection.get()->CallMethod("onLine", 1, currentLine.c_str());
				}
			}
		}
		else if (data_length <= TCPOBJECT_BUFFERSIZE)
		{
			Con::errorf(0, "Received Data: %u", data_length);

			connectionInfo->mIncomingBuffer += incoming_buffer;
		}
	}

	// Process Disconnect list
	for (std::set<std::unique_ptr<DX::TCPObject>>::iterator it = disconnections.begin(); it != disconnections.end(); it++)
		TCPObject_Disconnect((*it).get());

	return true;

}

bool conHTTPObjectDoNothing(Linker::SimObject *obj, S32 argc, const char *argv[])
{
	return true;
}