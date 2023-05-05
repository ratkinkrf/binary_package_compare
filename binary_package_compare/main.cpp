#include <fstream>
#include <string>

#include "json.h"
#include "json_builder.h"

const std::string query = "curl -X 'GET' "
                          "  https://rdb.altlinux.org/api/export/branch_binary_packages/";

int main(int argc, char **argv) {
    std::string branch_1 = argv[1];
    std::string branch_2 = argv[2];
    std::string arch = argv[3];

    std::string query1 = query + branch_1 + "?arch=" + arch + ">request1.json";
    system(query1.data());
    std::fstream file1;
    file1.open("request1.json");
    json::Document doc1(json::Load(file1));

    std::string query2 = query + branch_2 + "?arch=" + arch + ">request2.json";
    system(query2.data());
    std::fstream file2;
    file2.open("request2.json");
    json::Document doc2(json::Load(file2));

    json::Array branch1_to_2;
    json::Array ver_diff;
    bool contains = false;
    for (const auto &elem1 : doc1.GetRoot().AsDict().at("packages").AsArray()){
        for (const auto &elem2 : doc2.GetRoot().AsDict().at("packages").AsArray()){
            if (elem1.AsDict().at("name").AsString() == elem2.AsDict().at("name").AsString()){
                contains = true;
                if (elem1.AsDict().at("version").AsString() > elem2.AsDict().at("version").AsString()){
                    ver_diff.emplace_back(elem1);
                }
                break;
            }
        }
        if (!contains){
            branch1_to_2.emplace_back(elem1);
        }
        contains = false;
    }

    contains = false;
    json::Array branch2_to_1;
    for (const auto &elem1 : doc2.GetRoot().AsDict().at("packages").AsArray()){
        for (const auto &elem2 : doc1.GetRoot().AsDict().at("packages").AsArray()){
            if (elem1.AsDict().at("name").AsString() == elem2.AsDict().at("name").AsString()){
                contains = true;
                break;
            }
        }
        if (!contains){
            branch2_to_1.emplace_back(elem1);
        }
        contains = false;
    }

    std::ofstream version_difference;
    version_difference.open("version_difference.json");
    json::Print(json::Document{json::Builder{}.StartDict().Key("packages").Value(ver_diff).EndDict().Build()}, version_difference);

    std::ofstream branch1_to_2_diff;
    branch1_to_2_diff.open("branch1_to_2_diff.json");
    json::Print(json::Document{json::Builder{}.StartDict().Key("packages").Value(branch1_to_2).EndDict().Build()},branch1_to_2_diff);

    std::ofstream branch2_to_1_diff;
    branch2_to_1_diff.open("branch2_to_1_diff.json");
    json::Print(json::Document{json::Builder{}.StartDict().Key("packages").Value(branch2_to_1).EndDict().Build()},branch2_to_1_diff);

    return 0;
}
