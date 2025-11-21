#include <iostream>
#include <gtest/gtest.h>
/*
    断言宏的使用 
        ASSERT_  断言失败则退出
        EXPECT_  断言失败继续运行
    注意：
        断言宏，必须在单元测试宏函数中使用
*/

TEST(test, less_than) {
    int age = 20;
    EXPECT_LT(age, 18);
    printf("OK!\n");
}

TEST(test, great_than) {
    int age = 20;
    EXPECT_GT(age, 18);
    printf("OK!\n");
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
    return 0;
}