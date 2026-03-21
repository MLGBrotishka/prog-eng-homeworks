#include "jwt_auth_factory.hpp"

namespace taxi_service::auth::jwt {

JwtAuthCheckerFactory::JwtAuthCheckerFactory(
    const userver::components::ComponentContext& context)
    : auth_(context.FindComponent<JwtAuthComponent>()) {}

userver::server::handlers::auth::AuthCheckerBasePtr
JwtAuthCheckerFactory::MakeAuthChecker(
    const userver::server::handlers::auth::HandlerAuthConfig&) const {
  return auth_.Get();
}

}  // namespace auth::jwt