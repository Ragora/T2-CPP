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

$ccd = new SimSet(ClientCommandGroup);




function ProjCollisionCallback(%proj1, %proj2) {
	//echo(%proj1 SPC "collided with" SPC %proj2);
	if (isObject(%proj1)) {
		if (isObject(%proj2)) {
			//if (%proj1.sourceObject != %proj2.sourceObject) {
				if (CollideGroup.isMember(%proj2)) {
				if (CollideGroup.isMember(%proj1)) {
				CollideGroup.remove(%proj1);
				CollideGroup.remove(%proj2);
				echo("Radius Explosion");
				RadiusExplosion(%proj1,%proj1.position,400,0,1000,%proj1.sourceObject,$DamageType::Default);
				RadiusExplosion(%proj2,%proj2.position,400,0,1000,%proj2.sourceObject,$DamageType::Default);
				%proj1.delete();
				%proj2.delete();
				}
				}
			//}
		}
	}
}
function moveRoutineDone() {
    for (%x=0;%x<(MoveEffectSet.getCount()); %x++) {
        %obj=MoveEffectSet.getObject(%x);
	  if(%obj.getType() | $TypeMasks::PlayerObjectType) {
	        if (%obj.isjetting() || %obj.isjumping() ) { 
	           //echo ("Artificial Gravity Disengaged for" SPC %obj);
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