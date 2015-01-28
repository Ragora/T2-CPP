// this is a test for katabatic only with both client and server running their scripts.
// this should be run after joining the game, or you will fall through the map on spawn where the Team1 base used to be.





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

function moveRoutineDone() {
    for (%x=0;%x<(MoveEffectSet.getCount()); %x++) {
        %obj=MoveEffectSet.getObject(%x);
        %obj.setPosition(VectorAdd(%obj.position,$moveoffset));
    }
    MoveEffectSet.clear();
    
}

memPatch("5BBBDC",getServPAddr()); //Hooks the serverProcess function
$mg = new SimSet(MoveGroup);
MoveGroup.add(Team1); //Adds the Team1 base to the movement group
setMPS(-10,0.0,0.0); // Moves the base 0.5 meters per second on the X axis.