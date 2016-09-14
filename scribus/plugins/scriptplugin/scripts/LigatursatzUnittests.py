#!/usr/bin/env python
# -*- coding: utf8 -*-

"""
    This file as a hole is published under the MIT license:

    ✂----------------------------------------------------------------------

    The MIT License (MIT)

    Copyright (c) 2016 Lukas Sommer.

    Permission is hereby granted, free of charge, to any person obtaining
    a copy of this software and associated documentation files (the
    "Software"), to deal in the Software without restriction, including
    without limitation the rights to use, copy, modify, merge, publish,
    distribute, sublicense, and/or sell copies of the Software, and to
    permit persons to whom the Software is furnished to do so, subject to
    the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
    LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
    OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
    WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

    ✂----------------------------------------------------------------------
"""

from Ligatursatz import *
import scribus
import unittest
import types
import unicodedata

# This file is in UTF8. It uses also really quite some characters outside
# ASCII and even outside BMP. As sometimes the encoding makes problems
# (Scribus script console for example), we assert here that the encoding
# is interpreted correctly
if ord("a") != 97:
    raise RuntimeError("Source code encoding problem 01.")

if len("a") != 1:
    raise RuntimeError("Source code encoding problem 02.")

if ord(u"\xE4") != 0xE4:
    raise RuntimeError("Source code encoding problem 03.")

if len(u"\xE4") != 1:
    raise RuntimeError("Source code encoding problem 04.")

if ord(u"\u00E4") != 0xE4:
    raise RuntimeError("Source code encoding problem 05.")

if len(u"\u00E4") != 1:
    raise RuntimeError("Source code encoding problem 06.")

if ord(u"\U000000E4") != 0xE4:
    raise RuntimeError("Source code encoding problem 07.")

if len(u"\U000000E4") != 1:
    raise RuntimeError("Source code encoding problem 08.")

if ord(u"ä") != 0xE4:
    raise RuntimeError("Source code encoding problem 09.")

if len(u"ä") != 1:
    raise RuntimeError("Source code encoding problem 10.")

if ord(u"\u1E9E") != 0x1E9E:
    raise RuntimeError("Source code encoding problem 11.")

if len(u"\u1E9E") != 1:
    raise RuntimeError("Source code encoding problem 12.")

if ord(u"\U00001E9E") != 0x1E9E:
    raise RuntimeError("Source code encoding problem 13.")

if len(u"\U00001E9E") != 1:
    raise RuntimeError("Source code encoding problem 14.")

if ord(u"ẞ") != 0x1E9E:
    raise RuntimeError("Source code encoding problem 15.")

if len(u"ẞ") != 1:
    raise RuntimeError("Source code encoding problem 16.")

if ord(u"\U0001F404"[0]) != 0x1F404 and ord(u"\U0001F404"[0]) != 0xD83D:
    raise RuntimeError("Source code encoding problem 17.")

if len(u"\U0001F404") != 1 and len(u"\U0001F404") != 2:
    raise RuntimeError("Source code encoding problem 18.")

if ord(u"🐄"[0]) != 0x1F404 and ord(u"🐄"[0]) != 0xD83D:
    raise RuntimeError("Source code encoding problem 19.")

if len(u"🐄") != 1 and len(u"🐄") != 2:
    raise RuntimeError("Source code encoding problem 20.")


__version__ = '0.1'


def is_utf_32():
    """Older versions of Python (< 3.3) decide on the compile
    time of the Python interpreter if the type “unicode”
    (or, in 3.x versions, the string type) is a sequence of
    UTF-16 code units (a variable length encoding) or of
    UTF-32 code units (a fiexed length encoding). Starting
    from 3.3 Python should behave always as if it is a fixed
    length encoding (also if the internal representation might
    be different).
    Preconditions: None
    Postconditions: Returns True when this Python build behaves
    like working with UTF-32 encoding. Teturns False if this Python
    build behaves like working with UTF-16 encoding."""
    if len(u"a") == 1 and len(u"ẞ") == 1 and len(u"ﬀ") == 1 and len(u"🐄") == 1:
        return True
    if len(u"a") == 1 and len(u"ẞ") == 1 and len(u"ﬀ") == 1 and len(u"🐄") == 2:
        return False
    raise AssertionError("This exception should never be raised.")


def equivalent_characters(characters):
    """Takes a unicode string “characters”. It adds (+=) various
    transformations of the string content itself (upper, lower, title,
    all possible normalization forms) and all combinations of this
    transformations. Returns this result."""
    if type(characters) is not unicode:
        raise TypeError("")
    my_string = characters
    my_set = set(characters)
    stop = False
    while not stop:
        new_string = my_string + \
                     my_string.upper() + \
                     my_string.lower() + \
                     my_string.title() + \
                     unicodedata.normalize(u"NFC", my_string) + \
                     unicodedata.normalize(u"NFKC", my_string) + \
                     unicodedata.normalize(u"NFD", my_string) + \
                     unicodedata.normalize(u"NFKD", my_string)
        new_set = set(new_string)
        if new_set == my_set:
            stop = True
        else:
            my_string = new_string
            my_set = new_set
    return my_string


class HyphenatorTestCase(unittest.TestCase):
    """Tests for Hyphenator class."""

    def createHyphenatorWithNonUnicodePattern(self):
        Hyphenator("ab1cd")

    def hyphenateNonUnicodeWord(self):
        Hyphenator(u"ab1cd").hyphenate_word("abcd")

    def test_rejectNonUnicodePattern(self):
        """The constructor of the Hyphenation class raises a
        TypeError exception when called with a “str” argument."""
        self.assertRaises(
            TypeError,
            self.createHyphenatorWithNonUnicodePattern)

    def test_rejectNonUnicodeWords(self):
        """hyphenate_word raises a TypeError exception when
        called with a “str” argument."""
        self.assertRaises(TypeError, self.hyphenateNonUnicodeWord)

    def test_asciiPattern(self):
        """Pattern inside ASCII work as expected."""
        self.assertEqual(
            Hyphenator(u"ab1cd").hyphenate_word(u"abcd"),
            [u"ab", u"cd"])

    def test_ascii_pattern_final_number(self):
        """The algorithm risks to treat bad the pattern “von1”. For
        the word “von”, it could return “[u"von", u""]” instead
        of “[u"von"]”."""
        self.assertEqual(
            Hyphenator(u"von1").hyphenate_word(u"von"),
            [u"von"])

    def test_ascii_pattern_leading_number(self):
        """The algorithm risks to treat bad the pattern “von1”. For
        the word “von”, it could return “[u"von", u""]” instead
        of “[u"von"]”."""
        self.assertEqual(
            Hyphenator(u"1von").hyphenate_word(u"von"),
            [u"von"])

    def test_bmpPattern(self):
        """Pattern outside ASCII but inside Unicode BMP work as expected."""
        self.assertEqual(
            Hyphenator(u"äö1üß").hyphenate_word(u"äöüß"),
            [u"äö", u"üß"])

    def test_nonBmpPattern(self):
        """Pattern outside Unicode BMP work as expected."""
        self.assertEqual(
            Hyphenator(u"\U000103001\U00010304").hyphenate_word(
                u"\U00010300\U00010304"),
            [u"\U00010300", u"\U00010304"])
        self.assertEqual(
            Hyphenator(u"\U000103001\U00010304").hyphenate_word(
                u"abc\U00010300\U00010304def"),
            [u"abc\U00010300", u"\U00010304def"])

    def test_asciiUnicodeReturnsUnicode(self):
        """The elements of the array that is returned by hyphenate_word
        have the type “unicode”."""
        self.assertIs(
            type(Hyphenator(u"ab1cd").hyphenate_word(u"abcd")[1]),
            unicode)


