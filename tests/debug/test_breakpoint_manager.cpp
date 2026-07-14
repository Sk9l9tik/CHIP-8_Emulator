#include <gtest/gtest.h>

#include "debug/BreakpointManager.h"

class BreakpointManagerTest : public ::testing::Test {
protected:
    BreakpointManager manager;
};

TEST_F(BreakpointManagerTest, StartsEmpty) {
    EXPECT_TRUE(manager.all().empty());
    EXPECT_FALSE(manager.has(0x200));
}

TEST_F(BreakpointManagerTest, AddSingleBreakpoint) {
    manager.add(0x200);
    EXPECT_TRUE(manager.has(0x200));
    EXPECT_EQ(manager.all().size(), 1u);
}

TEST_F(BreakpointManagerTest, AddMultipleBreakpoints) {
    manager.add(0x200);
    manager.add(0x202);
    manager.add(0x204);
    EXPECT_EQ(manager.all().size(), 3u);
    EXPECT_TRUE(manager.has(0x200));
    EXPECT_TRUE(manager.has(0x202));
    EXPECT_TRUE(manager.has(0x204));
}

TEST_F(BreakpointManagerTest, AddDuplicateIsNoOp) {
    manager.add(0x200);
    manager.add(0x200);
    EXPECT_EQ(manager.all().size(), 1u);
}

TEST_F(BreakpointManagerTest, RemoveExistingBreakpoint) {
    manager.add(0x200);
    manager.remove(0x200);
    EXPECT_FALSE(manager.has(0x200));
    EXPECT_TRUE(manager.all().empty());
}

TEST_F(BreakpointManagerTest, RemoveNonExistentIsNoOp) {
    manager.add(0x200);
    manager.remove(0x300);
    EXPECT_EQ(manager.all().size(), 1u);
    EXPECT_TRUE(manager.has(0x200));
}

TEST_F(BreakpointManagerTest, RemoveFromEmptySetIsNoOp) {
    EXPECT_NO_THROW(manager.remove(0x200));
    EXPECT_TRUE(manager.all().empty());
}

TEST_F(BreakpointManagerTest, ToggleAddsWhenAbsent) {
    manager.toggle(0x200);
    EXPECT_TRUE(manager.has(0x200));
}

TEST_F(BreakpointManagerTest, ToggleRemovesWhenPresent) {
    manager.add(0x200);
    manager.toggle(0x200);
    EXPECT_FALSE(manager.has(0x200));
}

TEST_F(BreakpointManagerTest, ToggleTwiceRestoresOriginalState) {
    manager.toggle(0x200);
    manager.toggle(0x200);
    EXPECT_FALSE(manager.has(0x200));
}

TEST_F(BreakpointManagerTest, ClearRemovesAllBreakpoints) {
    manager.add(0x200);
    manager.add(0x202);
    manager.add(0x204);
    manager.clear();
    EXPECT_TRUE(manager.all().empty());
    EXPECT_FALSE(manager.has(0x200));
    EXPECT_FALSE(manager.has(0x202));
    EXPECT_FALSE(manager.has(0x204));
}

TEST_F(BreakpointManagerTest, ClearOnEmptySetIsNoOp) {
    EXPECT_NO_THROW(manager.clear());
    EXPECT_TRUE(manager.all().empty());
}

TEST_F(BreakpointManagerTest, AllReturnsCorrectContents) {
    manager.add(0x200);
    manager.add(0x300);
    const auto& all = manager.all();
    EXPECT_EQ(all.count(0x200), 1u);
    EXPECT_EQ(all.count(0x300), 1u);
    EXPECT_EQ(all.size(), 2u);
}

TEST_F(BreakpointManagerTest, HasReturnsFalseForUncheckedAddress) {
    manager.add(0x200);
    EXPECT_FALSE(manager.has(0x201));
}

TEST_F(BreakpointManagerTest, HandlesBoundaryAddresses) {
    manager.add(0x0000);
    manager.add(0xFFFF);
    EXPECT_TRUE(manager.has(0x0000));
    EXPECT_TRUE(manager.has(0xFFFF));
    EXPECT_EQ(manager.all().size(), 2u);
}
