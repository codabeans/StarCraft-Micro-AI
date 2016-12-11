#include "Player_DeepQ.h"
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <boost/filesystem.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"

using namespace SparCraft;
using namespace cv;
using namespace caffe;
using namespace std;

//constructor, sets important private member variables
//as well as constructs our caffe network, if it can
Player_DeepQ::Player_DeepQ (const IDType & playerID)
{
    _playerID = playerID;
    _frameNumber = 0;
    _logData = true;
    _notBeginning = false;
    _solverFile = "../models/solver.prototxt";
    _weightFile = "../models/weights.prototxt";

    initializeNet();
    Caffe::set_mode(Caffe::GPU);

    //create the directory for logging data
    boost::filesystem::create_directory("../data");
    boost::filesystem::create_directory("../data/images");
}

//check if the inputted string exists as a file
bool fileExists(string file){
    std::ifstream ifile(file.c_str());
    return (bool)ifile;
}

//To be ran at the initialization of the Player
//Loads the CNN as well as the weights (if there are any to load)
void Player_DeepQ::initializeNet()
{
    //TODO: Supress Caffe's output for initializing the net

    //Load the architecture from _solverFile, and init for TRAIN
    //If the file isn't found, give a fatalerror as the player would
    //be unable to play :'( feelsbadman.jpg
    //caffe will give its own error(s) if something is wrong with the files
    if(fileExists(_solverFile))
    {
        caffe::SolverParameter solver_param;
        caffe::ReadProtoFromTextFileOrDie(_solverFile, &solver_param);
        _solver.reset(caffe::SolverRegistry<float>::CreateSolver(solver_param));
    }
    else
        System::FatalError("Problem Opening Solver file");
    //Copy weights from a previously trained net of the same architecture
    //Don't have said file yet, soon!
    if(fileExists(_weightFile))
        _solver->net()->CopyTrainedLayersFrom(_weightFile);
}

//convert the ENUM ActionTypes to int values so a NN can hopefully make sense of them
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
    else
        return 0;
}

//Populate _img with the game's frame, as well as load actions in _moves
void Player_DeepQ::prepareModelInput(vector<Action> & moveVec)
{
    //get the frame, store it in _img
    _img = imread("frame.bmp", CV_LOAD_IMAGE_COLOR);

    //clear _moves from the previous turn
    _moves.clear();

    //Then load the contents in moveVec into _moves (a vector<vector<int> >)
    // ID, Action, Move X, Move Y, Move index (attackee, healee, etc.)
    for(int i = 0; i < moveVec.size(); i++)
    {
        Action move = moveVec[i];
        vector<float> unitMove{float(move.unit()), float(moveInt(move.type())), float(move.pos().x()), float(move.pos().y()), float(move.index())};
        _moves.push_back(unitMove);
    }
    vector<float> emptyMove{0,0,0,0,0};
    for(int i = 0; i < 10-moveVec.size(); i++) {
         _moves.push_back(emptyMove);
     }
}

void Player_DeepQ::wrapInputLayer(vector<Mat>* input_channels)
{
    Blob<float>* input_layer = _solver->net()->input_blobs()[0];

    int width = input_layer->width();
    int height = input_layer->height();
    float* input_data = input_layer->mutable_cpu_data();
    for (int i = 0; i < input_layer->channels(); ++i) {
        Mat channel(height, width, CV_8UC1, input_data);
        input_channels->push_back(channel);
        input_data += width * height;
    }
}

void Player_DeepQ::preprocess(vector<Mat>* input_channels)
{
    //make the image upright
    //so it's coordinate system is the same oreintation as the game
    flip(_img, _img, -1);
    flip(_img, _img, 1);
    //downscale the hell out of it (1200x720 -> 320x240)
    resize(_img, _img, Size(320,240));

    /* This operation will write the separate BGR planes directly to the
     * input layer of the network because it is wrapped by the cv::Mat
     * objects in input_channels. */
    cv::split(_img, *input_channels);

    for(int i=0; i < 3; i++)
    {
        input_channels->at(i).convertTo(input_channels->at(i), CV_32FC1);
    }
}