class GermanLigatureSupportTestcase(unittest.TestCase):
    """Tests for GermanLigatureSupport()."""

    def test_exampleWord_1(self):
        """The text “auffallend” is split between “auf” and “fallend”
        by the hyphenation algorithm."""
        self.assertEqual(
            Hyphenator(GermanLigatureSupport().patterns()).hyphenate_word(
                u"auffallend"),
            [u"auf", u"fallend"])

    def test_exampleWord_2(self):
        """The text “von” is not split
        by the hyphenation algorithm."""
        self.assertEqual(
            Hyphenator(GermanLigatureSupport().patterns()).hyphenate_word(
                u"von"),
            [u"von"])

    def test_dataType(self):
        """The data type of the return value of patterns() is “unicode”."""
        self.assertIs(type(GermanLigatureSupport().patterns()), unicode)

    def test_raiseOnNonUnicode(self):
        """The function simple_case_fold_for_lookup raises a TypeError
        exception when called with
        something different than a “unicode” argument."""
        with self.assertRaises(TypeError):
            GermanLigatureSupport().simple_case_fold_for_lookup("A")
        with self.assertRaises(TypeError):
            GermanLigatureSupport().simple_case_fold_for_lookup(0)
        with self.assertRaises(TypeError):
            GermanLigatureSupport().simple_case_fold_for_lookup(1)
        with self.assertRaises(TypeError):
            GermanLigatureSupport().simple_case_fold_for_lookup()

    def test_caseA(self):
        """simple_case_fold_for_lookup() converts “A” to “a” (changed)."""
        self.assertEqual(
            GermanLigatureSupport().simple_case_fold_for_lookup(u"A"),
            u"a")

    def test_caseB(self):
        """simple_case_fold_for_lookup() converts “b” to “b” (unchanged)."""
        self.assertEqual(
            GermanLigatureSupport().simple_case_fold_for_lookup(u"b"),
            u"b")

    def test_caseOUmlaut(self):
        """simple_case_fold_for_lookup() converts “Ö” to “ö” (changed)."""
        self.assertEqual(
            GermanLigatureSupport().simple_case_fold_for_lookup(u"Ö"),
            u"ö")

    def test_caseLowerSharpS(self):
        """simple_case_fold_for_lookup() converts “ß” to “ß” (unchanged)."""
        self.assertEqual(
            GermanLigatureSupport().simple_case_fold_for_lookup(u"ß"),
            u"ß")

    def test_caseUpperSharpS(self):
        """simple_case_fold_for_lookup() converts “ẞ” to “ß” (changed)."""
        self.assertEqual(
            GermanLigatureSupport().simple_case_fold_for_lookup(u"ẞ"),
            u"ß")

    def test_caseLongS(self):
        """simple_case_fold_for_lookup() converts “ſ” to “s” (changed)."""
        self.assertEqual(
            GermanLigatureSupport().simple_case_fold_for_lookup(u"ſ"),
            u"s")

    def test_caseDoubleLongS(self):
        """simple_case_fold_for_lookup() converts “ſ” to “s” (changed)
        – also multiple times."""
        self.assertEqual(
            GermanLigatureSupport().simple_case_fold_for_lookup(u"ſſ"),
            u"ss")

    def test_caseSimple(self):
        """U+0130 LATIN CAPITAL LETTER I WITH DOT ABOVE maps to U+0069
        LATIN SMALL LETTER I. Unicode defines two types of case folding:
        “Simple” and “Full”. “Simple” is a one-to-one mapping, so the
        number of Unicode Scalar Values in the string does not change. But
        “Full” is a one-to-many mapping, so the number of Unicode Scalar
        Values in the string might change. In SpecialCasing.txt of the
        Unicode standard, the U+0130 LATIN CAPITAL LETTER I WITH DOT ABOVE
        maps (for lower casing) to 2: U+0069 LATIN SMALL LETTER I + U+0307
        COMBINING DOT ABOVE. We do not want this behaviour here, so we
        control that the function really does only simple mapping. The
        simple mapping (defined in the Unicode Character Database) maps it
        to the single lowercase U+0069 LATIN SMALL LETTER I (without
        combining character).
        """
        self.assertEqual(
            GermanLigatureSupport().simple_case_fold_for_lookup(u"\u0130"),
            u"\u0069")

    # It might be useful to check if characters outside BMP would have
    # lowercase character insight BMP. This would change the count of
    # code units in UTF-16 encoded strings. However, the Unicode
    # Character Database 7.0 does not define such a mapping.

    def test_caseFoldingDoesNotModifyPatterns(self):
        """As the case folding function and the patterns have to work
        well together, applying
        the case folding to the patterns should not change the patterns."""
        data = GermanLigatureSupport()
        self.assertTrue(data.simple_case_fold_for_lookup(data.patterns()) ==
                        data.patterns())

    def test_getWordCharacters(self):
        """get_word_characters contains all characters in the pattern,
        except those with special meaning: “.” and whitespace and the
        numbers."""
        my_ligature_support = GermanLigatureSupport()
        my_patterns = my_ligature_support.patterns()
        my_characters = my_ligature_support.get_word_characters()
        for item in my_patterns:
            if item not in ". \n0123456789":
                if item not in my_characters:
                    print "\n\nUnittest test_getWordCharacters: Fail on item:"
                    print item
                    print "\n"
                self.assertTrue(item in my_characters)

    def test_get_word_characters_false_for_some_characters(self):
        """get_word_characters returns “false” for the non-german example
        character “Ψ”."""
        self.assertFalse(
            u"Ψ" in GermanLigatureSupport().get_word_characters())
        self.assertFalse(
            u"0" in GermanLigatureSupport().get_word_characters())
        self.assertFalse(
            u"1" in GermanLigatureSupport().get_word_characters())

    def test_get_word_characters_is_bmp_only(self):
        # We suppose that it contains exclusively Unicode scalar values
        # inside the BMP (Basic Multilingual Plane).
        word_characters = GermanLigatureSupport().get_word_characters()
        for char in word_characters:
            self.assertTrue(is_bmp_scalar_only(char))

    def test_get_word_characters_covers_patters(self):
        """ get_word_characters() should contain at least every
        character in the pattern and its uppercase."""
        # get the patterns
        temp = GermanLigatureSupport().patterns()
        # remove characters with special meaning within the pattern
        temp = re.sub(u"[0-9\. \n]", u"", temp)
        # add upper, lower, normal forms, uppercase sharp s and small long s
        temp = equivalent_characters(temp + "ẞſ")
        # transform to a set
        my_pattern_set = set(temp)
        # get word character set
        my_word_character_set = set(
            GermanLigatureSupport().get_word_characters())
        # test if
        self.assertTrue(my_pattern_set < my_word_character_set)

    def test_get_word_characters_decompisition(self):
        """get_word_characters() returns a string with characters. Some of these
        characters can be precomposed Unicode characters, to they have
        a canonical decomposition form. This test controls if the string
        contains for each character that is contains, also its canonical
        decomposition characters.
        """
        # get original word character set
        my_original_word_character_set = set(
            GermanLigatureSupport().get_word_characters())
        # decomposed form
        my_decomposed_string = unicodedata.normalize(
            "NFD",
            GermanLigatureSupport().get_word_characters())
        # get combined original and decomposed character set
        my_decomposed_word_character_set = set(
            GermanLigatureSupport().get_word_characters() + \
                my_decomposed_string)
        # test if
        self.assertEqual(
            my_original_word_character_set,
            my_decomposed_word_character_set)

