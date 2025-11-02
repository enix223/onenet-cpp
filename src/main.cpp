#include "command_line_parser.h"
#include "mqtt/async_client.h"
#include "onenet_client.h"
#include <atomic>
#include <condition_variable>
#include <csignal>
#include <fmt/base.h>
#include <iostream>
#include <mutex>
#include <sstream>

static std::mutex gShutdownMu;
static std::condition_variable gShutdownCv;
static std::atomic<bool> gShutdownRequest{false};

void signalHandler(int signalNum)
{
  if (signalNum != SIGINT && signalNum != SIGTERM)
  {
    return;
  }
  gShutdownRequest = true;
  try
  {
    gShutdownCv.notify_one();
  }
  catch (std::exception &e)
  {
  }
}

int main(int argc, const char *const argv[])
{
  cl::Logger logger{cl::LogLevel::INFO};
  cl::CommandLineParser argparser{"onenet", "onenet client"};
  argparser.add_mandatory<std::string>("p,product-id", "product id");
  argparser.add_mandatory<std::string>("s,product-secret", "product secret");
  argparser.add_mandatory<std::string>("d,device-name", "device name");
  argparser.add_mandatory<std::string>("t,device-secret", "device secret");
  auto opts = argparser.parse(argc, argv);

  auto pid = opts["product-id"].as<std::string>();
  auto ps = opts["product-secret"].as<std::string>();
  auto dn = opts["device-name"].as<std::string>();
  auto ds = opts["device-secret"].as<std::string>();
  logger.info("product id = {}, product secret = {}, device name = {}, device secret = {}", pid, ps, dn, ds);

  if (std::signal(SIGINT, signalHandler) == SIG_ERR || std::signal(SIGTERM, signalHandler) == SIG_ERR)
  {
    logger.error("failed to register signal handler");
    return 1;
  }

  cl::OneNetClient client{pid, ps, dn, ds};
  client.connect();

  {
    std::unique_lock<std::mutex> lock{gShutdownMu};
    gShutdownCv.wait(lock, [] { return gShutdownRequest.load(); });
  }

  logger.info("shutdown client");

  return 0;
}
