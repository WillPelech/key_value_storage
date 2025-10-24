#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <yaml-cpp/yaml.h>

struct kv_settings
{
    std::string data_dir;
    std::string file_name;
    int lock_time_out;
    int poll_interval;
    bool backup_on_write;
    std::string log_path;
};
struct node_settings {
    std::string node_id;
    std::string listen_port;
    std::string kv_port;
};

kv_settings get_kv_from_fp(const std::string& file_name) {
    YAML::Node config = YAML::LoadFile(file_name);
    YAML::Node kv = config["kv"];
    
    kv_settings settings;
    settings.data_dir = kv["data_dir"].as<std::string>();
    settings.file_name = kv["filename"].as<std::string>();  // note: filename instead of file_name
    settings.lock_time_out = kv["lock_timeout"].as<int>();  // note: lock_timeout instead of lock_time_out
    settings.poll_interval = kv["poll_interval"].as<int>();
    settings.backup_on_write = kv["backup_on_write"].as<bool>();
    settings.log_path = kv["log_path"].as<std::string>();
    
    return settings;
}

int main() {
    try {
        auto settings = get_kv_from_fp("kv.yaml");
        std::cout << "Settings loaded successfully:\n";
        std::cout << "Data directory: " << settings.data_dir << "\n";
        std::cout << "File name: " << settings.file_name << "\n";
        std::cout << "Lock timeout: " << settings.lock_time_out << "\n";
        std::cout << "Poll interval: " << settings.poll_interval << "\n";
        std::cout << "Backup on write: " << (settings.backup_on_write ? "true" : "false") << "\n";
        std::cout << "Log path: " << settings.log_path << "\n";
    } catch (const YAML::Exception& e) {
        std::cerr << "Error reading YAML file: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}

