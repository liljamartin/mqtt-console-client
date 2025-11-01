#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <csignal>
#include <cstdlib>
#include <atomic>
#include <thread>
#include <algorithm>

#include <mqtt/async_client.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

std::atomic<bool> running{true};

void signalHandler(int) { running = false; }

// Get the value after a flag (--flag or -f)
std::string getArgValue(const std::vector<std::string>& args,
                        const std::string& longFlag,
                        const std::string& shortFlag = "") {
    for (size_t i = 0; i + 1 < args.size(); ++i) {
        if (args[i] == longFlag || (!shortFlag.empty() && args[i] == shortFlag))
            return args[i + 1];
    }
    return "";
}

bool hasFlag(const std::vector<std::string>& args,
             const std::string& longFlag,
             const std::string& shortFlag = "") {
    return std::find(args.begin(), args.end(), longFlag) != args.end() ||
           (!shortFlag.empty() &&
            std::find(args.begin(), args.end(), shortFlag) != args.end());
}

// Split colon-separated topics and replace '%' with '+'
std::vector<std::string> splitTopics(const std::string& topicList) {
    std::vector<std::string> topics;
    std::stringstream ss(topicList);
    std::string t;
    while (std::getline(ss, t, ':')) {
        if (t.empty()) continue;
        for (auto &ch : t) if (ch == '%') ch = '+';
        topics.push_back(t);
    }
    return topics;
}

class print_callback : public virtual mqtt::callback {
public:
    void message_arrived(mqtt::const_message_ptr msg) override {
        std::cout << "\n---\nTopic: " << msg->get_topic() << "\nPayload:\n";
        const std::string payload = msg->to_string();
        try {
            auto j = json::parse(payload);
            std::cout << j.dump(2) << std::endl;
        } catch (...) {
            std::cout << payload << std::endl;
        }
    }

    void connection_lost(const std::string& cause) override {
        std::cerr << "\n[WARN] Connection lost";
        if (!cause.empty()) std::cerr << ": " << cause;
        std::cerr << std::endl;
    }
};

int main(int argc, char* argv[]) {
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    std::vector<std::string> args(argv + 1, argv + argc);

    std::string host = getArgValue(args, "--server", "-s");
    std::string portStr = getArgValue(args, "--port", "-p");
    std::string topicList = getArgValue(args, "--topics", "-t");
    std::string qosStr = getArgValue(args, "--qos", "-Q");
    std::string username = getArgValue(args, "--username", "-u");
    std::string password = getArgValue(args, "--password", "-P");

    if (host.empty() || topicList.empty()) {
        std::cerr << "Usage:\n  " << argv[0]
                  << " --server <host> --port <port> --topics <t1:t2:...> [--qos <0|1|2>] [--username <user> --password <pass>]\n"
                  << "Short flags: -s -p -t -Q -u -P\n";
        return 1;
    }

    int port = 1883;
    int qos = 1;
    try {
        if (!portStr.empty()) port = std::stoi(portStr);
        if (!qosStr.empty()) qos = std::stoi(qosStr);
    } catch (...) {
        std::cerr << "Invalid port or QoS value\n"; return 1;
    }

    std::string serverUrl = "tcp://" + host + ":" + std::to_string(port);
    auto topics = splitTopics(topicList);
    if (topics.empty()) {
        std::cerr << "No valid topics provided\n"; return 1;
    }

    std::string clientId = "mqtt_client_cpp_" + std::to_string(std::rand());
    mqtt::async_client client(serverUrl, clientId);

    print_callback cb;
    client.set_callback(cb);

    mqtt::connect_options connOpts;
    connOpts.set_clean_session(true);
    if (!username.empty()) connOpts.set_user_name(username);
    if (!password.empty()) connOpts.set_password(password);

    try {
        std::cout << "Connecting to " << serverUrl << " ...\n";
        client.connect(connOpts)->wait();
        std::cout << "Connected!\n";

        for (const auto& t : topics) {
            client.subscribe(t, qos)->wait();
            std::cout << "Subscribed to: " << t << " (QoS " << qos << ")\n";
        }

        std::cout << "Listening... press Ctrl+C to quit\n";
        while (running) std::this_thread::sleep_for(std::chrono::milliseconds(200));

        std::cout << "\nDisconnecting...\n";
        client.disconnect()->wait();
        std::cout << "Disconnected. Bye!\n";
    } catch (const mqtt::exception& exc) {
        std::cerr << "MQTT error: " << exc.what() << std::endl;
        return 1;
    }

    return 0;
}

