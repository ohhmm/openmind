#pragma once
#include <vector>
#include <chrono>

#define STRINGIFY_HELPER(x) #x
#define STRINGIFY(str) STRINGIFY_HELPER(str)

#define TEST(code) \
{\
	if ((code) == true)\
		printf("  correct: %s\n", STRINGIFY(code)); \
	else\
		printf("  wrong: %s\n", STRINGIFY(code)); \
}

#define TEST_MSG(code, msg) \
{\
	if ((code) == true)\
		printf("  correct: '%s'\n", STRINGIFY(code)); \
	else\
		printf("  wrong: '%s' - '%s'\n", STRINGIFY(code), msg); \
}

struct TestItem
{
    int type = -1;
    double value = 0;
};

std::vector<TestItem> gatheredItems;

void GatherItemsCB(int type, double value)
{
    TestItem ti;
    ti.type = type;
    ti.value = value;
    gatheredItems.push_back(ti);
}

void DiscardItemsCB([[maybe_unused]] int type, [[maybe_unused]] double value)
{
}

void RunPerformanceTests()
{
    printf("\nRunning performance tests...\n");
    DataStructure ds;
    const int NUM_ITEMS = 100000;
    const int TAG_TEST_ITEMS = 1000; // Smaller dataset for tag operations
    
    printf("\nTesting item addition performance...\n");
    const auto start_add = std::chrono::high_resolution_clock::now();
    // Add items with single tag for basic operations
    for (int i = 0; i < NUM_ITEMS; ++i) {
        ds.AddItem(i, i * 0.1, "tag1");
    }
    // Add items with multiple tags for tag operations testing
    DataStructure ds_tags;
    for (int i = 0; i < TAG_TEST_ITEMS; ++i) {
        ds_tags.AddItem(i, i * 0.1, "tag1", "tag2", "tag3");
    }
    const auto end_add = std::chrono::high_resolution_clock::now();
    const auto duration_add = std::chrono::duration_cast<std::chrono::milliseconds>(end_add - start_add);
    printf("Time to add %d items: %ld ms\n", NUM_ITEMS + TAG_TEST_ITEMS, duration_add.count());
    printf("Average time per addition: %.3f microseconds\n", (duration_add.count() * 1000.0) / (NUM_ITEMS + TAG_TEST_ITEMS));
    
    printf("\nTesting min/max getter performance...\n");
    double min_sum = 0, max_sum = 0;
    const auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 10000; ++i) {
        min_sum += ds.GetMinimumValue();
        max_sum += ds.GetMaximumValue();
    }
    const auto end = std::chrono::high_resolution_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    printf("Min/Max getter performance (10000 calls): %ld microseconds\n", duration.count());
    printf("Average time per call: %.3f microseconds\n", duration.count() / 10000.0);
    printf("Control sums: %.2f, %.2f\n", min_sum, max_sum);
    
    printf("\nTesting tag lookup performance...\n");
    int count = 0;
    const auto start2 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000; ++i) {
        ds_tags.RetrieveItemsWithAllTags([&count](int, double){ count++; }, "tag1", "tag2");
    }
    const auto end2 = std::chrono::high_resolution_clock::now();
    const auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(end2 - start2);
    printf("Tag lookup performance (1000 calls): %ld microseconds\n", duration2.count());
    printf("Average time per call: %.3f microseconds\n", duration2.count() / 1000.0);
    printf("Items found per call: %d\n", count / 1000);
    
    printf("\nTesting reachable items performance...\n");
    count = 0;
    const auto start3 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 100; ++i) {
        ds_tags.RetrieveReachableItems([&count](int, double){ count++; }, "tag1");
    }
    const auto end3 = std::chrono::high_resolution_clock::now();
    const auto duration3 = std::chrono::duration_cast<std::chrono::microseconds>(end3 - start3);
    printf("Reachable items performance (100 calls): %ld microseconds\n", duration3.count());
    printf("Average time per call: %.3f microseconds\n", duration3.count() / 100.0);
    printf("Items found per call: %d\n", count / 100);
    
    printf("\nFinished performance tests...\n");
}

void RunBasicTests()
{
    printf("Running basic tests...\n\n");

    DataStructure ds;

    ds.AddItem(1, 0.1, "tag1");
    ds.AddItem(2, 0.7, "tag2");
    ds.AddItem(3, 1.1, "tag3");
    ds.AddItem(4, 2.1, "tag1", "tag3");
    ds.AddItem(5, 2.3, "tag2", "tag1", "tag3");
    ds.AddItem(3, 1.1, "tag4");

    printf("\nTesting GetItemCount...\n");
    TEST(ds.GetItemCount() == 6);

    printf("\nTesting Min/Max Value...\n");
    TEST(ds.GetMinimumValue() == 0.1);
    TEST(ds.GetMaximumValue() == 2.3);

    printf("\nTesting NormalizeValues...\n");
    ds.NormalizeValues();

    TEST(ds.GetMinimumValue() == 0.0);
    TEST(ds.GetMaximumValue() == 1.0);

    printf("\nTesting RetrieveItemsWithAllTags...\n");
    gatheredItems.clear();
    ds.RetrieveItemsWithAllTags(GatherItemsCB, "tag1");
    TEST(gatheredItems.size() == 3);

    gatheredItems.clear();
    ds.RetrieveItemsWithAllTags(GatherItemsCB, "tag2");
    TEST(gatheredItems.size() == 2);

    gatheredItems.clear();
    ds.RetrieveItemsWithAllTags(GatherItemsCB, "tag3", "tag1");
    TEST(gatheredItems.size() == 2);

    gatheredItems.clear();
    ds.RetrieveItemsWithAllTags(GatherItemsCB, "tag1", "tag2", "tag3");
    TEST(gatheredItems.size() == 1);

    printf("\nTesting RetrieveReachableItems...\n");
    gatheredItems.clear();
    ds.RetrieveReachableItems(GatherItemsCB, "tag1");
    TEST(gatheredItems.size() == 5);

    gatheredItems.clear();
    ds.RetrieveReachableItems(GatherItemsCB, "tag4");
    TEST(gatheredItems.size() == 1);

    printf("\nTesting Clear...\n");
    ds.Clear();
    TEST(ds.GetItemCount() == 0);

    printf("\nTesting RemoveDuplicates...\n");
    ds.AddItem(1, 0.1, "tag1");
    ds.AddItem(2, 0.7, "tag2");
    ds.AddItem(3, 1.1, "tag3");
    ds.AddItem(4, 2.1, "tag1", "tag3");
    ds.AddItem(4, 2.1, "tag1", "tag3"); // duplicate item
    ds.AddItem(5, 2.3, "tag2", "tag1", "tag3");
    ds.AddItem(5, 2.3, "tag2", "tag1", "tag3"); // duplicate item
    ds.AddItem(3, 1.1, "tag3"); // duplicate item
    ds.AddItem(3, 1.1, "tag4");
    ds.RemoveDuplicates();
    TEST(ds.GetItemCount() == 6);

    printf("\nFinished basic tests...\n\n");
}

int main()
{
    RunBasicTests();
    RunPerformanceTests();
    return 0;
}
