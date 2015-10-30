#include <ncurses.h>
#include <iostream>
#include <gtest/gtest.h>
#include <Writer/NcursesWriter.h>

int main(int ac, char *av[])
{
    testing::InitGoogleTest(&ac, av);
    return RUN_ALL_TESTS();
}
