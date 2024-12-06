#include "editor.h"
#include "util.h"

#include <string_view>
#include <utility>
#include <concepts>
#include <ranges>
#include <filesystem>
#include <fstream>

#include <catch2/catch_test_macros.hpp>

struct EditorTest {
    void RequireView(std::string_view expected, size_t pos, size_t count) const {
        std::string result;
        for (auto c : editor.GetText(pos, count)) {
            result.push_back(c);
        }
        REQUIRE(result == expected);
    }

    void RequireText(std::string_view expected, size_t cursor_pos) const {
        REQUIRE(editor.GetPosition() == cursor_pos);
        REQUIRE(editor.Size() == expected.size());
        RequireView(expected, 0, editor.Size());
    }

    void Type(std::string_view text) {
        for (auto c : text) {
            editor.Type(c);
        }
    }

    void Clear() {
        while (editor.GetPosition() < editor.Size()) {
            editor.ShiftRight();
        }
        while (editor.Size()) {
            editor.Backspace();
        }
    }

    Editor editor;
};

TEST_CASE_METHOD(EditorTest, "Basic text typing") {
    RequireText("", 0);

    editor.Type('a');
    RequireText("a", 1);

    editor.Type('b');
    RequireText("ab", 2);

    editor.Type('c');
    RequireText("abc", 3);
}

TEST_CASE_METHOD(EditorTest, "Shift cursor") {
    Type("ab");
    editor.ShiftLeft();
    RequireText("ab", 1);

    editor.Type('c');
    RequireText("acb", 2);

    editor.ShiftRight();
    RequireText("acb", 3);

    editor.Type('d');
    RequireText("acbd", 4);
}

TEST_CASE_METHOD(EditorTest, "Cursor boundaries check") {
    editor.Type('a');
    editor.ShiftLeft();
    RequireText("a", 0);

    editor.ShiftLeft();
    RequireText("a", 0);

    editor.Type('b');
    RequireText("ba", 1);

    editor.ShiftRight();
    RequireText("ba", 2);

    editor.ShiftRight();
    RequireText("ba", 2);

    editor.Type('c');
    RequireText("bac", 3);
}

TEST_CASE_METHOD(EditorTest, "Backspace") {
    Type("abc");
    editor.Backspace();
    RequireText("ab", 2);

    editor.ShiftLeft();
    RequireText("ab", 1);

    editor.Backspace();
    RequireText("b", 0);

    editor.Backspace();
    RequireText("b", 0);

    editor.Type('c');
    RequireText("cb", 1);

    editor.ShiftRight();
    RequireText("cb", 2);

    editor.Backspace();
    RequireText("c", 1);

    editor.Backspace();
    RequireText("", 0);
}

TEST_CASE_METHOD(EditorTest, "Undo/Redo typing") {
    Type("abc");
    editor.Undo();
    RequireText("ab", 2);

    editor.Undo();
    RequireText("a", 1);

    editor.Redo();
    RequireText("ab", 2);

    editor.Redo();
    RequireText("abc", 3);
}

TEST_CASE_METHOD(EditorTest, "Undo/Redo are safe") {
    editor.Redo();
    RequireText("", 0);

    editor.Undo();
    RequireText("", 0);

    editor.Type('a');
    RequireText("a", 1);

    editor.Redo();
    RequireText("a", 1);

    editor.Undo();
    RequireText("", 0);

    editor.Undo();
    RequireText("", 0);
}

TEST_CASE_METHOD(EditorTest, "Type after Undo discards last Type") {
    Type("ab");
    editor.Undo();
    RequireText("a", 1);

    editor.Type('c');
    RequireText("ac", 2);

    editor.Redo();
    RequireText("ac", 2);
}

TEST_CASE_METHOD(EditorTest, "Undo/Redo ShiftLeft") {
    editor.Type('a');
    editor.ShiftLeft();
    RequireText("a", 0);

    editor.Undo();
    RequireText("a", 1);

    editor.Type('b');
    RequireText("ab", 2);

    editor.ShiftLeft();
    RequireText("ab", 1);

    editor.Undo();
    RequireText("ab", 2);

    editor.Redo();
    RequireText("ab", 1);

    editor.Type('c');
    RequireText("acb", 2);
}

TEST_CASE_METHOD(EditorTest, "Undo-Redo-Undo-Redo") {
    Type("ab");
    editor.Undo();
    editor.Redo();
    RequireText("ab", 2);
    editor.Undo();
    RequireText("a", 1);
    editor.Redo();
    RequireText("ab", 2);
}

