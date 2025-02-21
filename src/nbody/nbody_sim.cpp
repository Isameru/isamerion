
#include "nbody/nbody_sim.hpp"

NBodySim::NBodySim()
{
    static std::mt19937                   re(0);
    std::uniform_real_distribution<float> radiusDis(0.1f, 10.0f);
    std::uniform_real_distribution<float> velDis(-0.2f, 0.2f);
    std::uniform_real_distribution<float> massDis(0.01f, 0.5f);

    _bodies.push_back(Body{
        .pos  = vec3{-3.0f, 0.0f, 0.0f},
        .vel  = vec3{0.0f, 0.0f, 0.4f},
        .mass = 6.0f,
    });

    _bodies.push_back(Body{
        .pos  = vec3{+3.0f, 0.0f, 0.0f},
        .vel  = vec3{0.0f, 0.0f, -0.4f},
        .mass = 6.0f,
    });

    const int bc = 32;
    for (int i = 0; i < bc; ++i) {
        const float alpha  = (float)i * 2.0f * glm::pi<float>() / (float)bc;
        const float radius = radiusDis(re);

        Body body{
            .pos  = radius * vec3{cos(alpha), 0.0f, sin(alpha)},
            .vel  = 0.05f * radius * vec3{-sin(alpha), 0.0f, cos(alpha)},
            .mass = massDis(re) * massDis(re) * massDis(re),
        };

        _bodies.push_back(std::move(body));
    }
}

NBodySim::~NBodySim() {}

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

void NBodySim::step(float dt)
{
    dt = 0.01f;

    if (_step == 0) {
        _timeArr.resize(MaxTimePosSize);
        _timeArr[0] = _time;

        for (auto& body : _bodies) {
            body.posArr.resize(MaxTimePosSize);
            body.posArr[0] = body.pos;
        }
    }

    ++_step;
    _time += dt;

    float alpha      = 0.0f;
    int   alphaCount = 0;

    for (int ib1 = 0; ib1 < (int)_bodies.size(); ++ib1) {
        auto& b1 = _bodies[ib1];
        b1.accel = vec3{};

        for (int ib2 = 0; ib2 < (int)_bodies.size(); ++ib2) {
            if (ib1 == ib2) {
                continue;
            }

            auto& b2 = _bodies[ib2];

            const int startIndex = std::max(0, _step - (MaxTimePosSize - 1));

            auto foundIndex = bisectByIndex(startIndex, _step + 1, [&](int ix) -> bool {
                ix = ix % MaxTimePosSize;

                const auto pos  = b2.posArr[ix];
                const auto dist = glm::distance(b1.pos, pos);
                const auto past = _time - _timeArr[ix];

                return past * LightSpeed > dist;
            });

            alpha += (float)(foundIndex - startIndex) / (float)MaxTimePosSize;
            ++alphaCount;

            if (foundIndex <= startIndex) {
                continue;
            }

            if (foundIndex > _step) {
                foundIndex = _step;
            }

            const auto b2_pos = b2.posArr[foundIndex % MaxTimePosSize];

            const float distSq = glm::distance(b1.pos, b2_pos);
            const vec3  attr   = GravConst * (b2_pos - b1.pos) / (distSq * sqrt(distSq) + 0.001f);

            b1.accel += attr * b2.mass;
        }
    }

    const size_t timePosIdx = _step % MaxTimePosSize;
    _timeArr[timePosIdx]    = _time;

    for (auto& body : _bodies) {
        const auto vel0 = body.vel;
        assert(glm::length(body.vel) < LightSpeed);
        const auto vel_delta = body.accel * dt;
        assert(glm::length(vel_delta) < LightSpeed);

        if (glm::length2(vel_delta) > 0.0f) {
            // Split vel0 into 2 components: colinear to vel_delta and orthogonal to it.
            const auto vel0_coll = vel_delta * glm::dot(vel0, vel_delta) / glm::length2(vel_delta);
            const auto vel0_orho = vel0 - vel0_coll;

            body.vel = ((vel0_coll + vel_delta) + vel0_orho * std::sqrt(1.0f - glm::length2(vel_delta) * LightSpeedInvSq)) /
                       (1.0f + glm::dot(vel_delta, vel0_coll) * LightSpeedInvSq);

            if (glm::length2(body.vel) > MaxSpeedCap * MaxSpeedCap) {
                body.vel *= MaxSpeedCap / glm::length(body.vel);
            }

            assert(glm::length(body.vel) < LightSpeed);
        }

        body.pos += dt * 0.5f * (vel0 + body.vel);
        body.posArr[timePosIdx] = body.pos;
    }
}
