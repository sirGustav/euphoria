#include "core/palette_matplot.h"

#include "core/palette.h"

/*
New matplotlib colormaps by Nathaniel J. Smith, Stefan van der Walt,
and (in the case of viridis) Eric Firing.

This file and the colormaps in it are released under the CC0 license /
public domain dedication. We would appreciate credit if you use or
redistribute these colormaps, but do not impose any legal restrictions.

To the extent possible under law, the persons who associated CC0 with
mpl-colormaps have waived all copyright and related or neighboring rights
to mpl-colormaps.

You should have received a copy of the CC0 legalcode along with this
work.  If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/


namespace euphoria::core
{
    namespace
    {
        Rgbi
        C(unsigned char r, unsigned char g, unsigned char b)
        {
            return {r, g, b};
        }
    }  // namespace

    namespace palette
    {
        const Palette&
        Magma()
        {
            static const auto p = Palette {
                    "Magma",          C(0, 0, 4),       C(1, 0, 5),
                    C(1, 1, 6),       C(1, 1, 8),       C(2, 1, 9),
                    C(2, 2, 11),      C(2, 2, 13),      C(3, 3, 15),
                    C(3, 3, 18),      C(4, 4, 20),      C(5, 4, 22),
                    C(6, 5, 24),      C(6, 5, 26),      C(7, 6, 28),
                    C(8, 7, 30),      C(9, 7, 32),      C(10, 8, 34),
                    C(11, 9, 36),     C(12, 9, 38),     C(13, 10, 41),
                    C(14, 11, 43),    C(16, 11, 45),    C(17, 12, 47),
                    C(18, 13, 49),    C(19, 13, 52),    C(20, 14, 54),
                    C(21, 14, 56),    C(22, 15, 59),    C(24, 15, 61),
                    C(25, 16, 63),    C(26, 16, 66),    C(28, 16, 68),
                    C(29, 17, 71),    C(30, 17, 73),    C(32, 17, 75),
                    C(33, 17, 78),    C(34, 17, 80),    C(36, 18, 83),
                    C(37, 18, 85),    C(39, 18, 88),    C(41, 17, 90),
                    C(42, 17, 92),    C(44, 17, 95),    C(45, 17, 97),
                    C(47, 17, 99),    C(49, 17, 101),   C(51, 16, 103),
                    C(52, 16, 105),   C(54, 16, 107),   C(56, 16, 108),
                    C(57, 15, 110),   C(59, 15, 112),   C(61, 15, 113),
                    C(63, 15, 114),   C(64, 15, 116),   C(66, 15, 117),
                    C(68, 15, 118),   C(69, 16, 119),   C(71, 16, 120),
                    C(73, 16, 120),   C(74, 16, 121),   C(76, 17, 122),
                    C(78, 17, 123),   C(79, 18, 123),   C(81, 18, 124),
                    C(82, 19, 124),   C(84, 19, 125),   C(86, 20, 125),
                    C(87, 21, 126),   C(89, 21, 126),   C(90, 22, 126),
                    C(92, 22, 127),   C(93, 23, 127),   C(95, 24, 127),
                    C(96, 24, 128),   C(98, 25, 128),   C(100, 26, 128),
                    C(101, 26, 128),  C(103, 27, 128),  C(104, 28, 129),
                    C(106, 28, 129),  C(107, 29, 129),  C(109, 29, 129),
                    C(110, 30, 129),  C(112, 31, 129),  C(114, 31, 129),
                    C(115, 32, 129),  C(117, 33, 129),  C(118, 33, 129),
                    C(120, 34, 129),  C(121, 34, 130),  C(123, 35, 130),
                    C(124, 35, 130),  C(126, 36, 130),  C(128, 37, 130),
                    C(129, 37, 129),  C(131, 38, 129),  C(132, 38, 129),
                    C(134, 39, 129),  C(136, 39, 129),  C(137, 40, 129),
                    C(139, 41, 129),  C(140, 41, 129),  C(142, 42, 129),
                    C(144, 42, 129),  C(145, 43, 129),  C(147, 43, 128),
                    C(148, 44, 128),  C(150, 44, 128),  C(152, 45, 128),
                    C(153, 45, 128),  C(155, 46, 127),  C(156, 46, 127),
                    C(158, 47, 127),  C(160, 47, 127),  C(161, 48, 126),
                    C(163, 48, 126),  C(165, 49, 126),  C(166, 49, 125),
                    C(168, 50, 125),  C(170, 51, 125),  C(171, 51, 124),
                    C(173, 52, 124),  C(174, 52, 123),  C(176, 53, 123),
                    C(178, 53, 123),  C(179, 54, 122),  C(181, 54, 122),
                    C(183, 55, 121),  C(184, 55, 121),  C(186, 56, 120),
                    C(188, 57, 120),  C(189, 57, 119),  C(191, 58, 119),
                    C(192, 58, 118),  C(194, 59, 117),  C(196, 60, 117),
                    C(197, 60, 116),  C(199, 61, 115),  C(200, 62, 115),
                    C(202, 62, 114),  C(204, 63, 113),  C(205, 64, 113),
                    C(207, 64, 112),  C(208, 65, 111),  C(210, 66, 111),
                    C(211, 67, 110),  C(213, 68, 109),  C(214, 69, 108),
                    C(216, 69, 108),  C(217, 70, 107),  C(219, 71, 106),
                    C(220, 72, 105),  C(222, 73, 104),  C(223, 74, 104),
                    C(224, 76, 103),  C(226, 77, 102),  C(227, 78, 101),
                    C(228, 79, 100),  C(229, 80, 100),  C(231, 82, 99),
                    C(232, 83, 98),   C(233, 84, 98),   C(234, 86, 97),
                    C(235, 87, 96),   C(236, 88, 96),   C(237, 90, 95),
                    C(238, 91, 94),   C(239, 93, 94),   C(240, 95, 94),
                    C(241, 96, 93),   C(242, 98, 93),   C(242, 100, 92),
                    C(243, 101, 92),  C(244, 103, 92),  C(244, 105, 92),
                    C(245, 107, 92),  C(246, 108, 92),  C(246, 110, 92),
                    C(247, 112, 92),  C(247, 114, 92),  C(248, 116, 92),
                    C(248, 118, 92),  C(249, 120, 93),  C(249, 121, 93),
                    C(249, 123, 93),  C(250, 125, 94),  C(250, 127, 94),
                    C(250, 129, 95),  C(251, 131, 95),  C(251, 133, 96),
                    C(251, 135, 97),  C(252, 137, 97),  C(252, 138, 98),
                    C(252, 140, 99),  C(252, 142, 100), C(252, 144, 101),
                    C(253, 146, 102), C(253, 148, 103), C(253, 150, 104),
                    C(253, 152, 105), C(253, 154, 106), C(253, 155, 107),
                    C(254, 157, 108), C(254, 159, 109), C(254, 161, 110),
                    C(254, 163, 111), C(254, 165, 113), C(254, 167, 114),
                    C(254, 169, 115), C(254, 170, 116), C(254, 172, 118),
                    C(254, 174, 119), C(254, 176, 120), C(254, 178, 122),
                    C(254, 180, 123), C(254, 182, 124), C(254, 183, 126),
                    C(254, 185, 127), C(254, 187, 129), C(254, 189, 130),
                    C(254, 191, 132), C(254, 193, 133), C(254, 194, 135),
                    C(254, 196, 136), C(254, 198, 138), C(254, 200, 140),
                    C(254, 202, 141), C(254, 204, 143), C(254, 205, 144),
                    C(254, 207, 146), C(254, 209, 148), C(254, 211, 149),
                    C(254, 213, 151), C(254, 215, 153), C(254, 216, 154),
                    C(253, 218, 156), C(253, 220, 158), C(253, 222, 160),
                    C(253, 224, 161), C(253, 226, 163), C(253, 227, 165),
                    C(253, 229, 167), C(253, 231, 169), C(253, 233, 170),
                    C(253, 235, 172), C(252, 236, 174), C(252, 238, 176),
                    C(252, 240, 178), C(252, 242, 180), C(252, 244, 182),
                    C(252, 246, 184), C(252, 247, 185), C(252, 249, 187),
                    C(252, 251, 189), C(252, 253, 191)};
            return p;
        }
        const Palette&
        Inferno()
        {
            static const auto p = Palette {
                    "Inferno",        C(0, 0, 4),       C(1, 0, 5),
                    C(1, 1, 6),       C(1, 1, 8),       C(2, 1, 10),
                    C(2, 2, 12),      C(2, 2, 14),      C(3, 2, 16),
                    C(4, 3, 18),      C(4, 3, 20),      C(5, 4, 23),
                    C(6, 4, 25),      C(7, 5, 27),      C(8, 5, 29),
                    C(9, 6, 31),      C(10, 7, 34),     C(11, 7, 36),
                    C(12, 8, 38),     C(13, 8, 41),     C(14, 9, 43),
                    C(16, 9, 45),     C(17, 10, 48),    C(18, 10, 50),
                    C(20, 11, 52),    C(21, 11, 55),    C(22, 11, 57),
                    C(24, 12, 60),    C(25, 12, 62),    C(27, 12, 65),
                    C(28, 12, 67),    C(30, 12, 69),    C(31, 12, 72),
                    C(33, 12, 74),    C(35, 12, 76),    C(36, 12, 79),
                    C(38, 12, 81),    C(40, 11, 83),    C(41, 11, 85),
                    C(43, 11, 87),    C(45, 11, 89),    C(47, 10, 91),
                    C(49, 10, 92),    C(50, 10, 94),    C(52, 10, 95),
                    C(54, 9, 97),     C(56, 9, 98),     C(57, 9, 99),
                    C(59, 9, 100),    C(61, 9, 101),    C(62, 9, 102),
                    C(64, 10, 103),   C(66, 10, 104),   C(68, 10, 104),
                    C(69, 10, 105),   C(71, 11, 106),   C(73, 11, 106),
                    C(74, 12, 107),   C(76, 12, 107),   C(77, 13, 108),
                    C(79, 13, 108),   C(81, 14, 108),   C(82, 14, 109),
                    C(84, 15, 109),   C(85, 15, 109),   C(87, 16, 110),
                    C(89, 16, 110),   C(90, 17, 110),   C(92, 18, 110),
                    C(93, 18, 110),   C(95, 19, 110),   C(97, 19, 110),
                    C(98, 20, 110),   C(100, 21, 110),  C(101, 21, 110),
                    C(103, 22, 110),  C(105, 22, 110),  C(106, 23, 110),
                    C(108, 24, 110),  C(109, 24, 110),  C(111, 25, 110),
                    C(113, 25, 110),  C(114, 26, 110),  C(116, 26, 110),
                    C(117, 27, 110),  C(119, 28, 109),  C(120, 28, 109),
                    C(122, 29, 109),  C(124, 29, 109),  C(125, 30, 109),
                    C(127, 30, 108),  C(128, 31, 108),  C(130, 32, 108),
                    C(132, 32, 107),  C(133, 33, 107),  C(135, 33, 107),
                    C(136, 34, 106),  C(138, 34, 106),  C(140, 35, 105),
                    C(141, 35, 105),  C(143, 36, 105),  C(144, 37, 104),
                    C(146, 37, 104),  C(147, 38, 103),  C(149, 38, 103),
                    C(151, 39, 102),  C(152, 39, 102),  C(154, 40, 101),
                    C(155, 41, 100),  C(157, 41, 100),  C(159, 42, 99),
                    C(160, 42, 99),   C(162, 43, 98),   C(163, 44, 97),
                    C(165, 44, 96),   C(166, 45, 96),   C(168, 46, 95),
                    C(169, 46, 94),   C(171, 47, 94),   C(173, 48, 93),
                    C(174, 48, 92),   C(176, 49, 91),   C(177, 50, 90),
                    C(179, 50, 90),   C(180, 51, 89),   C(182, 52, 88),
                    C(183, 53, 87),   C(185, 53, 86),   C(186, 54, 85),
                    C(188, 55, 84),   C(189, 56, 83),   C(191, 57, 82),
                    C(192, 58, 81),   C(193, 58, 80),   C(195, 59, 79),
                    C(196, 60, 78),   C(198, 61, 77),   C(199, 62, 76),
                    C(200, 63, 75),   C(202, 64, 74),   C(203, 65, 73),
                    C(204, 66, 72),   C(206, 67, 71),   C(207, 68, 70),
                    C(208, 69, 69),   C(210, 70, 68),   C(211, 71, 67),
                    C(212, 72, 66),   C(213, 74, 65),   C(215, 75, 63),
                    C(216, 76, 62),   C(217, 77, 61),   C(218, 78, 60),
                    C(219, 80, 59),   C(221, 81, 58),   C(222, 82, 56),
                    C(223, 83, 55),   C(224, 85, 54),   C(225, 86, 53),
                    C(226, 87, 52),   C(227, 89, 51),   C(228, 90, 49),
                    C(229, 92, 48),   C(230, 93, 47),   C(231, 94, 46),
                    C(232, 96, 45),   C(233, 97, 43),   C(234, 99, 42),
                    C(235, 100, 41),  C(235, 102, 40),  C(236, 103, 38),
                    C(237, 105, 37),  C(238, 106, 36),  C(239, 108, 35),
                    C(239, 110, 33),  C(240, 111, 32),  C(241, 113, 31),
                    C(241, 115, 29),  C(242, 116, 28),  C(243, 118, 27),
                    C(243, 120, 25),  C(244, 121, 24),  C(245, 123, 23),
                    C(245, 125, 21),  C(246, 126, 20),  C(246, 128, 19),
                    C(247, 130, 18),  C(247, 132, 16),  C(248, 133, 15),
                    C(248, 135, 14),  C(248, 137, 12),  C(249, 139, 11),
                    C(249, 140, 10),  C(249, 142, 9),   C(250, 144, 8),
                    C(250, 146, 7),   C(250, 148, 7),   C(251, 150, 6),
                    C(251, 151, 6),   C(251, 153, 6),   C(251, 155, 6),
                    C(251, 157, 7),   C(252, 159, 7),   C(252, 161, 8),
                    C(252, 163, 9),   C(252, 165, 10),  C(252, 166, 12),
                    C(252, 168, 13),  C(252, 170, 15),  C(252, 172, 17),
                    C(252, 174, 18),  C(252, 176, 20),  C(252, 178, 22),
                    C(252, 180, 24),  C(251, 182, 26),  C(251, 184, 29),
                    C(251, 186, 31),  C(251, 188, 33),  C(251, 190, 35),
                    C(250, 192, 38),  C(250, 194, 40),  C(250, 196, 42),
                    C(250, 198, 45),  C(249, 199, 47),  C(249, 201, 50),
                    C(249, 203, 53),  C(248, 205, 55),  C(248, 207, 58),
                    C(247, 209, 61),  C(247, 211, 64),  C(246, 213, 67),
                    C(246, 215, 70),  C(245, 217, 73),  C(245, 219, 76),
                    C(244, 221, 79),  C(244, 223, 83),  C(244, 225, 86),
                    C(243, 227, 90),  C(243, 229, 93),  C(242, 230, 97),
                    C(242, 232, 101), C(242, 234, 105), C(241, 236, 109),
                    C(241, 237, 113), C(241, 239, 117), C(241, 241, 121),
                    C(242, 242, 125), C(242, 244, 130), C(243, 245, 134),
                    C(243, 246, 138), C(244, 248, 142), C(245, 249, 146),
                    C(246, 250, 150), C(248, 251, 154), C(249, 252, 157),
                    C(250, 253, 161), C(252, 255, 164)};
            return p;
        }
        const Palette&
        Plasma()
        {
            static const auto p = Palette {
                    "Plasma",        C(13, 8, 135),   C(16, 7, 136),
                    C(19, 7, 137),   C(22, 7, 138),   C(25, 6, 140),
                    C(27, 6, 141),   C(29, 6, 142),   C(32, 6, 143),
                    C(34, 6, 144),   C(36, 6, 145),   C(38, 5, 145),
                    C(40, 5, 146),   C(42, 5, 147),   C(44, 5, 148),
                    C(46, 5, 149),   C(47, 5, 150),   C(49, 5, 151),
                    C(51, 5, 151),   C(53, 4, 152),   C(55, 4, 153),
                    C(56, 4, 154),   C(58, 4, 154),   C(60, 4, 155),
                    C(62, 4, 156),   C(63, 4, 156),   C(65, 4, 157),
                    C(67, 3, 158),   C(68, 3, 158),   C(70, 3, 159),
                    C(72, 3, 159),   C(73, 3, 160),   C(75, 3, 161),
                    C(76, 2, 161),   C(78, 2, 162),   C(80, 2, 162),
                    C(81, 2, 163),   C(83, 2, 163),   C(85, 2, 164),
                    C(86, 1, 164),   C(88, 1, 164),   C(89, 1, 165),
                    C(91, 1, 165),   C(92, 1, 166),   C(94, 1, 166),
                    C(96, 1, 166),   C(97, 0, 167),   C(99, 0, 167),
                    C(100, 0, 167),  C(102, 0, 167),  C(103, 0, 168),
                    C(105, 0, 168),  C(106, 0, 168),  C(108, 0, 168),
                    C(110, 0, 168),  C(111, 0, 168),  C(113, 0, 168),
                    C(114, 1, 168),  C(116, 1, 168),  C(117, 1, 168),
                    C(119, 1, 168),  C(120, 1, 168),  C(122, 2, 168),
                    C(123, 2, 168),  C(125, 3, 168),  C(126, 3, 168),
                    C(128, 4, 168),  C(129, 4, 167),  C(131, 5, 167),
                    C(132, 5, 167),  C(134, 6, 166),  C(135, 7, 166),
                    C(136, 8, 166),  C(138, 9, 165),  C(139, 10, 165),
                    C(141, 11, 165), C(142, 12, 164), C(143, 13, 164),
                    C(145, 14, 163), C(146, 15, 163), C(148, 16, 162),
                    C(149, 17, 161), C(150, 19, 161), C(152, 20, 160),
                    C(153, 21, 159), C(154, 22, 159), C(156, 23, 158),
                    C(157, 24, 157), C(158, 25, 157), C(160, 26, 156),
                    C(161, 27, 155), C(162, 29, 154), C(163, 30, 154),
                    C(165, 31, 153), C(166, 32, 152), C(167, 33, 151),
                    C(168, 34, 150), C(170, 35, 149), C(171, 36, 148),
                    C(172, 38, 148), C(173, 39, 147), C(174, 40, 146),
                    C(176, 41, 145), C(177, 42, 144), C(178, 43, 143),
                    C(179, 44, 142), C(180, 46, 141), C(181, 47, 140),
                    C(182, 48, 139), C(183, 49, 138), C(184, 50, 137),
                    C(186, 51, 136), C(187, 52, 136), C(188, 53, 135),
                    C(189, 55, 134), C(190, 56, 133), C(191, 57, 132),
                    C(192, 58, 131), C(193, 59, 130), C(194, 60, 129),
                    C(195, 61, 128), C(196, 62, 127), C(197, 64, 126),
                    C(198, 65, 125), C(199, 66, 124), C(200, 67, 123),
                    C(201, 68, 122), C(202, 69, 122), C(203, 70, 121),
                    C(204, 71, 120), C(204, 73, 119), C(205, 74, 118),
                    C(206, 75, 117), C(207, 76, 116), C(208, 77, 115),
                    C(209, 78, 114), C(210, 79, 113), C(211, 81, 113),
                    C(212, 82, 112), C(213, 83, 111), C(213, 84, 110),
                    C(214, 85, 109), C(215, 86, 108), C(216, 87, 107),
                    C(217, 88, 106), C(218, 90, 106), C(218, 91, 105),
                    C(219, 92, 104), C(220, 93, 103), C(221, 94, 102),
                    C(222, 95, 101), C(222, 97, 100), C(223, 98, 99),
                    C(224, 99, 99),  C(225, 100, 98), C(226, 101, 97),
                    C(226, 102, 96), C(227, 104, 95), C(228, 105, 94),
                    C(229, 106, 93), C(229, 107, 93), C(230, 108, 92),
                    C(231, 110, 91), C(231, 111, 90), C(232, 112, 89),
                    C(233, 113, 88), C(233, 114, 87), C(234, 116, 87),
                    C(235, 117, 86), C(235, 118, 85), C(236, 119, 84),
                    C(237, 121, 83), C(237, 122, 82), C(238, 123, 81),
                    C(239, 124, 81), C(239, 126, 80), C(240, 127, 79),
                    C(240, 128, 78), C(241, 129, 77), C(241, 131, 76),
                    C(242, 132, 75), C(243, 133, 75), C(243, 135, 74),
                    C(244, 136, 73), C(244, 137, 72), C(245, 139, 71),
                    C(245, 140, 70), C(246, 141, 69), C(246, 143, 68),
                    C(247, 144, 68), C(247, 145, 67), C(247, 147, 66),
                    C(248, 148, 65), C(248, 149, 64), C(249, 151, 63),
                    C(249, 152, 62), C(249, 154, 62), C(250, 155, 61),
                    C(250, 156, 60), C(250, 158, 59), C(251, 159, 58),
                    C(251, 161, 57), C(251, 162, 56), C(252, 163, 56),
                    C(252, 165, 55), C(252, 166, 54), C(252, 168, 53),
                    C(252, 169, 52), C(253, 171, 51), C(253, 172, 51),
                    C(253, 174, 50), C(253, 175, 49), C(253, 177, 48),
                    C(253, 178, 47), C(253, 180, 47), C(253, 181, 46),
                    C(254, 183, 45), C(254, 184, 44), C(254, 186, 44),
                    C(254, 187, 43), C(254, 189, 42), C(254, 190, 42),
                    C(254, 192, 41), C(253, 194, 41), C(253, 195, 40),
                    C(253, 197, 39), C(253, 198, 39), C(253, 200, 39),
                    C(253, 202, 38), C(253, 203, 38), C(252, 205, 37),
                    C(252, 206, 37), C(252, 208, 37), C(252, 210, 37),
                    C(251, 211, 36), C(251, 213, 36), C(251, 215, 36),
                    C(250, 216, 36), C(250, 218, 36), C(249, 220, 36),
                    C(249, 221, 37), C(248, 223, 37), C(248, 225, 37),
                    C(247, 226, 37), C(247, 228, 37), C(246, 230, 38),
                    C(246, 232, 38), C(245, 233, 38), C(245, 235, 39),
                    C(244, 237, 39), C(243, 238, 39), C(243, 240, 39),
                    C(242, 242, 39), C(241, 244, 38), C(241, 245, 37),
                    C(240, 247, 36), C(240, 249, 33)};
            return p;
        }
        const Palette&
        Viridis()
        {
            static const auto p = Palette {
                    "Viridis",       C(68, 1, 84),    C(68, 2, 86),
                    C(69, 4, 87),    C(69, 5, 89),    C(70, 7, 90),
                    C(70, 8, 92),    C(70, 10, 93),   C(70, 11, 94),
                    C(71, 13, 96),   C(71, 14, 97),   C(71, 16, 99),
                    C(71, 17, 100),  C(71, 19, 101),  C(72, 20, 103),
                    C(72, 22, 104),  C(72, 23, 105),  C(72, 24, 106),
                    C(72, 26, 108),  C(72, 27, 109),  C(72, 28, 110),
                    C(72, 29, 111),  C(72, 31, 112),  C(72, 32, 113),
                    C(72, 33, 115),  C(72, 35, 116),  C(72, 36, 117),
                    C(72, 37, 118),  C(72, 38, 119),  C(72, 40, 120),
                    C(72, 41, 121),  C(71, 42, 122),  C(71, 44, 122),
                    C(71, 45, 123),  C(71, 46, 124),  C(71, 47, 125),
                    C(70, 48, 126),  C(70, 50, 126),  C(70, 51, 127),
                    C(70, 52, 128),  C(69, 53, 129),  C(69, 55, 129),
                    C(69, 56, 130),  C(68, 57, 131),  C(68, 58, 131),
                    C(68, 59, 132),  C(67, 61, 132),  C(67, 62, 133),
                    C(66, 63, 133),  C(66, 64, 134),  C(66, 65, 134),
                    C(65, 66, 135),  C(65, 68, 135),  C(64, 69, 136),
                    C(64, 70, 136),  C(63, 71, 136),  C(63, 72, 137),
                    C(62, 73, 137),  C(62, 74, 137),  C(62, 76, 138),
                    C(61, 77, 138),  C(61, 78, 138),  C(60, 79, 138),
                    C(60, 80, 139),  C(59, 81, 139),  C(59, 82, 139),
                    C(58, 83, 139),  C(58, 84, 140),  C(57, 85, 140),
                    C(57, 86, 140),  C(56, 88, 140),  C(56, 89, 140),
                    C(55, 90, 140),  C(55, 91, 141),  C(54, 92, 141),
                    C(54, 93, 141),  C(53, 94, 141),  C(53, 95, 141),
                    C(52, 96, 141),  C(52, 97, 141),  C(51, 98, 141),
                    C(51, 99, 141),  C(50, 100, 142), C(50, 101, 142),
                    C(49, 102, 142), C(49, 103, 142), C(49, 104, 142),
                    C(48, 105, 142), C(48, 106, 142), C(47, 107, 142),
                    C(47, 108, 142), C(46, 109, 142), C(46, 110, 142),
                    C(46, 111, 142), C(45, 112, 142), C(45, 113, 142),
                    C(44, 113, 142), C(44, 114, 142), C(44, 115, 142),
                    C(43, 116, 142), C(43, 117, 142), C(42, 118, 142),
                    C(42, 119, 142), C(42, 120, 142), C(41, 121, 142),
                    C(41, 122, 142), C(41, 123, 142), C(40, 124, 142),
                    C(40, 125, 142), C(39, 126, 142), C(39, 127, 142),
                    C(39, 128, 142), C(38, 129, 142), C(38, 130, 142),
                    C(38, 130, 142), C(37, 131, 142), C(37, 132, 142),
                    C(37, 133, 142), C(36, 134, 142), C(36, 135, 142),
                    C(35, 136, 142), C(35, 137, 142), C(35, 138, 141),
                    C(34, 139, 141), C(34, 140, 141), C(34, 141, 141),
                    C(33, 142, 141), C(33, 143, 141), C(33, 144, 141),
                    C(33, 145, 140), C(32, 146, 140), C(32, 146, 140),
                    C(32, 147, 140), C(31, 148, 140), C(31, 149, 139),
                    C(31, 150, 139), C(31, 151, 139), C(31, 152, 139),
                    C(31, 153, 138), C(31, 154, 138), C(30, 155, 138),
                    C(30, 156, 137), C(30, 157, 137), C(31, 158, 137),
                    C(31, 159, 136), C(31, 160, 136), C(31, 161, 136),
                    C(31, 161, 135), C(31, 162, 135), C(32, 163, 134),
                    C(32, 164, 134), C(33, 165, 133), C(33, 166, 133),
                    C(34, 167, 133), C(34, 168, 132), C(35, 169, 131),
                    C(36, 170, 131), C(37, 171, 130), C(37, 172, 130),
                    C(38, 173, 129), C(39, 173, 129), C(40, 174, 128),
                    C(41, 175, 127), C(42, 176, 127), C(44, 177, 126),
                    C(45, 178, 125), C(46, 179, 124), C(47, 180, 124),
                    C(49, 181, 123), C(50, 182, 122), C(52, 182, 121),
                    C(53, 183, 121), C(55, 184, 120), C(56, 185, 119),
                    C(58, 186, 118), C(59, 187, 117), C(61, 188, 116),
                    C(63, 188, 115), C(64, 189, 114), C(66, 190, 113),
                    C(68, 191, 112), C(70, 192, 111), C(72, 193, 110),
                    C(74, 193, 109), C(76, 194, 108), C(78, 195, 107),
                    C(80, 196, 106), C(82, 197, 105), C(84, 197, 104),
                    C(86, 198, 103), C(88, 199, 101), C(90, 200, 100),
                    C(92, 200, 99),  C(94, 201, 98),  C(96, 202, 96),
                    C(99, 203, 95),  C(101, 203, 94), C(103, 204, 92),
                    C(105, 205, 91), C(108, 205, 90), C(110, 206, 88),
                    C(112, 207, 87), C(115, 208, 86), C(117, 208, 84),
                    C(119, 209, 83), C(122, 209, 81), C(124, 210, 80),
                    C(127, 211, 78), C(129, 211, 77), C(132, 212, 75),
                    C(134, 213, 73), C(137, 213, 72), C(139, 214, 70),
                    C(142, 214, 69), C(144, 215, 67), C(147, 215, 65),
                    C(149, 216, 64), C(152, 216, 62), C(155, 217, 60),
                    C(157, 217, 59), C(160, 218, 57), C(162, 218, 55),
                    C(165, 219, 54), C(168, 219, 52), C(170, 220, 50),
                    C(173, 220, 48), C(176, 221, 47), C(178, 221, 45),
                    C(181, 222, 43), C(184, 222, 41), C(186, 222, 40),
                    C(189, 223, 38), C(192, 223, 37), C(194, 223, 35),
                    C(197, 224, 33), C(200, 224, 32), C(202, 225, 31),
                    C(205, 225, 29), C(208, 225, 28), C(210, 226, 27),
                    C(213, 226, 26), C(216, 226, 25), C(218, 227, 25),
                    C(221, 227, 24), C(223, 227, 24), C(226, 228, 24),
                    C(229, 228, 25), C(231, 228, 25), C(234, 229, 26),
                    C(236, 229, 27), C(239, 229, 28), C(241, 229, 29),
                    C(244, 230, 30), C(246, 230, 32), C(248, 230, 33),
                    C(251, 231, 35), C(253, 231, 37)};
            return p;
        }
    }  // namespace palette

}  // namespace euphoria::core