//Load a set of actions into the network
void Player_DeepQ::loadActions()
{
    //caffe stores the data as a long array of floats stored in a pointer
    Blob<float>* action_layer = _solver->net()->input_blobs()[1];
    float* data = action_layer->mutable_cpu_data();
    for(int i = 0; i < 10; i++)
    {
        for(int j = 0; j < 5; j++)
        {
            data[i*5+j] = _moves[i][j];
        }
    }
}

void Player_DeepQ::forward()
{
    vector<Mat> input_channels;
    wrapInputLayer(&input_channels);
    preprocess(&input_channels);

    loadActions();

    _solver->net()->Forward();
}

void Player_DeepQ::getNetOutput()
{
    boost::shared_ptr<Blob<float> > output_layer = _solver->net()->blob_by_name("reward");
    float output;
    for(int i = 0; i < 1; i++)
    {
        output = output_layer->cpu_data()[i];
    }
    _predictedReward =  output;
}

void Player_DeepQ::getMoves(GameState & state, const MoveArray & moves, vector<Action> & moveVec)
{
    if(_frameNumber == 0)
    {
        backward(state);
        if(rand() % 10 + 1 < 7)
            selectRandomMoves(moves, moveVec);
        else
            selectBestMoves(moves, moveVec);
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

//Select random moves to be used as a learning experience for the network
void Player_DeepQ::selectRandomMoves(const MoveArray & moves, std::vector<Action> & moveVec)
{
    moveVec.clear();
    for (IDType u(0); u<moves.numUnits(); ++u)
    {
        moveVec.push_back(moves.getMove(u, rand() % (moves.numMoves(u))));
    }
}

//Feed all (or at least a lot) of move sets into the network, and keep the one
//that the network thinks is best
void Player_DeepQ::selectBestMoves(const MoveArray & moves, std::vector<Action> & moveVec)
{
    //TODO: THIS

    //dummy code for now
    moveVec.clear();
    for (IDType u(0); u<moves.numUnits(); ++u)
    {
        moveVec.push_back(moves.getMove(u, rand() % (moves.numMoves(u))));
    }
}

void Player_DeepQ::getReward(GameState state)
{
    _actualReward = state.evalLTD2(_playerID);
}

void Player_DeepQ::setReward()
{
    //caffe stores the data as a long array of floats stored in a pointer
    Blob<float>* action_layer = _solver->net()->input_blobs()[2];
    float* data = action_layer->mutable_cpu_data();
    for(int i = 0; i < 10; i++)
    {
        for(int j = 0; j < 5; j++)
        {
            data[0] = _actualReward;
        }
    }
}

void Player_DeepQ::backward(GameState state)
{
    getReward(state);
    if(_actualReward != 0)
    {
         _notBeginning = true;
    }
    if(_notBeginning)
    {
        getNetOutput();

        //No need to do back prop if we're just gathering data
        if(!_logData)
        {
            setReward();
            //do the actual learning
            _solver->net()->Backward();
            _solver->net()->Update();

            //Monitoring purposes, REMOVE FOR FINAL VERSION
            cout << "Predicted: " << _predictedReward << " Actual: " << _actualReward << endl;
        }
    }
    logDataPoint();
}

string GenerateRandomString(int length)
{
    string randomString;
    for(int i = 0; i < length; i++)
    {
        char randomChar = 'A' + (random() % 26);
        randomString.push_back(randomChar);
    }
    return randomString;
}

//This creates a messy file as the action layer is just a  of size 10x5.
//image_dir action[0][0] action[0][1] ... action[10][5] labelapp
void Player_DeepQ::logDataPoint()
{
    //make sure that we have an image (the first run through won't)
    if(_img.empty() || _img.rows == 0 || _img.cols == 0 || !_logData)
        return;

    string imgFile = GenerateRandomString(15);;
    //check to make sure the generated file name doesn't exist before we save
    //the image

    while(fileExists(imgFile.append(".png")))
    {
        imgFile = GenerateRandomString(15);
    }

    //save the image
    imwrite("../data/images/" + string(imgFile).append(".png"), _img);

    //open file where we will log the data
    ofstream dataFile;

    dataFile.open("../data/train.txt", std::ios::app);

    //img dir
    dataFile << imgFile.append(".png") << " ";

    //moves
    for(int i = 0; i < 10; i++)
    {
        for(int j = 0; j < 5; j++)
        {
        dataFile << _moves[i][j] << " ";
        }
    }

    //reward plus new line to prepare for the next data point
    dataFile << _actualReward << endl;

}
