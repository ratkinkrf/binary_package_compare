#include <fstream>
#include <string>
#include <thread>

#include "json.h"
#include "json_builder.h"

const std::string query = "curl -X 'GET' "
                          "  https://rdb.altlinux.org/api/export/branch_binary_packages/";

json::Document LoadData(std::string branch_1, std::string arch, std::string path) {
    std::string query_to_perform = query + branch_1 + "?arch=" + arch + " >" + path;
    system(query_to_perform.data());
    std::fstream file;
    file.open(path);
    json::Document document(json::Load(file));
    return document;
}

int main(int argc, char **argv) {
    std::string branch_1 = argv[1];
    std::string branch_2 = argv[2];
    std::string arch = argv[3];

    json::Document doc1 = LoadData(branch_1, arch, "request1.json");

    json::Document doc2 = LoadData(branch_2, arch, "request2.json");

    json::Array branch1_to_2;
    json::Array ver_diff;
    bool contains = false;




    auto compare = [&](){
        for (const auto &elem1: doc1.GetRoot().AsDict().at("packages").AsArray()) {
                for (const auto &elem2: doc2.GetRoot().AsDict().at("packages").AsArray()) {
                    if (elem1.AsDict().at("name").AsString() == elem2.AsDict().at("name").AsString()) {
                        contains = true;
                        if (elem1.AsDict().at("version").AsString() > elem2.AsDict().at("version").AsString()) {
                            ver_diff.emplace_back(elem1);
                        }
                        break;
                    }
                }
                if (!contains) {
                    branch1_to_2.emplace_back(elem1);
                }
                contains = false;
            }
    };
    std::thread thr(compare);

    bool contains2 = false;
    json::Array branch2_to_1;
    for (const auto &elem1: doc2.GetRoot().AsDict().at("packages").AsArray()) {
        for (const auto &elem2: doc1.GetRoot().AsDict().at("packages").AsArray()) {
            if (elem1.AsDict().at("name").AsString() == elem2.AsDict().at("name").AsString()) {
                contains2 = true;
                break;
            }
        }
        if (!contains) {
            branch2_to_1.emplace_back(elem1);
        }
        contains2 = false;
    }

    thr.join();


    std::ofstream version_difference;
    version_difference.open("version_difference.json");
    json::Print(json::Document{json::Builder{}.StartDict().Key("packages").Value(ver_diff).EndDict().Build()},
                version_difference);

    std::ofstream branch1_to_2_diff;
    branch1_to_2_diff.open("branch1_to_2_diff.json");
    json::Print(json::Document{json::Builder{}.StartDict().Key("packages").Value(branch1_to_2).EndDict().Build()},
                branch1_to_2_diff);

    std::ofstream branch2_to_1_diff;
    branch2_to_1_diff.open("branch2_to_1_diff.json");
    json::Print(json::Document{json::Builder{}.StartDict().Key("packages").Value(branch2_to_1).EndDict().Build()},
                branch2_to_1_diff);

    return 0;
}
