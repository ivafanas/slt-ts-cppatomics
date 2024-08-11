#ifndef SLT_TS_CPPATOMICS_UTILS_H
#define SLT_TS_CPPATOMICS_UTILS_H

#include <cstdint>

namespace sltts {

void log_status(const char *str);
void log_status_param(const char *name, const char *value, int indent = 0);
void log_status_param(const char *name, int value, int indent = 0);
void log_status_param(const char *name, std::uint8_t value, int indent = 0);
void log_status_param(const char *name, std::uint16_t value, int indent = 0);
void log_status_param(const char *name, std::uint32_t value, int indent = 0);
void log_status_param(const char *name, std::uint64_t value, int indent = 0);

/// Arguments parsers are not designed neither for fully functional
/// boost::program_options analogue nor for fast parsing. It is just fast enough
/// and "correct enough" for naive mini project with minimum dependencies.
///
/// You can find their interface ugly and not C++-style. It is intentional.

/// Find "<name> <value>" sequence in command line arguments and write <value>
/// as int to |result| parameter. Returns false if sequence is found, but value
/// is not an integer.
bool get_arg_i(int argc, const char **argv, const char *name, int *result);

/// Find "<name> <value>" sequence in command line arguments and write <value>
/// as positive int to |result| parameter. Returns false if sequence is found,
/// but value is not a positive integer.
bool get_arg_pos_i(int argc, const char **argv, const char *name, int *result);

} // namespace sltts

#endif // SLT_TS_CPPATOMICS_UTILS_H

