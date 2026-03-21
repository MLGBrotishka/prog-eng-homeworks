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

#include <auth/jwt_auth_checker.hpp>
#include <auth/jwt_auth_factory.hpp>

int main(int argc, char* argv[]) {
    userver::server::handlers::auth::RegisterAuthCheckerFactory<
        taxi_service::auth::jwt::JwtAuthCheckerFactory>();
    
    auto component_list = userver::components::MinimalServerComponentList()
                              .Append<userver::server::handlers::Ping>()
                              .Append<userver::components::TestsuiteSupport>()
                              .Append<userver::clients::dns::Component>()
                              .Append<userver::clients::http::MiddlewarePipelineComponent>()
                              .Append<userver::components::HttpClientCore>()
                              .Append<userver::components::HttpClient>()
                              .Append<userver::server::handlers::TestsControl>()
                              .Append<taxi_service::auth::jwt::JwtAuthComponent>();

    taxi_service::driver::AppendDriverHandlers(component_list);

    return userver::utils::DaemonMain(argc, argv, component_list);
}
