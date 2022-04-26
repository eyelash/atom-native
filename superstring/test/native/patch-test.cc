#include "test-helpers.h"

using Change = Patch::Change;
using std::vector;

static optional<Text> null_text;

TEST_CASE("Patch::splice – simple non-overlapping") {
  Patch patch;

  patch.splice(NativePoint {0, 5}, NativePoint {0, 3}, NativePoint {0, 4});
  patch.splice(NativePoint {0, 10}, NativePoint {0, 3}, NativePoint {0, 4});
  REQUIRE(patch.get_changes() == vector<Change>({
    Change {
      NativePoint {0, 5}, NativePoint {0, 8},
      NativePoint {0, 5}, NativePoint {0, 9},
      nullptr, nullptr,
      0, 0, 0
    },
    Change {
      NativePoint {0, 9}, NativePoint {0, 12},
      NativePoint {0, 10}, NativePoint {0, 14},
      nullptr, nullptr,
      0, 0, 0
    }
  }));

  patch.splice(NativePoint {0, 2}, NativePoint {0, 2}, NativePoint {0, 1});
  REQUIRE(patch.get_changes() == vector<Change>({
    Change {
      NativePoint {0, 2}, NativePoint {0, 4},
      NativePoint {0, 2}, NativePoint {0, 3},
      nullptr, nullptr,
      0, 0, 0
    },
    Change {
      NativePoint {0, 5}, NativePoint {0, 8},
      NativePoint {0, 4}, NativePoint {0, 8},
      nullptr, nullptr,
      0, 0, 0
    },
    Change {
      NativePoint {0, 9}, NativePoint {0, 12},
      NativePoint {0, 9}, NativePoint {0, 13},
      nullptr, nullptr,
      0, 0, 0
    }
  }));

  patch.splice(NativePoint {0, 0}, NativePoint {0, 0}, NativePoint {0, 10});
  REQUIRE(patch.get_changes() == vector<Change>({
    Change {
      NativePoint {0, 0}, NativePoint {0, 0},
      NativePoint {0, 0}, NativePoint {0, 10},
      nullptr, nullptr,
      0, 0, 0
    },
    Change {
      NativePoint {0, 2}, NativePoint {0, 4},
      NativePoint {0, 12}, NativePoint {0, 13},
      nullptr, nullptr,
      0, 0, 0
    },
    Change {
      NativePoint {0, 5}, NativePoint {0, 8},
      NativePoint {0, 14}, NativePoint {0, 18},
      nullptr, nullptr,
      0, 0, 0
    },
    Change {
      NativePoint {0, 9}, NativePoint {0, 12},
      NativePoint {0, 19}, NativePoint {0, 23},
      nullptr, nullptr,
      0, 0, 0
    }
  }));
}

