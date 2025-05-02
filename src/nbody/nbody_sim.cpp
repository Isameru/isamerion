
#include "nbody/nbody_sim.hpp"

int bisectByIndex(int startIndex, int endIndex, const std::invocable<int> auto& isLess)
{
    while (startIndex != endIndex) {
        const int midIndex = (startIndex + endIndex) / 2;

        if (isLess(midIndex)) {
            startIndex = midIndex + 1;
        } else {
            endIndex = midIndex;
        }
    }

    return startIndex;
}

vec3 solveLightIntersection(float c, const vec3& A_pos, vec3 B0_pos, float pastTime0, vec3 B1_pos, float pastTime1, int iterCount)
{
    assert(pastTime0 >= pastTime1);

    float B0_dist = glm::distance(A_pos, B0_pos);
    float B1_dist = glm::distance(A_pos, B1_pos);

    while (true) {
        const float ct0 = c * pastTime0;
        const float ct1 = c * pastTime1;

        const float weight0 = ct0 - B0_dist;
        const float weight1 = B1_dist - ct1;

        assert(weight0 >= 0.0f);
        assert(weight1 >= 0.0f);
        const float alpha = weight0 / (weight0 + weight1 + 1e-5f);

        const vec3 X_pos = B0_pos + (B1_pos - B0_pos) * alpha;

        if (iterCount <= 1) {
            return X_pos;
        }

        float       X_dist    = glm::distance(A_pos, X_pos);
        const float pastTimeX = pastTime0 + (pastTime1 - pastTime0) * alpha;
        const float ctX       = c * pastTimeX;

        if (X_dist >= ctX) {
            B1_pos    = X_pos;
            B1_dist   = X_dist;
            pastTime1 = pastTimeX;
        } else {
            B0_pos    = X_pos;
            B0_dist   = X_dist;
            pastTime0 = pastTimeX;
        }

        --iterCount;
    }
}

NBodySim::NBodySim()
{
    static std::mt19937                   re(0);
    std::uniform_real_distribution<float> radiusDis(0.1f, 10.0f);
    std::uniform_real_distribution<float> velDis(-0.2f, 0.2f);
    std::uniform_real_distribution<float> massDis(0.01f, 0.5f);

    _bodies.push_back(Body{
        .pos  = vec3{0.0f, 0.0f, 0.0f},
        .vel  = vec3{0.0f, 0.0f, 0.0f},
        .mass = 5.0f,
    });

    const int bc = 99;
    for (int i = 0; i < bc; ++i) {
        const float alpha  = (float)i * 2.0f * glm::pi<float>() / (float)bc;
        const float radius = radiusDis(re);

        Body body{
            .pos  = radius * vec3{cos(alpha), 0.0f, sin(alpha)},
            .vel  = 0.15f * radius * vec3{-sin(alpha), 0.5f * velDis(re), cos(alpha)},
            .mass = massDis(re) * massDis(re) * massDis(re),
        };

        _bodies.push_back(std::move(body));
    }
}

NBodySim::~NBodySim() {}

static int magic = 0;  // TODO: Remove

