#include "mqtt/async_client.h"
#include <cxxopts.hpp>
#include <fmt/base.h>
#include <iostream>
#include <sstream>

const std::string SERVER_URL{"tls://mqttstls.heclouds.com:8883"};
const std::string CA_CERT_PATH = "onenet-ca.pem";

std::string buildToken()
{
  return "hi";
}

int main(int argc, const char *const argv[])
{
  cxxopts::Options options("onenet", "onenet client");
  options.add_options()                                                     // add options
      ("p,product-id", "product id", cxxopts::value<std::string>())         // product id
      ("s,product-secret", "product secret", cxxopts::value<std::string>()) // product secret
      ("d,device-name", "device name", cxxopts::value<std::string>())       // device name
      ("t,device-secret", "device secret", cxxopts::value<std::string>())   // device secret
      ("h,help", "help");
  auto opts = options.parse(argc, argv);
  if (opts.count("help"))
  {
    fmt::println(options.help());
    return 0;
  }

  auto pid = opts["product-id"].as<std::string>();
  auto ps = opts["product-secret"].as<std::string>();
  auto dn = opts["device-name"].as<std::string>();
  auto ds = opts["device-secret"].as<std::string>();
  fmt::println("product id = {}, product secret = {}, device name = {}, device secret = {}", pid, ps, dn, ds);

  // ssl options
  auto sslopts = mqtt::ssl_options_builder()
                     .trust_store(CA_CERT_PATH) // set ca cert trust store
                     .enable_server_cert_auth(true)
                     .finalize();

  // connect options
  auto token = buildToken();
  auto connOpts =
      mqtt::connect_options_builder().clean_session(true).ssl(sslopts).user_name(pid).password(token).finalize();
  mqtt::async_client client{SERVER_URL, dn};
  client.connect(connOpts);

  return 0;
}
