#include "jwt_auth_checker.hpp"

#include <jwt-cpp/jwt.h>

#include <userver/http/common_headers.hpp>
#include <userver/yaml_config/merge_schemas.hpp>

namespace taxi_service::auth::jwt {

namespace {
static constexpr std::string_view kAlgorithm = "Bearer ";
}  // namespace

JwtChecker::JwtChecker(const std::string& secret, const std::string& issuer) : secret_(secret), issuer_(issuer) {}

JwtChecker::AuthCheckResult JwtChecker::CheckAuth(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext& context) const {
  const std::string_view auth_header =
      request.GetHeader(userver::http::headers::kAuthorization);
  if (auth_header.empty()) {
    return AuthCheckResult{AuthCheckResult::Status::kTokenNotFound,
                           "Missing 'Authorization' header"};
  }

  if (!auth_header.starts_with(kAlgorithm)) {
    return AuthCheckResult{AuthCheckResult::Status::kInvalidToken,
                           "Invalid authorization type, expected 'Bearer'"};
  }

  const std::string_view token = auth_header.substr(kAlgorithm.length());
  try {
    auto decoded = ::jwt::decode<::jwt::traits::kazuho_picojson>(std::string(token));
    
    auto verifier = ::jwt::verify<::jwt::traits::kazuho_picojson>()
        .allow_algorithm(::jwt::algorithm::hs256{secret_})
        .with_issuer(issuer_);
    
    verifier.verify(decoded);
    
    if (decoded.has_payload_claim("user_id")) {
      auto user_id_claim = decoded.get_payload_claim("user_id");
      try {
        int64_t user_id = std::stoll(user_id_claim.as_string());
        context.SetData("user_id", user_id);
      } catch (const std::exception& e) {
        return AuthCheckResult{AuthCheckResult::Status::kForbidden,
                               "Failed to parse user_id claim: " + std::string(e.what())};
      }
    } else {
      return AuthCheckResult{AuthCheckResult::Status::kForbidden,
                             "Missing user_id claim in token"};
    }
    
    return {};

  } catch (const ::jwt::error::token_verification_exception& exc) {
    return AuthCheckResult{
        AuthCheckResult::Status::kInvalidToken,
        "Token verification failed: " + std::string{exc.what()}};
  } catch (const std::exception& ex) {
    return AuthCheckResult{
        AuthCheckResult::Status::kForbidden,
        "Token processing error: " + std::string{ex.what()}};
  }
}

JwtAuthComponent::JwtAuthComponent(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : LoggableComponentBase(config, context) {
  authorizer_ = std::make_shared<JwtChecker>(config["secret"].As<std::string>(), config["issuer"].As<std::string>("taxi-service"));
}

JwtCheckerPtr JwtAuthComponent::Get() const { return authorizer_; }

userver::yaml_config::Schema JwtAuthComponent::GetStaticConfigSchema() {
  return userver::yaml_config::MergeSchemas<LoggableComponentBase>(R"(
type: object
description: JWT Auth Checker Component
additionalProperties: false
properties:
        secret:
            type: string
            description: secret key for JWT validation
        issuer:
            type: string
            description: expected issuer for JWT validation
)");
}

}  // namespace taxi_service::auth::jwt