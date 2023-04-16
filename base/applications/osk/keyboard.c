/*
 * PROJECT:         MergeOS On-Screen Keyboard
 * LICENSE:         GPL-2.0-or-later (https://spdx.org/licenses/GPL-2.0-or-later)
 * PURPOSE:         Keyboard layout data.
 * COPYRIGHT:       Baruch Rutman (peterooch at gmail dot com)
 * 
 * This is partialy based on the original en-US dialog templates.
 */
#include "precomp.h"

KEY EnhancedKeyboardKeys[] = 
{
    {L"Esc", SCAN_CODE_110, 4, 4, 28, 28, 0, FALSE},
    {L"F1", SCAN_CODE_112, 68, 4, 28, 28, 0, FALSE},
    {L"F2", SCAN_CODE_113, 100, 4, 28, 28, 0, FALSE},
    {L"F3", SCAN_CODE_114, 132, 4, 28, 28, 0, FALSE},
    {L"F4", SCAN_CODE_115, 164, 4, 28, 28, 0, FALSE},
    {L"F5", SCAN_CODE_116, 228, 4, 28, 28, 0, FALSE},
    {L"F6", SCAN_CODE_117, 260, 4, 28, 28, 0, FALSE},
    {L"F7", SCAN_CODE_118, 292, 4, 28, 28, 0, FALSE},
    {L"F8", SCAN_CODE_119, 324, 4, 28, 28, 0, FALSE},
    {L"F9", SCAN_CODE_120, 388, 4, 28, 28, 0, FALSE},
    {L"F10", SCAN_CODE_121, 420, 4, 28, 28, 0, FALSE},
    {L"F11", SCAN_CODE_122, 452, 4, 28, 28, 0, FALSE},
    {L"F12", SCAN_CODE_123, 484, 4, 28, 28, 0, FALSE},
    {L"Prn", SCAN_CODE_124, 516, 4, 28, 28, 0, FALSE},
    {L"Stop", SCAN_CODE_125, 548, 4, 28, 28, 0, FALSE},
    {L"Attn", SCAN_CODE_126, 580, 4, 28, 28, 0, FALSE},
    {L"`", SCAN_CODE_1, 4, 35, 28, 28, 0, TRUE},
    {L"1", SCAN_CODE_2, 36, 35, 28, 28, 0, TRUE},
    {L"2", SCAN_CODE_3, 68, 35, 28, 28, 0, TRUE},
    {L"3", SCAN_CODE_4, 100, 35, 28, 28, 0, TRUE},
    {L"4", SCAN_CODE_5, 132, 35, 28, 28, 0, TRUE},
    {L"5", SCAN_CODE_6, 164, 35, 28, 28, 0, TRUE},
    {L"6", SCAN_CODE_7, 196, 35, 28, 28, 0, TRUE},
    {L"7", SCAN_CODE_8, 228, 35, 28, 28, 0, TRUE},
    {L"8", SCAN_CODE_9, 260, 35, 28, 28, 0, TRUE},
    {L"9", SCAN_CODE_10, 292, 35, 28, 28, 0, TRUE},
    {L"0", SCAN_CODE_11, 324, 35, 28, 28, 0, TRUE},
    {L"-", SCAN_CODE_12, 356, 35, 28, 28, 0, TRUE},
    {L"=", SCAN_CODE_13, 388, 35, 28, 28, 0, TRUE},
    {L"<--", SCAN_CODE_15, 420, 35, 92, 28, BS_ICON, FALSE},
    {L"Tab", SCAN_CODE_16, 4, 65, 60, 28, BS_ICON, FALSE},
    {L"q", SCAN_CODE_17, 67, 65, 28, 28, 0, TRUE},
    {L"w", SCAN_CODE_18, 97, 65, 28, 28, 0, TRUE},
    {L"e", SCAN_CODE_19, 128, 65, 28, 28, 0, TRUE},
    {L"r", SCAN_CODE_20, 158, 65, 28, 28, 0, TRUE},
    {L"t", SCAN_CODE_21, 188, 65, 28, 28, 0, TRUE},
    {L"y", SCAN_CODE_22, 219, 65, 28, 28, 0, TRUE},
    {L"u", SCAN_CODE_23, 249, 65, 28, 28, 0, TRUE},
    {L"i", SCAN_CODE_24, 280, 65, 28, 28, 0, TRUE},
    {L"o", SCAN_CODE_25, 310, 65, 28, 28, 0, TRUE},
    {L"p", SCAN_CODE_26, 340, 65, 28, 28, 0, TRUE},
    {L"[", SCAN_CODE_27, 371, 65, 28, 28, 0, TRUE},
    {L"]", SCAN_CODE_28, 401, 65, 28, 28, 0, TRUE},
    {L"lock", SCAN_CODE_30, 4, 96, 78, 28, BS_ICON, FALSE},
    {L"a", SCAN_CODE_31, 84, 96, 28, 28, 0, TRUE},
    {L"s", SCAN_CODE_32, 115, 96, 28, 28, 0, TRUE},
    {L"d", SCAN_CODE_33, 145, 96, 28, 28, 0, TRUE},
    {L"f", SCAN_CODE_34, 176, 96, 28, 28, 0, TRUE},
    {L"g", SCAN_CODE_35, 206, 96, 28, 28, 0, TRUE},
    {L"h", SCAN_CODE_36, 236, 96, 28, 28, 0, TRUE},
    {L"j", SCAN_CODE_37, 267, 96, 28, 28, 0, TRUE},
    {L"k", SCAN_CODE_38, 297, 96, 28, 28, 0, TRUE},
    {L"l", SCAN_CODE_39, 328, 96, 28, 28, 0, TRUE},
    {L";", SCAN_CODE_40, 358, 96, 28, 28, 0, TRUE},
    {L"'", SCAN_CODE_41, 388, 96, 28, 28, 0, TRUE},
    {L"\\", SCAN_CODE_29, 432, 65, 80, 28, 0, TRUE},
    {L"ret", SCAN_CODE_43, 419, 96, 93, 28, BS_ICON, FALSE},
    {L"shift", SCAN_CODE_44, 4, 126, 95, 28, BS_ICON|BS_PUSHLIKE|BS_AUTOCHECKBOX, FALSE},
    {L"z", SCAN_CODE_46, 102, 126, 28, 28, 0, TRUE},
    {L"x", SCAN_CODE_47, 132, 126, 28, 28, 0, TRUE},
    {L"c", SCAN_CODE_48, 163, 126, 28, 28, 0, TRUE},
    {L"v", SCAN_CODE_49, 193, 126, 28, 28, 0, TRUE},
    {L"b", SCAN_CODE_50, 224, 126, 28, 28, 0, TRUE},
    {L"n", SCAN_CODE_51, 254, 126, 28, 28, 0, TRUE},
    {L"m", SCAN_CODE_52, 284, 126, 28, 28, 0, TRUE},
    {L",", SCAN_CODE_53, 315, 126, 28, 28, 0, TRUE},
    {L".", SCAN_CODE_54, 345, 126, 28, 28, 0, TRUE},
    {L"/", SCAN_CODE_55, 376, 126, 28, 28, 0, TRUE},
    {L"shift", SCAN_CODE_57, 406, 126, 106, 28, BS_ICON|BS_PUSHLIKE|BS_AUTOCHECKBOX, FALSE},
    {L"ctrl", SCAN_CODE_58, 4, 156, 43, 28, BS_PUSHLIKE|BS_AUTOCHECKBOX, FALSE},
    {L"v", SCAN_CODE_84, 548, 156, 28, 28, BS_ICON, FALSE},
    {L"ROS", SCAN_CODE_127, 48, 156, 43, 28, BS_ICON|BS_PUSHLIKE|BS_AUTOCHECKBOX, FALSE},
    {L"alt", SCAN_CODE_60, 91, 156, 43, 28, BS_PUSHLIKE|BS_AUTOCHECKBOX, FALSE},
    {L"", SCAN_CODE_61, 134, 156, 204, 28, 0, FALSE},
    {L"alt", SCAN_CODE_62, 340, 156, 43, 28, BS_PUSHLIKE|BS_AUTOCHECKBOX, FALSE},
    {L"ROS", SCAN_CODE_128, 384, 156, 43, 28, BS_ICON|BS_PUSHLIKE|BS_AUTOCHECKBOX, FALSE},
    {L"menu", SCAN_CODE_129, 427, 156, 43, 28, BS_ICON, FALSE},
    {L"ctrl", SCAN_CODE_64, 470, 156, 42, 28, BS_PUSHLIKE|BS_AUTOCHECKBOX, FALSE},
    {L"ins", SCAN_CODE_75, 516, 35, 28, 28, 0, FALSE},
    {L"del", SCAN_CODE_76, 516, 65, 28, 28, 0, FALSE},
    {L"hm", SCAN_CODE_80, 548, 35, 28, 28, BS_ICON, FALSE},
    {L"end", SCAN_CODE_81, 548, 65, 28, 28, 0, FALSE},
    {L"pup", SCAN_CODE_85, 580, 35, 28, 28, BS_ICON, FALSE},
    {L"pdn", SCAN_CODE_86, 580, 65, 28, 28, BS_ICON, FALSE},
    {L"<-", SCAN_CODE_79, 516, 156, 28, 28, BS_ICON, FALSE},
    {L"^", SCAN_CODE_83, 548, 126, 28, 28, BS_ICON, FALSE},
    {L"->", SCAN_CODE_89, 580, 156, 28, 28, BS_ICON, FALSE},
    {L"nlk", SCAN_CODE_90, 612, 35, 28, 28, 0, FALSE},
    {L"7", SCAN_CODE_91, 612, 65, 28, 28, 0, FALSE},
    {L"4", SCAN_CODE_92, 612, 96, 28, 28, 0, FALSE},
    {L"1", SCAN_CODE_93, 612, 126, 28, 28, 0, FALSE},
    {L"/", SCAN_CODE_95, 644, 35, 28, 28, 0, FALSE},
    {L"8", SCAN_CODE_96, 644, 65, 28, 28, 0, FALSE},
    {L"5", SCAN_CODE_97, 644, 96, 28, 28, 0, FALSE},
    {L"2", SCAN_CODE_98, 644, 126, 28, 28, 0, FALSE},
    {L"0", SCAN_CODE_99, 612, 156, 60, 28, 0, FALSE},
    {L"*", SCAN_CODE_100, 676, 35, 28, 28, 0, FALSE},
    {L"9", SCAN_CODE_101, 676, 65, 28, 28, 0, FALSE},
    {L"6", SCAN_CODE_102, 676, 96, 28, 28, 0, FALSE},
    {L"3", SCAN_CODE_103, 676, 126, 28, 28, 0, FALSE},
    {L".", SCAN_CODE_104, 676, 156, 28, 28, 0, FALSE},
    {L"-", SCAN_CODE_105, 708, 35, 28, 28, 0, FALSE},
    {L"+", SCAN_CODE_106, 708, 65, 28, 59, 0, FALSE},
    {L"ent", SCAN_CODE_108, 708, 126, 28, 58, 0, FALSE},
};

