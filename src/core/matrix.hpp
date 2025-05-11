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

// ---―--―-――-―――-―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――-―――-――-―--―---

// A matrix class template that provides a 2D array-like structure with row access and element access by coordinates.
//
template<typename T> class Matrix
{
    std::vector<T> _data;
    ivec2          _size = {0, 0};

public:
    Matrix()                         = default;
    Matrix(const Matrix&)            = default;
    Matrix(Matrix&&)                 = default;
    Matrix& operator=(const Matrix&) = default;
    Matrix& operator=(Matrix&&)      = default;
    ~Matrix()                        = default;
    Matrix(ivec2 size, const std::optional<T>& initValue = std::nullopt) { reset(size, initValue); }

    ivec2 size() const noexcept { return _size; }
    void  reset(ivec2 size, const std::optional<T>& clearValue = std::nullopt)
    {
        if (size != _size) {
            _data.resize(size.x * size.y);
            _size = size;
        }
        if (clearValue) {
            clear(*clearValue);
        }
    }
    void clear(const T& value = T{}) noexcept { std::fill(_data.begin(), _data.end(), value); }

    std::span<T> row(int y) noexcept
    {
        assert(y >= 0 && y < _size.y);
        return std::span<T>(_data.data() + y * _size.x, _size.x);
    }

    std::span<const T> row(int y) const noexcept
    {
        assert(y >= 0 && y < _size.y);
        return std::span<const T>(_data.data() + y * _size.x, _size.x);
    }

    const T& operator()(ivec2 xy) const noexcept
    {
        assert(xy.x >= 0 && xy.x < _size.x);
        assert(xy.y >= 0 && xy.y < _size.y);
        return _data[xy.x + xy.y * _size.x];
    }

    T& operator()(ivec2 xy) noexcept
    {
        assert(xy.x >= 0 && xy.x < _size.x);
        assert(xy.y >= 0 && xy.y < _size.y);
        return _data[xy.x + xy.y * _size.x];
    }
};

// ---―--―-――-―――-―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――-―――-――-―--―---
