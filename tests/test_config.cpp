#include "../src/config/config.h"
#include "../src/_log/log.h"
// #include <iostream>
#include <yaml-cpp/yaml.h>

shs::ConfigVar<int>::ptr g_int_value_config = shs::Config::Lookup((int)8080, "system.port", "system.port description");
shs::ConfigVar<double>::ptr g_double_value_config = shs::Config::Lookup((double)8080.7777, "system.call", "system.call description");
shs::ConfigVar<std::vector<int>>::ptr g_vec_int_value_config = shs::Config::Lookup(std::vector<int>{2, 3}, "system.vec", "system.vec_int description");

void print_yaml(const YAML::Node& node, int level) {
    // 纯标量
    if(node.IsScalar()) {
        SHS_LOG_INFO(SHS_LOG_ROOT()) << std::string(level*4, ' ') << node.Scalar() << " - " << node.Type() << " - " << level;
    } else if(node.IsNull()) {
        SHS_LOG_INFO(SHS_LOG_ROOT()) << std::string(level*4, ' ') << "NULL - " << node.Type() << " - " << level;
    } else if(node.IsMap()) {
        for(auto it=node.begin(); it!=node.end(); ++it) {
            SHS_LOG_INFO(SHS_LOG_ROOT()) << std::string(level*4, ' ') << it->first << " - " << it->second.Type() << " - " << level;
            print_yaml(it->second, level+1);
        }
    } else if(node.IsSequence()) {
        for(size_t i=0; i<node.size(); ++i) {
            SHS_LOG_INFO(SHS_LOG_ROOT()) << std::string(level*4, ' ') << i << " - " << node[i].Type() << " - " << level;
            print_yaml(node[i], level+1);
        }
    } else if(!node.IsDefined()) {
        SHS_LOG_INFO(SHS_LOG_ROOT()) << std::string(level*4, ' ') << "UnDefined - " << node.Type() << " - " << level;
    }
}

void test_yaml() {
    YAML::Node root = YAML::LoadFile("D:\\repositories\\simple_http_server\\bin\\conf\\log.yml");
    print_yaml(root, 0);
}
void test_config() {
    // shs::Config::debug();
    SHS_LOG_INFO(SHS_LOG_ROOT()) << "before: " << g_int_value_config->getValue();
    SHS_LOG_INFO(SHS_LOG_ROOT()) << "before: " << g_double_value_config->toString();
    auto v = g_vec_int_value_config->getValue();
    for(const auto& i: v) {
        SHS_LOG_INFO(SHS_LOG_ROOT()) << "before: " << i;
    }
    // shs::Config::debug();
    YAML::Node root = YAML::LoadFile("D:\\repositories\\simple_http_server\\bin\\conf\\log.yml");
    shs::Config::LoadFromYaml(root);
    SHS_LOG_INFO(SHS_LOG_ROOT()) << "after: " << g_int_value_config->getValue();
    SHS_LOG_INFO(SHS_LOG_ROOT()) << "after: " << g_double_value_config->toString();
    v = g_vec_int_value_config->getValue();
    for(const auto& i: v) {
        SHS_LOG_INFO(SHS_LOG_ROOT()) << "after: " << i;
    }

}
int main(int argc, char** argv) {

    // test_yaml();
    test_config();
    return 0;
}