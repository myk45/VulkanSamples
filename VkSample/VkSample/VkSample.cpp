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

void printOptions()
{
    int option;
    printf("===================\n");
    printf("1. Run demos\n");
    printf("2. Run assignment\n");
    printf("3. Get Info on demos\n");
    printf("Choose option number\n");
    printf("===================\n");
    scanf("%d", &option);

    switch (option)
    {
    case 1:
        break;
    case 2:
        break;
    case 3:
        break;
    default:
        break;
    }
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