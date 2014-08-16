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
