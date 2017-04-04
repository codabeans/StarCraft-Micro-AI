#pragma once

#include "Common.h"
#include "Player.h"

namespace SparCraft
{
  /*----------------------------------------------------------------------
  | Deep Q learning
  |----------------------------------------------------------------------
  | Training:
  | 1) Feed a downscaled version of the game's frame as well as random actions
  | 2) Observe the reward
  | 3) Backprop based on observation
  | Running:
  | 1) Feed a downscaled version of the game's frame, as well as all (or a lot)
  |     of possible actions
  | 2) Select the action(s) that the network believes will give the highest
  |     reward.
  | 3) Perform said actions.
  | 4) ????
  | 5) Profit.
  `----------------------------------------------------------------------*/
  class Player_DeepQ : public Player
  {
  public:
    Player_DeepQ (const IDType & playerID);
    IDType getType() { return PlayerModels::DeepQ; }
    void getMoves(GameState & state, const MoveArray & moves, std::vector<Action> & moveVec);
  };
}