class InstructionProviderTestcase(unittest.TestCase):
    def test_throwsException(self):
        # Should raise an exception on non-Unicode argument.
        with self.assertRaises(TypeError):
            InstructionProvider().get_instructions("abc")

    def test_returnType(self):
        self.assertIs(
            type(InstructionProvider().get_instructions(u"abc")),
            list)

    def test_emptyString(self):
        self.assertEqual(
            InstructionProvider().get_instructions(u""),
            [])

    def test_foreignScript(self):
        # Test with some foreign characters inside BMP
        self.assertEqual(
            InstructionProvider().get_instructions(u"ぢっごかぽのた"),
            [None, None, None, None, None, None, None])

    def test_no_change_necessary(self):
        assert Hyphenator(GermanLigatureSupport().patterns()).hyphenate_word(
            u"von") == [u"von"]
        self.assertEqual(
            InstructionProvider().get_instructions(u"von"),
            [None, None, None])

    def test_addZwnj(self):
        assert Hyphenator(GermanLigatureSupport().patterns()).hyphenate_word(
                u"auffahrt") == [u"auf", u"fahrt"]
        self.assertEqual(
            InstructionProvider().get_instructions(u"Auffahrt"),
            [None, None, None, True, None, None, None, None])
        self.assertEqual(
            InstructionProvider().get_instructions(u"Schifffahrt"),
            [None, None, None, None, None, None, True, None, None, None, None])
        self.assertEqual(
            InstructionProvider().get_instructions(u"Schifffahrtsamt"),
            [None, None, None, None, None, None,
             True, None, None, None, None, None,
             True, None, None])

    def test_addZwnjToShyStrings(self):
        # Test if everything is okay when having soft hyphens in the string.
        self.assertEqual(
            InstructionProvider().get_instructions(u"Auff\u00ADahrt"),
            [None, None, None, True, None, None, None, None, None])
        self.assertEqual(
            InstructionProvider().get_instructions(u"Auff\u00AD\u00ADahrt"),
            [None, None, None, True, None, None, None, None, None, None])
        self.assertEqual(
            InstructionProvider().get_instructions(
                u"A\u00ADuff\u00AD\u00ADahrt"),
            [None, None, None, None, True, None, None, None, None, None, None])
        self.assertEqual(
            InstructionProvider().get_instructions(
                u"\u00ADAuff\u00AD\u00ADahrt"),
            [None, None, None, None, True, None, None, None, None, None, None])
        self.assertEqual(
            InstructionProvider().get_instructions(
                u"Schif\u00ADffahrt\u00ADsamt"),
            [None, None, None, None, None, None, None,
             True, None, None, None, None, None, None,
             True, None, None])

    def test_soft_hyphen_collision_1(self):
        """Test if everything is okay when having soft hyphens in the
        string, even if they are where there should be a ZWNJ."""
        self.assertEqual(
            InstructionProvider().get_instructions(u"Auf\u00ADfahrt"),
            [None, None, None, None, True, None, None, None, None])

    def test_soft_hyphen_collision_2(self):
        """Test if everything is okay when having soft hyphens in the
        string, even if they are where there should be a ZWNJ."""
        self.assertEqual(
            InstructionProvider().get_instructions(u"auf\u00ADfallen"),
            [None, None, None, None, True, None, None, None, None, None])

    def test_removeZwnj(self):
        self.assertEqual(
            InstructionProvider().get_instructions(
                u"Au\u200Cf\u200Cfahrt"),
            [None, None, False, None,
             None, None, None, None, None, None])
        self.assertEqual(
            InstructionProvider().get_instructions(
                u"Au\u200Cffahrt"),
            [None, None, False, None,
             True, None, None, None, None])
        self.assertEqual(
            InstructionProvider().get_instructions(
                u"\u200C\u200CAu\u200C\u200Cffahrt\u200C"),
            [False, False, None, None, False, False, None,
             True, None, None, None, None, False])
        self.assertEqual(
            InstructionProvider().get_instructions(
                u"\u200C\u200CAu\u200C\u200Cf\u00AD\u200C\u00ADfahrt\u200C"),
            [False, False, None, None, False, False, None, None, False, None,
             True, None, None, None, None, False])

    def test_outsideBmp(self):
        # Assume a certain result for using a BMP character that is
        # not in the patterns (here: first character).
        assert InstructionProvider().get_instructions(
            u"\u5614Au\u200C\u200Cf\u00AD\u200C\u00ADfahrt") ==\
               [None, None, None, False, False, None, None, False, None,
                True, None, None, None, None]
        # Now test if we get the same result if this first character
        # is outside BMP.
        if is_utf_32():
            self.assertEqual(
                InstructionProvider().get_instructions(
                    u"\U0001F404Au\u200C\u200Cf\u00AD\u200C\u00ADfahrt"),
                [None, None, None, False, False, None, None, False, None,
                 True, None, None, None, None])
        else:
            self.assertEqual(
                InstructionProvider().get_instructions(
                    u"\U0001F404Au\u200C\u200Cf\u00AD\u200C\u00ADfahrt"),
                [None, None, None, None, False, False, None, None, False, None,
                 True, None, None, None, None])

    def test_combiningCharacters(self):
        """Unicode can represent some glyphs by various alternative,
        but equivalent code point sequences. Example: “ä” can be
        represented as U+00E4 LATIN SMALL LETTER A WITH DIAERESIS, but it
        can also be respresented by the sequence U+0061 LATIN SMALL LETTER A
        U+0308 COMBINING DIAERESIS. Both are equivalent.

        The documentation of InstructionProvider.get_instructions() claims
        that it has hand-coded support for some equivalent forms. This
        test controls if this declaration in the documentation
        covers all characters in the patterns that have various
        representations (normal forms) in Unicode.
        """
        # get the patterns
        temp = GermanLigatureSupport().patterns()
        # remove characters with special meaning within the pattern
        temp = re.sub(u"[0-9\. \n]", u"", temp)
        # expand to all possible representations (Unicode normalization)
        temp = equivalent_characters(temp)
        # remove characters of which we know that they have no decomposition
        temp = re.sub(u"[a-zßA-Z]", u"", temp)
        # transform to a set
        my_combining_character_set = set(temp)
        # test if equal to what the documentation of
        # InstructionProvider.get_instructions() claims
        # to support as combining characters.
        self.assertTrue(
            my_combining_character_set <= \
                set(u"šŠâÂäÄéÉóÓöÖüÜ\u030C\u0302\u0308\u0301"))

    def test_normalization_example_1(self):
        # Salatöl
        assert InstructionProvider().get_instructions(u"SALATÖL") == \
               [None, None, None, None, None, True, None]
        self.assertEqual(
            InstructionProvider().get_instructions(u"SALATO\u0308L"),
            [None, None, None, None, None, True, None, None])
        self.assertEqual(
            InstructionProvider().get_instructions(u"Salato\u0308L"),
            [None, None, None, None, None, True, None, None])
        self.assertEqual(
            InstructionProvider().get_instructions(u"salato\u0308L"),
            [None, None, None, None, None, True, None, None])
        self.assertEqual(
            InstructionProvider().get_instructions(u"Salat\u00ADo\u0308L"),
            [None, None, None, None, None, None, True, None, None])

    def test_normalization_example_2(self):
        # Menükarte
        assert InstructionProvider().get_instructions(u"MENÜKARTE") == \
               [None, None, None, None, True, None, None, None, None]
        self.assertEqual(
            InstructionProvider().get_instructions(u"MENU\u0308KARTE"),
            [None, None, None, None, None, True, None, None, None, None])
        self.assertEqual(
            InstructionProvider().get_instructions(u"Menu\u0308karte"),
            [None, None, None, None, None, True, None, None, None, None])
        self.assertEqual(
            InstructionProvider().get_instructions(u"menu\u0308karte"),
            [None, None, None, None, None, True, None, None, None, None])
        self.assertEqual(
            InstructionProvider().get_instructions(u"Menu\u0308\u00ADkarte"),
            [None, None, None, None, None, None, True, None, None, None, None])


class IsBmpScalarOnlyTestcase(unittest.TestCase):
    def test_throwsException(self):
        with self.assertRaises(TypeError):
            is_bmp_scalar_only("abc")

    def test_isTrue(self):
        # Empty string
        self.assertTrue(is_bmp_scalar_only(u""))
        # First BMP codepoint
        self.assertTrue(is_bmp_scalar_only(u"\u0000"))
        # First BMP codepoint + “b”
        self.assertTrue(is_bmp_scalar_only(u"a\u0000b"))
        # Various control character codepoints (BMP)
        self.assertTrue(is_bmp_scalar_only(u"a\u0000b\u0000\u0001\u0002"))
        # Last BMP codepoint
        self.assertTrue(is_bmp_scalar_only(u"a\uFFFF"))
        # A normal character
        self.assertTrue(is_bmp_scalar_only(u"a"))
        self.assertTrue(is_bmp_scalar_only(u"\xE4"))
        self.assertTrue(is_bmp_scalar_only(u"\u00E4"))
        self.assertTrue(is_bmp_scalar_only(u"\U000000E4"))
        # LATIN CAPITAL LETTER SHARP S
        self.assertTrue(is_bmp_scalar_only(u"\u1E9E"))
        self.assertTrue(is_bmp_scalar_only(u"\U00001E9E"))
        self.assertTrue(is_bmp_scalar_only(u"ẞ"))
        # Replacement character
        self.assertTrue(is_bmp_scalar_only(u"\uFFFD"))
        # Byte order mark
        self.assertTrue(is_bmp_scalar_only(u"\uFEFF"))
        self.assertTrue(is_bmp_scalar_only(u"\uFFFE"))
        # Last codepoint of the first Unicode scalar value block
        self.assertTrue(is_bmp_scalar_only(u"\uD7FF"))
        # First codepoint of the second Unicode scalar value block
        self.assertTrue(is_bmp_scalar_only(u"\uE000"))

    def test_isFalse(self):
        # COW (a character outside BMP)
        self.assertFalse(is_bmp_scalar_only(u"\U0001F404"))
        self.assertFalse(is_bmp_scalar_only(u"\uD83D\uDC04"))
        self.assertFalse(is_bmp_scalar_only(u"\U0000D83D\U0000DC04"))
        self.assertFalse(is_bmp_scalar_only(u"🐄"))
        self.assertFalse(is_bmp_scalar_only(u"abc🐄abc"))
        self.assertFalse(is_bmp_scalar_only(u"🐄abc"))
        self.assertFalse(is_bmp_scalar_only(u"abc🐄"))
        # Half of a surrogate pair
        self.assertFalse(is_bmp_scalar_only(u"\U0000D800"))
        self.assertFalse(is_bmp_scalar_only(u"\uD800"))
        # Half of a surrogate pair with some ASCII codepoints
        self.assertFalse(is_bmp_scalar_only(u"abc\U0000D800abc"))
        self.assertFalse(is_bmp_scalar_only(u"abc\uD800abc"))


