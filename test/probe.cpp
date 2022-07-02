/* This file is part of the the dci project. Copyright (C) 2013-2022 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#include <dci/test.hpp>
#include <dci/host.hpp>
#include <dci/poll.hpp>
#include "ppn/transport.hpp"

using namespace dci;
using namespace dci::host;
using namespace dci::cmt;

using namespace dci::idl::ppn;
using namespace dci::idl::ppn::transport;

namespace
{
    struct ChannelImpl
        : public Channel<>::Opposite
    {
        Address _localAddress{"fakeScheme://fakeValue-local"};
        Address _remoteAddress{"fakeScheme://fakeValue-remote"};

        ChannelImpl()
            : Channel<>::Opposite(idl::interface::Initializer())
        {
            //in  localAddress    () -> Address;
            methods()->localAddress() += [this]
            {
                return cmt::readyFuture<>(_localAddress);
            };

            //in  remoteAddress   () -> Address;
            methods()->remoteAddress() += [this]
            {
                return cmt::readyFuture<>(_remoteAddress);
            };

            //out failed          (exception);

            //in  close           ();
            methods()->close() += []
            {
            };
            //out closed          ();

            //in output(bytes);  //stiac::remoteEdge
            methods()->output() += [this](Bytes&& data)
            {
                methods()->input(std::move(data));
            };

            //in  unlockInput     ();
            methods()->unlockInput() += []()
            {
            };

            //out input(bytes);    //stiac::remoteEdge
            //in  lockInput       ();
            methods()->lockInput() += []()
            {
            };
        }

        static Channel<> create()
        {
            ChannelImpl* channel = new ChannelImpl;
            channel->involvedChanged() += [channel](bool v)
            {
                if(!v)
                {
                    delete channel;
                }
            };
            return channel->opposite();
        }
    };

    struct AcceptorDownstream
        : public acceptor::Downstream<>::Opposite
    {
        Address _address{"fakeScheme://fakeValue-local"};

        AcceptorDownstream()
            : acceptor::Downstream<>::Opposite(idl::interface::Initializer())
        {
            methods()->address() += [this]()
            {
                return cmt::readyFuture(_address);
            };
        }

        static acceptor::Downstream<> create()
        {
            AcceptorDownstream* acceptor = new AcceptorDownstream;
            acceptor->involvedChanged() += [acceptor](bool v)
            {
                if(!v)
                {
                    delete acceptor;
                }
            };
            return acceptor->opposite();
        }
    };

    struct ConnectorDownstream
        : public connector::Downstream<>::Opposite
    {
        Address _address{"fakeScheme://fakeValue-remote"};

        ConnectorDownstream()
            : connector::Downstream<>::Opposite(idl::interface::Initializer())
        {
            methods()->address() += [this]()
            {
                return cmt::readyFuture(_address);
            };

            methods()->connect() += [](Address)
            {
                return cmt::readyFuture(ChannelImpl::create());
            };
        }

        static connector::Downstream<> create()
        {
            ConnectorDownstream* connector = new ConnectorDownstream;
            connector->involvedChanged() += [connector](bool v)
            {
                if(!v)
                {
                    delete connector;
                }
            };
            return connector->opposite();
        }
    };
}


/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_ppn_transport, probe_acceptor)
{
    Manager* manager = testManager();
    Acceptor<> a = manager->createService<Acceptor<>>().value();

    acceptor::Downstream<> la1 = AcceptorDownstream::create();
    acceptor::Downstream<> la2 = AcceptorDownstream::create();

    a->add(la1);
    a->add(la2);

    a->start();
    a->stop();
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_ppn_transport, probe_connector)
{
    Manager* manager = testManager();
    Connector<> c = manager->createService<Connector<>>().value();

    connector::Downstream<> lc1 = ConnectorDownstream::create();
    connector::Downstream<> lc2 = ConnectorDownstream::create();

    c->add(lc1);
    c->add(lc2);

    c->connect(Address{"fakeScheme://tratata"}).value();
}
