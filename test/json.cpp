//
// Created by lby on 2018/1/10.
//

#include <orz/codec/json.h>
#include <orz/utils/log.h>

int main(int argc, char *argv[])
{
    std::string json_string = R"({"version":"5.1.2","date":"@date","name":"VIPLFaceDetector5.1.2.m9d6.640x480.dat","data":"@file@model/VIPLFaceDetector5.1.2.m9d6.640x480.dat","params":{"stride":4,"end2end":1,"thresh1":0.7,"thresh2":0.7,"thresh3":0.85},"input":{"width_limit":640,"height_limit":480}})";
    auto value = orz::json2jug(json_string);
    ORZ_LOG(orz::INFO) << "Original json: " << json_string;
    ORZ_LOG(orz::INFO) << "Converted jug: " << value;
    return 0;
}