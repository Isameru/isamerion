
#pragma once

#include "core/basic_types.hpp"

class NBodySim
{
public:
    const float LightSpeed      = 25.0f;
    const float LightSpeedSq    = LightSpeed * LightSpeed;
    const float LightSpeedInvSq = 1.0f / LightSpeedSq;
    const float MaxSpeedCap     = 0.99 * LightSpeed;
    const float GravConst       = 0.1f;
    const int   MaxTimePosSize  = 512;

    struct Body {
        vec3         pos;
        vector<vec3> posArr;
        vec3         vel;
        float        mass;
        vec3         accelPrev;
        vec3         accel;
    };

    int           _step = 0;
    float         _time = 0.0f;
    vector<Body>  _bodies;
    vector<float> _timeArr;

public:
    NBodySim();
    ~NBodySim();

    void step(float dt);
};
