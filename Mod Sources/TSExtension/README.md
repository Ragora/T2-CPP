TSExtension
=====

A general extension to the Torque Scripting language.

Additions
====

#### GrenadeProjectile.getVelocity()
This function did not exist before in Torque Script. It returns the current velocity vector of a GrenadeProjectile.

#### GrenadeProjectile.getPosition()
This function already existed to return the position of a GrenadeProjectile object but it is now much more accurate.

#### Player.isJetting()
This function did not exist before in Torque Script, it returns whether or not the Player is currently jetting.

#### Player.isJumping()
This function did not exist before in Torque Script, it returns whether or not the Player is currently jumping.

#### sprintf(format, ...)
This function did not exist before in Torque Script. It returns a formatted string according to format with all variable
arguments up to a total of twenty filled in. Refer to C's sprintf for more information.

Note: Only %s should be used in the format as Torque Script passes all data around as strings.

#### reSearch(pattern, target)
Searches for the occurrences of a regular expression pattern within the target text.

Ex: echo(reSearch("[0-9]+", "ABC123DEF"));

#### reMatch(pattern, target)
Attempts to match the entire target string to the input regular expression pattern.

Ex: echo(reMatch("[A-z]+", "ABC")); 

#### reReplace(pattern, target, replace)
Replaces the regular expression pattern within the target text with some given replace text.

Ex: echo(reReplace("[0-9]", "123|456|789|12345678111111", "*"));

#### reIterBegin(pattern, target)
Begins a regular expression iterator through the target string, matching the input regular expression pattern. Use reIterNext to find all matching patterns and reIterEnd to determine if the iterator has ended.

Ex: echo(reIterBegin("[A-z]+\\|?", "ONE|TWO|THREE|FOUR|FIVE"));

#### reIterNext()
Returns the next matching pattern in the input text specified in reIterBegin.

Ex: %match = reIterNext();

#### reIterEnd()
Returns true when the regular expression iterator has ended.

Ex: while(!reIterEnd())
