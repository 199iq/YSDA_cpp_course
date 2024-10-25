#include <iostream>
#include <vector>
#include <memory>
#include <sstream>

void Banner() {
    std::cout << "****************************************\n";
    std::cout << "*   Welcome to ReAL Location Service   *\n";
    std::cout << "****************************************\n";
    std::cout << "* 1) Register User                     *\n";
    std::cout << "* 2) Check In                          *\n";
    std::cout << "* 3) View feed                         *\n";
    std::cout << "* 4) Subscribe                         *\n";
    std::cout << "* 5) Exit                              *\n";
    std::cout << "****************************************\n";
}

std::string LimitedGetline() {
    constexpr auto kMaxSize = 80;
    std::string line;
    while (line.size() <= kMaxSize) {
        if (auto c = std::cin.get(); std::cin && (c != '\n')) {
            line.push_back(c);
        } else {
            return line;
        }
    }
    std::cout << "Line is too long\n";
    std::exit(0);
}

template <class T>
T ReadFromLine() {
    if (T x; std::istringstream{LimitedGetline()} >> x) {
        return x;
    }
    std::cout << "Failed to read " << typeid(T).name() << '\n';
    std::exit(0);
}

struct Location {
    double longitude;
    double latitude;
};

struct User {
    std::string username;
    std::vector<Location> posts;

    std::vector<User*> subscribers;
    std::vector<std::pair<User*, Location*>> feed;
};

int main() {
    constexpr auto kMaxUsers = 100;
    std::vector<std::unique_ptr<User>> users;

    auto get_user = [&users]() -> User* {
        std::cout << "Enter username: ";
        for (auto username = LimitedGetline(); auto& user : users) {
            if (user->username == username) {
                return user.get();
            }
        }
        std::cout << "User not found\n";
        return nullptr;
    };

    for (auto i = 0; i < 1'000; ++i) {
        Banner();
        switch (auto cmd = ReadFromLine<int>()) {
            case 1: {
                if (users.size() >= kMaxUsers) {
                    std::cout << "Users limit reached\n";
                    break;
                }
                std::cout << "Enter username: ";
                auto username = LimitedGetline();
                auto& user = users.emplace_back(std::make_unique<User>());
                user->username = username;
                user->posts.reserve(16);
                std::cout << "Registered user " << username << '\n';
                break;
            }
            case 2: {
                auto user = get_user();
                if (!user) {
                    break;
                }

                std::cout << "Enter your location:\n";
                Location location = {.longitude = ReadFromLine<double>(),
                                     .latitude = ReadFromLine<double>()};

                user->posts.push_back(location);
                for (auto subscriber : user->subscribers) {
                    subscriber->feed.emplace_back(user, &user->posts.back());
                }

                std::cout << "User " << user->username << " checked in\n";
                break;
            }
            case 3: {
                auto user = get_user();
                if (!user) {
                    break;
                }

                std::cout << "Feed for user " << user->username << '\n';
                for (auto [p_user, p_location] : user->feed) {
                    std::cout << p_user->username << " is at " << p_location->longitude << " "
                              << p_location->latitude << '\n';
                }
                break;
            }
            case 4: {
                auto user = get_user();
                if (!user) {
                    break;
                }

                auto subscribe_to = get_user();
                if (!subscribe_to) {
                    break;
                }

                subscribe_to->subscribers.push_back(user);
                std::cout << "User " << user->username << " subscribed to "
                          << subscribe_to->username << '\n';
                break;
            }
            case 5: {
                return 0;
            }
            default: {
                std::cout << "Unknown command: " << cmd << '\n';
            }
        }
    }
}
