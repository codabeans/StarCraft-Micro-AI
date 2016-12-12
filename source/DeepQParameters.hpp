#pragma once

#include "Common.h"

namespace SparCraft
{
    class DeepQParameters;
}

class SparCraft::DeepQParameters
{
public:
    DeepQParameters(): _GPU(true) {}
    void setGPU(bool value) { _GPU = value; }
    bool getGPU() { return _GPU; }
private:
    bool _GPU;
};
