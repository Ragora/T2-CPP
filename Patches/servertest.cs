// this is a test for katabatic only with both client and server running their scripts.
// this should be run after joining the game, or you will fall through the map on spawn where the Team1 base used to be.




loadMod("TSExtension");
$movesim = new SimSet(MoveEffectSet);
function onMoveRoutine(%obj, %offset, %center, %radius){
    
    InitContainerRadiusSearch(%obj.getWorldBoxCenter(),%radius,$TypeMasks::PlayerObjectType | $TypeMasks::VehicleObjectType);
	%obj2=ContainerSearchNext();
    
    while (%obj2) {
        if (!(MoveEffectSet.isMember(%obj2))) {
            MoveEffectSet.add(%obj2);
        }
        
        %obj2 = ContainerSearchNext();
    }
    
    $moveoffset=%offset;
}
$collidegroup = new SimSet(CollideGroup);
function DiscImage::onFire(%data, %obj, %slot)
{
     if(%obj.fireTimeoutDisc)  //-nite-
      return;
//   %data.lightStart = getSimTime();

//   if( %obj.station $= "" && %obj.isCloaked() )
//   {
//      if( %obj.respawnCloakThread !$= "" )
//      {
//         Cancel(%obj.respawnCloakThread);
//         %obj.setCloaked( false );
//         %obj.respawnCloakThread = "";
//      }
//      else
//      {
//         if( %obj.getEnergyLevel() > 20 )
//         {
//            %obj.setCloaked( false );
//            %obj.reCloak = %obj.schedule( 500, "setCloaked", true );
//         }
//      }
//   }

        %vehicle = 0;
        %weapon = %obj.getMountedImage(0).item;

        if(%obj.client.mode[%weapon] == 0)
        {
             %projectile = "DiscProjectile";
             %mode = "LinearProjectile";
             %obj.fireTimeoutDisc = 1000;  //should be the same as 1.25  -Nite-
             %enUse = 0;
             %ammoUse = 1;
        }                       //1250
        else if(%obj.client.mode[%weapon] == 1)
        {
             %projectile = "TurboDisc";
             %mode = "LinearProjectile";
             %obj.fireTimeoutDisc = 1250;   //should be the same as .25  -Nite-
             %enUse = 7;
             %ammoUse = 1;
        }                          //230
        else if(%obj.client.mode[%weapon] == 2)
        {
             %projectile = "PowerDiscProjectile";
             %mode = "LinearProjectile";
             %enUse = 15;
             %ammoUse = 2;
             %obj.fireTimeoutDisc = 1500; //should be same as 2.25  -Nite-
        }                                //2250

        if(%obj.powerRecirculator)
          %enUse *= 0.75;
          
     if(%ammoUse > %obj.getInventory(%data.ammo) || %enUse > %obj.getEnergyLevel())
     {
          schedule(%obj.fireTimeoutDisc, 0, "DiscFireTimeoutClear", %obj);
          return;
     }

     %p = new(%mode)()
     {
         dataBlock        = %projectile;
         initialDirection = %obj.getMuzzleVector(%slot);
         initialPosition  = %obj.getMuzzlePoint(%slot);
         sourceObject     = %obj;
         sourceSlot       = %slot;
         vehicleObject    = %vehicle;
      };
	CollideGroup.add(%p);
   %useEnergyObj = %obj.getObjectMount();

   if(!%useEnergyObj)
        %useEnergyObj = %obj;

   %vehicle = %useEnergyObj.getType() & $TypeMasks::VehicleObjectType ? %useEnergyObj : 0;

   // Vehicle Damage Modifier
   if(%vehicle)
     %p.vehicleMod = %vehicle.damageMod;

   if(%obj.damageMod)
     %p.damageMod = %obj.damageMod;
     
   if (isObject(%obj.lastProjectile) && %obj.deleteLastProjectile)
      %obj.lastProjectile.delete();

   %obj.lastProjectile = %p;
   %obj.deleteLastProjectile = %data.deleteLastProjectile;
   MissionCleanup.add(%p);
   // all this for emulating state stuff -Nite-
   %obj.play3D(DiscFireSound);// play fire sounds -Nite-
   if( %obj.getState() !$= "Dead" )				// +soph
      %obj.setActionThread("light_recoil");  //Play anim recoil  -Nite-
   schedule(500, %obj.play3D(DiscReloadSound),%obj);//play reload sound 500ms after we fire  -Nite-
   schedule(%obj.fireTimeoutDisc, 0, "DiscFireTimeoutClear", %obj); //Use fire time out for each mode   -Nite-
  // serverCmdPlayAnim(%obj.client,"light_recoil"); //this worked too lol  -Nite-

   // AI hook
   if(%obj.client)
      %obj.client.projectile = %p;

   %obj.decInventory(%data.ammo, %ammoUse);
   %obj.useEnergy(%enUse);
   
   if(%obj.client.mode[%weapon] == 1)
      %obj.applyKick(-500);
      
   if(%obj.client.mode[%weapon] == 2)
      %obj.applyKick(-1000);
}




function ProjCollisionCallback(%proj1, %proj2) {
	echo(%proj1 SPC "collided with" SPC %proj2);
	%proj1.delete();
	%proj2.delete();
}
function moveRoutineDone() {
    for (%x=0;%x<(MoveEffectSet.getCount()); %x++) {
        %obj=MoveEffectSet.getObject(%x);
	  if(%obj.getType() | $TypeMasks::PlayerObjectType) {
	        if (%obj.isjetting() || %obj.isjumping() ) { 
	           echo ("Artificial Gravity Disengaged for" SPC %obj);
		  } else {
                 %obj.setPosition(VectorAdd(%obj.position,$moveoffset));
		  }
	  }
    }
    MoveEffectSet.clear();
    
}

memPatch("5BBBDC",getServPAddr()); //Hooks the serverProcess function
$mg = new SimSet(MoveGroup);
MoveGroup.add(Team1); //Adds the Team1 base to the movement group
setMPS(0.0,0.0,0.0); // Moves the base 0.5 meters per second on the X axis.