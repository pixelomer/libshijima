// 
// libshijima - C++ library for shimeji desktop mascots
// Copyright (C) 2024-2025 pixelomer
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
// 

#include "parser_error.hpp"

namespace shijima {

parser_error::parser_error(std::vector<std::string> const& trace,
    std::string const& error): std::exception(), m_trace(trace),
    m_error(error)
{
    for (size_t i=0; i<trace.size(); ++i) {
        m_what += "While " + trace[i] + ",\n";
    }
    m_what += error;
}

const char *parser_error::what() const noexcept {
    return m_what.c_str();
}

}
