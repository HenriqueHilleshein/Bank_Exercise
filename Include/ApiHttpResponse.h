#pragma once

#include <stdexcept>
#include <string>

namespace ApiHttpResponse {

inline bool isNotFoundMessage(const std::exception& exception)
{
    const std::string message = exception.what();
    return message.find("not found") != std::string::npos || message.find("Not found") != std::string::npos;
}

inline int statusCodeForException(const std::exception& exception)
{
    if (dynamic_cast<const std::invalid_argument*>(&exception) != nullptr) {
        return 400;
    }

    if (isNotFoundMessage(exception)) {
        return 404;
    }

    return 400;
}

} // namespace ApiHttpResponse