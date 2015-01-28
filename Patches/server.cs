// this is a test for katabatic only with both client and server running their scripts.
// this should be run after joining the game, or you will fall through the map on spawn where the Team1 base used to be.






memPatch("5BBBDC",getServPAddr()); //Hooks the serverProcess function
$mg = new SimSet(MoveGroup);
MoveGroup.add(Team1); //Adds the Team1 base to the movement group
setMPS(0.5); // Moves the base 0.5 meters per second on the X axis.