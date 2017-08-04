#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include <time.h>

#include "json.hpp"

using json = nlohmann::json;

std::string input;
std::vector<std::pair<int, int>> edges;
std::vector<int> mines;
int playerId;
int playersNum;

void readMap(const json& map, std::vector<std::pair<int, int>>& edges, std::vector<int>& mines)
{
	// reserve and read all rives
	edges.reserve(map["rivers"].size());
	for (auto& edge : map["rivers"])
	{
		edges.emplace_back(edge["source"], edge["target"]);
	}

	// reserve and read all mines
	mines.reserve(map["mines"].size());
	for (auto& mine : map["mines"])
		mines.emplace_back(mine);
}

int main()
{
	srand(time(NULL));

	{
		// intruduction
		auto intro = json{
			{"me", "Makinami"}
		};
		std::cout << intro << std::endl;

		std::getline(std::cin, input);
		auto introRes = json::parse(input);
		if (intro["me"] != introRes["you"])
		{
			std::cerr << "Failed introduction!";
			return 1;
		}
	}

	{
		// get setup and parse
		std::getline(std::cin, input);
		std::cout << input;
		auto setup = json::parse(input);
		playerId = setup["punter"];
		playersNum = setup["punters"];
		readMap(setup["map"], edges, mines);

		std::cout << json{
			{"ready", playerId}
		} << std::endl;
	}

	json turn;
	while (true) {
		std::getline(std::cin, input);
		turn = json::parse(input);

		// if 'move' doesn't exist, then the game has finished and we write out the results
		if (turn["move"].is_null())
		{
			for (auto& punter : turn["stop"]["scores"])
				std::cerr << "Punter " << punter["punter"] << ": " << punter["score"] << std::endl;
			break;
		}

		// otherwise execute last turn's moves
		for (auto& move : turn["move"]["moves"])
		{
			if (move["pass"].is_null())
			{
				// create edge that was claimed
				auto edge = std::make_pair<int, int>(move["claim"]["source"], move["claim"]["target"]);
				auto toErase = std::find(std::cbegin(edges), std::cend(edges), edge);
				if (toErase != std::cend(edges))
					edges.erase(toErase);
				else
				{
					// if couldn't find then swap the ends and this time it must exist
					std::swap(edge.first, edge.second);
					toErase = std::find(std::cbegin(edges), std::cend(edges), edge);
					edges.erase(toErase);
				}
			}			
		}

		// choose the edge randomly and claimed it
		auto it = rand() % edges.size();
		std::cout << json{
			{"claim", {
				{"punter", playerId},
				{"source", edges[it].first},
				{"target", edges[it].second}
			}}
		} << std::endl;
	}

	return 0;

}