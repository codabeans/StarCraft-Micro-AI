#include "Player_DeepQ.h"
#include <string>

using namespace SparCraft;
using namespace cv;
using namespace caffe;

std::string model = "/home/faust/Documents/starcraft-ai/sparcraft/models/train_val.prototxt";

Player_DeepQ::Player_DeepQ (const IDType & playerID)
{
    _playerID = playerID;
}

void Player_DeepQ::observeState(GameState state)
{
    _currState._dimX = state.getMap()->getPixelWidth();
    _currState._dimY = state.getMap()->getPixelHeight();

    //get the id, location and health of every allied unit on the map

    IDType friendly(_playerID);

    for (IDType u(0); u<state.numUnits(friendly); ++u)
    {
        _currState._friendlyID.push_back(u);
        _currState._friendlyPos.push_back(state.getUnit(friendly, u).currentPosition(state.getTime()));
        _currState._friendlyHpRemaining.push_back(state.getUnit(friendly,u).currentHP());
        _currState._friendlyEnergy.push_back(state.getUnit(friendly,u).currentEnergy());
    }

    //get the  id, location and health of every enemy unit on the map

    IDType enemy(state.getEnemy(_playerID));

    for (IDType u(0); u<state.numUnits(enemy); ++u)
    {
        _currState._enemyID.push_back(u);
        _currState._enemyPos.push_back(state.getUnit(enemy, u).currentPosition(state.getTime()));
        _currState._enemyHpRemaining.push_back(state.getUnit(enemy,u).currentHP());
        _currState._enemyEnergy.push_back(state.getUnit(enemy,u).currentEnergy());
    }

}

void Player_DeepQ::initializeNet(string model_file)
{
//  _net =  Net<float>(model_file, caffe::TRAIN);
}

void Player_DeepQ::prepareModelInput()
{
    _Img = Mat::zeros(_currState._dimX, _currState._dimY, CV_8UC(3));
    for(uint i = 0; i < _currState._friendlyID.size(); i++)
    {
        Vec3b color(_currState._friendlyID.at(i), _currState._friendlyHpRemaining.at(i), _currState._friendlyEnergy.at(i));
        _Img.at<Vec3b>(Point(_currState._friendlyPos.at(i).x(), _currState._friendlyPos.at(i).y())) = color;
    }

    for(uint i = 0; i < _currState._enemyID.size(); i++)
    {
        Vec3b color(_currState._enemyID.at(i), _currState._enemyHpRemaining.at(i), _currState._enemyEnergy.at(i));
        _Img.at<Vec3b>(Point(_currState._enemyPos.at(i).x(), _currState._enemyPos.at(i).y())) = color;
    }
}

void Player_DeepQ::forward()
{

}

void Player_DeepQ::getMoves()
{

}

void Player_DeepQ::computeLoss(GameState state)
{
    _loss = state.evalLTD2(_playerID);
}

void Player_DeepQ::backward()
{

}
