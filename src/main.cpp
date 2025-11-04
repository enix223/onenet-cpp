#include <fmt/base.h>

#include <atomic>
#include <condition_variable>
#include <csignal>
#include <iostream>
#include <mutex>
#include <sstream>

#include "base64_openssl.h"
#include "command_line_parser.h"
#include "onenet_client.h"
#include "url_util_httplib.h"

static std::mutex g_shutdown_mu;
static std::condition_variable g_shutdown_cv;
static std::atomic<bool> g_shutdown_request{false};

void SignalHandler(int signalNum)
{
  if (signalNum != SIGINT && signalNum != SIGTERM) {
    return;
  }
  g_shutdown_request = true;
  try {
    g_shutdown_cv.notify_one();
  } catch (std::exception& e) {
  }
}

int main(int argc, const char* const argv[])
{
  cl::Logger logger{(cl::LogLevel)CL_ONENET_LOG_LEVEL};
  cl::CommandLineParser argparser{"onenet", "onenet client"};
  argparser.AddMandatory<std::string>("p,product-id", "product id");
  argparser.AddMandatory<std::string>("s,product-secret", "product secret");
  argparser.AddMandatory<std::string>("d,device-name", "device name");
  argparser.AddMandatory<std::string>("t,device-secret", "device secret");
  argparser.AddMandatory<bool>("a,device-auth", "device level auth", "true");
  auto opts = argparser.Parse(argc, argv);

  auto pid = opts["product-id"].as<std::string>();
  auto ps = opts["product-secret"].as<std::string>();
  auto dn = opts["device-name"].as<std::string>();
  auto ds = opts["device-secret"].as<std::string>();
  auto da = opts["device-auth"].as<bool>();
  logger.Info(
      "product id = {}, product secret = {}, device name = {}, device secret = "
      "{}",
      pid, ps, dn, ds);

  if (std::signal(SIGINT, SignalHandler) == SIG_ERR ||
      std::signal(SIGTERM, SignalHandler) == SIG_ERR) {
    logger.Error("failed to register signal handler");
    return 1;
  }

  std::shared_ptr<cl::Base64> base64 = std::make_shared<cl::Base64Openssl>();
  std::shared_ptr<cl::UrlUtil> urlUtil = std::make_shared<cl::UrlUtilHttplib>();

  cl::OneNetClient client{da, pid, ps, dn, ds, base64, urlUtil};
  client.Connect();

  logger.Info("Press ctrl+c to quit");
  {
    std::unique_lock<std::mutex> lock{g_shutdown_mu};
    g_shutdown_cv.wait(lock, [] { return g_shutdown_request.load(); });
  }

  logger.Info("shutdown client");

  return 0;
}
