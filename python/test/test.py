#!/usr/bin/env python3

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

import os
import sys
import unittest
import types
import inspect
import importlib.util
from pathlib import Path

def main():
    # Determine this script's absolute path and project root
    # script is at .../python/test/test.py → parents[0]=test, [1]=python, [2]=project root
    script_path = Path(__file__).resolve()
    try:
        project_root = script_path.parents[2]
    except IndexError:
        # fallback: parent of parent
        project_root = script_path.parent.parent

    # Add our python module compiled-extension directory to sys.path,
    # so that `import genesis` works
    ext_dir = project_root / "bin" / "python"
    sys.path.insert(0, str(ext_dir))

    # Verify import of genesis extension
    try:
        import genesis  # noqa: F401
    except ImportError as e:
        print(f"ERROR: Failed to import genesis extension: {e}", file=sys.stderr)
        sys.exit(1)

    # Inject test_data_dir via `test_config`.
    # In the test files, this can be used as follows:
    #     import test_config
    #     data_file = os.path.join(test_config.test_data_dir, "sample.txt")
    data_dir = project_root / "test" / "data"
    test_cfg = types.SimpleNamespace(test_data_dir=str(data_dir))
    sys.modules["test_config"] = test_cfg

    # Discover and load all .py files under python/test
    tests_path = project_root / "python" / "test"
    loader = unittest.TestLoader()
    suite  = unittest.TestSuite()

    # We cannot use the unittest discover funciton, as it expects all tests
    # to be packages with `__init__.py` files, which seems cumbersome.
    # So instead, we just find all our files ourselves...
    # suite = loader.discover(start_dir=str(tests_path), pattern="*.py")
    for path in tests_path.rglob("*.py"):
        # Skip the runner itself
        if path.resolve() == script_path:
            continue

        # Dynamically load module from file
        spec = importlib.util.spec_from_file_location(str(path), str(path))
        module = importlib.util.module_from_spec(spec)
        sys.modules[spec.name] = module
        spec.loader.exec_module(module)

        # Find all TestCase subclasses defined *in* this module
        for _, obj in inspect.getmembers(module, inspect.isclass):
            if issubclass(obj, unittest.TestCase) and obj.__module__ == module.__name__:
                suite.addTests(loader.loadTestsFromTestCase(obj))

    # Define a custom result that only shows test._testMethodName
    class SimpleNameResult(unittest.TextTestResult):
        def getDescription(self, test):
            # just the bare test method name, e.g. "test_xxx"
            return test._testMethodName

    # Run test suite with our nice result printing
    # runner = unittest.TextTestRunner(verbosity=2)
    runner = unittest.TextTestRunner(
        verbosity=2,
        resultclass=SimpleNameResult
    )
    result = runner.run(suite)

    # Exit with appropriate status
    sys.exit(0 if result.wasSuccessful() else 1)

if __name__ == "__main__":
    main()
