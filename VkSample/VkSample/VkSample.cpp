#include "demobase.h"
#include "demos/demo1.h"

static bool initializeDemos(std::vector<DemoBase*>& demoList)
{
    DemoBase* demo1 = new Demo1;
    demo1->init();
    demoList.push_back(demo1);

    return true;
}

static DemoBase* chooseDemos(const std::vector<DemoBase*> demoList)
{
    unsigned int demoOption;

#ifdef WIN32
    system("cls");
#endif

    printf("===================\n");
    printf("Choose demo number\n");
    printf("===================\n");
    scanf("%d", &demoOption);

    if (demoList.size() < demoOption)
    {
        printf("\nInvalid input. Exiting!\n");
        exit(-1);
    }

    return demoList[demoOption - 1];
}

int main() 
{
    std::vector<DemoBase*> demoList;
    bool ret = initializeDemos(demoList);

    while (1) 
    {
        DemoBase* demo = chooseDemos(demoList);
        while (ret && !glfwWindowShouldClose(demo->getWindowHelper().getWindowHandle()))
        {
            glfwPollEvents();
            ret = demo->run();
        }
        demo->cleanUp();
    }

    return 0;
}