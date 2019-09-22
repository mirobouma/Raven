#pragma once

class MainLoop {
public:
    MainLoop();
    virtual ~MainLoop();

    virtual Error initialize(bool isStandalone) = 0;
    virtual void update(float deltaTime) = 0;
    virtual Error cleanup() = 0;

protected:
    bool _isStandalone;
};