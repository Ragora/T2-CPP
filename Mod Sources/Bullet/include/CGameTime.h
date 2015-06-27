/*
 *	CGameTime.h
 *  Small singleton class that is used for some time-sensitive operations.
 *  Copyright (c) 2013 Robert MacGregor
*/

#ifndef _INCLUDE_CGAMETIME_H_
#define _INCLUDE_CGAMETIME_H_

#include <time.h>

//! Root Game Namespace
namespace Game
{
    //! Singleton class for timing functions in FBLA
    class CGameTime
    {
        public:
        //! Get the instance
        static CGameTime *getPointer(void);
        //! Destroys the instance
        static void destroy(void);

        //! Update Timer
        void update(void);
        //! Returns the simtime in seconds
        float getSimTime(void);
        //! Returns the delta time in seconds
        float getDelta(void);

        private:
        //! Default Constructor
        CGameTime(void);
        //! Default Destructor
        ~CGameTime(void);

        float time_current_seconds, time_last_seconds, time_delta_seconds;
        float time_current_clocks, time_last_clocks, time_delta_clocks;
    }; // End Class CGameTime
} // End Namespace Game

#endif // _INCLUDE_CGAMETIME_H_
