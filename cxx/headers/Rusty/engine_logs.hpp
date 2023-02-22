#pragma once

extern "C" {
// To rusty vault
void vault_log(const char *str);
void vault_error(const char *str);
void vault_warning(const char *str);
}