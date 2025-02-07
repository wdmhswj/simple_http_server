#include "../src/config/config.h"
#include "../src/_log/log.h"
// #include <iostream>
#include <yaml-cpp/yaml.h>
#include "../src/util/util.h"

#if 1
shs::ConfigVar<int>::ptr g_int_value_config = shs::Config::Lookup((int)8080, "system.port", "system.port description");
shs::ConfigVar<float>::ptr g_fint_value_config = shs::Config::Lookup((float)8080, "system.port", "system.port description");
shs::ConfigVar<double>::ptr g_double_value_config = shs::Config::Lookup((double)8080.7777, "system.call", "system.call description");
shs::ConfigVar<std::vector<int>>::ptr g_vec_int_value_config = shs::Config::Lookup(std::vector<int>{2, 3}, "system.vec", "system.vec_int description");
shs::ConfigVar<std::list<int>>::ptr g_list_int_value_config = shs::Config::Lookup(std::list<int>{12, 13}, "system.list", "system.list_int description");
shs::ConfigVar<std::set<int>>::ptr g_set_int_value_config = shs::Config::Lookup(std::set<int>{12, 13}, "system.set", "system.set_int description");
shs::ConfigVar<std::unordered_set<int>>::ptr g_uset_int_value_config = shs::Config::Lookup(std::unordered_set<int>{12, 13}, "system.uset", "system.uset_int description");
shs::ConfigVar<std::map<std::string, int>>::ptr g_map_int_value_config = shs::Config::Lookup(std::map<std::string, int>{{"k",12}, {"w", 13}}, "system.map", "system.map description");
shs::ConfigVar<std::unordered_map<std::string, int>>::ptr g_umap_int_value_config = shs::Config::Lookup(std::unordered_map<std::string, int>{{"k",12}, {"w", 13}}, "system.umap", "system.umap description");

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
    // YAML::Node root = YAML::LoadFile("D:\\repositories\\simple_http_server\\bin\\conf\\log.yml");
    YAML::Node root = YAML::LoadFile(shs::chooseByOs("D:\\repositories\\simple_http_server\\bin\\conf\\log.yml", "/home/wsl/repositories/simple_http_server/bin/conf/log.yml"));
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
    XXM(g_umap_int_value_config, umap_int, before);
    // shs::Config::debug();
    // YAML::Node root = YAML::LoadFile("D:\\repositories\\simple_http_server\\bin\\conf\\log.yml");
    YAML::Node root = YAML::LoadFile(shs::chooseByOs("D:\\repositories\\simple_http_server\\bin\\conf\\log.yml", "/home/wsl/repositories/simple_http_server/bin/conf/log.yml"));
    shs::Config::LoadFromYaml(root);
    SHS_LOG_INFO(SHS_LOG_ROOT()) << "after: " << g_int_value_config->getValue();
    SHS_LOG_INFO(SHS_LOG_ROOT()) << "after: " << g_double_value_config->toString();
    
    XX(g_vec_int_value_config, vec_int, after);
    XX(g_list_int_value_config, list_int, after);
    XX(g_set_int_value_config, set_int, after);
    XX(g_uset_int_value_config, uset_int, after);
    XXM(g_map_int_value_config, map_int, after);
    XXM(g_umap_int_value_config, umap_int, after);

}

#endif

class Person {
public:
    Person() {};
    std::string m_name;
    int m_age = 0;
    bool m_sex = 0;

    std::string toString() const {
        std::stringstream ss;
        ss << "[Person name=" << m_name
           << " age=" << m_age
           << " sex=" << m_sex
           << "]";
        return ss.str();
    }

    bool operator==(const Person& oth) const {
        return m_name == oth.m_name
            && m_age == oth.m_age
            && m_sex == oth.m_sex;
    }
};

namespace shs {
template<>
class Lexical_cast<std::string, Person> {
public:
    Person operator()(const std::string& v) {
        YAML::Node node = YAML::Load(v);
        Person p;
        p.m_name = node["name"].as<std::string>();
        p.m_age = node["age"].as<int>();
        p.m_sex = node["sex"].as<bool>();
        return p;
    }
};

template<>
class Lexical_cast<Person, std::string> {
public:
    std::string operator()(const Person& p) {
        YAML::Node node;
        node["name"] = p.m_name;
        node["age"] = p.m_age;
        node["sex"] = p.m_sex;
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

}

shs::ConfigVar<Person>::ptr g_person =
    shs::Config::Lookup(Person(), "class.person" ,"system person");

shs::ConfigVar<std::map<std::string, Person> >::ptr g_person_map =
    shs::Config::Lookup(std::map<std::string, Person>(), "class.map", "system person");

shs::ConfigVar<std::map<std::string, std::vector<Person> > >::ptr g_person_vec_map =
    shs::Config::Lookup(std::map<std::string, std::vector<Person> >(), "class.vec_map", "system person");

shs::ConfigVar<std::list<Person>>::ptr g_list_person =
    shs::Config::Lookup(std::list<Person>(), "class.list_person", "system person");

void test_class() {
// shs::ConfigVar<Person>::ptr g_person =
//     shs::Config::Lookup(Person(), "class.person" ,"system person");

// shs::ConfigVar<std::map<std::string, Person> >::ptr g_person_map =
//     shs::Config::Lookup(std::map<std::string, Person>(), "class.map", "system person");

// shs::ConfigVar<std::map<std::string, std::vector<Person> > >::ptr g_person_vec_map =
//     shs::Config::Lookup(std::map<std::string, std::vector<Person> >(), "class.vec_map", "system person");

// shs::ConfigVar<std::list<Person>>::ptr g_list_person =
//     shs::Config::Lookup(std::list<Person>(), "class.list_person", "system person");

    SHS_LOG_INFO(SHS_LOG_ROOT()) << "before: " << g_person->getValue().toString() << " - " << g_person->toString();

#define XX_PM(g_var, prefix) \
    { \
        auto m = g_person_map->getValue(); \
        for(auto& i : m) { \
            SHS_LOG_INFO(SHS_LOG_ROOT()) <<  prefix << ": " << i.first << " - " << i.second.toString(); \
        } \
        SHS_LOG_INFO(SHS_LOG_ROOT()) <<  prefix << ": size=" << m.size(); \
    }
    g_person->addListener(2333, [](const Person& o, const Person& n) {
        SHS_LOG_INFO(SHS_LOG_ROOT()) << "old value: " << o.toString() << "; new value: " << n.toString();
    });

    XX_PM(g_person_map, "class.map before");
    SHS_LOG_INFO(SHS_LOG_ROOT()) << "before: " << g_person_vec_map->toString();
    SHS_LOG_INFO(SHS_LOG_ROOT()) << "before: " << g_list_person->toString();

    // YAML::Node root = YAML::LoadFile("D:\\repositories\\simple_http_server\\bin\\conf\\log.yml");
    YAML::Node root = YAML::LoadFile(shs::chooseByOs("D:\\repositories\\simple_http_server\\bin\\conf\\log.yml", "/home/wsl/repositories/simple_http_server/bin/conf/log.yml"));
    shs::Config::LoadFromYaml(root);

    SHS_LOG_INFO(SHS_LOG_ROOT()) << "after: " << g_person->getValue().toString() << " - " << g_person->toString();
    XX_PM(g_person_map, "class.map after");
    SHS_LOG_INFO(SHS_LOG_ROOT()) << "after: " << g_person_vec_map->toString();
    SHS_LOG_INFO(SHS_LOG_ROOT()) << "after: " << g_list_person->toString();
}

int main(int argc, char** argv) {

    // test_yaml();
    // test_config();
    test_class();
    return 0;
}