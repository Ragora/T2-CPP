WatchDog
=====

A dog that watches your game. In essence, all it does is spawns a secondary thread that awaits to be "petted" by the main thread from its ServerProcess method. In the event that the main thread takes too long to pet the dog (by default, around 8 seconds), the entire process is terminated by the watchdog thread. Tribes 2 is apparently prone to a lot of lockups, many with no apparent cause. This Watchdog makes it possible for the game to detect that it has been caught in an infinite loop somewhere and could potentially be modified to take corrective action.
