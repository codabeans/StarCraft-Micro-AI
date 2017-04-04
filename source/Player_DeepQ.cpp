#include "../include/Player_DeepQ.h"
#include "../include/sparcraft.pb.h"
#include "../include/Player.h"
#include "../include/Game.h"
#include "iostream"

using namespace SparCraft;
using namespace std;

//constructor, sets important private member variables
Player_DeepQ::Player_DeepQ(const IDType & playerID, const DeepQParameters & params)
{
    _playerID = playerID;
    _params = params;

    // Verify that the version of the library that we linked against is
    // compatible with the version of the headers we compiled against.
    GOOGLE_PROTOBUF_VERIFY_VERSION;
}

void setFeatures(sparcraft::features* unit, Unit u)
{
    unit->set_id(u.ID());
    unit->set_hp(u.currentHP());
    unit->set_posx(u.x());
    unit->set_posy(u.y());
}

void Player_DeepQ::getMoves(GameState & state, const MoveArray & moves, vector<Action> & moveVec)
{

  sparcraft::Army units;

  // Read the existing address book.
  fstream input("../gamestate.txt", ios::in | ios::binary);

  for (IDType p(0); p<Constants::Num_Players; ++p)
  {
      for (IDType u(0); u<state.numUnits(p); ++u)
      {
          setFeatures(units.add_unit(), state.getUnit(p, u));
      }
  }

  // Write the new address book back to disk.
  fstream output("../gamestate.txt", ios::out | ios::trunc | ios::binary);
  units.SerializeToOstream(&output);


  moveVec.clear();
  for (IDType u(0); u<moves.numUnits(); ++u)
  {
      moveVec.push_back(moves.getMove(u, rand() % (moves.numMoves(u))));
  }
}
