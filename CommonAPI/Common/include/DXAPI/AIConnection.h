#pragma once

#include <DXAPI/Move.h>
#include <DXAPI/GameConnection.h>

namespace DX
{
	class AIConnection : public GameConnection
	{
		public:
			AIConnection(unsigned int obj);

			//! Whether or not this bot should use custom moves.
			bool mUseCustomMoves;

			//! The custom move for this bot.
			Move mCustomMove;

			/**
			 *	@brief Computes the current move for this bot.
			 */
			void generateMove(void);
	};
} // End NameSpace DX
