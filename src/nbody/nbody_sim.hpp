
#pragma once

#include "core/basic_types.hpp"

class NBodySim
{
    constexpr static const int NUM_LIGHT_INTERSECT_INTERP_STEPS = 2;

public:
    const float LightSpeed      = 2.0f;
    const float LightSpeedSq    = LightSpeed * LightSpeed;
    const float LightSpeedInvSq = 1.0f / LightSpeedSq;
    const float MaxSpeedCap     = 0.99 * LightSpeed;
    const float GravConst       = 0.5f;
    const int   MaxRecordCount  = 128;

    struct Body {
        vec3         pos;
        vector<vec3> posArr;
        vector<int16_t> lastFoundIndices;
        vec3         vel;
        float        mass;
        vec3         accelPrev;
        vec3         accel;
    };

    int           _step = 0;
    float         _time = 0.0f;
    vector<Body>  _bodies;
    vector<float> _timeArr;
    int           _recordIdx          = 0;
    int           _recordStepInterval = 16;

public:
    NBodySim();
    ~NBodySim();

    float simTime() const { return _time; }
    void  step(float dt);

private:
    void applyGravAccel(int body1_ix, int body2_ix);
};
