#ifndef __DEMOBASE__
#define __DEMOBASE__

#include "vkutils.h"

class DemoBase
{
public:
    DemoBase()  {}
    ~DemoBase() {}

    bool init();
    virtual bool run();
    bool cleanUp();

    const glfwWindowHelper& getWindowHelper() const
    {
        return _glfwHelper;
    }

protected:
    mVkInstance         _inst;
    mVkDevice           _dev;
    mVkSwapChain        _swapChain;
    mVkCommandPool      _cmdPool;
    mVkSemaphore        _semaphore;
    mVkGraphicsPipeline _graphicsPipeline;
    mVkRenderPass       _renderPass;
    glfwWindowHelper    _glfwHelper;
};

#endif