def close_all_documents():
    """Closes all open documents (without saving changes)."""
    while scribus.haveDoc() > 0:
        scribus.closeDoc()


class ScribusTestcaseBaseClass(unittest.TestCase):

    def createOneDocument(self):
        """Create one new document."""
        scribus.newDocument(scribus.PAPER_A4,
                            (20, 20, 20, 20),
                            scribus.PORTRAIT,
                            1,
                            scribus.UNIT_MILLIMETERS,
                            scribus.PAGE_1,
                            0,
                            1)

    def closeAllDocumentsAndCreateOneNewDocument(self):
        close_all_documents()
        self.createOneDocument()

    def getIdentifierDataType(self):
        # Create a helper document
        self.createOneDocument()
        # Create an object
        scribus.createText(20, 20, 20, 20, "test1")
        # Get an object identifier and determine its data type
        scribus.selectObject("test1")
        return type(scribus.getSelectedObject())


class GetAffectedTextObjectsTestcase(ScribusTestcaseBaseClass):
    def test_noDocument(self):
        """The function returns an empty list when no document is
        available (and does not crash)."""
        close_all_documents()
        if scribus.haveDoc() != 0:
            raise AssertionError()
        self.assertEqual(get_affected_text_objects(), [])

    def test_twoDocuments(self):
        """The function operates only on the selection of the
        current document, while all other
        open documents are ignored."""
        close_all_documents()
        self.createOneDocument()
        scribus.createText(20, 20, 20, 20, "test01")
        scribus.createText(20, 20, 20, 20, "test02")
        scribus.createText(20, 20, 20, 20, "test03")
        scribus.selectObject("test01")
        self.createOneDocument()
        scribus.createText(20, 20, 20, 20, "test04")
        scribus.createText(20, 20, 20, 20, "test05")
        scribus.createText(20, 20, 20, 20, "test06")
        scribus.selectObject("test05")
        if scribus.haveDoc() != 2:
            raise AssertionError()
        self.assertEqual(get_affected_text_objects(), ["test05"])

    def test_noSelection(self):
        """If no object is selected, then an empty list is returned."""
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createText(20, 20, 20, 20, "test07")
        scribus.createText(20, 20, 20, 20, "test08")
        scribus.createText(20, 20, 20, 20, "test09")
        self.assertEqual(get_affected_text_objects(), [])

    def test_onlyTextFrames(self):
        """If there are also non-text-frame objects, these objects are
        not returned."""
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createText(20, 20, 20, 20, "test10")
        scribus.selectObject("test10")
        scribus.createRect(20, 20, 20, 20, "test11")
        scribus.selectObject("test11")
        scribus.createText(20, 20, 20, 20, "test12")
        scribus.selectObject("test12")
        self.assertEqual(get_affected_text_objects(), ["test10", "test12"])


