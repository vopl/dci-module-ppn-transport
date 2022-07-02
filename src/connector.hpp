/* This file is part of the the dci project. Copyright (C) 2013-2022 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#pragma once

#include "pch.hpp"
#include "endpoints.hpp"

namespace dci::module::ppn::transport
{
    class Connector
        : public api::Connector<>::Opposite
        , public host::module::ServiceBase<Connector>
    {
    public:
        Connector();
        ~Connector();

        api::connector::Downstream<> getBestDownstreamFor(const api::Address& address, const Set<api::connector::Downstream<>>& blacklist);

    private:
        cmt::task::Owner _tol;
        Endpoints<api::connector::Downstream<>> _endpoints;
    };
}
