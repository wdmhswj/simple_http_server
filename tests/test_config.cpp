#include "../src/config/config.h"
#include "../src/_log/log.h"
#include <iostream>

shs::ConfigVar<int>::ptr g_int_value_config = shs::Config::Lookup((int)8080, "system.port", "system.port description");
shs::ConfigVar<double>::ptr g_double_value_config = shs::Config::Lookup((double)8080.7777, "system.call", "system.call description");

int main(int argc, char** argv) {
    SHS_LOG_INFO(SHS_LOG_ROOT()) << g_int_value_config->getValue();
    SHS_LOG_INFO(SHS_LOG_ROOT()) << g_double_value_config->toString();

    return 0;
}