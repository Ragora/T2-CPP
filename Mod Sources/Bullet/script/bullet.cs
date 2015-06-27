//--------------------------------------------------------------------------------------------------------------------------
//	bullet.cs
//	Experimental Bullet implementation for Tribes 2 Construction.
//	Copyright (c) 2013 Robert MacGregor
//======================================================================


//---------------------------------------------------------------------------------------------------------------------------
//	Configuration Parameters
//======================================================================
$Bullet::PhysicsUpdateMS = 100;	// The delay in milliseconds before updating static shapes

//---------------------------------------------------------------------------------------------------------------------------
//	Geek code.
//======================================================================
function StaticShape::setMass(%this,%mass)
{
	if (%this.bullet_id $= "")
	{
		if (!bulletInitialized())
			bulletInitialize();

		%position = %this.getPosition();
		%size = %this.getRealSize();
		
		%id = bulletCreateCube(%mass, getWord(%size, 0)/2, 
										getWord(%size, 1)/2, 
									  	getWord(%size, 2)/2);
		%this.bullet_id = %id;
		%this.bullet_mass = %mass;
		bulletSetPosition(%id, getWord(%position, 0), 
							     getWord(%position, 1), 
							     getWord(%position,2));

		if (%mass == 0)
		{
			%this.setWorldBoxCenter(bulletGetPosition(%id));
			%this.setRotation(bulletGetRotation(%id));
		}
		else
		{
			$Bullet::ObjectArray[$Bullet::ObjectCount] = %this;
			$Bullet::ObjectCount++;
		}
	}
	else
	{
		if (%this.bullet_mass == 0 && %mass != 0)
		{
			$Bullet::ObjectArray[$Bullet::ObjectCount] = %this;
			$Bullet::ObjectCount++;
		}
		else if (%this.bullet_mass != 0 && %mass == 0)
		{
			for (%i = %this.bullet_id; %i < $Bullet::ObjectCount; %i++)
				$Bullet::ObjectArray[%i] = $Bullet::ObjectArray[%i+1];
			$Bullet::ObjectCount--;
		}

		%this.bullet_mass = %mass;
		bulletSetMass(%this.bullet_id, %mass);
	}

	return true;
}

function bulletPhysicsUpdate()
{		
	cancel($Bullet::ScheduleID);
	if (!bulletInitialized() || !$Bullet::Enabled)
	{
		$Bullet::ObjectCount = 0;
		return false;
	}

	bulletUpdate();
	for (%i = 0; %i < $Bullet::ObjectCount; %i++)
	{
		%object = $Bullet::ObjectArray[%i];
		%object.setWorldBoxCenter(bulletGetPosition(%object.bullet_id));

		%rotation = bulletGetRotation(%object.bullet_id);
		%object.setRotation(%rotation);
	}
	$Bullet::ScheduleID = schedule($Bullet::PhysicsUpdateMS,0,"bulletPhysicsUpdate");
	return true;
}

$Bullet::Enabled = false;
$Bullet::ScheduleID = 0;
$Bullet::ObjectCount = 0;

//---------------------------------------------------------------------------------------------------------------------------
//	Chat commands to mess with Bullet functionality.
//======================================================================
function ccTogglePhysics(%sender)
{
	if (!%sender.isSuperAdmin)
	{
		messageClient(%sender,'msgClient',"\c3You are not a super administrator.");
		return false;
	}
	
	$Bullet::Enabled = !$Bullet::Enabled;
	if ($Bullet::Enabled)
	{
		messageAll('msgAll', "\c3" @ %sender.namebase SPC "has enabled Bullet physics! ~wfx/misc/bounty_completed.wav");
		bulletPhysicsUpdate();
	}
	else
		messageAll('msgAll', "\c3" @ %sender.namebase SPC "has disabled Bullet physics! ~wfx/misc/bounty_completed.wav");
	return true;
}

function ccSetGravity(%sender, %args)
{
	if (!%sender.isSuperAdmin)
	{
		messageClient(%sender,'msgClient',"\c3You are not a super administrator.");
		return false;
	}
	
	%x = getWord(%args, 0);
	%y = getWord(%args, 1);
	%z = getWord(%args, 2);
	if (%x $= "" || %y $= "" || %z $= "")
	{
		messageClient(%sender,'msgClient',"\c3Usage: /setGravity left-right(X) forward-back(Y) up-down(Z)");
		return true;
	}
	
	bulletSetGravity(%x,%y,%z);
	messageAll('msgAll', "\c3" @ %sender.namebase SPC "has changed the Bullet gravity vector to (" @ %x @ "," @ %y @ "," @ %z @ ")! ~wfx/misc/bounty_completed.wav");
	return true;
}