KEY StandardKeyboardKeys[] = 
{
    {L"Esc", SCAN_CODE_110, 4, 4, 28, 28, 0, FALSE},
    {L"F1", SCAN_CODE_112, 68, 4, 28, 28, 0, FALSE},
    {L"F2", SCAN_CODE_113, 100, 4, 28, 28, 0, FALSE},
    {L"F3", SCAN_CODE_114, 132, 4, 28, 28, 0, FALSE},
    {L"F4", SCAN_CODE_115, 164, 4, 28, 28, 0, FALSE},
    {L"F5", SCAN_CODE_116, 196, 4, 28, 28, 0, FALSE},
    {L"F6", SCAN_CODE_117, 228, 4, 28, 28, 0, FALSE},
    {L"F7", SCAN_CODE_118, 292, 4, 28, 28, 0, FALSE},
    {L"F8", SCAN_CODE_119, 324, 4, 28, 28, 0, FALSE},
    {L"F9", SCAN_CODE_120, 356, 4, 28, 28, 0, FALSE},
    {L"F10", SCAN_CODE_121, 388, 4, 28, 28, 0, FALSE},
    {L"F11", SCAN_CODE_122, 420, 4, 28, 28, 0, FALSE},
    {L"F12", SCAN_CODE_123, 452, 4, 28, 28, 0, FALSE},
    {L"Prn", SCAN_CODE_124, 486, 4, 28, 28, 0, FALSE},
    {L"Stop", SCAN_CODE_125, 518, 4, 28, 28, 0, FALSE},
    {L"Attn", SCAN_CODE_126, 550, 4, 28, 28, 0, FALSE},
    {L"`", SCAN_CODE_1, 4, 35, 28, 28, 0, TRUE},
    {L"1", SCAN_CODE_2, 36, 35, 28, 28, 0, TRUE},
    {L"2", SCAN_CODE_3, 68, 35, 28, 28, 0, TRUE},
    {L"3", SCAN_CODE_4, 100, 35, 28, 28, 0, TRUE},
    {L"4", SCAN_CODE_5, 132, 35, 28, 28, 0, TRUE},
    {L"5", SCAN_CODE_6, 164, 35, 28, 28, 0, TRUE},
    {L"6", SCAN_CODE_7, 196, 35, 28, 28, 0, TRUE},
    {L"7", SCAN_CODE_8, 228, 35, 28, 28, 0, TRUE},
    {L"8", SCAN_CODE_9, 260, 35, 28, 28, 0, TRUE},
    {L"9", SCAN_CODE_10, 292, 35, 28, 28, 0, TRUE},
    {L"0", SCAN_CODE_11, 324, 35, 28, 28, 0, TRUE},
    {L"-", SCAN_CODE_12, 356, 35, 28, 28, 0, TRUE},
    {L"=", SCAN_CODE_13, 388, 35, 28, 28, 0, TRUE},
    {L"<--", SCAN_CODE_15, 420, 35, 63, 28, BS_ICON, FALSE},
    {L"Tab", SCAN_CODE_16, 4, 65, 60, 28, BS_ICON, FALSE},
    {L"q", SCAN_CODE_17, 67, 65, 28, 28, 0, TRUE},
    {L"w", SCAN_CODE_18, 97, 65, 28, 28, 0, TRUE},
    {L"e", SCAN_CODE_19, 128, 65, 28, 28, 0, TRUE},
    {L"r", SCAN_CODE_20, 158, 65, 28, 28, 0, TRUE},
    {L"t", SCAN_CODE_21, 188, 65, 28, 28, 0, TRUE},
    {L"y", SCAN_CODE_22, 219, 65, 28, 28, 0, TRUE},
    {L"u", SCAN_CODE_23, 249, 65, 28, 28, 0, TRUE},
    {L"i", SCAN_CODE_24, 280, 65, 28, 28, 0, TRUE},
    {L"o", SCAN_CODE_25, 310, 65, 28, 28, 0, TRUE},
    {L"p", SCAN_CODE_26, 340, 65, 28, 28, 0, TRUE},
    {L"[", SCAN_CODE_27, 371, 65, 28, 28, 0, TRUE},
    {L"]", SCAN_CODE_28, 401, 65, 28, 28, 0, TRUE},
    {L"lock", SCAN_CODE_30, 4, 96, 78, 28, BS_ICON, FALSE},
    {L"a", SCAN_CODE_31, 84, 96, 28, 28, 0, TRUE},
    {L"s", SCAN_CODE_32, 115, 96, 28, 28, 0, TRUE},
    {L"d", SCAN_CODE_33, 145, 96, 28, 28, 0, TRUE},
    {L"f", SCAN_CODE_34, 176, 96, 28, 28, 0, TRUE},
    {L"g", SCAN_CODE_35, 206, 96, 28, 28, 0, TRUE},
    {L"h", SCAN_CODE_36, 236, 96, 28, 28, 0, TRUE},
    {L"j", SCAN_CODE_37, 267, 96, 28, 28, 0, TRUE},
    {L"k", SCAN_CODE_38, 297, 96, 28, 28, 0, TRUE},
    {L"l", SCAN_CODE_39, 328, 96, 28, 28, 0, TRUE},
    {L";", SCAN_CODE_40, 358, 96, 28, 28, 0, TRUE},
    {L"'", SCAN_CODE_41, 388, 96, 28, 28, 0, TRUE},
    {L"\\", SCAN_CODE_29, 432, 65, 51, 28, 0, TRUE},
    {L"ret", SCAN_CODE_43, 419, 96, 64, 28, BS_ICON, FALSE},
    {L"shift", SCAN_CODE_44, 4, 126, 96, 28, BS_ICON|BS_PUSHLIKE|BS_AUTOCHECKBOX, FALSE},
    {L"z", SCAN_CODE_46, 102, 126, 28, 28, 0, TRUE},
    {L"x", SCAN_CODE_47, 132, 126, 28, 28, 0, TRUE},
    {L"c", SCAN_CODE_48, 163, 126, 28, 28, 0, TRUE},
    {L"v", SCAN_CODE_49, 193, 126, 28, 28, 0, TRUE},
    {L"b", SCAN_CODE_50, 224, 126, 28, 28, 0, TRUE},
    {L"n", SCAN_CODE_51, 254, 126, 28, 28, 0, TRUE},
    {L"m", SCAN_CODE_52, 284, 126, 28, 28, 0, TRUE},
    {L",", SCAN_CODE_53, 315, 126, 28, 28, 0, TRUE},
    {L".", SCAN_CODE_54, 345, 126, 28, 28, 0, TRUE},
    {L"/", SCAN_CODE_55, 376, 126, 28, 28, 0, TRUE},
    {L"shift", SCAN_CODE_57, 406, 126, 77, 28, BS_ICON|BS_PUSHLIKE|BS_AUTOCHECKBOX, FALSE},
    {L"ctrl", SCAN_CODE_58, 4, 156, 43, 28, BS_PUSHLIKE|BS_AUTOCHECKBOX, FALSE},
    {L"v", SCAN_CODE_84, 518, 156, 28, 28, BS_ICON, FALSE},
    {L"ROS", SCAN_CODE_127, 48, 156, 43, 28, BS_ICON|BS_PUSHLIKE|BS_AUTOCHECKBOX, FALSE},
    {L"alt", SCAN_CODE_60, 91, 156, 43, 28, BS_PUSHLIKE|BS_AUTOCHECKBOX, FALSE},
    {L"", SCAN_CODE_61, 134, 156, 174, 28, 0, FALSE},
    {L"alt", SCAN_CODE_62, 310, 156, 43, 28, BS_PUSHLIKE|BS_AUTOCHECKBOX, FALSE},
    {L"ROS", SCAN_CODE_128, 353, 156, 43, 28, BS_ICON|BS_PUSHLIKE|BS_AUTOCHECKBOX, FALSE},
    {L"menu", SCAN_CODE_129, 396, 156, 43, 28, BS_ICON, FALSE},
    {L"ctrl", SCAN_CODE_64, 440, 156, 43, 28, BS_PUSHLIKE|BS_AUTOCHECKBOX, FALSE},
    {L"ins", SCAN_CODE_75, 486, 35, 28, 28, 0, FALSE},
    {L"del", SCAN_CODE_76, 486, 65, 28, 28, 0, FALSE},
    {L"hm", SCAN_CODE_80, 518, 35, 28, 28, BS_ICON, FALSE},
    {L"end", SCAN_CODE_81, 518, 65, 28, 28, 0, FALSE},
    {L"pup", SCAN_CODE_85, 550, 35, 28, 28, BS_ICON, FALSE},
    {L"pdn", SCAN_CODE_86, 550, 65, 28, 28, BS_ICON, FALSE},
    {L"<-", SCAN_CODE_79, 486, 156, 28, 28, BS_ICON, FALSE},
    {L"^", SCAN_CODE_83, 518, 126, 28, 28, BS_ICON, FALSE},
    {L"->", SCAN_CODE_89, 550, 156, 28, 28, BS_ICON, FALSE},
    {L"nlk", SCAN_CODE_90, 582, 35, 28, 28, 0, FALSE},
    {L"7", SCAN_CODE_91, 582, 65, 28, 28, 0, FALSE},
    {L"4", SCAN_CODE_92, 582, 96, 28, 28, 0, FALSE},
    {L"1", SCAN_CODE_93, 582, 126, 28, 28, 0, FALSE},
    {L"/", SCAN_CODE_95, 614, 35, 28, 28, 0, FALSE},
    {L"8", SCAN_CODE_96, 614, 65, 28, 28, 0, FALSE},
    {L"5", SCAN_CODE_97, 614, 96, 28, 28, 0, FALSE},
    {L"2", SCAN_CODE_98, 614, 126, 28, 28, 0, FALSE},
    {L"0", SCAN_CODE_99, 582, 156, 60, 28, 0, FALSE},
    {L"*", SCAN_CODE_100, 646, 35, 28, 28, 0, FALSE},
    {L"9", SCAN_CODE_101, 646, 65, 28, 28, 0, FALSE},
    {L"6", SCAN_CODE_102, 646, 96, 28, 28, 0, FALSE},
    {L"3", SCAN_CODE_103, 646, 126, 28, 28, 0, FALSE},
    {L".", SCAN_CODE_104, 646, 156, 28, 28, 0, FALSE},
    {L"-", SCAN_CODE_105, 678, 35, 28, 28, 0, FALSE},
    {L"+", SCAN_CODE_106, 678, 65, 28, 59, 0, FALSE},
    {L"ent", SCAN_CODE_108, 678, 126, 28, 58, 0, FALSE},
};

KEYBOARD_STRUCT EnhancedKeyboard = 
{ 
    EnhancedKeyboardKeys,
    _countof(EnhancedKeyboardKeys),
    { 746, 193 },
    { 620, 6 },
    { 40, 15 },
    40,
    { 628, 25 },
    { 7, 5 },
    40
};

KEYBOARD_STRUCT StandardKeyboard = 
{ 
    StandardKeyboardKeys,
    _countof(StandardKeyboardKeys),
    { 586, 193 },
    { 588, 6 },
    { 40, 15 },
    40,
    { 596, 25 },
    { 7, 5 },
    40
};
