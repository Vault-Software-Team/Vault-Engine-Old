#include "engine_logs.hpp"
#include "api.hpp"

void vault_log(const char *str) {
    HyperAPI::Log log(str, HyperAPI::LOG_INFO);
}

void vault_error(const char *str) {
    HyperAPI::Log log(str, HyperAPI::LOG_ERROR);
}
void vault_warning(const char *str) {
    HyperAPI::Log log(str, HyperAPI::LOG_WARNING);
}