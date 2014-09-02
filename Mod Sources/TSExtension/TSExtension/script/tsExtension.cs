function tsExtensionLoop()
{
	if (!$TSExtension::isActive)
	{
		error("TSExtension.cs - Unable to run loop! Is the DLL installed?");
		return;
	}
	
	if (isEventPending($TSExtension::LastUpdateHandle))
		cancel($TSExtension::LastUpdateHandle);
	
	tsExtensionUpdate();
	
	$TSExtension::LastUpdateHandle = schedule(32,0,"tsExtensionLoop");
}

if (!$TSExtension::isActive)
	loadMod("TSExtension");
tsExtensionLoop();

// Test
function Whatever::onSocketCreationFailed(%this){ error("Failed to create Socket"); }
function Whatever::onConnectFailed(%this, %wsaError){ error(%wsaError); }
function Whatever::onConnected(%this){ echo("GOOD"); }
function Whatever::onDNSFailed(%this){ error("DNS Failed"); }
function Whatever::onDisconnect(%this){ echo("Disconnected"); }

function Whatever::onLine(%this, %line)
{
	echo(%line);
	%this.buffer = %this.buffer @ %line;
	
	//if (trim(%line) $= "")
	//	%this.altDisconnect();
}
