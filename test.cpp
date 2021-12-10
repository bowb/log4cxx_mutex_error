// test.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <unistd.h>
#include <filesystem>
#include <thread>
#include <stdio.h>
#include <stdarg.h>
#include <string>

#include <log4cxx/logmanager.h>
#include <log4cxx/mdc.h>
#include <log4cxx/helpers/loglog.h>
#include <log4cxx/xml/domconfigurator.h>
#include <log4cxx/patternlayout.h>
#include <log4cxx/net/socketappender.h>
#include <log4cxx/consoleappender.h>
#include <log4cxx/basicconfigurator.h>

void init_log4cxx()
{
    log4cxx::helpers::LogLog::setInternalDebugging(true);
    log4cxx::helpers::LogLog::setQuietMode(false);
    //log4cxx::xml::DOMConfigurator::configure("logging.xml");
    log4cxx::LayoutPtr layout = std::static_pointer_cast<log4cxx::Layout>(std::make_shared<log4cxx::PatternLayout>("%d{yyyy/MM/dd HH:mm:ss.sssZ}  [3344:%.6t]  [%32F(%4L)]  [%-5p]  %m%n"));
    auto appender = std::make_shared<log4cxx::net::SocketAppender>();
    appender->setLayout(layout);
    appender->setName(LOG4CXX_STR("BENCH"));
    appender->setRemoteHost(LOG4CXX_STR("localhost"));
    appender->setReconnectionDelay(5000);
    appender->setPort(4445);
    log4cxx::helpers::Pool pool;
    appender->activateOptions(pool);
    log4cxx::BasicConfigurator::configure(appender);

    //auto consoleAppender = std::make_shared<log4cxx::ConsoleAppender>(layout);
    //consoleAppender->setName(LOG4CXX_STR("Console"));

    //consoleAppender->activateOptions(pool);
    //log4cxx::BasicConfigurator::configure(consoleAppender);

    log4cxx::Logger::getRootLogger()->setLevel(log4cxx::Level::getAll());
}

void bench(int howmany, log4cxx::LoggerPtr log)
{
    using std::chrono::duration;
    using std::chrono::duration_cast;
    using std::chrono::high_resolution_clock;

    auto start = high_resolution_clock::now();
    for (auto i = 0; i < howmany; ++i)
    {
        log->forcedLog(log4cxx::Level::getInfo(), "Hello logger: msg number", log4cxx::spi::LocationInfo(__FILE__, "", __LINE__));
    }

    auto delta = high_resolution_clock::now() - start;
    auto delta_d = duration_cast<duration<double>>(delta).count();

    char res[1000] = { 0 };
    snprintf(res, sizeof(res),"{%s} Elapsed: {%f} secs {%d}/sec", "left", delta_d, int(howmany / delta_d));
    LOG4CXX_INFO(log, std::string(res));
}

void bench_mt(int howmany, log4cxx::LoggerPtr log, size_t thread_count)
{
    using std::chrono::duration;
    using std::chrono::duration_cast;
    using std::chrono::high_resolution_clock;

    std::vector<std::thread> threads;
    threads.reserve(thread_count);
    auto start = high_resolution_clock::now();
    for (size_t t = 0; t < thread_count; ++t)
    {
        threads.emplace_back([&]() {
            for (int j = 0; j < howmany / static_cast<int>(thread_count); j++)
            {
                log->forcedLog(log4cxx::Level::getInfo(), "Hello logger: msg number", log4cxx::spi::LocationInfo(__FILE__, "", __LINE__));
            }
        });
    }

    for (auto &t : threads)
    {
        t.join();
    };

    auto delta = high_resolution_clock::now() - start;
    auto delta_d = duration_cast<duration<double>>(delta).count();
    char res[1000] = { 0 };
    snprintf(res, sizeof(res),"mt {%s} Elapsed: {%f} secs {%d}/sec", "left", delta_d, int(howmany / delta_d));
    LOG4CXX_INFO(log, std::string(res));
}

void bench_single_threaded(int iters)
{
    log4cxx::LoggerPtr appLogger = log4cxx::Logger::getLogger(L"BENCH");
    LOG4CXX_INFO(appLogger, "**************************************************************");
    LOG4CXX_INFO(appLogger, "Single threaded: messages ");
    LOG4CXX_INFO(appLogger, "**************************************************************");
    bench(iters, appLogger);
}


int main(int argc, char *argv[])
{
    init_log4cxx();
    int iters = 25000000;
    size_t threads = 4;
    int max_threads = 1000;

    auto logger =  log4cxx::Logger::getLogger(L"BENCH");
    try
    {
        bench_single_threaded(iters);
        bench_mt(iters, logger, 1);
        bench_mt(iters, logger, 2);
    }
    catch (std::exception &ex)
    {
        // spdlog::error(ex.what());
        std::cout << "failed:" << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;

    return 0;
}