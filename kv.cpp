#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <mutex>
#include <optional>
#include <unordered_map>
#include <filesystem>
#include <fstream>
#include <sys/socket.h>
#include <yaml-cpp/yaml.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <cstring>
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
    int listen_port;
    std::string kv_port;
    node_settings(const std::string& nid,int lp,const std::string kvp):
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
    int listen_port = node["listen_port"].as<int>();
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

        std::ofstream logFile(config.log_path,std::ios_base::app);
        logFile<<message<<"\n";
        logFile.close();

    }
};
// struct sock_addr_in{
//     uint16_t sin_port;
//     uint32_t sin_addr;
//     uint8_t sin_family;
//     uint8_t sin_zero[8];
// };
int create_server_socket(int port){
// domain, type sock_stream for tcp, sock_dram for udp, protocol typically 0
    int server_socket = socket(AF_INET,SOCK_STREAM,0);   
    if(server_socket == -1){
        throw std::runtime_error("Failed to create server socket");
    }
    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;
    if(bind(server_socket,(struct sockaddr_in*)&server_address,sizeof(server_address)) == -1){
        close(server_socket);
        throw std::runtime_error("Failed to bind server socket");
    
    }
    if(listen(server_socket,5) == -1){
        close(server_socket);
        throw std::runtime_error("Failed to listen on server socket");
    }
    return server_socket;
}
int server_loop(int server_socket){
    while (true){
        int client_socket = accept(server_socket,nullptr,nullptr); 
        if (client_socket == -1){
            throw std::runtime_error("Failed to accept client connection");
        }
        std::thread client_thread(handle_client,client_socket);
        client_thread.detach();
    }    

}

void handle_client(int client_socket) {
    char buf[1024];
    for (;;) {
        ssize_t n = recv(client_socket, buf, sizeof(buf), 0);
        if (n <= 0) break; // closed or error
        send(client_socket, buf, n, 0); // echo back
    }
    close(client_socket);
}

int main(){ 
    kv_settings conf = get_kv_from_fp("kv.yaml");
    node_settings node_conf = get_node_from_fp("kv.yaml","node1");
    KV_store key_value_storage = KV_store(conf);

    key_value_storage.put("test","test");
    std::cout<<key_value_storage.get("test")<<"\n";
    key_value_storage.put("qpaso","mwah mwah mwah");
    std::cout<<key_value_storage.get("qpaso")<<"\n";
    std::cout<<server_socket(node_conf.listen_port) << "\n";

}
/*

{a,b} pq, leader selection, id number
_update time _ 50 ms 
{a,b} 
{a,b}

*/

