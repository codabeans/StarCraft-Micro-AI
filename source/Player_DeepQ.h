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
  /*----------------------------------------------------------------------
  | Deep Q learning
  |----------------------------------------------------------------------
  | Training:
  | 1) Feed a downscaled version of the game's frmae as well as random actions
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
    void initializeNet();
    void prepareModelInput(std::vector<Action> & moveVec);
    void wrapInputLayer(std::vector<cv::Mat>* input_channels);
    void preprocess(std::vector<cv::Mat>* input_channels);
    void loadActions();
    void forward();
    void getNetOutput();
    void getMoves(GameState & state, const MoveArray & moves, std::vector<Action> & moveVec);
    void selectRandomMoves(const MoveArray & moves, std::vector<Action> & moveVec);
    void selectBestMoves(const MoveArray & moves, std::vector<Action> & moveVec);
    void getReward(GameState state);
    void setReward();
    void backward(GameState state);

private:
    bool _notBeginning;
    int _frameNumber;
    cv::Mat _img;
    std::vector<std::vector<std::vector<std::vector<float> > > > _moves;
    float _reward;
    float _futureReward;
    std::shared_ptr<caffe::Net<float> > _net;
    std::string _modelFile;
    std::string _weightFile;
  };
}
