#ifndef __DEMOBASE__
#define __DEMOBASE__

#include "vkutils.h"

class DemoBase
{
public:
    DemoBase()          {}
    virtual ~DemoBase() {}

    virtual bool init();
    virtual bool run();
    virtual bool cleanUp();

    // Every demo is different!
    virtual const char* getInfo() const = 0;

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