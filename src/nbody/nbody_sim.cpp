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

#include "nbody/nbody_sim.hpp"

// ---―--―-――-―――-―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――-―――-――-―--―---

void NBodySim::respawn(vector<Body>&& bodies)
{
    _step      = 0;
    _recordIdx = 0;
    _time      = 0.0f;
    _histTimeArr.resize(MaxRecordCount);
    _histTimeArr[0] = _time;

    _bodies             = std::move(bodies);
    const int bodyCount = (int)_bodies.size();

    _histPosMat.reset({(int)MaxRecordCount, bodyCount}, vec3{});
    _histInterMat.reset({bodyCount, bodyCount}, LightIntersectCacheEntry{0, 0.0f});

    for (int ib = 0; ib < bodyCount; ++ib) {
        _histPosMat({0, ib}) = _bodies[ib].pos;
    }
}

void NBodySim::step(float dt)
{
    dt = std::min(dt, 0.01f);
    dt = std::max(dt, 0.0001f);

    // Compute all-to-all accelerations between bodies for the current/last frame.
    //
    const int bodyCount = (int)_bodies.size();
    if (bodyCount == 0) {
        return;
    }

    for (int ib1 = 0; ib1 < bodyCount; ++ib1) {
        Body& b1     = _bodies[ib1];
        b1.accelPrev = std::exchange(b1.accel, vec3{});

        for (int ib2 = 0; ib2 < ib1; ++ib2) {
            applyGravAccel(ib1, ib2);
            applyGravAccel(ib2, ib1);
        }
    }

    // Progress the counters to the next simulation frame.
    //
    ++_step;
    if (_step % RecordStepInterval == 1) {
        ++_recordIdx;
    }

    _time += dt;
    _histTimeArr[_recordIdx % MaxRecordCount] = _time;

    // Update the positions and velocities of all bodies.
    //
    for (int ib = 0; ib < bodyCount; ++ib) {
        Body& body = _bodies[ib];

        auto       vel0      = body.vel;
        const auto vel0_len2 = glm::length2(body.vel);
        if (vel0_len2 > MaxSpeedCap * MaxSpeedCap) {
            vel0 *= MaxSpeedCap * MaxSpeedCap / vel0_len2;
        }
        assert(glm::length(vel0) < LightSpeed);

        auto       vel_delta      = 0.5f * (body.accelPrev + body.accel) * GravConst * dt;
        const auto vel_delta_len2 = glm::length2(vel_delta);
        if (vel_delta_len2 > MaxSpeedCap * MaxSpeedCap) {
            vel_delta *= MaxSpeedCap * MaxSpeedCap / vel_delta_len2;
        }
        assert(glm::length(vel_delta) < LightSpeed);

        if (glm::length2(vel_delta) > 0.0f) {
            // Split vel0 into 2 components: colinear to vel_delta and orthogonal to it.
            const auto vel0_coll = vel_delta * glm::dot(vel0, vel_delta) / glm::length2(vel_delta);
            const auto vel0_orho = vel0 - vel0_coll;

            body.vel = ((vel0_coll + vel_delta) + vel0_orho * std::sqrt(1.0f - glm::length2(vel_delta) * LightSpeedInvSq)) / (1.0f + glm::dot(vel_delta, vel0_coll) * LightSpeedInvSq);

            const auto body_vel_len2 = glm::length2(body.vel);
            if (body_vel_len2 > MaxSpeedCap * MaxSpeedCap) {
                body.vel *= MaxSpeedCap * MaxSpeedCap / body_vel_len2;
            }

            assert(glm::length(body.vel) < LightSpeed);
        }

        body.pos += dt * 0.5f * (vel0 + body.vel);
        _histPosMat({_recordIdx % MaxRecordCount, ib}) = body.pos;
    }
}

void NBodySim::applyGravAccel(int target_body_idx, int source_body_idx)
{
    const int rec_start = std::max(0, (_recordIdx - (MaxRecordCount - 1)));
    const int rec_end   = _recordIdx + 1;
    assert(rec_end > rec_start);

    auto&       target_body = _bodies[target_body_idx];
    const auto& source_body = _bodies[source_body_idx];
    const auto  s_pos_arr   = _histPosMat.row(source_body_idx);

    auto& [hist_record_idx, hist_alpha] = _histInterMat({target_body_idx, source_body_idx});

    vec3  s0_pos{};
    vec3  s1_pos{};
    float beta{};

    while (true) {
        int s0_idx = hist_record_idx;
        if (s0_idx < rec_start) {
            s0_idx          = rec_start;
            hist_record_idx = s0_idx;
            hist_alpha      = 0.0f;
        }

        int s1_idx = s0_idx + 1;
        if (s1_idx >= rec_end) {
            return;
        }

        s0_pos = s_pos_arr[s0_idx % MaxRecordCount];
        s1_pos = s_pos_arr[s1_idx % MaxRecordCount];

        const float s0_past_time = _time - _histTimeArr[s0_idx % MaxRecordCount];
        const float s1_past_time = _time - _histTimeArr[s1_idx % MaxRecordCount];
        assert(s0_past_time >= s1_past_time);

        const float alpha        = hist_alpha;
        const vec3  sa_pos       = s0_pos + (s1_pos - s0_pos) * alpha;
        const float sa_past_time = s0_past_time + (s1_past_time - s0_past_time) * alpha;
        float       sa_dist2      = glm::distance2(target_body.pos, sa_pos);
        const float sa_ct2        = LightSpeedSq * sa_past_time;
        const float sa_weight    = sa_ct2 - sa_dist2;

        if (sa_weight < 0.0f) {
            float       s0_dist2   = glm::distance2(target_body.pos, s0_pos);
            const float s0_ct2     = LightSpeedSq * s0_past_time;
            const float s0_weight = s0_ct2 - s0_dist2;

            if (s0_weight < 0.0f) {
                if (s0_idx == rec_start) {
                    return;
                } else {
                    --hist_record_idx;
                    hist_alpha = 1.0f;
                    continue;
                }
            } else {
                beta = (alpha * s0_weight - 1.0f * sa_weight) / (s0_weight - sa_weight + 0.00001f);
                assert(std::isfinite(beta));
                break;
            }

        } else {
            float       s1_dist2   = glm::distance2(target_body.pos, s1_pos);
            const float s1_ct2     = LightSpeedSq * s1_past_time;
            const float s1_weight = s1_dist2 - s1_ct2;

            if (s1_weight < 0.0f) {
                ++hist_record_idx;
                hist_alpha = 0.0f;
                continue;
            } else {
                beta = (alpha * s1_weight + 1.0f * sa_weight) / (sa_weight + s1_weight + 0.00001f);
                assert(std::isfinite(beta));
                break;
            }
        }
    }

    const vec3  sb_pos = s0_pos + (s1_pos - s0_pos) * beta;
    const float distSq = glm::distance2(sb_pos, target_body.pos);
    const vec3  attr   = (sb_pos - target_body.pos) / (distSq * std::sqrt(distSq) + 0.001f);
    target_body.accel += attr * source_body.mass;

    hist_alpha = beta;
    assert(hist_alpha >= 0.0f && hist_alpha <= 1.0f);
}

// ---―--―-――-―――-―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――-―――-――-―--―---
