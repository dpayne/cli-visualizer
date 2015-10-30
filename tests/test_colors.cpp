#include <ncurses.h>
#include <iostream>
#include <gtest/gtest.h>
#include <Writer/NcursesWriter.h>

#pragma clang diagnostic ignored "-Wweak-vtables"

namespace
{

class ColorsTest : public ::testing::Test
{
  public:
    ColorsTest()
    {
        m_ncurses_writer =
            std::unique_ptr<vis::NcursesWriter>{new vis::NcursesWriter{}};
    }

    virtual ~ColorsTest()
    {
    }

    vis::NcursesWriter *get_writer()
    {
        return m_ncurses_writer.get();
    }

  protected:
    // called before every test
    virtual void SetUp()
    {
    }

    // called after every test
    virtual void TearDown()
    {
    }

  private:
    std::unique_ptr<vis::NcursesWriter> m_ncurses_writer;
};
}

TEST_F(ColorsTest, FirstTest)
{
    vis::NcursesWriter *writer = get_writer();

    std::string msg{"hello world"};
    writer->write(0, 0, 0, msg);
    writer->flush();
}
