// Copyright 2009-2018 Toby Schneider (http://gobysoft.org/index.wt/people/toby)
//                     GobySoft, LLC (2013-)
//                     Massachusetts Institute of Technology (2007-2014)
//                     Community contributors (see AUTHORS file)
//
//
// This file is part of the Goby Underwater Autonomy Project Libraries
// ("The Goby Libraries").
//
// The Goby Libraries are free software: you can redistribute them and/or modify
// them under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 2.1 of the License, or
// (at your option) any later version.
//
// The Goby Libraries are distributed in the hope that they will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Goby.  If not, see <http://www.gnu.org/licenses/>.

#pragma once
#ifndef CORONER_GROUPS_20191125H
#define CORONER_GROUPS_20191125H

#include "goby/middleware/group.h"

namespace goby
{
namespace middleware
{
namespace groups
{
constexpr goby::middleware::Group health_request{"goby::health::request"};
constexpr goby::middleware::Group health_response{"goby::health::response"};
constexpr goby::middleware::Group health_report{"goby::health::report"};

} // namespace groups
} // namespace middleware
} // namespace goby

#endif
