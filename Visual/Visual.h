#pragma once

struct Vulkan;

struct Visual
{
    Visual();
    ~Visual();

    void Draw();

private:
    Vulkan m_Vulkan;
};