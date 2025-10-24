#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <mutex>
#include <filesystem>
#include <yaml-cpp/yaml.h>

struct kv_settings
{
    std::string data_dir;
    int lock_time_out;
    int poll_interval;
    bool backup_on_write;
    std::string log_path;
    
    kv_settings(std::string& dir, int timeout, int interval, bool backup, std::string& log)
        : data_dir(dir)
        , lock_time_out(timeout)
        , poll_interval(interval)
        , backup_on_write(backup)
        , log_path(log)
    {}
};

struct node_settings{
    std::string node_id;
    std::string listen_port;
    std::string kv_port;
};


kv_settings get_kv_from_fp(std::string file_name){
    YAML::Node config = YAML::LoadFile(file_name);
    YAML::Node kv = config["kv"];
    std::string data_dir = kv["data_dir"].as<std::string>();
    int lock_timeout = kv["lock_timeout"].as<int>(); 
    int poll_interval = kv["poll_interval"].as<int>();  
    bool back_on_write = kv["backup_on_write"].as<bool>();  
    std::string log_path = kv["log_path"].as<std::string>();
    return kv_settings(data_dir,lock_timeout,poll_interval,back_on_write,log_path);
}

node_settings get_node_from_fp(std::string file_name, std::string node_id){
    YAML::Node node_conf = YAML::LoadFile(file_name);
    YAML::Node node = config[node_id];
    std::string node_id = node["node_id"];
    std::string listen_port = node["listen_port"];
    std::string kv_port = node["kv_port"];
    return node_settings(node_id,listen_port,kv_port);
}

class KV_store {
    public:
    KV_store(const kv_settings& kv_config):config(kv_config){
        std::filesystem::create_directories(kv_config.data_dir);
    }

    std::optional<std::string> get (std::string key){
        std::scoped_lock lk(mu);
        auto result = kv.find(key);
        return result->second;

    }

    void put(std::string key, std::string value){
        std::scoped_lock lk(mu);
        kv[key]=value;
        return; 
    }

    private:
    kv_settings config;
    std::unordered_map<std::string,std::string> kv;
    mutable std::mutex mu;

}
int main(){ 
    kv_settings conf = get_kv_from_fp("kv.yaml");
    node_settings node_conf = ("kv.yaml","node1");
    YA
    
    

}


