#include "../src/config/config.h"
#include "../src/_log/log.h"
// #include <iostream>
#include <yaml-cpp/yaml.h>

shs::ConfigVar<int>::ptr g_int_value_config = shs::Config::Lookup((int)8080, "system.port", "system.port description");
shs::ConfigVar<double>::ptr g_double_value_config = shs::Config::Lookup((double)8080.7777, "system.call", "system.call description");
shs::ConfigVar<std::vector<int>>::ptr g_vec_int_value_config = shs::Config::Lookup(std::vector<int>{2, 3}, "system.vec", "system.vec_int description");
shs::ConfigVar<std::list<int>>::ptr g_list_int_value_config = shs::Config::Lookup(std::list<int>{12, 13}, "system.list", "system.list_int description");
shs::ConfigVar<std::set<int>>::ptr g_set_int_value_config = shs::Config::Lookup(std::set<int>{12, 13}, "system.set", "system.set_int description");
shs::ConfigVar<std::unordered_set<int>>::ptr g_uset_int_value_config = shs::Config::Lookup(std::unordered_set<int>{12, 13}, "system.uset", "system.uset_int description");
shs::ConfigVar<std::map<std::string, int>>::ptr g_map_int_value_config = shs::Config::Lookup(std::map<std::string, int>{{"k",12}, {"w", 13}}, "system.map", "system.map description");

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

#define XX(g_var, name, prefix) \
{   \
    auto v = g_var->getValue(); \
    for(const auto& i: v) {\
        SHS_LOG_INFO(SHS_LOG_ROOT()) << #prefix " " #name ": " << i;   \
    }\
    SHS_LOG_INFO(SHS_LOG_ROOT()) << "yaml: " << g_var->toString();  \
}

#define XXM(g_var, name, prefix) \
{   \
    auto v = g_var->getValue(); \
    for(const auto& i: v) {\
        SHS_LOG_INFO(SHS_LOG_ROOT()) << #prefix " " #name ": " << "first->" << i.first << ", second->" << i.second;   \
    }\
    SHS_LOG_INFO(SHS_LOG_ROOT()) << "yaml: " << g_var->toString();  \
}
    XX(g_vec_int_value_config, vec_int, before);
    XX(g_list_int_value_config, list_int, before);
    XX(g_set_int_value_config, set_int, before);
    XX(g_uset_int_value_config, uset_int, before);
    XXM(g_map_int_value_config, map_int, before);
    // shs::Config::debug();
    YAML::Node root = YAML::LoadFile("D:\\repositories\\simple_http_server\\bin\\conf\\log.yml");
    shs::Config::LoadFromYaml(root);
    SHS_LOG_INFO(SHS_LOG_ROOT()) << "after: " << g_int_value_config->getValue();
    SHS_LOG_INFO(SHS_LOG_ROOT()) << "after: " << g_double_value_config->toString();
    
    XX(g_vec_int_value_config, vec_int, after);
    XX(g_list_int_value_config, list_int, after);
    XX(g_set_int_value_config, set_int, after);
    XX(g_uset_int_value_config, uset_int, after);
    XXM(g_map_int_value_config, map_int, after);

}
int main(int argc, char** argv) {

    // test_yaml();
    test_config();
    return 0;
}