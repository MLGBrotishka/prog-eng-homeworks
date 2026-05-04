#include "handlers.hpp"

#include <userver/components/minimal_server_component_list.hpp>
#include <userver/server/handlers/ping.hpp>
#include <userver/clients/http/component.hpp>
#include <userver/clients/http/component_core.hpp>
#include <userver/clients/http/middlewares/pipeline_component.hpp>
#include <userver/clients/dns/component.hpp>
#include <userver/server/handlers/tests_control.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/utils/daemon_run.hpp>

#include <userver/storages/mongo/component.hpp>
#include "../common/mongo_database.hpp"

#include <userver/storages/redis/component.hpp>
#include <userver/storages/secdist/component.hpp>
#include <userver/storages/secdist/provider_component.hpp>

int main(int argc, char* argv[]) {
    auto component_list = userver::components::MinimalServerComponentList()
                              .Append<userver::server::handlers::Ping>()
                              .Append<userver::components::TestsuiteSupport>()
                              .Append<userver::clients::dns::Component>()
                              .Append<userver::clients::http::MiddlewarePipelineComponent>()
                              .Append<userver::components::HttpClientCore>()
                              .Append<userver::components::HttpClient>()
                              .Append<userver::server::handlers::TestsControl>()
                              .Append<userver::components::Mongo>("mongo-taxi-db")
                              .Append<userver::components::Secdist>()
                              .Append<userver::components::DefaultSecdistProvider>()
                              .Append<userver::components::Redis>("redis-taxi");

    taxi_service::user::AppendUserHandlers(component_list);

    return userver::utils::DaemonMain(argc, argv, component_list);
}