#pragma once

template <typename T> T* createBuffer1D(int w, int h);
template <typename T> T** createBuffer2D(int w, int h);

template <typename T> void deleteBuffer1D(T* buffer, int w, int h);
template <typename T> void deleteBuffer2D(T** buffer, int w, int h);

template <typename T> void clearBuffer1D(T* buffer, int w, int h);
template <typename T> void clearBuffer2D(T** buffer, int w, int h);

template <typename T> void putBuffer1D(T* buffer, T value, int x, int y, int h);
template <typename T> void putBuffer2D(T** buffer, T value, int x, int y);