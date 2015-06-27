/*
*   CGameTime.cpp
*   Small singleton class that is used for some time-sensitive operations.
*   Copyright (c) 2013 Robert MacGregor
*/

#include <stdafx.h>
#include <CGameTime.h>

//! Root Game Namespace
namespace Game
{
        // This stores an instance of our CGameTime class that is returned at all times except when it is NULL, for which it is then created.
        CGameTime *time_instance = NULL;

        //! Default Constructor
        CGameTime::CGameTime(void) : time_current_seconds(0),       // The current sim time in seconds
                                         time_last_seconds(0),      // The last stored sim time in seconds
                                         time_delta_seconds(0),     // The delta between the current and last sim times in seconds
                                         time_current_clocks(0),    // The current sim time in clocks
                                         time_last_clocks(0),       // The last sim time in clocks
                                         time_delta_clocks(0)       // The delta between the current and last sim times in clocks
        {

        } // End Default Constructor

        //! Default Destructor
        CGameTime::~CGameTime(void)
        {

        } // End Default Destructor

        //! Gets the stored instance of CGameTime if it already exists
        CGameTime *CGameTime::getPointer(void)
        {
            if (time_instance == NULL)
                time_instance = new CGameTime();
            return time_instance;
        }

        //! Destroys the stored instance of CGameTime
        void CGameTime::destroy(void)
        {
            if (time_instance != NULL)
            {
                delete time_instance;
                time_instance = NULL;
            }
        }

        //! Updates all of the stored times on the instance of CGameTime
        void CGameTime::update(void)
        {
            time_last_clocks = time_current_clocks;
            time_current_clocks = clock();
            time_delta_clocks = time_current_clocks-time_last_clocks;

			// Conversions prevent you trying to divide an integer which would only return 0
            time_last_seconds = (float)time_last_clocks/CLOCKS_PER_SEC;
            time_current_seconds = (float)time_current_clocks/CLOCKS_PER_SEC;
            time_delta_seconds = (float)time_delta_clocks/CLOCKS_PER_SEC;
        }

        //! Returns the simtime in seconds
        float CGameTime::getSimTime(void)
        {
            return time_current_seconds;
        }

        //! Returns the delta sim time in seconds
        float CGameTime::getDelta(void)
        {
           return time_delta_seconds;
        }
} // End Namespace Game
