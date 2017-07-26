#include "../include/Player_DeepQ.h"
#include "../include/Player.h"
#include "../include/Game.h"
#include "iostream"

using namespace SparCraft;
using namespace std;

//constructor, sets important private member variables
Player_DeepQ::Player_DeepQ(const IDType & playerID)
{
    _playerID = playerID;
}

void Player_DeepQ::getMoves(GameState & state, const MoveArray & moves, vector<Action> & moveVec)
{
  moveVec.clear();
  for (IDType u(0); u<moves.numUnits(); ++u)
  {
      moveVec.push_back(moves.getMove(u, rand() % (moves.numMoves(u))));
  }
}
