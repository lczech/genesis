# Genesis - A toolkit for working with phylogenetic data.
# Copyright (C) 2023-2025 Giannis Reppas and Lucas Czech
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# Contact:
# Lucas Czech <lucas.czech@sund.ku.dk>
# University of Copenhagen, Globe Institute, Section for GeoGenetics
# Oster Voldgade 5-7, 1350 Copenhagen K, Denmark

import unittest, os
from genesis.genesis.util import container
import test_config

class TestGenesisutilContainerMruCacheBasics(unittest.TestCase):
    def setUp(self):
        # capacity 5, loads str->int
        self.cache = container.MruCache(5)
        self.cache.load_function = lambda s: int(s)

    def test_empty_and_properties(self):
        self.assertEqual(self.cache.size(), 0)
        self.assertTrue(self.cache.empty())
        self.assertEqual(self.cache.capacity(), 5)
        self.assertFalse(self.cache.contains("nope"))

    def test_fill_and_eviction(self):
        # fetch values 0..4
        for i in range(5):
            self.cache.fetch(str(i))
        self.assertEqual(self.cache.size(), 5)

        # fetch more: forces eviction of oldest '0','1'
        self.cache.fetch('5')
        self.cache.fetch('6')
        self.assertEqual(self.cache.size(), 5)
        # fetching existing does not change
        self.assertEqual(self.cache.fetch('6'), 6)

        # contains check
        self.assertFalse(self.cache.contains('0'))
        self.assertFalse(self.cache.contains('1'))
        for key in ['2','3','4','5','6']:
            self.assertTrue(self.cache.contains(key))

        # touch and further eviction
        self.cache.touch('3')
        self.cache.touch('7')
        self.cache.touch('8')
        self.assertFalse(self.cache.contains('2'))
        self.assertFalse(self.cache.contains('4'))

        # shrink capacity to 3
        self.cache.capacity(3)
        self.assertEqual(self.cache.size(), 3)
        for key in ['3','7','8']:
            self.assertTrue(self.cache.contains(key))
        self.assertEqual(self.cache.fetch('8'), 8)

        # limitless capacity
        self.cache.capacity(0)
        self.assertEqual(self.cache.size(), 3)
        self.assertEqual(self.cache.capacity(), 0)

        # add more
        self.cache.touch('9')
        self.cache.touch('10')
        self.assertEqual(self.cache.size(), 5)

        # existing fetch
        self.assertEqual(self.cache.fetch('8'), 8)
        self.assertEqual(self.cache.size(), 5)

        # clear
        self.cache.clear()
        self.assertEqual(self.cache.size(), 0)
        self.assertTrue(self.cache.empty())

class TestGenesisutilContainerMruCacheLimitless(unittest.TestCase):
    def setUp(self):
        # default capacity (0 == limitless)
        self.cache = container.MruCache()
        self.cache.load_function = lambda s: int(s)

    def test_limitless_growth(self):
        for i in range(5):
            self.cache.fetch(str(i))
        self.assertEqual(self.cache.size(), 5)
        # re-fetch existing
        self.assertEqual(self.cache.fetch('1'), 1)
        self.assertEqual(self.cache.fetch('2'), 2)
        self.assertEqual(self.cache.size(), 5)

class TestGenesisutilContainerMruCacheFailLoading(unittest.TestCase):
    def setUp(self):
        self.cache = container.MruCache(3)
        self.cache.load_function = lambda s: int(s)

    def test_failures_do_not_insert(self):
        with self.assertRaises(ValueError): self.cache.fetch('abc')
        with self.assertRaises(ValueError): self.cache.fetch('xyz')
        self.assertFalse(self.cache.contains('abc'))
        self.assertFalse(self.cache.contains('xyz'))
        self.assertEqual(self.cache.size(), 0)
        self.assertTrue(self.cache.empty())
        self.assertEqual(self.cache.capacity(), 3)

        # load good values
        for i in range(5):
            self.cache.fetch(str(i))
        # capacity 3 => only '2','3','4' remain
        self.assertEqual(self.cache.size(), 3)
        for key in ['2','3','4']:
            self.assertTrue(self.cache.contains(key))

        # further load failures
        with self.assertRaises(ValueError): self.cache.fetch('l')
        with self.assertRaises(ValueError): self.cache.fetch('m')
        # state unchanged
        self.assertEqual(self.cache.size(), 3)
        for key in ['2','3','4']:
            self.assertTrue(self.cache.contains(key))

class TestGenesisutilContainerMruCacheFiles(unittest.TestCase):
    def setUp(self):
        self.cache = container.MruCache(3)
        # load simple text from files in JSON_DIR
        self.data_dir = os.path.join(test_config.test_data_dir, "util/json")
        self.cache.load_function = lambda fname: open(os.path.join(self.data_dir, fname), 'r').read()

    def test_file_eviction_and_errors(self):
        # fetch 4 files: fail2.jtest .. fail5.jtest
        for name in ['fail2.jtest','fail3.jtest','fail4.jtest','fail5.jtest']:
            self.cache.fetch(name)
        self.assertEqual(self.cache.size(), 3)
        self.assertFalse(self.cache.contains('fail2.jtest'))
        for name in ['fail3.jtest','fail4.jtest','fail5.jtest']:
            self.assertTrue(self.cache.contains(name))

        # missing file
        with self.assertRaises(Exception): self.cache.fetch('not_there')
        # no change
        self.assertEqual(self.cache.size(), 3)
        for name in ['fail3.jtest','fail4.jtest','fail5.jtest']:
            self.assertTrue(self.cache.contains(name))

class TestGenesisutilContainerMruCacheSharedPtr(unittest.TestCase):
    def setUp(self):
        self.cache = container.MruCache(3)
        self.data_dir = os.path.join(test_config.test_data_dir, "util/json")
        # load returns Python string simulating shared_ptr
        self.cache.load_function = lambda fname: open(os.path.join(self.data_dir, fname), 'r').read()

    def test_fetch_copy_shared(self):
        # fetch_copy returns a new reference (string)
        content = self.cache.fetch_copy('fail2.jtest')
        self.assertIsInstance(content, str)
        # fetch_copy repeatedly
        for name in ['fail2.jtest','fail3.jtest','fail4.jtest','fail5.jtest']:
            self.cache.fetch_copy(name)
        self.assertEqual(self.cache.size(), 3)
        self.assertFalse(self.cache.contains('fail2.jtest'))
        for name in ['fail3.jtest','fail4.jtest','fail5.jtest']:
            self.assertTrue(self.cache.contains(name))
        with self.assertRaises(Exception): self.cache.fetch_copy('not_there')
        self.assertEqual(self.cache.size(), 3)
        for name in ['fail3.jtest','fail4.jtest','fail5.jtest']:
            self.assertTrue(self.cache.contains(name))
