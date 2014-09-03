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

package TSExtension
{
	function LoginDlg::onWake(%this)
	{
		parent::onWake(%this);
		
		if (!$TSExtension::isActive)
		{
			loadMod("TSExtension");
			schedule(2000,0,"tsExtensionLoop");
		}
	}
	
	function GameGUI::onWake(%this)
	{
		parent::onWake(%this);
		
		if (!$TSExtension::isActive)
		{
			//loadMod("TSExtension");
			//schedule(2000,0,"tsExtensionLoop");
		}
	}
};

if (!isActivePackage(TSExtension))
	activatePackage(TSExtension);
	
memPatch("005BD190","C3");
memPatch("005BD1F0","C3");
memPatch("005BD210","C3");
