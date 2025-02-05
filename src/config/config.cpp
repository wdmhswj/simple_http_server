#include "config.h"
#include <list>
#include <utility>
#include "../_log/log.h"
// #include <iostream>

namespace shs {

Config::ConfigVarMap Config::m_datas;

// "A.B", 10
// A:
//  B: 10
//  C: str
// 将 YAML:::Node 的树状结构通过字符串前缀的方式拉平
static void ListAllMember(const std::string& prefix, const YAML::Node& node, std::list<std::pair<std::string, const YAML::Node>>& output) {
    if(prefix.find_first_not_of("abcdefghijklmnopqrstuvwxyz._1234567890")!=prefix.npos) {
        SHS_LOG_ERROR(SHS_LOG_ROOT()) << "Config invalid name: " << prefix << " : " << node;
        return;
    }
    output.push_back(std::make_pair(prefix, node));
    if(node.IsMap()) {
        for(auto it=node.begin(); it!=node.end(); ++it) {
            ListAllMember(prefix.empty()?it->first.Scalar():prefix+"."+it->first.Scalar(), it->second, output);
        }
    }
}

// 使用 YAML::Node 初始化配置模块
void Config::LoadFromYaml(const YAML::Node& root) {
    std::list<std::pair<std::string, const YAML::Node>> all_nodes;
    ListAllMember("", root, all_nodes);

    for(auto& i: all_nodes) {
        if(i.first.empty()) 
            continue;
        std::string key;
        key.resize(i.first.size());
        std::transform(i.first.begin(), i.first.end(), key.begin(), ::tolower);
        // std::cout << key << " -> ";
        auto var = LookupBase(key);
        if(var) {
            if(i.second.IsScalar()) {
                var->fromString(i.second.Scalar());
                // std::cout << i.second.Scalar() << std::endl;
            } else {
                std::stringstream ss;
                ss << i.second;
                var->fromString(ss.str());
                // std::cout << ss.str() << std::endl;
            }
        }
    }

    // debug();

}
// 查询配置参数，返回配置参数的基类
ConfigVarBase::ptr Config::LookupBase(const std::string& name) {
    auto it = m_datas.find(name);
    return it==m_datas.end() ? nullptr : it->second;
}


}