TEST_CASE("Patch::splice - overlapping with text") {
  Patch patch;

  patch.splice(
    NativePoint {0, 5},
    NativePoint {0, 3},
    NativePoint {0, 4},
    Text {u"abc"},
    Text {u"1234"}
  );
  REQUIRE(patch.get_changes() == vector<Change>({
    Change {
      NativePoint {0, 5}, NativePoint {0, 8},
      NativePoint {0, 5}, NativePoint {0, 9},
      get_text(u"abc").get(),
      get_text(u"1234").get(),
      0, 0, 0
    },
  }));

  // overlaps lower bound, has no upper bound.
  patch.splice(
    NativePoint {0, 7},
    NativePoint {0, 3},
    NativePoint {0, 4},
    Text {u"34d"},
    Text {u"5678"}
  );
  REQUIRE(patch.get_changes() == vector<Change>({
    Change {
      NativePoint {0, 5}, NativePoint {0, 9},
      NativePoint {0, 5}, NativePoint {0, 11},
      get_text(u"abcd").get(),
      get_text(u"125678").get(),
      0, 0, 0
    },
  }));

  // overlaps upper bound, has no lower bound.
  patch.splice(
    NativePoint {0, 3},
    NativePoint {0, 3},
    NativePoint {0, 4},
    Text {u"efa"},
    Text {u"1234"}
  );
  REQUIRE(patch.get_changes() == vector<Change>({
    {
      NativePoint {0, 3}, NativePoint {0, 9},
      NativePoint {0, 3}, NativePoint {0, 12},
      get_text(u"efabcd").get(),
      get_text(u"123425678").get(),
      0, 0, 0
    },
  }));

  // doesn't overlap lower bound, has no upper bound
  patch.splice(
    NativePoint {0, 15},
    NativePoint {0, 3},
    NativePoint {0, 4},
    Text {u"ghi"},
    Text {u"5678"}
  );
  REQUIRE(patch.get_changes() == vector<Change>({
    Change {
      NativePoint {0, 3}, NativePoint {0, 9},
      NativePoint {0, 3}, NativePoint {0, 12},
      get_text(u"efabcd").get(),
      get_text(u"123425678").get(),
      0, 0, 0
    },
    Change {
      NativePoint {0, 12}, NativePoint {0, 15},
      NativePoint {0, 15}, NativePoint {0, 19},
      get_text(u"ghi").get(),
      get_text(u"5678").get(),
      6, 9, 0
    },
  }));

  // surrounds two changes, has no lower or upper bound
  patch.splice(
    NativePoint {0, 1},
    NativePoint {0, 21},
    NativePoint {0, 5},
    Text {u"xx123425678yyy5678zzz"},
    Text {u"99999"}
  );
  REQUIRE(patch.get_changes() == vector<Change>({
    Change {
      NativePoint {0, 1}, NativePoint {0, 18},
      NativePoint {0, 1}, NativePoint {0, 6},
      get_text(u"xxefabcdyyyghizzz").get(),
      get_text(u"99999").get(),
      0, 0, 0
    }
  }));
}

TEST_CASE("Patch::splice - deleted_text_size") {
  Patch patch;

  patch.splice(NativePoint {0, 2}, NativePoint {0, 3}, NativePoint {0, 5}, optional<Text> {}, Text {u"xxxxx"}, 3);
  patch.splice(NativePoint {1, 0}, NativePoint {0, 0}, NativePoint {0, 1}, optional<Text> {}, Text {u"x"}, 0);
  REQUIRE(patch.get_changes().back().preceding_old_text_size == 3);

  patch.splice(NativePoint {0, 1}, NativePoint {0, 2}, NativePoint {0, 5}, optional<Text> {}, Text {u"xxxxx"}, 2);
  REQUIRE(patch.get_changes().back().preceding_old_text_size == 4);

  patch.splice(NativePoint {0, 8}, NativePoint {0, 4}, NativePoint {0, 5}, optional<Text> {}, Text {u"xxxxx"}, 4);
  REQUIRE(patch.get_changes().back().preceding_old_text_size == 6);

  patch.splice(NativePoint {0, 5}, NativePoint {0, 3}, NativePoint {0, 5}, optional<Text> {}, Text {u"xxxxx"}, 3);
  REQUIRE(patch.get_changes().back().preceding_old_text_size == 6);

  patch.splice(NativePoint {0, 0}, NativePoint {0, 16}, NativePoint {0, 5}, optional<Text> {}, Text {u"xxxxx"}, 16);
  REQUIRE(patch.get_changes().back().preceding_old_text_size == 8);
}

TEST_CASE("Patch::splice - inconsistent old text") {
  Patch patch;
  auto result = patch.splice(NativePoint{0, 0}, NativePoint{0, 0}, NativePoint{1, 4}, Text{u""}, Text{u"\n    "}, 0);
  REQUIRE(result == true);

  result = patch.splice(NativePoint{1, 0}, NativePoint{1, 0}, NativePoint{0, 0}, Text{u"  \n"}, Text{u""}, 3);
  REQUIRE(result == false);
  REQUIRE(patch.get_changes() == vector<Patch::Change>({
    Change{
      NativePoint{0, 0}, NativePoint{0, 0},
      NativePoint{0, 0}, NativePoint{1, 4},
      get_text(u"").get(),
      get_text(u"\n    ").get(),
      0, 0, 0
    },
  }));
}

