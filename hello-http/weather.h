#pragma once

#include <string>
#include <memory>
#include <optional>
#include <stdexcept>

struct Location {
    double lat;
    double lon;
};

struct WeatherForecast {
    double temp;
    double feels_like;
};

struct YandexAPIError : std::runtime_error {
    YandexAPIError(int http_code, std::string details)
        : std::runtime_error{"api error: code=" + std::to_string(http_code) +
                             " details=" + details},
          http_code{http_code},
          details{std::move(details)} {
    }

    int http_code;
    std::string details;
};

class IForecaster {
public:
    virtual ~IForecaster() = default;
    virtual WeatherForecast ForecastWeather(std::optional<Location> where = std::nullopt) = 0;
};

std::unique_ptr<IForecaster> CreateYandexForecaster(
    const std::string& api_key,
    const std::string& api_endpoint = "https://api.weather.yandex.ru/v1/forecast");
