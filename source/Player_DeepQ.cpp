#include "../include/Player_DeepQ.h"

using namespace SparCraft;
using namespace caffe;
using namespace std;

//constructor, sets important private member variables
//as well as constructs our caffe network, if it can
Player_DeepQ::Player_DeepQ(const IDType & playerID, const DeepQParameters & params)
{
    _playerID = playerID;
    _params = params;
    _frameNumber = 0;
    _logData = true;
    _notBeginning = false;
    _modelFile = "../models/model.prototxt";
    _weightFile = "../models/weights.prototxt";

    initializeNet();

    //Per the parameter, set the hardware usage in caffe
    if(_params.getGPU())
    {
        Caffe::set_mode(Caffe::GPU);
    }
    else
    {
        Caffe::set_mode(Caffe::CPU);
    }
}

//check if the inputted string exists as a file
bool fileExists(string file)
{
    std::ifstream ifile(file.c_str());
    return (bool)ifile;
}

//To be ran at the initialization of the Player
//Loads the CNN as well as the weights (if there are any to load)
void Player_DeepQ::initializeNet()
{
    //Load the architecture from _modelFile, and init for TRAIN
    //If the file isn't found, give a fatalerror as the player would
    //be unable to play :'( feelsbadman.jpg
    //caffe will give its own error(s) if something is wrong with the files
    if(fileExists(_modelFile))
        _net.reset(new Net<float>(_modelFile, TRAIN));
    else
        System::FatalError("Problem Opening Model file");
    //Copy weights from a previously trained net of the same architecture
    //Don't have said file yet, soon!
    if(fileExists(_weightFile))
        _net->CopyTrainedLayersFrom(_weightFile);
}

//convert the ENUM ActionTypes to int values so a NN can hopefully make sense of them
int moveInt(const IDType moveType)
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
void Player_DeepQ::prepareModelInput(const vector<Action> & moveVec)
{
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
    for(int i = 0; i < 10-moveVec.size(); i++)
    {
        _moves.push_back(emptyMove);
    }
}

//Load a set of actions into the network
void Player_DeepQ::loadActions()
{
    // caffe stores the data as a long array of floats stored in a pointer
    Blob<float>* action_layer = _net->input_blobs()[1];
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
    loadActions();

    _net->Forward();
}

void Player_DeepQ::getNetOutput()
{
    boost::shared_ptr<Blob<float> > output_layer = _net->blob_by_name("reward");
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
    else
    {
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

void Player_DeepQ::getReward(const GameState state)
{
    _actualReward = state.evalLTD2(_playerID);
}

void Player_DeepQ::setReward()
{
    //caffe stores the data as a long array of floats stored in a pointer
    Blob<float>* action_layer = _net->input_blobs()[2];
    float* data = action_layer->mutable_cpu_data();
    for(int i = 0; i < 10; i++)
    {
        for(int j = 0; j < 5; j++)
        {
            data[0] = _actualReward;
        }
    }
}

void Player_DeepQ::backward(const GameState state)
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
            _net->Backward();
            _net->Update();

            //Monitoring purposes, REMOVE FOR FINAL VERSION
            cout << "Predicted: " << _predictedReward << " Actual: " << _actualReward << endl;
        }
    }
    //logDataPoint();
}

string GenerateRandomString(const int length)
{
    string randomString;
    for(int i = 0; i < length; i++)
    {
        char randomChar = 'A' + (random() % 26);
        randomString.push_back(randomChar);
    }
    return randomString;
}
