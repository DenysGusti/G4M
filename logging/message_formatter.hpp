#ifndef G4M_EUROPE_DG_STRING_FORMATTER_HPP
#define G4M_EUROPE_DG_STRING_FORMATTER_HPP

#include <string>
#include <array>
#include <vector>
#include <exception>
#include <algorithm>
#include <source_location>
#include <filesystem>
#include <chrono>
#include <format>
#include <ranges>
#include <iostream>
#include <functional>

#include "log_level.hpp"

using namespace std;

namespace g4m::logging::concrete {

    const constexpr array messageLiterals = {'T', 't', 'L', 'S', 'M'};

    using path = filesystem::path;
    using clock = chrono::system_clock;

    // helper functions, auto values are too difficult

    auto as_local(auto tp) {
        return chrono::zoned_time{chrono::current_zone(), tp};
    }

    auto as_rounded(const clock::time_point tp) {
        return chrono::round<chrono::seconds>(tp);
    }

    string to_string_short(const auto tp) {
        return format("{:%T}", tp);
    }

    string to_string_long(const auto tp) {
        return format("{:%F %T %Z}", tp);
    }

    string to_string(const source_location &source) {
        return format("{}:{}:{}",
                      path(source.file_name()).filename().string(), source.function_name(), source.line());
    }

    class MessageFormatter {
    public:
        MessageFormatter() {
            updateLiteralPositions();
        }

        /*
        %T - long timestamp, %t - short timestamp
        %L - log level
        %S - source
        %M - message (always)
        Example: "[%T] [%L] [%S]: %M" (default format)
        */
        void setFormat(const string_view str) {
            if (str.find("%M") == string::npos)
                throw invalid_argument{"Invalid formatting string"};

            message_format = str;
            updateLiteralPositions();
        }

        [[nodiscard]] string
        formatMessage(const LogLevel level, const string_view message, const source_location &source) noexcept {
            string formatted_message = message_format;

            for (const auto &[pos, literal]: literal_positions)
                formatted_message.replace(pos, 2, stringFromLiteral(level, message, source, literal));

            return formatted_message;
        }

    private:
        string message_format = "[%T] [%L] [%S]: %M";
        vector<pair<size_t, char> > literal_positions;

        void constexpr updateLiteralPositions() noexcept {
            literal_positions.clear();

            literal_positions.reserve(messageLiterals.size());
            for (size_t pos = string::npos; const auto literal: messageLiterals){
                pos = message_format.find("%"s + literal);
                if (pos != string::npos)
                    literal_positions.emplace_back(pos, literal);
            }

            ranges::sort(literal_positions, greater<>{});  // to replace literals from the end
        }

        static string stringFromLiteral(const LogLevel level, const string_view message, const source_location &source,
                                        const char literal) {
            switch (literal) {
                case 'T':
                    return to_string_long(as_local(clock::now()));
                case 't':
                    return to_string_short(as_local(as_rounded(clock::now())));
                case 'L':
                    return convert_level.at(level);
                case 'S':
                    return to_string(source);
                case 'M':
                    return string{message};
                default:
                    return "Unknown message literal";
            }
        }
    };
}

#endif