TEST_CASE_METHOD(EditorTest, "Undo/Redo ShiftRight") {
    editor.Type('a');
    editor.ShiftLeft();
    editor.ShiftRight();
    RequireText("a", 1);

    editor.Undo();
    RequireText("a", 0);

    editor.Type('b');
    RequireText("ba", 1);

    editor.ShiftRight();
    RequireText("ba", 2);

    editor.Undo();
    RequireText("ba", 1);

    editor.Redo();
    RequireText("ba", 2);

    editor.Type('c');
    RequireText("bac", 3);
}

TEST_CASE_METHOD(EditorTest, "Undo/Redo Backspace") {
    Type("ab");
    editor.Backspace();
    RequireText("a", 1);

    editor.Undo();
    RequireText("ab", 2);

    editor.Redo();
    RequireText("a", 1);
}

TEST_CASE_METHOD(EditorTest, "Command does nothing - no undo") {
    editor.Type('a');
    editor.ShiftRight();
    RequireText("a", 1);

    editor.Undo();
    RequireText("", 0);
}

TEST_CASE_METHOD(EditorTest, "GetText") {
    constexpr size_t kSize = 50;
    constexpr auto kRange = std::views::iota(size_t{0}, kSize + 1);
    const auto text = RandomGenerator{572'720}.GenString(kSize);
    std::string_view view{text};

    Type(text);
    for (auto cursor : std::views::reverse(kRange)) {
        REQUIRE(editor.Size() == kSize);
        REQUIRE(editor.GetPosition() == cursor);
        for (auto pos : kRange) {
            for (auto count : std::views::iota(size_t{0}, kSize + 1 - pos)) {
                RequireView(view.substr(pos, count), pos, count);
            }
        }
        editor.ShiftLeft();
    }
    RequireText(view, 0);
}

TEST_CASE_METHOD(EditorTest, "AntiStupid test") {
    constexpr auto kSize = 1'000'000;

    RandomGenerator rand{864'473};
    const auto text = RandomGenerator{864'473}.GenString(kSize);

    Type(text);
    RequireText(text, text.size());
    Clear();

    for (auto c : std::views::reverse(text)) {
        editor.Type(c);
        editor.ShiftLeft();
    }
    RequireText(text, 0);

    for (auto i = 0; i < kSize / 2; ++i) {
        editor.ShiftRight();
    }
    RequireText(text, kSize / 2);

    for (auto i = 0; i < kSize / 2; ++i) {
        editor.Backspace();
    }
    RequireText({text.begin() + kSize / 2, text.end()}, 0);
}

TEST_CASE_METHOD(EditorTest, "Random typing") {
    constexpr auto kTextSize = 1'000'000;
    constexpr auto kCursorPosition = kTextSize / 2;
    constexpr auto kNumRandomOperations = 10 * kTextSize;
    constexpr auto kSeed = 864'473;
    constexpr auto kViewCount = 10'000;
    constexpr auto kViewPos = kCursorPosition - kViewCount / 2;
    // Number of Undo() calls that restore text after random typing
    constexpr auto kNumUndo = 4'530'798;

    RandomGenerator gen{kSeed};
    auto text = gen.GenString(kTextSize);
    Type(text);
    for (auto i = kCursorPosition; i < kTextSize; ++i) {
        editor.ShiftLeft();
    }

    std::ifstream in{std::filesystem::path{__FILE__}.parent_path() / "random.txt"};
    for (auto i : std::views::iota(0, kNumRandomOperations)) {
        switch (gen.GenInt(0, 6)) {
            case 0:
                editor.Type(gen.GenChar());
                break;
            case 1:
                editor.ShiftLeft();
                break;
            case 2:
                editor.ShiftRight();
                break;
            case 3:
                editor.Backspace();
                break;
            case 4:
                editor.Undo();
                break;
            case 5:
                editor.Redo();
                break;
            case 6:
                auto a = gen.GenInt<size_t>(0, editor.Size());
                auto b = gen.GenInt<size_t>(0, editor.Size());
                (a < b) ? editor.GetText(a, b - a) : editor.GetText(b, a - b);
                break;
        }
        if ((i + 1) % kTextSize == 0) {
            size_t cursor;
            std::string text;
            if (in >> cursor >> text) {
                REQUIRE(editor.GetPosition() == cursor);
                RequireView(text, kViewPos, kViewCount);
            } else {
                FAIL("Can't read from random.txt");
            }
        }
    }

    for (auto i = 0; i < kNumUndo; ++i) {
        editor.Undo();
    }
    RequireText(text, kCursorPosition);

    for (auto i = kCursorPosition; i < 2 * kTextSize; ++i) {
        editor.Undo();
    }
    RequireText("", 0);
}
