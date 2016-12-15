#include <Writer/NcursesWriter.h>
#include <gtest/gtest.h>
#include <iostream>
#include <ncurses.h>

int main(int ac, char *av[])
{
    testing::InitGoogleTest(&ac, av);
    return RUN_ALL_TESTS();
}
