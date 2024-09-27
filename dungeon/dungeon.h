#pragma once

#include <vector>
#include <string>
#include <memory>
#include <stdexcept>

class Room;

class Door {
public:
    bool IsOpen() const {
        return open_;
    }

    bool TryOpen(const std::string& key) {
        if (key == key_) {
            open_ = true;
        }
        return open_;
    }

    const Room* GoThrough() const {
        if (!open_) {
            throw std::runtime_error{"Door is closed"};
        }
        return to_;
    }

    explicit Door(Room* to) : open_{true}, to_{to} {
    }
    Door(Room* to, const std::string& key) : open_{false}, key_{key}, to_{to} {
    }

    Door(const Door&) = delete;
    Door(Door&&) = delete;
    Door& operator=(const Door&) = delete;
    Door& operator=(Door&&) = delete;

private:
    bool open_;
    const std::string key_;
    const Room* const to_;
};

class Room {
public:
    bool IsFinal() const {
        return is_final_;
    }

    size_t NumKeys() const {
        return keys_.size();
    }

    const std::string& GetKey(size_t index) const {
        return keys_[index];
    }

    size_t NumDoors() const {
        return doors_.size();
    }

    Door* GetDoor(size_t index) const {
        return doors_[index].get();
    }

    void AddDoor(Room* to) {
        doors_.emplace_back(std::make_unique<Door>(to));
        to->doors_.emplace_back(std::make_unique<Door>(this));
    }

    void AddDoorWithKey(Room* to, const std::string& key) {
        doors_.emplace_back(std::make_unique<Door>(to, key));
        to->doors_.emplace_back(std::make_unique<Door>(this, key));
    }

    void AddKey(const std::string& key) {
        keys_.push_back(key);
    }

    explicit Room(bool is_final) : is_final_{is_final} {
    }

    Room(const Room&) = delete;
    Room(Room&&) = delete;
    Room& operator=(const Room&) = delete;
    Room& operator=(Room&&) = delete;

private:
    const bool is_final_;

    std::vector<std::unique_ptr<Door>> doors_;
    std::vector<std::string> keys_;
};
