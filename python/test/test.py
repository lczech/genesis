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
import re
import unittest
import types
import inspect
import importlib.util
from pathlib import Path

# ================================================================================================
#     Pretty Print
# ================================================================================================

class TableTestResult(unittest.TextTestResult):
    """
    Collects each test's formatted class/name and status,
    then prints a three-column table at the end of the run.
    """
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        # will hold tuples of (formatted_class, formatted_method, status)
        self._rows = []

    def _format(self, test, status):
        # raw names
        cls = test.__class__.__name__
        mth = test._testMethodName

        # strip prefixes
        if cls.startswith('Test'):
            cls = cls[len('Test'):]
        if mth.startswith('test_'):
            mth = mth[len('test_'):]

        # CamelCase → ::-joined
        words = re.findall(r'[A-Z][a-z0-9]*', cls)
        cls_fmt = '::'.join(words) if words else cls

        return cls_fmt, mth, status

    def addSuccess(self, test):
        # record success without printing a dot
        unittest.TestResult.addSuccess(self, test)
        self._rows.append(self._format(test, 'ok'))

    def addFailure(self, test, err):
        unittest.TestResult.addFailure(self, test, err)
        self._rows.append(self._format(test, 'FAIL'))

    def addError(self, test, err):
        unittest.TestResult.addError(self, test, err)
        self._rows.append(self._format(test, 'ERROR'))

    def addSkip(self, test, reason):
        unittest.TestResult.addSkip(self, test, reason)
        self._rows.append(self._format(test, f'SKIP({reason})'))

    def stopTestRun(self):
        super().stopTestRun()
        # compute column widths
        cls_w = max(len(r[0]) for r in self._rows + [('Class',)])
        mth_w = max(len(r[1]) for r in self._rows + [('', 'Test')])
        res_w = max(len(r[2]) for r in self._rows + [('', '', 'Result')])

        # header
        header = f"{'Class':<{cls_w}} | {'Test':<{mth_w}} | {'Result':<{res_w}}"
        sep    = f"{'-'*cls_w}-+-{'-'*mth_w}-+-{'-'*res_w}"
        self.stream.writeln('')
        self.stream.writeln(header)
        self.stream.writeln(sep)

        # rows in recorded order (loader guarantees alphabetical)
        for cls_fmt, mth_fmt, status in self._rows:
            line = f"{cls_fmt:<{cls_w}} | {mth_fmt:<{mth_w}} | {status:<{res_w}}"
            self.stream.writeln(line)

# Simpler alternative: Define a custom result that only shows test._testMethodName
class SimpleNameResult(unittest.TextTestResult):
    def getDescription(self, test):
        # test.id() == 'module.ClassName.methodName'
        class_name = test.__class__.__name__
        method_name = test._testMethodName
        return f"{class_name}.{method_name}"

# ================================================================================================
#     Test Suite
# ================================================================================================

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

    # Run test suite with our nice result printing
    # runner = unittest.TextTestRunner(verbosity=2)
    runner = unittest.TextTestRunner(
        # verbosity=2,
        # resultclass=SimpleNameResult
        resultclass=TableTestResult
    )
    result = runner.run(suite)

    # Exit with appropriate status
    sys.exit(0 if result.wasSuccessful() else 1)

if __name__ == "__main__":
    main()
