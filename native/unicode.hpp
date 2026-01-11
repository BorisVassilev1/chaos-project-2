#pragma once

#include <string>

std::wstring UNICODE_TO_WIDE(const std::string &str);
std::string	 WIDE_TO_UNICODE(const std::wstring &wstr);
