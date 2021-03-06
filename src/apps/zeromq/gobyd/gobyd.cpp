// Copyright 2009-2018 Toby Schneider (http://gobysoft.org/index.wt/people/toby)
//                     GobySoft, LLC (2013-)
//                     Massachusetts Institute of Technology (2007-2014)
//
//
// This file is part of the Goby Underwater Autonomy Project Binaries
// ("The Goby Binaries").
//
// The Goby Binaries are free software: you can redistribute them and/or modify
// them under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// The Goby Binaries are distributed in the hope that they will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Goby.  If not, see <http://www.gnu.org/licenses/>.

#include "goby/middleware/application/interface.h"
#include "goby/middleware/gobyd/groups.h"
#include "goby/middleware/protobuf/intervehicle.pb.h"
#include "goby/middleware/transport/intervehicle.h"
#include "goby/zeromq/transport/interprocess.h"

#include "goby/zeromq/protobuf/gobyd_config.pb.h"

#include "goby/middleware/terminate/terminate.h"

using namespace goby::util::logger;
using goby::glog;

namespace goby
{
namespace apps
{
namespace zeromq
{
class Daemon : public goby::middleware::Application<protobuf::GobyDaemonConfig>
{
  public:
    Daemon();
    ~Daemon();

  private:
    void run() override;

  private:
    // for handling ZMQ Interprocess Communications
    std::unique_ptr<zmq::context_t> router_context_;
    std::unique_ptr<zmq::context_t> manager_context_;
    goby::zeromq::Router router_;
    goby::zeromq::Manager manager_;
    std::unique_ptr<std::thread> router_thread_;
    std::unique_ptr<std::thread> manager_thread_;

    // For hosting an InterVehiclePortal
    goby::middleware::InterThreadTransporter interthread_;
    goby::zeromq::InterProcessPortal<goby::middleware::InterThreadTransporter> interprocess_;
    std::unique_ptr<goby::middleware::InterVehiclePortal<decltype(interprocess_)> > intervehicle_;
};
} // namespace zeromq
} // namespace apps
} // namespace goby

int main(int argc, char* argv[]) { return goby::run<goby::apps::zeromq::Daemon>(argc, argv); }

goby::apps::zeromq::Daemon::Daemon()
    : router_context_(new zmq::context_t(app_cfg().router_threads())),
      manager_context_(new zmq::context_t(1)),
      router_(*router_context_, app_cfg().interprocess()),
      manager_(*manager_context_, app_cfg().interprocess(), router_),
      router_thread_(new std::thread([&] { router_.run(); })),
      manager_thread_(new std::thread([&] { manager_.run(); })),
      interprocess_(app_cfg().interprocess())
{
    if (!app_cfg().interprocess().has_platform())
    {
        glog.is(WARN) && glog << "Using default platform name of "
                              << app_cfg().interprocess().platform() << std::endl;
    }

    if (app_cfg().has_intervehicle())
        intervehicle_.reset(new goby::middleware::InterVehiclePortal<decltype(interprocess_)>(
            interprocess_, app_cfg().intervehicle()));

    // handle goby_terminate request
    interprocess_.subscribe<goby::middleware::groups::terminate_request,
                            goby::middleware::protobuf::TerminateRequest>(
        [this](const goby::middleware::protobuf::TerminateRequest& request) {
            bool match = false;
            goby::middleware::protobuf::TerminateResponse resp;
            std::tie(match, resp) =
                goby::middleware::terminate::check_terminate(request, app_cfg().app().name());
            if (match)
            {
                interprocess_.publish<goby::middleware::groups::terminate_response>(resp);
                // as gobyd mediates all interprocess() comms; wait for a bit to hopefully get our response out before shutting down
                sleep(1);
                quit();
            }
        });
}

goby::apps::zeromq::Daemon::~Daemon()
{
    manager_context_.reset();
    router_context_.reset();
    manager_thread_->join();
    router_thread_->join();
}

void goby::apps::zeromq::Daemon::run()
{
    if (intervehicle_)
    {
        intervehicle_->poll();

        //        intervehicle_->poll(std::chrono::milliseconds(100) / time::SimulatorSettings::warp_factor);
        //        goby::middleware::intervehicle::protobuf::Status status;
        //        status.set_tx_queue_size(intervehicle_->tx_queue_size());
        //        interprocess_.publish<goby::middleware::groups::intervehicle_outbound>(status);
    }
    else
    {
        interprocess_.poll();
    }
}
