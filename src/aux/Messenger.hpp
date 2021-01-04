#pragma once

#include <iostream>
#include <string>

#include "magic_enum.hpp"

enum class MessageSource
{
    CLIENT,
    SERVER,
    TESTMAN,
    NET,
};

enum class MessageSeverity
{
    INFO,
    WARNING,
    ERROR,
};

static void Message(const std::string &Msg, MessageSource Src, MessageSeverity Sev)
{
    std::string CombinedMsg = "[" + std::string(magic_enum::enum_name(Sev)) + "]" + "\t" + "[" + std::string(magic_enum::enum_name(Src)) + "]" + "\t" + Msg + "\n";
    std::cerr << CombinedMsg;
}