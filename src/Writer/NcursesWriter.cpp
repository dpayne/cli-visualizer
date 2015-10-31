/*
 * NcursesWriter.cpp
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#include <ncurses.h>
#include "Writer/NcursesWriter.h"

vis::NcursesWriter::NcursesWriter()
{
    initscr();
    curs_set(0);          // sets the cursor to invisible

    if(has_colors() == TRUE)
    {
        start_color();        // turns on color
        use_default_colors(); // uses default colors of terminal, which allows
                              // transparency to work

        if(can_change_color() == FALSE )
        {
            // initialize color pairs
            setup_colors();
        }
    }
}

void vis::NcursesWriter::setup_colors()
{
    init_color(1, 0, 0, 0);
    init_pair(1, 1, -1);
    init_color(2, 0, 0, 51);
    init_pair(2, 2, -1);
    init_color(3, 0, 0, 102);
    init_pair(3, 3, -1);
    init_color(4, 0, 0, 153);
    init_pair(4, 4, -1);
    init_color(5, 0, 0, 204);
    init_pair(5, 5, -1);
    init_color(6, 0, 0, 255);
    init_pair(6, 6, -1);
    init_color(7, 0, 51, 0);
    init_pair(7, 7, -1);
    init_color(8, 0, 51, 51);
    init_pair(8, 8, -1);
    init_color(9, 0, 51, 102);
    init_pair(9, 9, -1);
    init_color(10, 0, 51, 153);
    init_pair(10, 10, -1);
    init_color(11, 0, 51, 204);
    init_pair(11, 11, -1);
    init_color(12, 0, 51, 255);
    init_pair(12, 12, -1);
    init_color(13, 0, 102, 0);
    init_pair(13, 13, -1);
    init_color(14, 0, 102, 51);
    init_pair(14, 14, -1);
    init_color(15, 0, 102, 102);
    init_pair(15, 15, -1);
    init_color(16, 0, 102, 153);
    init_pair(16, 16, -1);
    init_color(17, 0, 102, 204);
    init_pair(17, 17, -1);
    init_color(18, 0, 102, 255);
    init_pair(18, 18, -1);
    init_color(19, 0, 153, 0);
    init_pair(19, 19, -1);
    init_color(20, 0, 153, 51);
    init_pair(20, 20, -1);
    init_color(21, 0, 153, 102);
    init_pair(21, 21, -1);
    init_color(22, 0, 153, 153);
    init_pair(22, 22, -1);
    init_color(23, 0, 153, 204);
    init_pair(23, 23, -1);
    init_color(24, 0, 153, 255);
    init_pair(24, 24, -1);
    init_color(25, 0, 204, 0);
    init_pair(25, 25, -1);
    init_color(26, 0, 204, 51);
    init_pair(26, 26, -1);
    init_color(27, 0, 204, 102);
    init_pair(27, 27, -1);
    init_color(28, 0, 204, 153);
    init_pair(28, 28, -1);
    init_color(29, 0, 204, 204);
    init_pair(29, 29, -1);
    init_color(30, 0, 204, 255);
    init_pair(30, 30, -1);
    init_color(31, 0, 255, 0);
    init_pair(31, 31, -1);
    init_color(32, 0, 255, 51);
    init_pair(32, 32, -1);
    init_color(33, 0, 255, 102);
    init_pair(33, 33, -1);
    init_color(34, 0, 255, 153);
    init_pair(34, 34, -1);
    init_color(35, 0, 255, 204);
    init_pair(35, 35, -1);
    init_color(36, 0, 255, 255);
    init_pair(36, 36, -1);
    init_color(37, 51, 0, 0);
    init_pair(37, 37, -1);
    init_color(38, 51, 0, 51);
    init_pair(38, 38, -1);
    init_color(39, 51, 0, 102);
    init_pair(39, 39, -1);
    init_color(40, 51, 0, 153);
    init_pair(40, 40, -1);
    init_color(41, 51, 0, 204);
    init_pair(41, 41, -1);
    init_color(42, 51, 0, 255);
    init_pair(42, 42, -1);
    init_color(43, 51, 51, 0);
    init_pair(43, 43, -1);
    init_color(44, 51, 51, 51);
    init_pair(44, 44, -1);
    init_color(45, 51, 51, 102);
    init_pair(45, 45, -1);
    init_color(46, 51, 51, 153);
    init_pair(46, 46, -1);
    init_color(47, 51, 51, 204);
    init_pair(47, 47, -1);
    init_color(48, 51, 51, 255);
    init_pair(48, 48, -1);
    init_color(49, 51, 102, 0);
    init_pair(49, 49, -1);
    init_color(50, 51, 102, 51);
    init_pair(50, 50, -1);
    init_color(51, 51, 102, 102);
    init_pair(51, 51, -1);
    init_color(52, 51, 102, 153);
    init_pair(52, 52, -1);
    init_color(53, 51, 102, 204);
    init_pair(53, 53, -1);
    init_color(54, 51, 102, 255);
    init_pair(54, 54, -1);
    init_color(55, 51, 153, 0);
    init_pair(55, 55, -1);
    init_color(56, 51, 153, 51);
    init_pair(56, 56, -1);
    init_color(57, 51, 153, 102);
    init_pair(57, 57, -1);
    init_color(58, 51, 153, 153);
    init_pair(58, 58, -1);
    init_color(59, 51, 153, 204);
    init_pair(59, 59, -1);
    init_color(60, 51, 153, 255);
    init_pair(60, 60, -1);
    init_color(61, 51, 204, 0);
    init_pair(61, 61, -1);
    init_color(62, 51, 204, 51);
    init_pair(62, 62, -1);
    init_color(63, 51, 204, 102);
    init_pair(63, 63, -1);
    init_color(64, 51, 204, 153);
    init_pair(64, 64, -1);
    init_color(65, 51, 204, 204);
    init_pair(65, 65, -1);
    init_color(66, 51, 204, 255);
    init_pair(66, 66, -1);
    init_color(67, 51, 255, 0);
    init_pair(67, 67, -1);
    init_color(68, 51, 255, 51);
    init_pair(68, 68, -1);
    init_color(69, 51, 255, 102);
    init_pair(69, 69, -1);
    init_color(70, 51, 255, 153);
    init_pair(70, 70, -1);
    init_color(71, 51, 255, 204);
    init_pair(71, 71, -1);
    init_color(72, 51, 255, 255);
    init_pair(72, 72, -1);
    init_color(73, 102, 0, 0);
    init_pair(73, 73, -1);
    init_color(74, 102, 0, 51);
    init_pair(74, 74, -1);
    init_color(75, 102, 0, 102);
    init_pair(75, 75, -1);
    init_color(76, 102, 0, 153);
    init_pair(76, 76, -1);
    init_color(77, 102, 0, 204);
    init_pair(77, 77, -1);
    init_color(78, 102, 0, 255);
    init_pair(78, 78, -1);
    init_color(79, 102, 51, 0);
    init_pair(79, 79, -1);
    init_color(80, 102, 51, 51);
    init_pair(80, 80, -1);
    init_color(81, 102, 51, 102);
    init_pair(81, 81, -1);
    init_color(82, 102, 51, 153);
    init_pair(82, 82, -1);
    init_color(83, 102, 51, 204);
    init_pair(83, 83, -1);
    init_color(84, 102, 51, 255);
    init_pair(84, 84, -1);
    init_color(85, 102, 102, 0);
    init_pair(85, 85, -1);
    init_color(86, 102, 102, 51);
    init_pair(86, 86, -1);
    init_color(87, 102, 102, 102);
    init_pair(87, 87, -1);
    init_color(88, 102, 102, 153);
    init_pair(88, 88, -1);
    init_color(89, 102, 102, 204);
    init_pair(89, 89, -1);
    init_color(90, 102, 102, 255);
    init_pair(90, 90, -1);
    init_color(91, 102, 153, 0);
    init_pair(91, 91, -1);
    init_color(92, 102, 153, 51);
    init_pair(92, 92, -1);
    init_color(93, 102, 153, 102);
    init_pair(93, 93, -1);
    init_color(94, 102, 153, 153);
    init_pair(94, 94, -1);
    init_color(95, 102, 153, 204);
    init_pair(95, 95, -1);
    init_color(96, 102, 153, 255);
    init_pair(96, 96, -1);
    init_color(97, 102, 204, 0);
    init_pair(97, 97, -1);
    init_color(98, 102, 204, 51);
    init_pair(98, 98, -1);
    init_color(99, 102, 204, 102);
    init_pair(99, 99, -1);
    init_color(100, 102, 204, 153);
    init_pair(100, 100, -1);
    init_color(101, 102, 204, 204);
    init_pair(101, 101, -1);
    init_color(102, 102, 204, 255);
    init_pair(102, 102, -1);
    init_color(103, 102, 255, 0);
    init_pair(103, 103, -1);
    init_color(104, 102, 255, 51);
    init_pair(104, 104, -1);
    init_color(105, 102, 255, 102);
    init_pair(105, 105, -1);
    init_color(106, 102, 255, 153);
    init_pair(106, 106, -1);
    init_color(107, 102, 255, 204);
    init_pair(107, 107, -1);
    init_color(108, 102, 255, 255);
    init_pair(108, 108, -1);
    init_color(109, 153, 0, 0);
    init_pair(109, 109, -1);
    init_color(110, 153, 0, 51);
    init_pair(110, 110, -1);
    init_color(111, 153, 0, 102);
    init_pair(111, 111, -1);
    init_color(112, 153, 0, 153);
    init_pair(112, 112, -1);
    init_color(113, 153, 0, 204);
    init_pair(113, 113, -1);
    init_color(114, 153, 0, 255);
    init_pair(114, 114, -1);
    init_color(115, 153, 51, 0);
    init_pair(115, 115, -1);
    init_color(116, 153, 51, 51);
    init_pair(116, 116, -1);
    init_color(117, 153, 51, 102);
    init_pair(117, 117, -1);
    init_color(118, 153, 51, 153);
    init_pair(118, 118, -1);
    init_color(119, 153, 51, 204);
    init_pair(119, 119, -1);
    init_color(120, 153, 51, 255);
    init_pair(120, 120, -1);
    init_color(121, 153, 102, 0);
    init_pair(121, 121, -1);
    init_color(122, 153, 102, 51);
    init_pair(122, 122, -1);
    init_color(123, 153, 102, 102);
    init_pair(123, 123, -1);
    init_color(124, 153, 102, 153);
    init_pair(124, 124, -1);
    init_color(125, 153, 102, 204);
    init_pair(125, 125, -1);
    init_color(126, 153, 102, 255);
    init_pair(126, 126, -1);
    init_color(127, 153, 153, 0);
    init_pair(127, 127, -1);
    init_color(128, 153, 153, 51);
    init_pair(128, 128, -1);
    init_color(129, 153, 153, 102);
    init_pair(129, 129, -1);
    init_color(130, 153, 153, 153);
    init_pair(130, 130, -1);
    init_color(131, 153, 153, 204);
    init_pair(131, 131, -1);
    init_color(132, 153, 153, 255);
    init_pair(132, 132, -1);
    init_color(133, 153, 204, 0);
    init_pair(133, 133, -1);
    init_color(134, 153, 204, 51);
    init_pair(134, 134, -1);
    init_color(135, 153, 204, 102);
    init_pair(135, 135, -1);
    init_color(136, 153, 204, 153);
    init_pair(136, 136, -1);
    init_color(137, 153, 204, 204);
    init_pair(137, 137, -1);
    init_color(138, 153, 204, 255);
    init_pair(138, 138, -1);
    init_color(139, 153, 255, 0);
    init_pair(139, 139, -1);
    init_color(140, 153, 255, 51);
    init_pair(140, 140, -1);
    init_color(141, 153, 255, 102);
    init_pair(141, 141, -1);
    init_color(142, 153, 255, 153);
    init_pair(142, 142, -1);
    init_color(143, 153, 255, 204);
    init_pair(143, 143, -1);
    init_color(144, 153, 255, 255);
    init_pair(144, 144, -1);
    init_color(145, 204, 0, 0);
    init_pair(145, 145, -1);
    init_color(146, 204, 0, 51);
    init_pair(146, 146, -1);
    init_color(147, 204, 0, 102);
    init_pair(147, 147, -1);
    init_color(148, 204, 0, 153);
    init_pair(148, 148, -1);
    init_color(149, 204, 0, 204);
    init_pair(149, 149, -1);
    init_color(150, 204, 0, 255);
    init_pair(150, 150, -1);
    init_color(151, 204, 51, 0);
    init_pair(151, 151, -1);
    init_color(152, 204, 51, 51);
    init_pair(152, 152, -1);
    init_color(153, 204, 51, 102);
    init_pair(153, 153, -1);
    init_color(154, 204, 51, 153);
    init_pair(154, 154, -1);
    init_color(155, 204, 51, 204);
    init_pair(155, 155, -1);
    init_color(156, 204, 51, 255);
    init_pair(156, 156, -1);
    init_color(157, 204, 102, 0);
    init_pair(157, 157, -1);
    init_color(158, 204, 102, 51);
    init_pair(158, 158, -1);
    init_color(159, 204, 102, 102);
    init_pair(159, 159, -1);
    init_color(160, 204, 102, 153);
    init_pair(160, 160, -1);
    init_color(161, 204, 102, 204);
    init_pair(161, 161, -1);
    init_color(162, 204, 102, 255);
    init_pair(162, 162, -1);
    init_color(163, 204, 153, 0);
    init_pair(163, 163, -1);
    init_color(164, 204, 153, 51);
    init_pair(164, 164, -1);
    init_color(165, 204, 153, 102);
    init_pair(165, 165, -1);
    init_color(166, 204, 153, 153);
    init_pair(166, 166, -1);
    init_color(167, 204, 153, 204);
    init_pair(167, 167, -1);
    init_color(168, 204, 153, 255);
    init_pair(168, 168, -1);
    init_color(169, 204, 204, 0);
    init_pair(169, 169, -1);
    init_color(170, 204, 204, 51);
    init_pair(170, 170, -1);
    init_color(171, 204, 204, 102);
    init_pair(171, 171, -1);
    init_color(172, 204, 204, 153);
    init_pair(172, 172, -1);
    init_color(173, 204, 204, 204);
    init_pair(173, 173, -1);
    init_color(174, 204, 204, 255);
    init_pair(174, 174, -1);
    init_color(175, 204, 255, 0);
    init_pair(175, 175, -1);
    init_color(176, 204, 255, 51);
    init_pair(176, 176, -1);
    init_color(177, 204, 255, 102);
    init_pair(177, 177, -1);
    init_color(178, 204, 255, 153);
    init_pair(178, 178, -1);
    init_color(179, 204, 255, 204);
    init_pair(179, 179, -1);
    init_color(180, 204, 255, 255);
    init_pair(180, 180, -1);
    init_color(181, 255, 0, 0);
    init_pair(181, 181, -1);
    init_color(182, 255, 0, 51);
    init_pair(182, 182, -1);
    init_color(183, 255, 0, 102);
    init_pair(183, 183, -1);
    init_color(184, 255, 0, 153);
    init_pair(184, 184, -1);
    init_color(185, 255, 0, 204);
    init_pair(185, 185, -1);
    init_color(186, 255, 0, 255);
    init_pair(186, 186, -1);
    init_color(187, 255, 51, 0);
    init_pair(187, 187, -1);
    init_color(188, 255, 51, 51);
    init_pair(188, 188, -1);
    init_color(189, 255, 51, 102);
    init_pair(189, 189, -1);
    init_color(190, 255, 51, 153);
    init_pair(190, 190, -1);
    init_color(191, 255, 51, 204);
    init_pair(191, 191, -1);
    init_color(192, 255, 51, 255);
    init_pair(192, 192, -1);
    init_color(193, 255, 102, 0);
    init_pair(193, 193, -1);
    init_color(194, 255, 102, 51);
    init_pair(194, 194, -1);
    init_color(195, 255, 102, 102);
    init_pair(195, 195, -1);
    init_color(196, 255, 102, 153);
    init_pair(196, 196, -1);
    init_color(197, 255, 102, 204);
    init_pair(197, 197, -1);
    init_color(198, 255, 102, 255);
    init_pair(198, 198, -1);
    init_color(199, 255, 153, 0);
    init_pair(199, 199, -1);
    init_color(200, 255, 153, 51);
    init_pair(200, 200, -1);
    init_color(201, 255, 153, 102);
    init_pair(201, 201, -1);
    init_color(202, 255, 153, 153);
    init_pair(202, 202, -1);
    init_color(203, 255, 153, 204);
    init_pair(203, 203, -1);
    init_color(204, 255, 153, 255);
    init_pair(204, 204, -1);
    init_color(205, 255, 204, 0);
    init_pair(205, 205, -1);
    init_color(206, 255, 204, 51);
    init_pair(206, 206, -1);
    init_color(207, 255, 204, 102);
    init_pair(207, 207, -1);
    init_color(208, 255, 204, 153);
    init_pair(208, 208, -1);
    init_color(209, 255, 204, 204);
    init_pair(209, 209, -1);
    init_color(210, 255, 204, 255);
    init_pair(210, 210, -1);
    init_color(211, 255, 255, 0);
    init_pair(211, 211, -1);
    init_color(212, 255, 255, 51);
    init_pair(212, 212, -1);
    init_color(213, 255, 255, 102);
    init_pair(213, 213, -1);
    init_color(214, 255, 255, 153);
    init_pair(214, 214, -1);
    init_color(215, 255, 255, 204);
    init_pair(215, 215, -1);
    init_color(216, 255, 255, 255);
    init_pair(216, 216, -1);
}

void vis::NcursesWriter::write(int32_t height, int32_t width, uint8_t color,
                               const std::string &msg)
{
    attron(COLOR_PAIR(color));

    move(height, width);
    addnstr(msg.c_str(), static_cast<int>(msg.size()));

    attroff(COLOR_PAIR(color));
}

void vis::NcursesWriter::clear()
{
    standend();
    erase();
}

void vis::NcursesWriter::flush()
{
    refresh();
}

int32_t vis::NcursesWriter::get_max_color_size() const
{
    if(has_colors() == TRUE)
    {
        return COLORS;
    }
    else
    {
        return 1;
    }
}

uint8_t vis::NcursesWriter::to_color(int32_t number, int32_t max,
                                     bool wrap) const
{
    const auto colors_size = get_max_color_size();
    const auto index = (number * colors_size) / max;

    return static_cast<uint8_t>(wrap ? index % colors_size
                                     : std::min(index, colors_size - 1));
}

vis::NcursesWriter::~NcursesWriter()
{
    endwin();
}
