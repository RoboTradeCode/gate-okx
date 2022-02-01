//#include <boost/log/expressions.hpp>
//#include <boost/log/sinks/text_file_backend.hpp>
//#include <boost/log/utility/setup/file.hpp>
//#include <boost/log/utility/setup/common_attributes.hpp>
//#include "log.h"
//
//namespace logging = boost::log;
//namespace sinks = boost::log::sinks;
//namespace keywords = boost::log::keywords;
//
//void init_logging()
//{
//    logging::add_file_log
//    (
//        keywords::file_name = "sample_%N.log",
//        keywords::time_based_rotation = sinks::file::rotation_at_time_interval(boost::posix_time::hours(72)),
//        keywords::format = "[%TimeStamp%]: %Message%"
//    );
//}
