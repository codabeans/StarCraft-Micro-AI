#pragma once

#include "Common.h"
#include "Player.h"
#include <opencv2/core/core.hpp>
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
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "DeepQParameters.hpp"

#include <caffe/caffe.hpp>

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
    Player_DeepQ (const IDType & playerID, const DeepQParameters & params);
    IDType getType() { return PlayerModels::DeepQ; }
    void initializeNet();
    void prepareModelInput(const std::vector<Action> & moveVec);
    void wrapInputLayer(std::vector<cv::Mat>* input_channels);
    void preprocess(std::vector<cv::Mat>* input_channels);
    void loadActions();
    void forward();
    void getNetOutput();
    void getMoves(GameState & state, const MoveArray & moves, std::vector<Action> & moveVec);
    void selectRandomMoves(const MoveArray & moves, std::vector<Action> & moveVec);
    void selectBestMoves(const MoveArray & moves, std::vector<Action> & moveVec);
    void getReward(const GameState state);
    void setReward();
    void backward(const GameState state);
    void logDataPoint();

private:
    bool _logData;
    bool _notBeginning;
    DeepQParameters _params;
    int _frameNumber;
    cv::Mat _img;
    std::vector<std::vector<float> > _moves;
    float _predictedReward;
    float _actualReward;
    std::shared_ptr<caffe::Net<float> > _net;
    std::string _modelFile;
    std::string _weightFile;
  };
}