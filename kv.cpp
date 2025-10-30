#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <mutex>
#include <optional>
#include <unordered_map>
#include <filesystem>
#include <fstream>
#include <yaml-cpp/yaml.h>

struct kv_settings
{
    std::string data_dir;
    int lock_time_out;
    int poll_interval;
    bool backup_on_write;
    std::string log_path;
    
    kv_settings(const std::string& dir, int timeout, int interval, bool backup,const std::string& log)
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
    node_settings(const std::string& nid,const std::string& lp,const std::string kvp):
    node_id(nid), listen_port(lp),kv_port(kvp){}
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
    YAML::Node node =node_conf[node_id];
    std::string node_id_ret = node["node_id"].as<std::string>();
    std::string listen_port = node["listen_port"].as<std::string>();
    std::string kv_port = node["kv_port"].as<std::string>();
    return node_settings(node_id_ret,listen_port,kv_port);
}

class KV_store {
    private:
    kv_settings config;
    std::unordered_map<std::string,std::string> kv;
    mutable std::mutex mu_;
    public:
    KV_store(const kv_settings& kv_config):config(kv_config){
        std::filesystem::create_directories(kv_config.data_dir);
    }

    std::string get(std::string key){
        std::scoped_lock lk(mu_);
        auto result = kv.find(key);
        if(result ==kv.end()){
            log("Key not found");
            return "";
        }
        return result->second;
    }

    void put(std::string key, std::string value){
        std::scoped_lock lk(mu_);
        kv[key]=value;
        return; 
    }
    void log(const std::string& message){
        // Ensure parent directory exists
        std::filesystem::path logPath(config.log_path);
        if(logPath.has_parent_path()){
            std::filesystem::create_directories(logPath.parent_path());
        }
        
        std::ofstream logFile(config.log_path, std::ios_base::app);
        if(!logFile.is_open()){
            std::cerr << "Failed to open log file: " << config.log_path << "\n";
            return;
        }
        logFile << message << "\n";
        logFile.close();
    }
};

int main(){ 
    kv_settings conf = get_kv_from_fp("kv.yaml");
    std::cout << conf.log_path << "\n";
    node_settings node_conf = get_node_from_fp("kv.yaml","node1");
    KV_store key_value_storage = KV_store(conf);

    key_value_storage.put("test","test");
    std::cout<<key_value_storage.get("test")<<"\n";
 //   key_value_storage.put("qpaso","mwah mwah mwah");
    std::cout<<key_value_storage.get("qpaso")<<"\n";
}