TEST_CASE("Patch::find_changes_in_new_range") {
  Patch patch;

  patch.splice(NativePoint{0, 5}, NativePoint{0, 3}, NativePoint{0, 4});
  patch.splice(NativePoint{0, 10}, NativePoint{0, 3}, NativePoint{0, 4});
  patch.splice(NativePoint{0, 2}, NativePoint{0, 2}, NativePoint{0, 1});
  patch.splice(NativePoint{0, 0}, NativePoint{0, 0}, NativePoint{0, 10});

  REQUIRE(patch.get_changes_in_new_range(NativePoint{0, 12}, NativePoint{0, 20}) == vector<Change>({
    Change {
      NativePoint {0, 2}, NativePoint {0, 4},
      NativePoint {0, 12}, NativePoint {0, 13},
      nullptr, nullptr,
      0, 0, 0
    },
    Change {
      NativePoint {0, 5}, NativePoint {0, 8},
      NativePoint {0, 14}, NativePoint {0, 18},
      nullptr, nullptr,
      0, 0, 0
    },
    Change {
      NativePoint {0, 9}, NativePoint {0, 12},
      NativePoint {0, 19}, NativePoint {0, 23},
      nullptr, nullptr,
      0, 0, 0
    }
  }));

  REQUIRE(patch.get_changes_in_new_range(NativePoint{0, 12}, NativePoint{0, 15}) == vector<Change>({
    Change {
      NativePoint {0, 2}, NativePoint {0, 4},
      NativePoint {0, 12}, NativePoint {0, 13},
      nullptr, nullptr,
      0, 0, 0
    },
    Change {
      NativePoint {0, 5}, NativePoint {0, 8},
      NativePoint {0, 14}, NativePoint {0, 18},
      nullptr, nullptr,
      0, 0, 0
    },
  }));
}

TEST_CASE("Patch::serialize") {
  Patch patch;

  patch.splice(NativePoint {0, 5}, NativePoint {0, 3}, NativePoint {0, 4});
  patch.splice(NativePoint {0, 10}, NativePoint {0, 3}, NativePoint {0, 4});
  patch.splice(NativePoint {0, 2}, NativePoint {0, 2}, NativePoint {0, 1});
  patch.splice(NativePoint {0, 0}, NativePoint {0, 0}, NativePoint {0, 10});
  patch.grab_change_starting_before_old_position(NativePoint {0, 5}); // splay the middle
  REQUIRE(patch.get_changes() == vector<Change>({
    Change {
      NativePoint {0, 0}, NativePoint {0, 0},
      NativePoint {0, 0}, NativePoint {0, 10},
      nullptr, nullptr,
      0, 0, 0
    },
    Change {
      NativePoint {0, 2}, NativePoint {0, 4},
      NativePoint {0, 12}, NativePoint {0, 13},
      nullptr, nullptr,
      0, 0, 0
    },
    Change {
      NativePoint {0, 5}, NativePoint {0, 8},
      NativePoint {0, 14}, NativePoint {0, 18},
      nullptr, nullptr,
      0, 0, 0
    },
    Change {
      NativePoint {0, 9}, NativePoint {0, 12},
      NativePoint {0, 19}, NativePoint {0, 23},
      nullptr, nullptr,
      0, 0, 0
    }
  }));

  vector<uint8_t> bytes;
  Serializer serializer(bytes);
  patch.serialize(serializer);

  Deserializer deserializer(bytes);
  Patch patch_copy(deserializer);
  REQUIRE(patch_copy.get_changes() == vector<Change>({
    Change {
      NativePoint {0, 0}, NativePoint {0, 0},
      NativePoint {0, 0}, NativePoint {0, 10},
      nullptr, nullptr,
      0, 0, 0
    },
    Change {
      NativePoint {0, 2}, NativePoint {0, 4},
      NativePoint {0, 12}, NativePoint {0, 13},
      nullptr, nullptr,
      0, 0, 0
    },
    Change {
      NativePoint {0, 5}, NativePoint {0, 8},
      NativePoint {0, 14}, NativePoint {0, 18},
      nullptr, nullptr,
      0, 0, 0
    },
    Change {
      NativePoint {0, 9}, NativePoint {0, 12},
      NativePoint {0, 19}, NativePoint {0, 23},
      nullptr, nullptr,
      0, 0, 0
    }
  }));
}
