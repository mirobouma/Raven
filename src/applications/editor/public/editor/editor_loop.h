#pragma once
#include "core/main/main_loop.h"

class EditorLoop : public MainLoop {
public:
    virtual Error initialize(bool isStandalone) override;
    virtual void update(float deltaTime) override;
    virtual Error cleanup() override;
};