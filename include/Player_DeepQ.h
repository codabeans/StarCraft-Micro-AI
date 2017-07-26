#pragma once

#include "Common.h"
#include "Player.h"
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <memory>
#include <algorithm>
#include <iosfwd>
#include <string>
#include <utility>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <boost/filesystem.hpp>
#include "DeepQParameters.hpp"

#include <caffe/caffe.hpp>

namespace SparCraft
{
  /*----------------------------------------------------------------------
  | Deep Q learning
  `----------------------------------------------------------------------*/
  class Player_DeepQ : public Player
  {
  public:
    Player_DeepQ (const IDType & playerID);
    Player_DeepQ (const IDType & playerID, const DeepQParameters & params);
    IDType getType() { return PlayerModels::DeepQ; }
    void initializeNet();
    void prepareModelInput(const std::vector<Action> & moveVec);
    void loadActions();
    void forward();
    void getNetOutput();
    void getMoves(GameState & state, const MoveArray & moves, std::vector<Action> & moveVec);
    void selectRandomMoves(const MoveArray & moves, std::vector<Action> & moveVec);
    void selectBestMoves(const MoveArray & moves, std::vector<Action> & moveVec);
    void getReward(const GameState state);
    void setReward();
    void backward(const GameState state);

private:
    bool _logData;
    bool _notBeginning;
    DeepQParameters _params;
    int _frameNumber;
    std::vector<std::vector<float> > _moves;
    float _predictedReward;
    float _actualReward;
    std::shared_ptr<caffe::Net<float> > _net;
    std::string _modelFile;
    std::string _weightFile;
  };
}
