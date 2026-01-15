#pragma once
#include "GameService.h"
#include "GameProtocol.pb.h"
#include <string>
#include <vector>
#include "Game.h"

class BinaryGameService
{
public:
	struct ActionResult {
		bool success;
		std::string message;
		bool isEndTurn;
	};

	static ActionResult ProcessPlayerAction(Game& game, int userId, const std::string& binaryData);
	static std::vector<std::pair<int, std::string>> PrepareBroadcastMessages(Game& game);
};



