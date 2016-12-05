#pragma once

#include "Common.h"
#include "Player.h"
#include <opencv2/core/core.hpp>
#include <vector>
#include <memory>
#include <algorithm>
#include <iosfwd>
#include <string>
#include <utility>

#include <caffe/caffe.hpp>

namespace SparCraft
{

  struct MapState
  {
    int _dimX;
    int _dimY;

    std::vector<int> _friendlyID;
    std::vector<Position> _friendlyPos;
    std::vector<int> _friendlyHpRemaining;
    std::vector<int> _friendlyEnergy;

    std::vector<int> _enemyID;
    std::vector<Position> _enemyPos;
    std::vector<int> _enemyHpRemaining;
    std::vector<int> _enemyEnergy;
  };

  /*----------------------------------------------------------------------
  | Deep Q learning
  |----------------------------------------------------------------------
  | 1) Feed the game state into a Convolutional neural network:
  | 2) Do a forward pass of the CNN
  | 3) Execute the moves the CNN outputs
  | 4) Observe the game
  | 5) Backprop based on observation
  `----------------------------------------------------------------------*/
  class Player_DeepQ : public Player
  {
  public:
    Player_DeepQ (const IDType & playerID);
    IDType getType() { return PlayerModels::DeepQ; }
    void init(GameState state);
    void getMapDims(GameState state);
    void observeState(GameState state);
    int getControllableUnits(GameState state);
    void makeNet(GameState state);
    void initializeNet();
    void prepareModelInput(std::vector<Action> & moveVec);
    void wrapInputLayer(std::vector<cv::Mat>* input_channels);
    void preprocess(const cv::Mat& img,std::vector<cv::Mat>* input_channels);
    void forward();
    void getNetOutput();
    void getMoves(GameState & state, const MoveArray & moves, std::vector<Action> & moveVec);
    void selectRandomMoves(const MoveArray & moves, std::vector<Action> & moveVec);
    void selectBestMoves(const MoveArray & moves, std::vector<Action> & moveVec);
    void getReward(GameState state);
    void backward(GameState state);

private:
    bool _init;
    int _frameNumber;
    MapState _currState;
    cv::Mat _img;
    float _reward;
    float _futureReward;
    std::shared_ptr<caffe::Net<float> > _net;
    std::string _modelFile;
  };
}
