#include <cstdint>
#include <iostream>
#include <vector>
#include <functional>
#include <deque>
#include <cassert>
#include <set>
#include <string>
#include <map>
#include <list>
#include <unordered_map>
#include <unordered_set>

using namespace std;

// TODO: Implement as much of the data structure as possible.
// Optimize both for performance and memory usage (make trade-offs as you see fit).
// The function signatures should not be modified, only add the implementations.
// The interface can be considered final, meaning you can fully optimize 
// the implementation under the assumption that no other requirements will be added later.
// Assume that data lookups are more frequent than modifications.
// The data structure does not need to be thread-safe.
//
// NOTE: Be aware that there is test code at the bottom of this file that you can run to check whether your implementation generally works.
//
// NOTE: You are allowed to use the STL and lookup stuff on the internet (StackOverflow, Google/Bing, ChatGPT, etc).
//       Treat the test like regular work, use all the tools that you would also use in day to day work.
class DataStructure
{
public:

    DataStructure()
    {
    }

    ~DataStructure()
    {
        Clear();
    }

    /// \brief Resets the data structure to an empty state.
    void Clear()
    {
        items.clear();
        tagIndexMap.clear();
        tagMaskToItems.clear();
        nextTagBitIndex = 0;
        minValue = 0.0;
        maxValue = 1.0;
    }

    /// \brief Adds a new item to the data structure.
    ///
    /// The item has a type, a value and up to three tags from a maximum of 30 different tags.
    void AddItem(int itemType, double itemValue, const char* tag1 = nullptr, const char* tag2 = nullptr, const char* tag3 = nullptr)
    {
        auto getTagBit = [&](const char* tagStr) -> uint32_t {
            if (!tagStr) return 0u;
            auto it = tagIndexMap.find(tagStr);
            if (it == tagIndexMap.end()) {
                if (nextTagBitIndex >= 30) {
                    return 0u;
                }
                tagIndexMap[tagStr] = nextTagBitIndex++;
            }
            uint8_t bitPos = tagIndexMap[tagStr];
            return 1u << bitPos;
        };

        uint32_t newTagsMask = getTagBit(tag1) | getTagBit(tag2) | getTagBit(tag3);

        Item item;
        item.type = itemType;
        item.value = itemValue;
        item.tagMask = newTagsMask;

        if (items.empty()) {
            minValue = itemValue;
            maxValue = itemValue;
        } else {
            if (itemValue < minValue) minValue = itemValue;
            if (itemValue > maxValue) maxValue = itemValue;
        }

        size_t newIndex = items.size();
        items.push_back(item);
        updateTagMaskIndex(newIndex, newTagsMask);
    }

    /// \brief Returns the number of items that were previously added.
    uint32_t GetItemCount() const
    {
        return static_cast<uint32_t>(items.size());
    }

    /// \brief Returns the smallest value among all items.
    ///
    /// Returns zero, if the data structure is empty.
    double GetMinimumValue() const
    {
        return items.empty() ? 0.0 : minValue;
    }

    /// \brief Returns the biggest value among all items.
    ///
    /// Returns one, if the data structure is empty.
    double GetMaximumValue() const
    {
        return items.empty() ? 1.0 : maxValue;
    }

    /// \brief Normalizes all item values, such that the smallest value ends up being zero and the largest value ends up being one.
    ///
    /// Does nothing, if the structure is empty or all values are equal.
    void NormalizeValues()
    {
        if (items.empty()) {
            return;
        }

        double min = GetMinimumValue();
        double max = GetMaximumValue();
        
        if (min == max) {
            return;
        }
        
        double range = max - min;
        for (auto& item : items) {
            item.value = (item.value - min) / range;
        }
        
        minValue = 0.0;
        maxValue = 1.0;
        // Values are normalized
    }

    using RetrieveItemCallback = std::function<void(int type, double value)>;

