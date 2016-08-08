#ifndef __VK_UTILS__
#define __VK_UTILS__

#include <cstdio>
#include <vulkan/vulkan.h>

#define VK_LOG printf

/* All utility wrapper classses will start with an 'm' prefix */
class mVkInstance 
{
public:
    // Constructor does nothing.
    mVkInstance();

    // Instance is created here. Note that Vulkan usually needs a lot of
    // initializations to be done. We paranthesize the value list to make code
    // more readable.
    bool createInstance();

    ~mVkInstance();

private:
    VkInstance _inst;
};



#endif