class StoryInterfaceTestcase(ScribusTestcaseBaseClass):
    def test_ConstructorRaiseOnMissingArgument(self):
        with self.assertRaises(TypeError):
            StoryInterface()

    def test_constructorRaiseOnWrongArgumentType(self):
        # assert that this is “str”
        assert (self.getIdentifierDataType() is str)
        # We know now for that the identifier type is “str”. We test now
        # various other data types to make sure that they raise an exception.
        with self.assertRaises(TypeError):
            StoryInterface(1)
        with self.assertRaises(TypeError):
            StoryInterface(0)
        with self.assertRaises(TypeError):
            StoryInterface([])
        with self.assertRaises(TypeError):
            StoryInterface(["abc"])
        with self.assertRaises(TypeError):
            StoryInterface(u"test")
        with self.assertRaises(TypeError):
            StoryInterface(("a", "b"))
        with self.assertRaises(TypeError):
            StoryInterface(None)
        with self.assertRaises(TypeError):
            StoryInterface("")

    def test_lengthIsCorrect(self):
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createText(80, 80, 80, 80, "test01")
        my_interface_object = StoryInterface("test01")
        with self.assertRaises(IndexError):
            my_interface_object.read_text(0, my_interface_object.length() + 1)
        self.assertIs(type(my_interface_object.length()), int)
        self.assertEqual(my_interface_object.length(), 0)
        with self.assertRaises(IndexError):
            my_interface_object.read_text(my_interface_object.length(), 0)
        with self.assertRaises(IndexError):
            my_interface_object.read_text(my_interface_object.length(), 1)
        scribus.insertText(u"abc", 0, "test01")
        with self.assertRaises(IndexError):
            my_interface_object.read_text(0, my_interface_object.length() + 1)
        self.assertIs(type(my_interface_object.length()), int)
        with self.assertRaises(IndexError):
            my_interface_object.read_text(my_interface_object.length(), 0)
        with self.assertRaises(IndexError):
            my_interface_object.read_text(my_interface_object.length(), 1)
        self.assertIs(my_interface_object.length(), 3)
        scribus.selectText(0, 0, "test01")
        self.assertIs(my_interface_object.length(), 3)
        scribus.selectText(1, 1, "test01")
        self.assertIs(my_interface_object.length(), 3)

    def test_lengthRaises(self):
        self.closeAllDocumentsAndCreateOneNewDocument()
        my_interface_object = StoryInterface("test01")
        # Should raise an exception if object does not exist.
        with self.assertRaises(scribus.NoValidObjectError):
            my_interface_object.length()
        # Should raise an exception if object is not a textFrame.
        scribus.createRect(80, 80, 80, 80, "test01")
        with self.assertRaises(BaseException):
            my_interface_object.length()

    def test_readTextRaisesOnArgumentError(self):
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createText(20, 20, 20, 20, "test01")
        my_interface_object = StoryInterface("test01")
        with self.assertRaises(TypeError):
            my_interface_object.read_text()
        with self.assertRaises(TypeError):
            my_interface_object.read_text(0)
        with self.assertRaises(TypeError):
            my_interface_object.read_text(1)
        with self.assertRaises(TypeError):
            my_interface_object.read_text(0.0, 0.0)
        with self.assertRaises(TypeError):
            my_interface_object.read_text("0")
        with self.assertRaises(TypeError):
            my_interface_object.read_text("")
        with self.assertRaises(IndexError):
            my_interface_object.read_text(-1, 1)
        with self.assertRaises(IndexError):
            my_interface_object.read_text(1, -1)

    def test_readTextRaisesOnScribusObjectTypeError1(self):
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createRect(20, 20, 20, 20, "test02")
        my_interface_object = StoryInterface("test02")
        with self.assertRaises(RuntimeError):
            my_interface_object.read_text(0, 0)

    def test_readTextRaisesOnScribusObjectTypeError2(self):
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createRect(20, 20, 20, 20, "test02")
        my_interface_object = StoryInterface("test02")
        with self.assertRaises(RuntimeError):
            my_interface_object.read_text(0, 1)

    def test_readTextRaisesOnScribusObjectTypeError3(self):
        # Test behaviour for non-existing object
        self.closeAllDocumentsAndCreateOneNewDocument()
        my_interface_object = StoryInterface("test02")
        with self.assertRaises(scribus.NoValidObjectError):
            my_interface_object.read_text(0, 0)

    def test_readTextRaisesOnScribusObjectTypeError4(self):
        # Test behaviour for non-existing object
        self.closeAllDocumentsAndCreateOneNewDocument()
        my_interface_object = StoryInterface("test02")
        with self.assertRaises(scribus.NoValidObjectError):
            my_interface_object.read_text(0, 1)

    def test_readTextRaisesIndexErrorWhenOutOfRange(self):
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createText(20, 20, 20, 20, "test01")
        my_interface_object = StoryInterface("test01")
        scribus.insertText(u"abc", 0, "test01")
        with self.assertRaises(IndexError):
            my_interface_object.read_text(0, 10)
        with self.assertRaises(IndexError):
            my_interface_object.read_text(2, 10)
        with self.assertRaises(IndexError):
            my_interface_object.read_text(1, -1)
        with self.assertRaises(IndexError):
            my_interface_object.read_text(-1, 0)
        assert my_interface_object.length() == 3
        with self.assertRaises(IndexError):
            my_interface_object.read_text(4, 0)
        with self.assertRaises(IndexError):
            my_interface_object.read_text(3, 0)
        with self.assertRaises(IndexError):
            my_interface_object.read_text(3, 1)

    def test_returnUnicodeType(self):
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createText(80, 80, 80, 80, "test01")
        my_interface_object = StoryInterface("test01")
        scribus.insertText(u"abc", 0, "test01")
        self.assertIs(type(my_interface_object.read_text(0, 1)), unicode)

    def test_returnUnicodeTypeEvenForEmptyContent(self):
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createText(80, 80, 80, 80, "test01")
        my_interface_object = StoryInterface("test01")
        scribus.insertText(u"abc", 0, "test01")
        self.assertIs(type(my_interface_object.read_text(1, 0)), unicode)

    def test_readAsciiTextCorrectlyFromNonLinkedFrame(self):
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createText(80, 80, 80, 80, "test01")
        my_interface_object = StoryInterface("test01")
        scribus.insertText(u"abc", 0, "test01")
        self.assertEqual(my_interface_object.read_text(0, 0), u"")
        self.assertEqual(my_interface_object.read_text(0, 1), u"a")
        self.assertEqual(my_interface_object.read_text(0, 2), u"ab")
        self.assertEqual(my_interface_object.read_text(0, 3), u"abc")
        self.assertEqual(my_interface_object.read_text(1, 0), u"")
        self.assertEqual(my_interface_object.read_text(1, 1), u"b")
        self.assertEqual(my_interface_object.read_text(1, 2), u"bc")
        self.assertEqual(my_interface_object.read_text(2, 0), u"")
        self.assertEqual(my_interface_object.read_text(2, 1), u"c")

    def test_readBmpTextCorrectlyFromNonLinkedFrame(self):
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createText(80, 80, 80, 80, "test01")
        my_interface_object = StoryInterface("test01")
        scribus.insertText(u"äöü", 0, "test01")
        self.assertEqual(my_interface_object.read_text(0, 0), u"")
        self.assertEqual(my_interface_object.read_text(0, 1), u"ä")
        self.assertEqual(my_interface_object.read_text(0, 2), u"äö")
        self.assertEqual(my_interface_object.read_text(0, 3), u"äöü")
        self.assertEqual(my_interface_object.read_text(1, 0), u"")
        self.assertEqual(my_interface_object.read_text(1, 1), u"ö")
        self.assertEqual(my_interface_object.read_text(1, 2), u"öü")
        self.assertEqual(my_interface_object.read_text(2, 0), u"")
        self.assertEqual(my_interface_object.read_text(2, 1), u"ü")

    def test_readNonBmpTextCorrectlyFromNonLinkedFrame(self):
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createText(80, 80, 80, 80, "test01")
        my_interface_object = StoryInterface("test01")
        scribus.insertText(u"🐄🐅🐆🐇🐈", 0, "test01")
        self.assertEqual(
            my_interface_object.read_text(0, my_interface_object.length()),
            u"🐄🐅🐆🐇🐈")

    def test_readBmpTextCorrectlyFromLinkedFrame(self):
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createText(80, 80, 80, 80, "test01")
        scribus.createText(80, 80, 80, 80, "test02")
        my_interface_object_1 = StoryInterface("test01")
        my_interface_object_2 = StoryInterface("test02")
        scribus.linkTextFrames("test01", "test02")
        scribus.insertText(u"äöü", 0, "test01")
        self.assertEqual(my_interface_object_1.read_text(1, 1), u"ö")
        self.assertEqual(my_interface_object_2.read_text(1, 1), u"ö")

    def test_deleteSingleCharacterCorrectlyFromNonLinkedFrame(self):
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createText(80, 80, 80, 80, "test01")
        my_interface_object = StoryInterface("test01")
        scribus.insertText(u"äßü", 0, "test01")
        my_interface_object.delete_text(1, 1)
        self.assertEqual(
            my_interface_object.read_text(0, my_interface_object.length()),
            u"äü")
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createText(80, 80, 80, 80, "test01")
        my_interface_object = StoryInterface("test01")
        scribus.insertText(u"äßü", 0, "test01")
        my_interface_object.delete_text(0, 1)
        self.assertEqual(
            my_interface_object.read_text(0, my_interface_object.length()),
            u"ßü")
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createText(80, 80, 80, 80, "test01")
        my_interface_object = StoryInterface("test01")
        scribus.insertText(u"äßü", 0, "test01")
        my_interface_object.delete_text(2, 1)
        self.assertEqual(
            my_interface_object.read_text(0, my_interface_object.length()),
            u"äß")

    def test_deleteMultipleCharactersCorrectlyFromNonLinkedFrame(self):
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createText(80, 80, 80, 80, "test01")
        my_interface_object = StoryInterface("test01")
        scribus.insertText(u"äßkkßü", 0, "test01")
        my_interface_object.delete_text(1, 4)
        self.assertEqual(
            my_interface_object.read_text(0, my_interface_object.length()),
            u"äü")
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createText(80, 80, 80, 80, "test01")
        my_interface_object = StoryInterface("test01")
        scribus.insertText(u"äßkkßü", 0, "test01")
        my_interface_object.delete_text(0, 4)
        self.assertEqual(
            my_interface_object.read_text(0, my_interface_object.length()),
            u"ßü")
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createText(80, 80, 80, 80, "test01")
        my_interface_object = StoryInterface("test01")
        scribus.insertText(u"äßkkßü", 0, "test01")
        my_interface_object.delete_text(2, 4)
        self.assertEqual(
            my_interface_object.read_text(0, my_interface_object.length()),
            u"äß")

    def test_deleteMultipleCharactersCorrectlyFromLinkedFrame(self):
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createText(80, 80, 80, 80, "test01")
        scribus.createText(80, 80, 80, 80, "test02")
        my_interface_object_1 = StoryInterface("test01")
        my_interface_object_2 = StoryInterface("test02")
        scribus.linkTextFrames("test01", "test02")
        scribus.insertText(u"äöüßéáú", 0, "test02")
        my_interface_object_2.delete_text(3, 2)
        self.assertEqual(
            my_interface_object_2.read_text(0, my_interface_object_2.length()),
            u"äöüáú")
        self.assertEqual(
            my_interface_object_1.read_text(0, my_interface_object_1.length()),
            u"äöüáú")

    def test_delTextRaisesOnArgumentError(self):
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createText(20, 20, 20, 20, "test01")
        my_interface_object = StoryInterface("test01")
        with self.assertRaises(TypeError):
            my_interface_object.delete_text()
        with self.assertRaises(TypeError):
            my_interface_object.delete_text(0)
        with self.assertRaises(TypeError):
            my_interface_object.delete_text(1)
        with self.assertRaises(TypeError):
            my_interface_object.delete_text(0.0, 0.0)
        with self.assertRaises(TypeError):
            my_interface_object.delete_text("0")
        with self.assertRaises(TypeError):
            my_interface_object.delete_text("")
        with self.assertRaises(IndexError):
            my_interface_object.delete_text(-1, 1)
        with self.assertRaises(IndexError):
            my_interface_object.delete_text(1, -1)

    def test_delTextRaisesOnScribusObjectTypeError1(self):
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createRect(20, 20, 20, 20, "test02")
        my_interface_object = StoryInterface("test02")
        with self.assertRaises(RuntimeError):
            my_interface_object.delete_text(0, 0)

    def test_delTextRaisesOnScribusObjectTypeError2(self):
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createRect(20, 20, 20, 20, "test02")
        my_interface_object = StoryInterface("test02")
        with self.assertRaises(RuntimeError):
            my_interface_object.delete_text(0, 1)

    def test_delTextRaisesOnScribusObjectTypeError3(self):
        # Test behaviour for non-existing object
        self.closeAllDocumentsAndCreateOneNewDocument()
        my_interface_object = StoryInterface("test02")
        with self.assertRaises(scribus.NoValidObjectError):
            my_interface_object.delete_text(0, 0)

    def test_delTextRaisesOnScribusObjectTypeError4(self):
        # Test behaviour for non-existing object
        self.closeAllDocumentsAndCreateOneNewDocument()
        my_interface_object = StoryInterface("test02")
        with self.assertRaises(scribus.NoValidObjectError):
            my_interface_object.delete_text(0, 1)

    def test_deleteTextCorrectly(self):
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createText(20, 20, 20, 20, "test01")
        my_interface_object = StoryInterface("test01")
        scribus.insertText(u"abc", 0, "test01")
        my_interface_object.delete_text(1, 0)
        self.assertEqual(
            my_interface_object.read_text(
                0,
                my_interface_object.length()),
            u"abc"
        )
        scribus.createText(20, 20, 20, 20, "test02")
        my_interface_object = StoryInterface("test02")
        scribus.insertText(u"abc", 0, "test02")
        my_interface_object.delete_text(0, 1)
        self.assertEqual(
            my_interface_object.read_text(
                0,
                my_interface_object.length()),
            u"bc"
        )
        scribus.createText(20, 20, 20, 20, "test03")
        my_interface_object = StoryInterface("test03")
        scribus.insertText(u"abc", 0, "test03")
        my_interface_object.delete_text(1, 1)
        self.assertEqual(
            my_interface_object.read_text(
                0,
                my_interface_object.length()),
            u"ac"
        )
        scribus.createText(20, 20, 20, 20, "test04")
        my_interface_object = StoryInterface("test04")
        scribus.insertText(u"abc", 0, "test04")
        my_interface_object.delete_text(2, 1)
        self.assertEqual(
            my_interface_object.read_text(
                0,
                my_interface_object.length()),
            u"ab"
        )
        scribus.createText(20, 20, 20, 20, "test05")
        my_interface_object = StoryInterface("test05")
        scribus.insertText(u"äöüß", 0, "test05")
        my_interface_object.delete_text(0, 2)
        self.assertEqual(
            my_interface_object.read_text(
                0,
                my_interface_object.length()),
            u"üß"
        )
        scribus.createText(20, 20, 20, 20, "test06")
        my_interface_object = StoryInterface("test06")
        scribus.insertText(u"äöüß", 0, "test06")
        my_interface_object.delete_text(1, 2)
        self.assertEqual(
            my_interface_object.read_text(
                0,
                my_interface_object.length()),
            u"äß"
        )
        scribus.createText(20, 20, 20, 20, "test07")
        my_interface_object = StoryInterface("test07")
        scribus.insertText(u"äöüß", 0, "test07")
        my_interface_object.delete_text(0, 2)
        self.assertEqual(
            my_interface_object.read_text(
                0,
                my_interface_object.length()),
            u"üß"
        )

    def test_deleteBmpTextCorrectlyFromLinkedFrame(self):
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createText(80, 80, 80, 80, "test01")
        scribus.createText(80, 80, 80, 80, "test02")
        my_interface_object_1 = StoryInterface("test01")
        my_interface_object_2 = StoryInterface("test02")
        scribus.linkTextFrames("test01", "test02")
        scribus.insertText(u"äöüßéá", 0, "test01")
        my_interface_object_1.delete_text(1, 1)
        self.assertEqual(
            my_interface_object_1.read_text(
                0,
                my_interface_object_1.length()),
            u"äüßéá")
        self.assertEqual(
            my_interface_object_2.read_text(
                0,
                my_interface_object_2.length()),
            u"äüßéá")
        my_interface_object_2.delete_text(3, 1)
        self.assertEqual(
            my_interface_object_1.read_text(
                0,
                my_interface_object_1.length()),
            u"äüßá")
        self.assertEqual(
            my_interface_object_2.read_text(
                0,
                my_interface_object_2.length()),
            u"äüßá")

    def test_delTextRaisesIndexErrorWhenOutOfRange(self):
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createText(20, 20, 20, 20, "test01")
        my_interface_object = StoryInterface("test01")
        scribus.insertText(u"abc", 0, "test01")
        with self.assertRaises(IndexError):
            my_interface_object.delete_text(0, 10)
        with self.assertRaises(IndexError):
            my_interface_object.delete_text(2, 10)
        with self.assertRaises(IndexError):
            my_interface_object.delete_text(1, -1)
        with self.assertRaises(IndexError):
            my_interface_object.delete_text(-1, 0)
        assert my_interface_object.length() == 3
        with self.assertRaises(IndexError):
            my_interface_object.delete_text(0, 4)
        with self.assertRaises(IndexError):
            my_interface_object.delete_text(3, 0)
        with self.assertRaises(IndexError):
            my_interface_object.delete_text(3, 1)

    def test_returnNoneType(self):
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createText(80, 80, 80, 80, "test01")
        my_interface_object = StoryInterface("test01")
        scribus.insertText(u"abc", 0, "test01")
        self.assertIs(
            type(my_interface_object.delete_text(0, 1)),
            types.NoneType)

    def test_returnNoneTypeEvenForEmptyContent(self):
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createText(80, 80, 80, 80, "test01")
        my_interface_object = StoryInterface("test01")
        scribus.insertText(u"abc", 0, "test01")
        self.assertIs(type(
            my_interface_object.delete_text(1, 0)),
            types.NoneType)

    def test_delTextCorrectlyFromNonLinkedFrame(self):
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createText(80, 80, 80, 80, "test01")
        my_interface_object = StoryInterface("test01")
        scribus.insertText(u"abc", 0, "test01")
        my_interface_object.delete_text(0, 0)
        self.assertEqual(
            my_interface_object.read_text(0, my_interface_object.length()),
            u"abc")
        scribus.createText(80, 80, 80, 80, "test02")
        my_interface_object = StoryInterface("test02")
        scribus.insertText(u"abc", 0, "test02")
        my_interface_object.delete_text(0, 1)
        self.assertEqual(
            my_interface_object.read_text(0, my_interface_object.length()),
            u"bc")
        scribus.createText(80, 80, 80, 80, "test03")
        my_interface_object = StoryInterface("test03")
        scribus.insertText(u"abc", 0, "test03")
        my_interface_object.delete_text(0, 2)
        self.assertEqual(
            my_interface_object.read_text(0, my_interface_object.length()),
            u"c")
        scribus.createText(80, 80, 80, 80, "test04")
        my_interface_object = StoryInterface("test04")
        scribus.insertText(u"abc", 0, "test04")
        my_interface_object.delete_text(0, 3)
        self.assertEqual(
            my_interface_object.length(),
            0)
        scribus.createText(80, 80, 80, 80, "test05")
        my_interface_object = StoryInterface("test05")
        scribus.insertText(u"abc", 0, "test05")
        my_interface_object.delete_text(1, 0)
        self.assertEqual(
            my_interface_object.read_text(0, my_interface_object.length()),
            u"abc")
        scribus.createText(80, 80, 80, 80, "test06")
        my_interface_object = StoryInterface("test06")
        scribus.insertText(u"abc", 0, "test06")
        my_interface_object.delete_text(1, 1)
        self.assertEqual(
            my_interface_object.read_text(0, my_interface_object.length()),
            u"ac")
        scribus.createText(80, 80, 80, 80, "test07")
        my_interface_object = StoryInterface("test07")
        scribus.insertText(u"abc", 0, "test07")
        my_interface_object.delete_text(1, 2)
        self.assertEqual(
            my_interface_object.read_text(0, my_interface_object.length()),
            u"a")

    def test_delCorrectlyFromNonLinkedFrameWithOutsideBmp(self):
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createText(80, 80, 80, 80, "test08")
        my_interface_object = StoryInterface("test08")
        scribus.insertText(u"a🐄bc", 0, "test08")
        my_interface_object.delete_text(1, 1)
        self.assertEqual(
            my_interface_object.read_text(my_interface_object.length() - 2, 1),
            u"b")

    def test_delTextCorrectlyFromLinkedFrame1(self):
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createText(80, 80, 80, 80, "test01")
        scribus.createText(80, 80, 80, 80, "test02")
        my_interface_object_1 = StoryInterface("test01")
        my_interface_object_2 = StoryInterface("test02")
        scribus.linkTextFrames("test01", "test02")
        scribus.insertText(u"äöü", 0, "test01")
        my_interface_object_1.delete_text(1, 1)
        self.assertEqual(
            my_interface_object_1.read_text(
                0,
                my_interface_object_1.length()),
            u"äü")
        self.assertEqual(
            my_interface_object_2.read_text(
                0,
                my_interface_object_2.length()),
            u"äü")

    def test_delTextCorrectlyFromLinkedFrame2(self):
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createText(80, 80, 80, 80, "test01")
        scribus.createText(80, 80, 80, 80, "test02")
        my_interface_object_1 = StoryInterface("test01")
        my_interface_object_2 = StoryInterface("test02")
        scribus.linkTextFrames("test01", "test02")
        scribus.insertText(u"äöü", 0, "test01")
        my_interface_object_2.delete_text(1, 1)
        self.assertEqual(
            my_interface_object_1.read_text(
                0,
                my_interface_object_1.length()),
            u"äü")
        self.assertEqual(
            my_interface_object_2.read_text(
                0,
                my_interface_object_2.length()),
            u"äü")

    def test_insertTextRaisesOnArgumentError(self):
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createText(20, 20, 20, 20, "test01")
        my_interface_object = StoryInterface("test01")
        with self.assertRaises(TypeError):
            my_interface_object.insert_text()
        with self.assertRaises(TypeError):
            my_interface_object.insert_text(0)
        with self.assertRaises(TypeError):
            my_interface_object.insert_text(1)
        with self.assertRaises(TypeError):
            my_interface_object.insert_text(0.0, 0.0)
        with self.assertRaises(TypeError):
            my_interface_object.insert_text("0")
        with self.assertRaises(TypeError):
            my_interface_object.insert_text("")
        with self.assertRaises(TypeError):
            my_interface_object.insert_text("a", 0)
        with self.assertRaises(IndexError):
            my_interface_object.insert_text(u"a", -1)
        with self.assertRaises(IndexError):
            my_interface_object.insert_text(u"a", 1)

    def test_insert_correctly(self):
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createText(80, 80, 80, 80, "test08")
        my_interface_object = StoryInterface("test08")
        scribus.insertText(u"abc", 0, "test08")
        my_interface_object.insert_text(u"x", 1)
        self.assertEqual(
            my_interface_object.read_text(0, my_interface_object.length()),
            u"axbc")
        scribus.createText(80, 80, 80, 80, "test09")
        my_interface_object = StoryInterface("test09")
        scribus.insertText(u"abc", 0, "test09")
        my_interface_object.insert_text(u"xx", 1)
        self.assertEqual(
            my_interface_object.read_text(0, my_interface_object.length()),
            u"axxbc")
        scribus.createText(80, 80, 80, 80, "test10")
        my_interface_object = StoryInterface("test10")
        scribus.insertText(u"abc", 0, "test10")
        my_interface_object.insert_text(u"x", 0)
        self.assertEqual(
            my_interface_object.read_text(0, my_interface_object.length()),
            u"xabc")
        scribus.createText(80, 80, 80, 80, "test11")
        my_interface_object = StoryInterface("test11")
        scribus.insertText(u"abc", 0, "test11")
        my_interface_object.insert_text(u"xx", 0)
        self.assertEqual(
            my_interface_object.read_text(0, my_interface_object.length()),
            u"xxabc")
        scribus.createText(80, 80, 80, 80, "test12")
        my_interface_object = StoryInterface("test12")
        scribus.insertText(u"abc", 0, "test12")
        my_interface_object.insert_text(u"x", 3)
        self.assertEqual(
            my_interface_object.read_text(0, my_interface_object.length()),
            u"abcx")
        scribus.createText(80, 80, 80, 80, "test13")
        my_interface_object = StoryInterface("test13")
        scribus.insertText(u"abc", 0, "test13")
        my_interface_object.insert_text(u"xx", 3)
        self.assertEqual(
            my_interface_object.read_text(0, my_interface_object.length()),
            u"abcxx")

    def test_insert_correctly_to_linked_frame_1(self):
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createText(80, 80, 80, 80, "test01")
        scribus.createText(80, 80, 80, 80, "test02")
        my_interface_object_1 = StoryInterface("test01")
        my_interface_object_2 = StoryInterface("test02")
        scribus.linkTextFrames("test01", "test02")
        scribus.insertText(u"abc", 0, "test01")
        my_interface_object_1.insert_text(u"x", 1)
        self.assertEqual(
            my_interface_object_1.read_text(
                0,
                my_interface_object_1.length()),
            u"axbc")
        self.assertEqual(
            my_interface_object_2.read_text(
                0,
                my_interface_object_2.length()),
            u"axbc")

    def test_insert_correctly_to_linked_frame_2(self):
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createText(80, 80, 80, 80, "test01")
        scribus.createText(80, 80, 80, 80, "test02")
        my_interface_object_1 = StoryInterface("test01")
        my_interface_object_2 = StoryInterface("test02")
        scribus.linkTextFrames("test01", "test02")
        scribus.insertText(u"abc", 0, "test01")
        my_interface_object_2.insert_text(u"x", 1)
        self.assertEqual(
            my_interface_object_1.read_text(
                0,
                my_interface_object_1.length()),
            u"axbc")
        self.assertEqual(
            my_interface_object_2.read_text(
                0,
                my_interface_object_2.length()),
            u"axbc")