    /// \brief Uses the provided callback to report all items that have at least all the given tags.
    void RetrieveItemsWithAllTags(RetrieveItemCallback callback, const char* tag1, const char* tag2 = nullptr, const char* tag3 = nullptr) const
    {
        if (!tag1) return;

        uint32_t requiredMask = 0;
        auto getTagBitConst = [&](const char* tagStr) -> uint32_t {
            if (!tagStr) return 0u;
            auto it = tagIndexMap.find(tagStr);
            return (it != tagIndexMap.end()) ? (1u << it->second) : 0u;
        };

        requiredMask = getTagBitConst(tag1);
        if (requiredMask == 0) return;
        if (tag2) {
            uint32_t tag2Bit = getTagBitConst(tag2);
            if (tag2Bit == 0) return;
            requiredMask |= tag2Bit;
        }
        if (tag3) {
            uint32_t tag3Bit = getTagBitConst(tag3);
            if (tag3Bit == 0) return;
            requiredMask |= tag3Bit;
        }

        // Check all items with matching tag combinations
        for (const auto& entry : tagMaskToItems) {
            if ((entry.first & requiredMask) == requiredMask) {
                const auto& matchingItems = entry.second;
                for (size_t idx : matchingItems) {
                    callback(items[idx].type, items[idx].value);
                }
            }
        }
    }

    /// \brief Reports all items that are reachable from the items with the start tag, by following shared tags.
    ///
    /// Two items are considered 'connected', if both items have at least one tag in common.
    /// Items with the tag 'startTag' are reachable.
    /// Items that are connected to a reachable item are also reachable.
    /// The callback should be executed exactly once for every reachable item.
    void RetrieveReachableItems(RetrieveItemCallback callback, const char* startTag) const
    {
        if (!startTag) return;

        auto it = tagIndexMap.find(startTag);
        if (it == tagIndexMap.end()) return;

        uint32_t startMask = 1u << it->second;
        std::vector<bool> visited(items.size(), false);
        std::vector<size_t> currentLevel, nextLevel;

        // Find initial items with the start tag
        for (size_t i = 0; i < items.size(); ++i) {
            if (items[i].tagMask & startMask) {
                currentLevel.push_back(i);
                visited[i] = true;
                callback(items[i].type, items[i].value);
            }
        }

        // Process level by level
        while (!currentLevel.empty()) {
            nextLevel.clear();
            
            // For each item in current level
            for (size_t idx : currentLevel) {
                const uint32_t currentMask = items[idx].tagMask;
                
                // Find unvisited items sharing any tags
                for (size_t i = 0; i < items.size(); ++i) {
                    if (!visited[i] && (items[i].tagMask & currentMask)) {
                        visited[i] = true;
                        nextLevel.push_back(i);
                        callback(items[i].type, items[i].value);
                    }
                }
            }
            
            currentLevel = std::move(nextLevel);
        }
    }

    /// \brief Removes duplicates of all items.
    ///
    /// Duplicates are items that are exactly identical to another item (same type, value and tags).
    void RemoveDuplicates()
    {
        if (items.empty()) return;
        
        std::vector<Item> uniqueItems;
        uniqueItems.reserve(items.size());
        
        for (const auto& item : items) {
            bool isDuplicate = false;
            for (const auto& uniqueItem : uniqueItems) {
                if (item == uniqueItem) {
                    isDuplicate = true;
                    break;
                }
            }
            
            if (!isDuplicate) {
                uniqueItems.push_back(item);
            }
        }
        
        items = std::move(uniqueItems);
    }

private:
    struct Item {
        int type;
        double value;
        uint32_t tagMask = 0;

        bool operator==(const Item& other) const {
            return type == other.type && 
                   value == other.value && 
                   tagMask == other.tagMask;
        }
    };

    std::vector<Item> items;
    std::unordered_map<std::string, uint8_t> tagIndexMap;
    std::unordered_map<uint32_t, std::vector<size_t>> tagMaskToItems;
    uint8_t nextTagBitIndex = 0;
    double minValue = 0.0;
    double maxValue = 1.0;

    void updateTagMaskIndex(size_t itemIndex, uint32_t tagMask) {
        tagMaskToItems[tagMask].push_back(itemIndex);
    }
};

// if this file is missing, check back with the person who gave you the test
#include "ArrCppTest.h"


