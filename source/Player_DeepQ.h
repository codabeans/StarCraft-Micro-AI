#pragma once

#include "Common.h"
#include "Player.h"
#include <opencv2/core/core.hpp>
#include <vector>

//#include <caffe/caffe.hpp>

//Caffe::set_Mode(Caffe::GPU);

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

    void observeState(GameState state);
    void prepareModelInput();
    void forward();
    void getMoves();
    void computeLoss(GameState state);
    void backward();

private:
    MapState _currState;
    cv::Mat _Img;
    float _loss;
    //Net<float> caffe_train_net('/home/innovation/Documents/caffe/models/bvlc_alexnet/train_val.prototxt');
};
}