function ccSetMass(%sender, %args)
{
	if (!%sender.isSuperAdmin)
	{
		messageClient(%sender,'msgClient',"\c3You are not a super administrator.");
		return false;
	}
	
	%mass = getWord(%args, 0);
	if (%mass $= "") %mass = 0;
	
	%pos = %sender.player.getMuzzlePoint($WeaponSlot);
	%vec = %sender.player.getMuzzleVector($WeaponSlot);
	%targetpos = vectorAdd(%pos,vectorscale(%vec,100));
	%obj = containerraycast(%pos,%targetpos,$Typemasks::StaticShapeObjectType,%sender.player);
	%obj = getword(%obj,0);
	
	if (!isObject(%obj))
	{
		messageClient(%sender,'msgClient',"\c3No object within 100 meters.");
		return false;
	}

	if (%obj.bullet_id $= "")
	{
		%obj.setMass(%mass);
		messageClient(%sender, 'msgClient',"\c3Object initialized with Bullet using a mass of " @ %mass @ ".");
	}
	else
	{
		%obj.setMass(%mass);
		messageClient(%sender, 'msgClient',"\c3Object now has a mass of " @ %mass @ ".");
	}
	return true;
}

function ccSyncPieces(%sender, %args)
{
	if (!%sender.isSuperAdmin)
	{
		messageClient(%sender,'msgClient',"\c3You are not a super administrator.");
		return false;
	}
	
	%mass = getWord(%args, 0);
	if (%mass $= "")
	{
		messageClient(%sender,'msgClient',"\c3Usage: /syncPieces <mass>");
		return false;
	}
	
	for (%i = 0; %i < Deployables.getCount(); %i++)
	{
		%piece = Deployables.getObject(%i);
		if(%piece.owner == %sender && %piece.bullet_id $="")
			%piece.setMass(%mass);
	}
	messageAll('msgAll',"\c3" @ %sender.namebase SPC "made their inactive pieces sync with Bullet! ~wfx/misc/bounty_completed.wav"); 
	return true;
}

function ccDesyncPiece(%sender)
{
	if (!%sender.isSuperAdmin)
	{
		messageClient(%sender,'msgClient',"\c3You are not a super administrator.");
		return false;
	}

	%pos = %sender.player.getMuzzlePoint($WeaponSlot);
	%vec = %sender.player.getMuzzleVector($WeaponSlot);
	%targetpos = vectorAdd(%pos,vectorscale(%vec,100));
	%obj = containerraycast(%pos,%targetpos,$Typemasks::StaticShapeObjectType,%sender.player);
	%obj = getword(%obj,0);

	if (!isObject(%obj))
	{
		messageClient(%sender,'msgClient',"\c3No object within 100 meters.");
		return false;
	}

	if (%obj.bullet_id $= "")
	{
		messageClient(%sender,'msgClient',"\c3That object is not physically bound.");
		return false;
	}
	
	%id = %obj.bullet_id;
	bulletDeleteObject(%id);

	for (%i = %id; %i < $Bullet::ObjectCount; %i++)
		$Bullet::ObjectArray[%i] = $Bullet::ObjectArray[%i+1];
	$Bullet::ObjectCount--;
	%object.bullet_id = "";
	
	messageClient(%sender, 'msgClient', "\c3Object has been desynchronized from Bullet.");
	return true;
}

function ccBulletHelp(%sender)
{
	if (!%sender.isSuperAdmin)
	{
		messageClient(%sender,'msgClient',"\c3You are not a super administrator.");
		return false;
	}

	messageClient(%sender, 'msgClient',"\c3/setMass <mass>, /togglePhysics, /setGravity <x> <y> <z>, /desyncPiece, /syncPieces");
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------
//	Callbacks (they don't do anything)
//======================================================================
function onBulletObjectSleep(%id)
{
}

function onBulletObjectWake(%id)
{
}


//--------------------------------------------------------------------------------------------------------------------------
//	Package Code
//======================================================================
package bulletPackage
{
	function disconnect()
	{
		parent::disconnect();
		if (bulletInitialized())
			bulletDeinitialize();
	}

	function quit()
	{
		if (bulletInitialized())
			bulletDeinitialize();
		parent::quit();
	}
};
if (!isActivePackage(bulletPackage))
	activatePackage(bulletPackage);
