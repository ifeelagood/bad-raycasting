#include "buffer.h"

#include <algorithm>


template <typename T> T* createBuffer1D(int w, int h)
{
    T* buffer = new T[w*h];
    return buffer;
}

template <typename T> T** createBuffer2D(int w, int h)
{
    T** buffer = new T*[h];

    for (int i = 0; i < h; i++)
    {
        buffer[i] = new T[w];
    }

    return buffer;
}

template <typename T> void deleteBuffer1D(T* buffer, int w, int h)
{
    delete[] buffer;
}

template <typename T> void deleteBuffer2D(T** buffer, int w, int h)
{
    for (int i = 0; i < h; i++)
    {
        delete[] buffer[i];
    }

    delete[] buffer;
}

template <typename T> void clearBuffer1D(T* buffer, int w, int h)
{
    int l = w*h;
    std::fill(&buffer[0], &buffer[l], 0u);
}

template <typename T> void clearBuffer2D(T** buffer, int w, int h)
{
    std::fill(&buffer[0][0], &buffer[h-1][w-1], 0u);
}

template <typename T> void putBuffer1D(T* buffer, T value, int x, int y, int h)
{
    int i = x + y * h;

    buffer[i] = value;
}

template <typename T> void putBuffer2D(T** buffer, T value, int x, int y)
{
    buffer[y][x] = value;
}