class DoLigatureSettingTestcase(ScribusTestcaseBaseClass):
    def test_single_word_1(self):
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createText(80, 80, 80, 80, "test01")
        my_interface_object = StoryInterface("test01")
        scribus.insertText(
            u"Auffallen",
            0,
            "test01")
        scribus.deselectAll()
        scribus.selectObject("test01")
        do_ligature_setting()
        self.assertEqual(
            my_interface_object.read_text(
                0,
                my_interface_object.length()),
            u"Auf\u200Cfallen")

    def test_single_word_2a(self):
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createText(80, 80, 80, 80, "test01")
        my_interface_object = StoryInterface("test01")
        scribus.insertText(
            u"Schiff\u00ADfahrt",
            0,
            "test01")
        scribus.deselectAll()
        scribus.selectObject("test01")
        do_ligature_setting()
        self.assertEqual(
            my_interface_object.read_text(
                0,
                my_interface_object.length()),
            u"Schiff\u00AD\u200Cfahrt")

    def test_single_word_2b(self):
        my_test_text = u"auf\u00ADfallen"
        assert InstructionProvider().get_instructions(my_test_text) == \
            [None, None, None, None, True, None, None, None, None, None]
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createText(80, 80, 80, 80, "test01")
        my_interface_object = StoryInterface("test01")
        scribus.insertText(
            my_test_text,
            0,
            "test01")
        scribus.deselectAll()
        scribus.selectObject("test01")
        do_ligature_setting()
        self.assertEqual(
            my_interface_object.read_text(
                0,
                my_interface_object.length()),
            u"auf\u00AD\u200Cfallen")

    def test_single_word_3(self):
        """Delete 1 ZWNJ correctly."""
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createText(80, 80, 80, 80, "test01")
        my_interface_object = StoryInterface("test01")
        scribus.insertText(
            u"Sc\u200Chiff",
            0,
            "test01")
        scribus.deselectAll()
        scribus.selectObject("test01")
        do_ligature_setting()
        self.assertEqual(
            my_interface_object.read_text(
                0,
                my_interface_object.length()),
            u"Schiff")

    def test_single_word_4(self):
        """Delete 2 ZWNJ correctly."""
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createText(80, 80, 80, 80, "test01")
        my_interface_object = StoryInterface("test01")
        scribus.insertText(
            u"Sc\u200Chif\u200Cf",
            0,
            "test01")
        scribus.deselectAll()
        scribus.selectObject("test01")
        do_ligature_setting()
        self.assertEqual(
            my_interface_object.read_text(
                0,
                my_interface_object.length()),
            u"Schiff")

    def test_single_word_5(self):
        """Delete ZWNJs in the middle and leading and
        trailing ZWNJs correctly."""
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createText(80, 80, 80, 80, "test01")
        my_interface_object = StoryInterface("test01")
        scribus.insertText(
            u"\u200C\u200CSc\u200C\u200Chif\u200Cf\u200C\u200C",
            0,
            "test01")
        scribus.deselectAll()
        scribus.selectObject("test01")
        do_ligature_setting()
        self.assertEqual(
            my_interface_object.read_text(
                0,
                my_interface_object.length()),
            u"Schiff")

    def test_single_word_6(self):
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createText(80, 80, 80, 80, "test01")
        my_interface_object = StoryInterface("test01")
        scribus.insertText(
            u"Sc\u00ADhi\u200Cfffah\u200Crtsamtsstube",
            0,
            "test01")
        scribus.deselectAll()
        scribus.selectObject("test01")
        do_ligature_setting()
        self.assertEqual(
            my_interface_object.read_text(
                0,
                my_interface_object.length()),
            u"Sc\u00ADhiff\u200Cfahrts\u200Camts\u200Cstube")

    def test_single_word_7(self):
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createText(80, 80, 80, 80, "test01")
        my_interface_object = StoryInterface("test01")
        scribus.insertText(
            u"Sc\u00ADhifffah\u200Crtsamt",
            0,
            "test01")
        scribus.deselectAll()
        scribus.selectObject("test01")
        do_ligature_setting()
        self.assertEqual(
            my_interface_object.read_text(
                0,
                my_interface_object.length()),
            u"Sc\u00ADhiff\u200Cfahrts\u200Camt")

    def test_single_word_8(self):
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createText(80, 80, 80, 80, "test01")
        my_interface_object = StoryInterface("test01")
        scribus.insertText(
            u"von",
            0,
            "test01")
        scribus.deselectAll()
        scribus.selectObject("test01")
        do_ligature_setting()
        self.assertEqual(
            my_interface_object.read_text(
                0,
                my_interface_object.length()),
            u"von")

    def test_single_word_9(self):
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createText(80, 80, 80, 80, "test01")
        my_interface_object = StoryInterface("test01")
        scribus.insertText(
            u"Salatöl",
            0,
            "test01")
        scribus.deselectAll()
        scribus.selectObject("test01")
        do_ligature_setting()
        self.assertEqual(
            my_interface_object.read_text(
                0,
                my_interface_object.length()),
            u"Salat\u200Cöl")

    def test_single_word_10(self):
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createText(80, 80, 80, 80, "test01")
        my_interface_object = StoryInterface("test01")
        scribus.insertText(
            u"SALATÖL",
            0,
            "test01")
        scribus.deselectAll()
        scribus.selectObject("test01")
        do_ligature_setting()
        self.assertEqual(
            my_interface_object.read_text(
                0,
                my_interface_object.length()),
            u"SALAT\u200CÖL")

    def test_single_word_11(self):
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createText(80, 80, 80, 80, "test01")
        my_interface_object = StoryInterface("test01")
        scribus.insertText(
            u"Salato\u0308l",
            0,
            "test01")
        scribus.deselectAll()
        scribus.selectObject("test01")
        do_ligature_setting()
        self.assertEqual(
            my_interface_object.read_text(
                0,
                my_interface_object.length()),
            u"Salat\u200Co\u0308l")

    def test_single_word_12(self):
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createText(80, 80, 80, 80, "test01")
        my_interface_object = StoryInterface("test01")
        scribus.insertText(
            u"SALATO\u0308L",
            0,
            "test01")
        scribus.deselectAll()
        scribus.selectObject("test01")
        do_ligature_setting()
        self.assertEqual(
            my_interface_object.read_text(
                0,
                my_interface_object.length()),
            u"SALAT\u200CO\u0308L")

    def test_apply_to_linked_frame_1(self):
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createText(80, 80, 80, 80, "test01")
        scribus.createText(80, 80, 80, 80, "test02")
        my_interface_object_1 = StoryInterface("test01")
        my_interface_object_2 = StoryInterface("test02")
        scribus.linkTextFrames("test01", "test02")
        scribus.insertText(
            u"Auffallen Schiff\u00ADfahrt Sc\u00ADhifffah\u200Crtsamt",
            0,
            "test01")
        scribus.deselectAll()
        scribus.selectObject("test01")
        do_ligature_setting()
        self.assertEqual(
            my_interface_object_1.read_text(
                0,
                my_interface_object_1.length()),
            u"Auf\u200Cfallen Schiff\u00AD\u200Cfahrt Sc\u00ADhiff\u200Cfahrts\u200Camt")
        self.assertEqual(
            my_interface_object_2.read_text(
                0,
                my_interface_object_2.length()),
            u"Auf\u200Cfallen Schiff\u00AD\u200Cfahrt Sc\u00ADhiff\u200Cfahrts\u200Camt")

    def test_apply_to_linked_frame_2(self):
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createText(80, 80, 80, 80, "test01")
        scribus.createText(80, 80, 80, 80, "test02")
        my_interface_object_1 = StoryInterface("test01")
        my_interface_object_2 = StoryInterface("test02")
        scribus.linkTextFrames("test01", "test02")
        scribus.insertText(
            u"Auffallen Schiff\u00ADfahrt "
            u"Sc\u00ADhifffah\u200Crtsamt "
            u"Straſſen\u200Cſchlacht "
            u"StraẞEn\u200CSChlacht "
            u"Straſ\u200Cſ\u200Cenſc\u200Chlacht "
            u"S\u200Ctraẞ\u200CEn\u200CS\u200CChlacht",
            0,
            "test01")
        scribus.deselectAll()
        scribus.selectObject("test02")
        do_ligature_setting()
        self.assertEqual(
            my_interface_object_1.read_text(
                0,
                my_interface_object_1.length()),
            u"Auf\u200Cfallen Schiff\u00AD\u200Cfahrt "
            u"Sc\u00ADhiff\u200Cfahrts\u200Camt "
            u"Straſſen\u200Cſchlacht "
            u"StraẞEn\u200CSChlacht "
            u"Straſſen\u200Cſchlacht "
            u"StraẞEn\u200CSChlacht")
        self.assertEqual(
            my_interface_object_2.read_text(
                0,
                my_interface_object_2.length()),
            u"Auf\u200Cfallen Schiff\u00AD\u200Cfahrt "
            u"Sc\u00ADhiff\u200Cfahrts\u200Camt "
            u"Straſſen\u200Cſchlacht "
            u"StraẞEn\u200CSChlacht "
            u"Straſſen\u200Cſchlacht "
            u"StraẞEn\u200CSChlacht")

    def test_leading_trailing_1(self):
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createText(80, 80, 80, 80, "test01")
        my_interface_object = StoryInterface("test01")
        scribus.insertText(
            u"\u200C\u200C\u200Cauffallen\u200C\u200C\u200C",
            0,
            "test01")
        scribus.deselectAll()
        scribus.selectObject("test01")
        do_ligature_setting()
        self.assertEqual(
            my_interface_object.read_text(
                0,
                my_interface_object.length()),
            u"auf\u200Cfallen")

    def test_leading_trailing_2(self):
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createText(80, 80, 80, 80, "test01")
        my_interface_object = StoryInterface("test01")
        scribus.insertText(
            u"\u00AD\u200C\u200Cau\u00ADf\u00ADf\u00ADallen\u200C\u200C\u200C\u00AD",
            0,
            "test01")
        scribus.deselectAll()
        scribus.selectObject("test01")
        do_ligature_setting()
        self.assertEqual(
            my_interface_object.read_text(
                0,
                my_interface_object.length()),
            u"\u00ADau\u00ADf\u00AD\u200Cf\u00ADallen\u00AD")

    def test_soft_hyphen_in_the_middle(self):
        my_test_text = u"auf\u00ADfallen"
        assert InstructionProvider().get_instructions(my_test_text) == \
            [None, None, None, None, True, None, None, None, None, None]
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createText(80, 80, 80, 80, "test01")
        my_interface_object = StoryInterface("test01")
        scribus.insertText(
            my_test_text,
            0,
            "test01")
        assert my_interface_object.read_text(0, my_interface_object.length()) == \
            my_test_text
        scribus.deselectAll()
        scribus.selectObject("test01")
        do_ligature_setting()
        self.assertEqual(
            my_interface_object.read_text(
                0,
                my_interface_object.length()),
            u"auf\u00AD\u200Cfallen")

    def test_clear_text_selection_after(self):
        """After calling do_ligature_setting() there should not be any
        text selection."""
        self.closeAllDocumentsAndCreateOneNewDocument()
        scribus.createText(80, 80, 80, 80, "test01")
        scribus.insertText(
            u"abc",
            0,
            "test01")
        scribus.deselectAll()
        scribus.selectObject("test01")
        scribus.selectText(1, 1)
        assert scribus.getAllText().decode("utf8", "strict") == u"b"
        do_ligature_setting()
        self.assertEqual(scribus.getAllText().decode("utf8", "strict"), u"abc")
        scribus.deleteText()
        self.assertEqual(scribus.getAllText().decode("utf8", "strict"), u"")


if scribus.haveDoc() > 0:
    scribus.messageBox(
        "",
        "The unit tests can only be executed when no document is open.")
else:
    try:
        scribus.messageBox(
            "",
            "The unit tests will be executed. This may take some time. "
                "When the ligature setting dialog appears, please"
                "choose “OK”. When the unit tests have finished, a message "
                "will be "
                "displayed. The result of the unit tests will be written "
                "however to the standard output.")

        unittest.main()
    finally:
        scribus.setRedraw(True)  # Just to be sure (you never know…)
        close_all_documents()
        scribus.messageBox("", "The unit tests have finished.")
