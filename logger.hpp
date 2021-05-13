/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014-2019,  mGuard Project
 *
 * This file is part of mGuard Project.

/*! \file logger.hpp
 * \brief Define macros and auxiliary functions for logging.
 *
 * This file defines the macros that mGuard uses for logging
 * messages. An intrepid hacker could replace this system cleanly by
 * providing a system that redefines all of the _LOG_* macros with an
 * arbitrary system, as long as the underlying system accepts strings.
 */

#ifndef MGUARD_LOGGER_HPP
#define MGUARD_LOGGER_HPP

#include <ndn-cxx/util/logger.hpp>

#define INIT_LOGGER(name) NDN_LOG_INIT(MGUARD.name)

#define MGUARD_LOG_TRACE(x) NDN_LOG_TRACE(x)
#define MGUARD_LOG_DEBUG(x) NDN_LOG_DEBUG(x)
#define MGUARD_LOG_INFO(x) NDN_LOG_INFO(x)
#define MGUARD_LOG_WARN(x) NDN_LOG_WARN(x)
#define MGUARD_LOG_ERROR(x) NDN_LOG_ERROR(x)
#define MGUARD_LOG_FATAL(x) NDN_LOG_FATAL(x)

#endif // MGUARD_LOGGER_HPP
