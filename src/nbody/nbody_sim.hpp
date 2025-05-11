/*
    MIT License
    Copyright (c) 2025 Mariusz Łapiński

      ▄█     ▄████████    ▄████████    ▄▄▄▄███▄▄▄▄      ▄████████    ▄████████  ▄█   ▄██████▄  ███▄▄▄▄
      ███    ███    ███   ███    ███  ▄██▀▀▀███▀▀▀██▄   ███    ███   ███    ███ ███  ███    ███ ███▀▀▀██▄
      ███▌   ███    █▀    ███    ███  ███   ███   ███   ███    █▀    ███    ███ ███▌ ███    ███ ███   ███
      ███▌   ███          ███    ███  ███   ███   ███  ▄███▄▄▄      ▄███▄▄▄▄██▀ ███▌ ███    ███ ███   ███
      ███▌ ▀███████████ ▀███████████  ███   ███   ███ ▀▀███▀▀▀     ▀▀███▀▀▀▀▀   ███▌ ███    ███ ███   ███
      ███           ███   ███    ███  ███   ███   ███   ███    █▄  ▀███████████ ███  ███    ███ ███   ███
      ███     ▄█    ███   ███    ███  ███   ███   ███   ███    ███   ███    ███ ███  ███    ███ ███   ███
      █▀    ▄████████▀    ███    █▀    ▀█   ███   █▀    ██████████   ███    ███ █▀    ▀██████▀   ▀█   █▀
                                                                    ███    ███
*/

#pragma once

#include "core/basic_types.hpp"
#include "core/matrix.hpp"

// ---―--―-――-―――-―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――-―――-――-―--―---

class NBodySim
{
public:
    const float LightSpeed         = 10.0f;
    const float LightSpeedSq       = LightSpeed * LightSpeed;
    const float LightSpeedInvSq    = 1.0f / LightSpeedSq;
    const float MaxSpeedCap        = 0.999 * LightSpeed;
    const float GravConst          = 1.0f;
    const int   MaxRecordCount     = 512;
    const int   RecordStepInterval = 16;

    struct Body {
        vec3  pos;
        vec3  vel;
        float mass;
        vec3  accelPrev;
        vec3  accel;
    };

    struct LightIntersectCacheEntry {
        int   recordIdx;
        float alpha;
    };

    int                              _step      = 0;
    int                              _recordIdx = 0;
    float                            _time      = 0.0f;
    vector<float>                    _histTimeArr;
    vector<Body>                     _bodies;
    Matrix<vec3>                     _histPosMat;
    Matrix<LightIntersectCacheEntry> _histInterMat;

public:
    NBodySim() = default;
    ~NBodySim() = default;

    void  respawn(vector<Body>&& bodies);
    float simTime() const { return _time; }
    void  step(float dt);

private:
    void applyGravAccel(int body1_ix, int body2_ix);
};

// ---―--―-――-―――-―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――-―――-――-―--―---