void NBodySim::step(float dt)
{
    dt = 0.01f;
    magic = 0;

    if (_step == 0) {
        _timeArr.resize(MaxRecordCount);
        assert(_recordIdx == 0);
        _timeArr[0] = _time;

        for (auto& body : _bodies) {
            body.posArr.resize(MaxRecordCount);
            body.posArr[0] = body.pos;
            body.lastFoundIndices.resize(_bodies.size(), -1);
        }
    }

    if (_step % _recordStepInterval == 0) {
        ++_recordIdx;
    }

    ++_step;
    _time += dt;

    _timeArr[_recordIdx % MaxRecordCount] = _time;

    for (int ib1 = 0; ib1 < (int)_bodies.size(); ++ib1) {
        auto& b1 = _bodies[ib1];
        b1.accel = vec3{};

        for (int ib2 = 0; ib2 < ib1; ++ib2) {
            //auto& b2 = _bodies[ib2];

            applyGravAccel(ib1, ib2);
            applyGravAccel(ib2, ib1);
        }
    }

    for (auto& body : _bodies) {
        const auto vel0 = body.vel;
        assert(glm::length(body.vel) < LightSpeed);
        const auto vel_delta = body.accel * GravConst * dt;
        assert(glm::length(vel_delta) < LightSpeed);

        if (glm::length2(vel_delta) > 0.0f) {
            // Split vel0 into 2 components: colinear to vel_delta and orthogonal to it.
            const auto vel0_coll = vel_delta * glm::dot(vel0, vel_delta) / glm::length2(vel_delta);
            const auto vel0_orho = vel0 - vel0_coll;

            body.vel = ((vel0_coll + vel_delta) + vel0_orho * std::sqrt(1.0f - glm::length2(vel_delta) * LightSpeedInvSq)) / (1.0f + glm::dot(vel_delta, vel0_coll) * LightSpeedInvSq);

            if (glm::length2(body.vel) > MaxSpeedCap * MaxSpeedCap) {
                body.vel *= MaxSpeedCap / glm::length(body.vel);
            }

            assert(glm::length(body.vel) < LightSpeed);
        }

        body.pos += dt * 0.5f * (vel0 + body.vel);
        body.posArr[_recordIdx % MaxRecordCount] = body.pos;
    }

    if (_step % 60 == 0)
    {
        std::cout << "mario: " << magic << std::endl;
    }
}

void NBodySim::applyGravAccel(int body1_ix, int body2_ix)
{
    auto& b1 = _bodies[body1_ix];
    const auto& b2 = _bodies[body2_ix];

    int foundIndex = b1.lastFoundIndices[body2_ix];

    const int startIndex = std::max(0, (_recordIdx - (MaxRecordCount - 1)));

    if (foundIndex == -1) {
        foundIndex = bisectByIndex(startIndex, _recordIdx + 1, [&](int ix) -> bool {
            ix = ix % MaxRecordCount;

            const auto pos  = b2.posArr[ix];
            const auto dist = glm::distance(b1.pos, pos);
            const auto past = _time - _timeArr[ix];

            return past * LightSpeed > dist;
        });
    } else {
        while (foundIndex <= _recordIdx) {
            const auto pos1  = b2.posArr[foundIndex % MaxRecordCount];
            const auto dist1 = glm::distance(b1.pos, pos1);
            const auto past  = _time - _timeArr[foundIndex % MaxRecordCount];
            if (past * LightSpeed < dist1) {
                break;
            }
            ++foundIndex;
        }

        while (foundIndex >= startIndex) {
            const auto pos  = b2.posArr[(foundIndex + MaxRecordCount - 1) % MaxRecordCount];
            const auto dist = glm::distance(b1.pos, pos);
            const auto past = _time - _timeArr[(foundIndex + MaxRecordCount - 1) % MaxRecordCount];
            if (past * LightSpeed > dist) {
                break;
            }
            --foundIndex;
        }
    }

    if (foundIndex <= startIndex) {
        return;
    }

    if (foundIndex > _recordIdx) {
        foundIndex = _recordIdx;
    }

    b1.lastFoundIndices[body2_ix] = foundIndex;
    magic                         = std::max(magic, _recordIdx - foundIndex);

    const auto b2_ix_0 = (foundIndex + (MaxRecordCount - 1)) % MaxRecordCount;
    const auto b2_ix_1 = foundIndex % MaxRecordCount;

    const vec3 b2_pos = solveLightIntersection(LightSpeed, b1.pos, b2.posArr[b2_ix_0], (_time - _timeArr[b2_ix_0]), b2.posArr[b2_ix_1], (_time - _timeArr[b2_ix_1]), NUM_LIGHT_INTERSECT_INTERP_STEPS);

    const float distSq = glm::distance(b1.pos, b2_pos);
    const vec3  attr   = (b2_pos - b1.pos) / (distSq * std::sqrt(distSq) + 0.001f);

    b1.accel += attr * b2.mass;
}
