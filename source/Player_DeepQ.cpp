#include "Player_DeepQ.h"
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"

using namespace SparCraft;
using namespace cv;
using namespace caffe;
using namespace std;

Player_DeepQ::Player_DeepQ (const IDType & playerID)
{
    _playerID = playerID;
    _frameNumber = 0;
    _notBeginning = false;
    _modelFile = "../models/model.prototxt";
    _weightFile = "../models/weights.prototxt";
    initializeNet();
    Caffe::set_mode(Caffe::GPU);
}

void Player_DeepQ::observeState(GameState state)
{
    //get the id and location of every allied unit on the map

    IDType friendly(_playerID);
    for (IDType u(0); u<state.numUnits(friendly); ++u)
    {
        _currState._friendlyID.push_back(u);
        _currState._friendlyPos.push_back(state.getUnit(friendly, u).currentPosition(state.getTime()));
    }

    //get the  id, location and health of every enemy unit on the map

    IDType enemy(state.getEnemy(_playerID));
    for (IDType u(0); u<state.numUnits(enemy); ++u)
    {
        _currState._enemyID.push_back(u);
        _currState._enemyPos.push_back(state.getUnit(enemy, u).currentPosition(state.getTime()));
    }
}

void Player_DeepQ::initializeNet()
{
    //Load the architecture from _modelFile, and init for TRAIN
    _net.reset(new Net<float>(_modelFile, TRAIN));

    //Copy weights from a previously trained net of the same architecture
    //Don't have said file yet, soon!
    //_net->CopyTrainedLayersFrom(_weightFile);
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
    _img = imread("/home/faust/Documents/starcraft-ai/deepcraft/bin/frame.bmp", CV_LOAD_IMAGE_COLOR);
    flip(_img, _img, -1);
    flip(_img, _img, 1);
    resize(_img, _img, Size(320,240));
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
    if(_frameNumber == 0)
    {
        backward(state);

        observeState(state);
        if( _notBeginning)
            saveDataPoint();
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

string randomString(size_t length)
{
    auto randchar = []() -> char
        {
            const char charset[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
            const size_t max_index = (sizeof(charset) - 1);
            return charset[ rand() % max_index ];
        };
    std::string str(length,0);
    std::generate_n( str.begin(), length, randchar );
    return str;
}

//Save the previous image and the corresponding reward
void Player_DeepQ::saveDataPoint()
{
    //TODO: THIS
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
    //TODO: THIS
}

void Player_DeepQ::getReward(GameState state)
{
    _futureReward = state.evalLTD2(_playerID);
}

void Player_DeepQ::backward(GameState state)
{
    getReward(state);
    getNetOutput();
    if(_futureReward != 0)
    {
         _notBeginning = true;
    }
}
