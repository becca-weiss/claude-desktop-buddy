#include <unity.h>

#include "stats.h"

// Native (no-hardware) smoke tests for the pure logic in src/stats.h.
// The header's Arduino/NVS dependencies are satisfied by the stub headers
// in test/stubs/, wired in via the env:native build flags in
// platformio.ini. No firmware source is modified or refactored.

// stats.h keeps file-static state, so every test starts from a clean slate.
static void resetStatsState() {
  memset(&_stats, 0, sizeof(_stats));
  _lastBridgeTokens = 0;
  _tokensSynced = false;
  _levelUpPending = false;
  _dirty = false;
}

void setUp() { resetStatsState(); }
void tearDown() {}

// --- statsOnApproval: velocity ring buffer ---------------------------------

void test_approval_velocity_ring_buffer_wraps(void) {
  // 10 approvals into an 8-slot ring: only the last 8 (30..100) survive.
  for (uint32_t s = 10; s <= 100; s += 10) statsOnApproval(s);
  TEST_ASSERT_EQUAL_UINT8(8, _stats.velCount);
  // Sorted: 30,40,50,60,70,80,90,100 → median slot (n/2 = 4) is 70.
  TEST_ASSERT_EQUAL_UINT16(70, statsMedianVelocity());
}

void test_approval_seconds_clamped_to_uint16_max(void) {
  statsOnApproval(100000);
  TEST_ASSERT_EQUAL_UINT16(65535, statsMedianVelocity());
}

void test_median_velocity_empty_returns_zero(void) {
  TEST_ASSERT_EQUAL_UINT16(0, statsMedianVelocity());
}

// --- statsOnBridgeTokens: delta accounting ---------------------------------

void test_bridge_tokens_first_sight_is_latch_not_credit(void) {
  statsOnBridgeTokens(50000);
  // The bridge's cumulative total is a baseline, not new tokens — without
  // the latch a device reboot would re-credit the whole session.
  TEST_ASSERT_EQUAL_UINT32(0, _stats.tokens);
}

void test_bridge_tokens_deltas_accumulate(void) {
  statsOnBridgeTokens(0);      // latch
  statsOnBridgeTokens(500);    // +500
  statsOnBridgeTokens(10000);  // +9500
  TEST_ASSERT_EQUAL_UINT32(10000, _stats.tokens);
}

void test_bridge_tokens_restart_does_not_credit(void) {
  statsOnBridgeTokens(1000);   // latch
  statsOnBridgeTokens(1500);   // +500
  statsOnBridgeTokens(200);    // bridge restarted: resync, no delta
  TEST_ASSERT_EQUAL_UINT32(500, _stats.tokens);
  statsOnBridgeTokens(350);    // resumes from the restarted baseline
  TEST_ASSERT_EQUAL_UINT32(650, _stats.tokens);
}

void test_bridge_tokens_level_up_at_threshold(void) {
  statsOnBridgeTokens(0);      // latch
  statsOnBridgeTokens(49999);  // just under the level boundary
  TEST_ASSERT_EQUAL_UINT8(0, _stats.level);
  TEST_ASSERT_FALSE(statsPollLevelUp());
  statsOnBridgeTokens(50000);  // crosses TOKENS_PER_LEVEL
  TEST_ASSERT_EQUAL_UINT8(1, _stats.level);
  TEST_ASSERT_TRUE(statsPollLevelUp());
  TEST_ASSERT_FALSE(statsPollLevelUp());  // level-up is consumed once
}

// --- mood / fed progress ----------------------------------------------------

void test_fed_progress_pips(void) {
  TEST_ASSERT_EQUAL_UINT8(0, statsFedProgress());
  statsOnBridgeTokens(0);
  statsOnBridgeTokens(25000);  // 25K of 50K → 5 of 10 pips
  TEST_ASSERT_EQUAL_UINT8(5, statsFedProgress());
}

void test_mood_tier_no_data_is_neutral(void) {
  TEST_ASSERT_EQUAL_UINT8(2, statsMoodTier());
}

void test_mood_tier_follows_velocity(void) {
  statsOnApproval(10);   // vel < 15 → top tier, no denials to drag it down
  TEST_ASSERT_EQUAL_UINT8(4, statsMoodTier());
}

void test_mood_tier_denial_ratio_penalizes(void) {
  statsOnApproval(10);   // tier 4 from velocity
  statsOnDenial();
  statsOnDenial();
  statsOnDenial();       // d > a → -2 tiers
  TEST_ASSERT_EQUAL_UINT8(2, statsMoodTier());
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_approval_velocity_ring_buffer_wraps);
  RUN_TEST(test_approval_seconds_clamped_to_uint16_max);
  RUN_TEST(test_median_velocity_empty_returns_zero);
  RUN_TEST(test_bridge_tokens_first_sight_is_latch_not_credit);
  RUN_TEST(test_bridge_tokens_deltas_accumulate);
  RUN_TEST(test_bridge_tokens_restart_does_not_credit);
  RUN_TEST(test_bridge_tokens_level_up_at_threshold);
  RUN_TEST(test_fed_progress_pips);
  RUN_TEST(test_mood_tier_no_data_is_neutral);
  RUN_TEST(test_mood_tier_follows_velocity);
  RUN_TEST(test_mood_tier_denial_ratio_penalizes);
  return UNITY_END();
}
