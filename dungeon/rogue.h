#pragma once

#include "dungeon.h"

#include <stdexcept>
#include <iostream>
#include <vector>
#include <unordered_set>
#include <iterator>

const Room* VisitOpenRooms(const Room* room, std::unordered_set<std::string>* keys,
    std::unordered_set<const Room*>* used_rooms, std::unordered_set<Door*>* closed_doors) {
        // visit all the rooms that are reachable from the current room, take their keys & closed_doors
        std::unordered_set<const Room*>& used = *used_rooms;
        std::unordered_set<std::string>& cur_keys = *keys;
        std::unordered_set<Door*>& cur_closed_doors = *closed_doors;

        used.insert(room);

        if (room->IsFinal()) {
            return room;
        }

        // take all the keys
        size_t total_keys = room->NumKeys();
        for (size_t i = 0; i < total_keys; i++) {
            cur_keys.insert(room->GetKey(i));
        }

        const Room* res = nullptr;

        size_t total_doors = room->NumDoors();
        for (size_t i = 0; i < total_doors; i++) {
            Door* door = room->GetDoor(i);
            if (door->IsOpen()) {
                const Room* next_room = door->GoThrough();
                if (!used.count(next_room)) {
                    res = VisitOpenRooms(next_room, keys, used_rooms, closed_doors);
                    if (res != nullptr) {
                        return res;
                    }
                }
            } else {
                cur_closed_doors.insert(door);
            }
        }
        return res;
}

const Room* FindFinalRoom(const Room* starting_room) {
    std::unordered_set<Door*> closed_doors; // store all the doors that are still locked
    std::unordered_set<std::string> keys; // store keys that we currently have
    std::unordered_set<const Room*> used_rooms; // store rooms that we have already visited
    // std::unordered_set<Door*> opened_doors; // store all the doors that are opened

    // process starting room
    const Room* res = VisitOpenRooms(starting_room, &keys, &used_rooms, &closed_doors);
    if (res != nullptr) {
        return res;
    }

    int cnt_opened = 1;
    while (cnt_opened) {
        // try all the keys to unlock all the doors
        std::unordered_set<std::string> new_keys;
        std::unordered_set<Door*> new_closed_doors;
        std::unordered_set<Door*> open_now;

        int opened = 0;
        for (auto it_door = closed_doors.begin(); it_door != closed_doors.end(); it_door++) {
            for (auto it_key = keys.begin(); it_key != keys.end(); it_key++) {
                Door* door = *it_door;
                if (door->TryOpen(*it_key)) {
                    // we can open current door using current key
                    opened++;

                    const Room* next_room = door->GoThrough();
                    if (!used_rooms.count(next_room)) {
                        res = VisitOpenRooms(next_room, &new_keys, &used_rooms, &new_closed_doors);
                        if (res != nullptr) {
                            return res;
                        }
                    }
                    open_now.insert(*it_door);
                    break;
                }
            }
        }

        cnt_opened = opened;
        // add new keys
        for (auto &x: new_keys) {
            keys.insert(x);
        }
        // add new closed doors
        for (auto &x: new_closed_doors) {
            closed_doors.insert(x);
        }
        // delete opened doors
        for (auto &x: open_now) {
            closed_doors.erase(x);
        }
    }
    return nullptr;
    // throw std::runtime_error{"Not implemented"};
}
