/* This file is part of the the dci project. Copyright (C) 2013-2022 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#pragma once

#include "pch.hpp"

namespace dci::module::ppn::transport
{
    class Acceptor
        : public api::Acceptor<>::Opposite
        , public host::module::ServiceBase<Acceptor>
    {
    public:
        Acceptor();
        ~Acceptor();

    private:
        struct Downstream
        {
            Downstream(api::acceptor::Downstream<>&& instance, Acceptor* upstream);
            ~Downstream();

            void start();
            void stop();

            api::acceptor::Downstream<> _instance;
            sbs::Owner                  _sbsOwner;
            bool                        _needStart = false;
            bool                        _started = false;
            size_t                      _failed = 0;

            dci::poll::Timer            _restartTicker;
        };
        using DownstreamPtr = std::unique_ptr<Downstream>;

    private:
        using Downstreams = List<DownstreamPtr>;
        Downstreams _downstreams;

        bool _started {false};
    };
}
