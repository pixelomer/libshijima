#pragma once

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

#include <stdexcept>
#include <vector>
#include <string>

namespace shijima {

class parser_error : public std::exception {
private:
    std::vector<std::string> m_trace;
    std::string m_error;
    std::string m_what;
public:
    std::vector<std::string> const& trace() const noexcept {
        return m_trace;
    }
    std::string const& error() const noexcept {
        return m_error;
    }
    virtual const char *what() const noexcept override;
    parser_error(std::vector<std::string> const& trace, std::string const& what);
};

}