#include "Player_DeepQ.h"
#include <fstream>
#include <iostream>
#include <opencv2/highgui/highgui.hpp>

using namespace SparCraft;
using namespace cv;
using namespace caffe;
using namespace std;

Player_DeepQ::Player_DeepQ (const IDType & playerID)
{
    _playerID = playerID;
    _init = false;
    _frameNumber = 0;
}

void Player_DeepQ::init(GameState state)
{
    getMapDims(state);
    makeNet(state);
    initializeNet();
    observeState(state);
    _init = true;
}

void Player_DeepQ::getMapDims(GameState state)
{
    //Pixel dimensions when no map is specifies
    _currState._dimX = 1280;
    _currState._dimY = 704;

    //TODO: dynamically pull the arena size
    //_currState._dimX = state.getMap()->getWalkTileWidth();
    //_currState._dimY = state.getMap()->getWalkTileHeight();
}
void Player_DeepQ::observeState(GameState state)
{
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

// Get the total number of controllable units from GameState and use that to
// set the number of outputs of the network
void Player_DeepQ::makeNet(GameState state)
{
    int units = getControllableUnits(state);
    string fileName = "../models/" + to_string(units) + ".prototxt";
    _modelFile = fileName;

    ofstream modelFile(fileName);
    ifstream modelArchitectureFile("../models/example.prototxt");

    string strReplace = "  input_param { shape: { dim: X dim: X dim: X dim: X } }";
    string strNew = "  input_param { shape: { dim: 1 dim: 3 dim: " + to_string(_currState._dimX) +" dim: " + to_string(_currState._dimY) + " } }";

    if(!modelFile.is_open() || !modelArchitectureFile.is_open())
    {
        cout << "Error opening file(s)!" << endl;
        return;
    }
    string strTemp;
    while(std::getline(modelArchitectureFile, strTemp))
    {
        if(strTemp == strReplace)
        {
            strTemp = strNew;
        }
        strTemp += "\n";
        modelFile << strTemp;
    }
}

int Player_DeepQ::getControllableUnits(GameState state)
{
    IDType friendly(_playerID);
    int number_of_controllable_units = 0;
    for (IDType u(0); u<state.numUnits(friendly); ++u)
    {
        number_of_controllable_units++;
    }

    return number_of_controllable_units;
}

void Player_DeepQ::initializeNet()
{
    _net.reset(new Net<float>(_modelFile, TRAIN));
}

int moveInt(IDType moveType)
{
    if (moveType == ActionTypes::ATTACK)
    {
        return 10;
    }
    else if (moveType == ActionTypes::MOVE)
    {
        return 20;
    }
    else if (moveType == ActionTypes::RELOAD)
    {
        return 30;
    }
    else if (moveType == ActionTypes::PASS)
    {
        return 40;
    }
    else if (moveType == ActionTypes::HEAL)
    {
        return 50;
    }

    return 0;
}

void Player_DeepQ::prepareModelInput(vector<Action> & moveVec)
{
    for(int i = 0; i < moveVec.size(); i++)
    {
        Action move = moveVec[i];
        cout << moveInt(move.type()) << endl;
    }
    _img = Mat::zeros(_currState._dimY, _currState._dimX, CV_8UC(3));
    for(uint i = 0; i < _currState._friendlyID.size(); i++)
    {
        Action move = moveVec[0];
        int moveID = -1;
        for(int j = 0; j < moveVec.size(); j++)
        {
            Action move = moveVec[j];
            if(_currState._friendlyID.at(i) == int(move.unit()))
            {
                moveID = j;
            }
        }
        if(moveID != -1)
        {
            move = moveVec[moveID];
            Vec4b color(_currState._friendlyID.at(i), _currState._friendlyHpRemaining.at(i), _currState._friendlyEnergy.at(i), moveInt(move.type()), move.pos().x(), move.pos().y());
            _img.at<Vec4b>(Point(_currState._friendlyPos.at(i).x() - 200, _currState._friendlyPos.at(i).y() - 200)) = color;
        }
        else
        {
            cout << "no match found :(" << endl;
        }
    }

    for(uint i = 0; i < _currState._enemyID.size(); i++)
    {
        Vec4b color(_currState._enemyID.at(i), _currState._enemyHpRemaining.at(i), _currState._enemyEnergy.at(i), 200);
        _img.at<Vec4b>(Point(_currState._enemyPos.at(i).x() - 200, _currState._enemyPos.at(i).y() - 200)) = color;
    }
}

void Player_DeepQ::wrapInputLayer(vector<Mat>* input_channels) {
    Blob<float>* input_layer = _net->input_blobs()[0];

    int width = input_layer->width();
    int height = input_layer->height();
    float* input_data = input_layer->mutable_cpu_data();
    for (int i = 0; i < input_layer->channels(); ++i) {
        Mat channel(height, width, CV_32FC1, input_data);
        input_channels->push_back(channel);
        input_data += width * height;
    }
}

void Player_DeepQ::preprocess(const Mat& img, vector<Mat>* input_channels) {
    /* Convert the input image to the input image format of the network. */
    Mat sample = img;
    Mat sample_float;
    sample.convertTo(sample_float, CV_32FC3);

    /* This operation will write the separate BGR planes directly to the
    * input layer of the network because it is wrapped by the cv::Mat
    * objects in input_channels. */
    split(sample_float, *input_channels);
}

void Player_DeepQ::forward()
{
    Blob<float>* input_layer = _net->input_blobs()[0];

    vector<Mat> input_channels;
    wrapInputLayer(&input_channels);

    preprocess(_img, &input_channels);

    _net->Forward();
}

void Player_DeepQ::getNetOutput()
{
    Blob<float>* output_layer = _net->output_blobs()[0];
    float output;;
    for(int i = 0; i < 1; i++)
    {
        output = output_layer->cpu_data()[i];
    }
    _reward =  output;
}

void Player_DeepQ::getMoves(GameState & state, const MoveArray & moves, vector<Action> & moveVec)
{
    if(!_init)
    {
        init(state);
    }
    if(_frameNumber == 0)
    {
        backward(state);

        observeState(state);

        forward();
        selectRandomMoves(moves, moveVec);
        prepareModelInput(moveVec);
        forward();
    }
    else {
        _frameNumber ++;
        if(_frameNumber > 4){
            _frameNumber == 0;
        }
    }
}

void Player_DeepQ::selectRandomMoves(const MoveArray & moves, std::vector<Action> & moveVec)
{
    moveVec.clear();
    for (IDType u(0); u<moves.numUnits(); ++u)
    {
        moveVec.push_back(moves.getMove(u, rand() % (moves.numMoves(u))));
    }
}
void Player_DeepQ::selectBestMoves(const MoveArray & moves, std::vector<Action> & moveVec)
{

}

void Player_DeepQ::getReward(GameState state)
{
    _futureReward = state.evalLTD2(_playerID);
}

void Player_DeepQ::backward(GameState state)
{
    getReward(state);
    getNetOutput();
    cout << "Reward: " << _reward << endl;
    cout << "Future Reward: " << _futureReward << endl;
}
